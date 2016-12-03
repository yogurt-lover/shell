#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/wait.h>
#include <sys/types.h>

#include "prompt.h"
#include "parse.h"
#include "redirect.h"
#include "debug.h"
#include "pipes.h"
#include "exec.h"

// signals (Ctrl-C)
// resizing buffers
// error handling
// previous command
// config + builtins

/*======== int print_args() ==========
 * Inputs: char **args
 		   int num_args
 * Returns: none
 *
 * Debug method. Prints all parsed arguments placed in args array.
 * ====================*/
void print_args(char **args, int num_args) {
	fprintf(stderr, "ARGUMENTS:<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	int k = 0;
	for (; k < num_args; k++) fprintf(stderr, "arg: ~|%s|~\n", args[k]);
	fprintf(stderr, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}

/*======== void process() ==========
 * Inputs: none
 * Returns: none
 *
 * Main processing method. Begins by printing the Shell prompt.
   awaits user input, and runs read_raw() on it.
 * The sequence of parsing and execution goes:
 * * Separation of raw_input using ';' as a delimiter (each string being a single_input)
 * * Set file redirection ('<', '>') and remove the arguments that come along them from single_input
 * * Calculating number of pipes in the single_input
 * * If there aren't any pipes, then exec_single (in exec.c) will be called
 * * Otherwise, single_input will be separated using '|' as a delimiter
 * * exec_pipe (in pipes.c) will be called on each substring
 * * If file redirection has occurred (checked in the redirect variable), then they will be restored to the 
 *   normal stdin, stdout, and/or sterr file descriptors
 * * If exit was called without any pipes, status will have been sest to zero, and the program will exit
 * * Otherwise, the process() loop called in the main function will keep going.
 * ====================*/
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

/*======== int main() ==========
 * Inputs: none
 * Returns: 0.
 *
 * Runs main processing method, process().
 * ====================*/
int main() {
	while (1) process();
	return 0;
}
