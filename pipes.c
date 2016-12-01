#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "pipes.h"
#include "exec.h"

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
		fprintf(stderr, "shell: cannot pipe through a shell built-in\n");
		return 0;
	}
	pipe(pipes[command_num % 2]);
	pipe_handler(args, command_num, pipe_num, pipes);
	return 1;
}
