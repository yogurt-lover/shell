#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include "redirect.h"

#define STDIN_CHANGED 0b0001
#define STDOUT_CHANGED 0b0010
#define STDERR_CHANGED 0b0100
#define STD_FAILED 0b1000

// DEBUG VAR DEAL WITH

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

