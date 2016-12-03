#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "exec.h"
#include "prompt.h"
#include "shell_name.h"

void cd_def(char **args) {
	if (args[1]) {
		int chdir_ret = chdir(args[1]);
		if (chdir_ret == -1) perror(""SHELL);
	}
	else chdir(get_home_dir());
}

void exec_coreutil(char **args) {
	if (execvp(args[0], args) == -1)
		fprintf(stderr, "%s: %s: command not found in PATH\n", SHELL, args[0]);
}

void exec_single(char **args, int *status) {
	if (!strcmp(args[0], "cd")) cd_def(args);
	else if (!strcmp(args[0], "exit")) *status = 0;
	else {
		int f = fork();
		if (f == 0) exec_coreutil(args);
	}
}

