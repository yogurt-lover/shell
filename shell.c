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
// multiple <, >
// no need for new_std*?


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
	hostname[256] = '\0';
	gethostname(hostname, 255);
	return hostname;
}

char *get_cwd() {
	char *cwd = (char *)malloc(256*sizeof(char));
	cwd[256] = '\0';
	getcwd(cwd, 255);
	return cwd;
}

void print_prompt() {
	char *cwd = get_cwd();
	char *username = get_username();
	char *hostname = get_hostname();
	char *home = get_home_dir();
	char *dir = strstr(cwd, home) ? cwd+strlen(home) : cwd;
	printf("%s%s%s@%s:~%s%s\n$ ", CYAN, BOLD, username, hostname, dir, NRML );
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
	if (c == EOF) fprintf(stderr, "\n");
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

int change_stdout(char *input, int *redirect, int *dup_stdout, int *new_stdout) {
	char *new_stdout_path;
	int append_flag = strstr(input, ">>") ? O_APPEND : O_TRUNC;

	char *input_dup = strdup(input);
	strsep(&input_dup, ">");
	new_stdout_path = strtok(input_dup, " \n\r\t");

	if (DEBUG) fprintf(stderr, "new_stdout_path: ~|%s|~\n", new_stdout_path);
	if (new_stdout_path) {
		*redirect = *redirect | 0b01;
		*dup_stdout = dup(1);
		close(1);
		*new_stdout = open(new_stdout_path, O_WRONLY | append_flag | O_CREAT , 0644);
	}
	else {
		fprintf(stderr, "shell: OUT file not specified\n");
		return 0;
	}
	if (DEBUG) fprintf(stderr, "redirect after >: %d\n", *redirect);
	return 1;
}

int change_stdin(char *input, int *redirect, int *dup_stdin, int *new_stdin) {
	char *new_stdin_path;

	char *input_dup = strdup(input);
	strsep(&input_dup, "<");
	new_stdin_path = strtok(input_dup, " \n\r\t");

	if (DEBUG) fprintf(stderr, "new_stdin_path: ~|%s|~\n", new_stdin_path);
	if (new_stdin_path) {
		*redirect = *redirect | 0b10;
		*dup_stdin = dup(0);
		close(0);
		*new_stdin = open(new_stdin_path, O_RDONLY);
	}
	else {
		fprintf(stderr, "shell: IN file not specified\n");
		return 0;
	}
	if (DEBUG) fprintf(stderr, "redirect after <: %d\n", *redirect);
	return 1;
}

void restore_stdout(int dup_stdout, int new_stdout) {
	close(new_stdout);
	dup2(dup_stdout, 1);
	close(dup_stdout);
}

void restore_stdin(int dup_stdin, int new_stdin) {
	close(new_stdin);
	dup2(dup_stdin, 0);
	close(dup_stdin);
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
		int redirect, dup_stdout, new_stdout, dup_stdin, new_stdin;
		char *single_input;
		char **args;

		redirect = 0;

		single_input = strsep(&s, ";");
		if (DEBUG) fprintf(stderr, "single_input:~|%s|~\n", single_input);

		if (!single_input || !strlen(single_input)) return 1;

		if (strchr(single_input, '>'))
			if (!change_stdout(single_input, &redirect, &dup_stdout, &new_stdout)) return 1;

		if (strchr(single_input, '<'))
			if (!change_stdin(single_input, &redirect, &dup_stdin, &new_stdin)) return 1;

		single_input = strsep(&single_input, "><");
		if (DEBUG) fprintf(stderr, "new single_input:~|%s|~\n", single_input);

		//piping goes here
		int pipe_num;
		char *p = single_input;
		for (pipe_num = 0; p[pipe_num]; p[pipe_num]=='|' ? pipe_num++ : *p++);
		fprintf(stderr, "pipe_num: %d\n", pipe_num);

		args = get_args(single_input, &num_args);
		if (DEBUG) print_args(args, num_args);

		status = execute(args, &num_args);

		if (redirect & 0b01) restore_stdout(dup_stdout, new_stdout);
		if (redirect & 0b10) restore_stdin(dup_stdin, new_stdin);

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
