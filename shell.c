#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>

#include <sys/wait.h>
#include <sys/types.h>

#define NRML "\x1B[0m"
#define CYAN "\x1B[36m"
#define BOLD "\x1B[1m"
#define DEBUG 0

// signals (Ctrl-C)
// resizing buffers
// redirection << < | > >>
// cd - (history)
// error handling
// previous command
// config + builtins
// *


char *get_home_dir() {
	struct passwd *pw = getpwuid(getuid());
	return pw->pw_dir;
}

char *get_username() {
	struct passwd *pw = getpwuid(getuid());
	return pw->pw_name;
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
	free(hostname);
	free(cwd);
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
		//exit(1);
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

int execute(char **args, int *num_args) {
	if (DEBUG) fprintf(stderr, "OUTPUT:\n----------------------------\n");

	// whitespace or no characters at all
	if (!args[0]) return 1;

	// cd (might put this someplace else)
	if (!strcmp(args[0], "cd")) {
		if (args[1]) {
			if (chdir(args[1])) perror("shell");
		}
		else chdir(get_home_dir());
		return 1;
	}

	// exit (this too)
	if (!strcmp(args[0], "exit")) return 0;

	// system binaries
	int f = fork();
	if (f == 0) {
		if (execvp(args[0], args) == -1) {
			fprintf(stderr, "%s: command not found\n", args[0]);
		}
	}
	else wait(NULL);

	if (DEBUG) fprintf(stderr, "----------------------------\n");
	return 1;
}

int change_stdout(char *input, int *redirect, int *dup_stdout, int *dup_stderr) {
	char *new_out_path;
	int append_flag = strstr(input, ">>") ? O_APPEND : O_TRUNC;

	char *input_dup = strdup(input);
	strsep(&input_dup, ">");
	new_out_path = strtok(input_dup, " >\n\r\t");

	if (DEBUG) fprintf(stderr, "new_out_path: ~|%s|~\n", new_out_path);
	if (new_out_path) {
		if (strstr(input, "&>")) {
			*redirect = *redirect | 0b110;
			*dup_stdout = dup(STDOUT_FILENO);
			*dup_stderr = dup(STDERR_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			open(new_out_path, O_WRONLY | append_flag | O_CREAT, 0644); // opens with fd 1
			dup2(STDOUT_FILENO, STDERR_FILENO); // dup 1 to 2
		}
		else if (strstr(input, "2>")) {
			*redirect = *redirect | 0b100;
			*dup_stderr = dup(STDERR_FILENO);
			close(STDERR_FILENO);
			open(new_out_path, O_WRONLY | append_flag | O_CREAT , 0644); // opens with fd 2
		}
		else {
			*redirect = *redirect | 0b010;
			*dup_stdout = dup(STDOUT_FILENO);
			close(STDOUT_FILENO);
			open(new_out_path, O_WRONLY | append_flag | O_CREAT , 0644); // opens with fd 1
		}
	}
	else {
		fprintf(stderr, "shell: OUT file not specified\n");
		return 0;
	}
	if (DEBUG) fprintf(stderr, "redirect after >: %d\n", *redirect);
	return 1;
}

int change_stdin(char *input, int *redirect, int *dup_stdin) {
	char *new_stdin_path;

	char *input_dup = strdup(input);
	strsep(&input_dup, "<");
	new_stdin_path = strtok(input_dup, " \n\r\t");

	if (DEBUG) fprintf(stderr, "new_stdin_path: ~|%s|~\n", new_stdin_path);
	if (new_stdin_path) {
		*redirect = *redirect | 0b001;
		*dup_stdin = dup(STDIN_FILENO);
		close(STDOUT_FILENO);
		open(new_stdin_path, O_RDONLY);
	}
	else {
		fprintf(stderr, "shell: IN file not specified\n");
		return 0;
	}
	if (DEBUG) fprintf(stderr, "redirect after <: %d\n", *redirect);
	return 1;
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
	fprintf(stderr, "----------------------------\n");
	fprintf(stderr, "ARGUMENTS\n");
	int k = 0;
	for (; k < num_args; k++) fprintf(stderr, "arg: ~|%s|~\n", args[k]);
	fprintf(stderr, "----------------------------\n");
}

int process() {
	int status;
	char *raw_input;

	print_prompt();
	raw_input = read_raw();
	if (DEBUG) fprintf(stderr, "raw_input: ~|%s|~\n", raw_input);

	char *s = raw_input;
	while (s) {
		int num_args;
		int redirect, dup_stdin, dup_stdout, dup_stderr;
		char *single_input;
		char **args;

		redirect = 0;

		single_input = strsep(&s, ";");
		if (DEBUG) fprintf(stderr, "single_input:~|%s|~\n", single_input);

		if (!single_input || !strlen(single_input)) return 1;

		if (strchr(single_input, '>'))
			if (!change_stdout(single_input, &redirect, &dup_stdout, &dup_stderr)) return 1;

		if (strchr(single_input, '<'))
			if (!change_stdin(single_input, &redirect, &dup_stdin)) return 1;

		single_input = strsep(&single_input, "><");
		if (redirect & 0b010 || redirect & 0b100) {
			char last_char = single_input[strlen(single_input)-1];
			if (DEBUG) fprintf(stderr, "last_char: %c\n", last_char);
			if (last_char == '&' || last_char == '1' || last_char == '2')
				*strrchr(single_input, last_char) = 0;
		}

		if (DEBUG) fprintf(stderr, "new single_input:~|%s|~\n", single_input);

		//piping goes here
		/*
		int pipe_num;
		char *p = single_input;
		for (pipe_num = 0; p[pipe_num]; p[pipe_num]=='|' ? pipe_num++ : *p++);
		if (DEBUG) fprintf(stderr, "pipe_num: %d\n", pipe_num);
		*/

		args = get_args(single_input, &num_args);
		if (DEBUG) print_args(args, num_args);

		status = execute(args, &num_args);

		if (redirect & 0b001) restore_stdin(dup_stdin);
		if (redirect & 0b010) restore_stdout(dup_stdout);
		if (redirect & 0b100) restore_stderr(dup_stderr);

		free(args);
	}
	free(raw_input);
	if (DEBUG) fprintf(stderr, "status: %d\n", status);
	return status;
}

int main() {
	int status = 1;
	while (status) status = process();
	return 0;
}
