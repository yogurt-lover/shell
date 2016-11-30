#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/wait.h>
#include <sys/types.h>

#define NRML "\x1B[0m"
#define CYAN "\x1B[36m"
#define BOLD "\x1B[1m"
#define STDIN_CHANGED 0b0001
#define STDOUT_CHANGED 0b0010
#define STDERR_CHANGED 0b0100
#define STD_FAILED 0b1000
#define DEBUG 1

// signals (Ctrl-C)
// resizing buffers
// redirection << < | > >>
// cd - (history)
// error handling
// previous command
// config + builtins
// *


char *get_home_dir() {
	return getenv("HOME");
}

char *get_username() {
	return getenv("USER");
}

char *get_hostname() {
	char *hostname = (char *)malloc(256*sizeof(char));
	hostname[255] = '\0';
	gethostname(hostname, 255);
	return hostname;
}

char *get_cwd() {
	char *cwd = (char *)malloc(256*sizeof(char));
	cwd[255] = '\0';
	getcwd(cwd, 255);
	return cwd;
}

void print_prompt() {
	char *cwd = get_cwd();
	char *username = get_username();
	char *hostname = get_hostname();
	char *home = get_home_dir();
	char *dir = cwd;
	char in_home_dir = 0;
	if (strstr(cwd, home)) {
		dir += strlen(home);
		in_home_dir = '~';
	}
	printf("%s%s%s@%s:%c%s%s\n$ ", CYAN, BOLD, username, hostname, in_home_dir, dir, NRML );
	free(cwd);
	free(hostname);
}

char *read_raw() {
	char *buff = (char *)malloc(256*sizeof(char));
	char c;
	int counter = 0;
	c = getchar();
	for (; c != '\n' && c != EOF; counter++) {
		buff[counter] = c;
		c = getchar();
	}
	buff[counter] = '\0';
	if (c == EOF) {
		fprintf(stderr, "\n");
	}
	return buff;
}

char **get_args(char *input, int *num_args) {
	char **args = (char **)malloc(256*sizeof(char*));
	char *arg = strtok(input, " \n\r\t");
	int counter;
	for (counter = 0; arg; counter++) {
		if (!strcmp(arg, "~")) arg = get_home_dir();
		args[counter] = arg;
		arg = strtok(NULL, " \n\r\t");
	}
	args[counter] = arg;
	*num_args = counter;
	return args;
}


void redirect_stdout(int *redirect, int *dup_stdout) {
	*redirect = *redirect | STDOUT_CHANGED;
	*dup_stdout = dup(STDOUT_FILENO);
	close(STDOUT_FILENO);
}

void redirect_stderr(int *redirect, int *dup_stderr) {
	*redirect = *redirect | STDERR_CHANGED;
	*dup_stderr = dup(STDERR_FILENO);
	close(STDERR_FILENO);
}

void redirect_stdin(int *redirect, int *dup_stdin) {
	*redirect = *redirect | STDIN_CHANGED;
	*dup_stdin = dup(STDIN_FILENO);
	close(STDIN_FILENO);
}
void change_stdout(char *input, int *redirect, int *dup_stdout, int *dup_stderr) {
	char *new_out_path;
	int append_flag = strstr(input, ">>") ? O_APPEND : O_TRUNC;

	char *input_dup = strdup(input);
	strsep(&input_dup, ">");
	new_out_path = strtok(input_dup, " >\n\r\t");

	if (DEBUG) fprintf(stderr, "new_out_path: ~|%s|~\n", new_out_path);
	if (new_out_path) {
		if (strstr(input, "&>")) {
			redirect_stdout(redirect, dup_stdout);
			open(new_out_path, O_WRONLY | append_flag | O_CREAT, 0644); // opens with fd 1
			redirect_stderr(redirect, dup_stderr);
			dup2(STDOUT_FILENO, STDERR_FILENO); // dup 1 to 2
		}
		else if (strstr(input, "2>")) {
			redirect_stderr(redirect, dup_stderr);
			open(new_out_path, O_WRONLY | append_flag | O_CREAT , 0644); // opens with fd 2
		}
		else {
			redirect_stdout(redirect, dup_stdout);
			open(new_out_path, O_WRONLY | append_flag | O_CREAT , 0644); // opens with fd 1
		}
	}
	else {
		fprintf(stderr, "shell: OUT file not specified\n");
		*redirect = *redirect | STD_FAILED;
	}
	if (DEBUG) fprintf(stderr, "redirect after >: %d\n", *redirect);
}

void change_stdin(char *input, int *redirect, int *dup_stdin) {
	char *new_stdin_path;

	char *input_dup = strdup(input);
	strsep(&input_dup, "<");
	new_stdin_path = strtok(input_dup, " \n\r\t");

	if (DEBUG) fprintf(stderr, "new_stdin_path: ~|%s|~\n", new_stdin_path);
	if (new_stdin_path) {
		redirect_stdin(redirect, dup_stdin);
		open(new_stdin_path, O_RDONLY);
	}
	else {
		fprintf(stderr, "shell: IN file not specified\n");
		*redirect = *redirect | STD_FAILED;
	}
	if (DEBUG) fprintf(stderr, "redirect after <: %d\n", *redirect);
}

void restore_stdin(int dup_stdin) {
	close(STDIN_FILENO);
	dup(dup_stdin);
	close(dup_stdin);
}

void restore_stdout(int dup_stdout) {
	close(STDOUT_FILENO);
	dup(dup_stdout);
	close(dup_stdout);
}

void restore_stderr(int dup_stderr) {
	close(STDERR_FILENO);
	dup(dup_stderr);
	close(dup_stderr);
}

void print_args(char **args, int num_args) {
	fprintf(stderr, "ARGUMENTS:<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	int k = 0;
	for (; k < num_args; k++) fprintf(stderr, "arg: ~|%s|~\n", args[k]);
	fprintf(stderr, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}

void cd_def(char **args) {
	if (args[1]) {
		int chdir_ret = chdir(args[1]);
		if (chdir_ret == -1) perror("shell");
	}
	else chdir(get_home_dir());
}

void exec_coreutil(char **args) {
	if (execvp(args[0], args) == -1)
		fprintf(stderr, "shell: %s: command not found in PATH\n", args[0]);
}

void exec_single(char **args, int *status) {
	//if (!args[0]) return;
	if (!strcmp(args[0], "cd")) cd_def(args);
	else if (!strcmp(args[0], "exit")) *status = 0;
	else {
		int f = fork();
		if (f == 0) exec_coreutil(args);
	}
}

void set_pipe_stdin(int command_num, int pipes[2][2]) {
	close(STDIN_FILENO);
	dup(pipes[(command_num + 1) % 2][0]);
	close(pipes[(command_num + 1) % 2][0]);
	close(pipes[(command_num + 1) % 2][1]);
}

void set_pipe_stdout(int command_num, int pipes[2][2]) {
	close(STDOUT_FILENO);
	dup(pipes[command_num % 2][1]);
	close(pipes[command_num % 2][0]);
	close(pipes[command_num % 2][1]);
}

void pipe_handler(char **args, int command_num, int pipe_num, int pipes[2][2]) {
	int f = fork();
	if (f == 0) {
		if (command_num == 0) {
			set_pipe_stdout(command_num, pipes);
			exec_coreutil(args);
		}
		else if (command_num == pipe_num) {
			set_pipe_stdin(command_num, pipes);
			exec_coreutil(args);
		}
		else {
			set_pipe_stdin(command_num, pipes);
			set_pipe_stdout(command_num, pipes);
			exec_coreutil(args);
		}
	}
	if (command_num > 0) {
		close(pipes[(command_num + 1) % 2][0]);
		close(pipes[(command_num + 1) % 2][1]);
	}
}

int exec_pipe(char **args, int num_args, int command_num, int pipe_num, int pipes[2][2]) {
	if (!num_args) {
		fprintf(stderr, "shell: expected a command to pipe through\n");
		return 0;
	}
	if (!strcmp(args[0], "cd") || !strcmp(args[0], "exit")) {
		fprintf(stderr, "shell: cannot pipe through a shell built-ins\n");
		return 0;
	}
	pipe(pipes[command_num % 2]);
	pipe_handler(args, command_num, pipe_num, pipes);
	return 1;
}

void process() {
	int status = 1;
	char *raw_input;

	print_prompt();
	raw_input = read_raw();
	if (DEBUG) fprintf(stderr, "raw_input: ~|%s|~\n", raw_input);

	char *s = raw_input;
	while (s) {
		int redirect, dup_stdin, dup_stdout, dup_stderr;
		char *single_input;
		redirect = 0;

		single_input = strsep(&s, ";");
		if (DEBUG) fprintf(stderr, "single_input:~|%s|~\n", single_input);

		if (single_input && strlen(single_input)) {

			if (strchr(single_input, '<'))
				change_stdin(single_input, &redirect, &dup_stdin);
			if (strchr(single_input, '>'))
				change_stdout(single_input, &redirect, &dup_stdout, &dup_stderr);
			if (!(redirect & STD_FAILED)) {
				single_input = strsep(&single_input, "><");
				if (redirect & STDOUT_CHANGED || redirect & STDERR_CHANGED) {
					char last_char = single_input[strlen(single_input)-1];
					if (DEBUG) fprintf(stderr, "last_char: %c\n", last_char);
					if (last_char == '&' || last_char == '1' || last_char == '2')
						*strrchr(single_input, last_char) = 0;
				}
				if (DEBUG) fprintf(stderr, "new single_input:~|%s|~\n", single_input);
				if (single_input && strlen(single_input)) {
					int num_args, pipe_num, command_num;
					int pipes[2][2];
					char *p, *q;
					char **args;

					pipe_num = command_num = 0;
					p = q = single_input;
					for (; p[pipe_num]; p[pipe_num]=='|' ? pipe_num++ : *p++);
					if (DEBUG) fprintf(stderr, "pipe_num: %d\n", pipe_num);
					for (; q; command_num++) {
						single_input = strsep(&q, "|");
						if (DEBUG) fprintf(stderr, "new single_input:~|%s|~\n", single_input);
						args = get_args(single_input, &num_args);
						if (DEBUG) print_args(args, num_args);
						if (pipe_num) {
							if (exec_pipe(args, num_args, command_num, pipe_num, pipes) == 0) break;
						}
						else exec_single(args, &status);
					}
					while (wait(NULL) > 0);
					free(args);
				}
				if (redirect & STDIN_CHANGED) restore_stdin(dup_stdin);
				if (redirect & STDOUT_CHANGED) restore_stdout(dup_stdout);
				if (redirect & STDERR_CHANGED) restore_stderr(dup_stderr);
			}
		}
	}
	free(raw_input);
	if (!status) exit(EXIT_SUCCESS);
}

int main() {
	while (1) process();
	return 0;
}
