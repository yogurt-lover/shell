#ifndef _REDIRECTH_
#define _REDIRECTH_

#define STDIN_CHANGED 0b0001
#define STDOUT_CHANGED 0b0010
#define STDERR_CHANGED 0b0100
#define STD_FAILED 0b1000

/*======== void redirect_stdout() ==========
 * Inputs: int *redirect
           int *dup_stdout
 * Returns: none
 *
 * Duplicates and closes file descriptor 1 (STDOUT)
 * Sets dup_stdout to the duplicate of STDOUT
 * Sets the stdout flag in redirect to 1
 * ====================*/
void redirect_stdout(int *redirect, int *dup_stdout);

/*======== void redirect_stderr() ==========
 * Inputs: int *redirect
           int *dup_stderr
 * Returns: none
 *
 * Duplicates and closes file descriptor 2 (STDERR)
 * Sets dup_stderr to the duplicate of STDERR
 * Sets the stderr flag in redirect to 1
 * ====================*/
void redirect_stderr(int *redirect, int *dup_stderr);

/*======== void redirect_stdin() ==========
 * Inputs: int *redirect
           int *dup_stdin
 * Returns: none
 *
 * Duplicates and closes file descriptor 0 (STDIN)
 * Sets dup_stdin to the duplicate of STDIN
 * Sets the stdin flag in redirect to 1
 * ====================*/
void redirect_stdin(int *redirect, int *dup_stdin);

/*======== void change_stdout() ==========
 * Inputs: char *input
           int *redirect
           int *dup_stdout
           int *dup_stderr
 * Returns: none
 *
 * Redirects stdout and/or stderr to the file specified in the argument after the '>' character in the input string
 * Doesn't modify the input
 * Checks for the occurrence of ">>" in the input string so an append flag can be added
 * If an argument isn't found after the '>' characters, then the failed flag in redirect is set to 1
 * Otherwise, stdout and stderr will be changed using redirect_stdout and redirect_stderr depending on which
 * redirect characters were used (&>, 2>, >)
 * ====================*/
void change_stdout(char *input, int *redirect, int *dup_stdout, int *dup_stderr);

/*======== void change_stdin() ==========
 * Inputs: char *input
           int *redirect
           int *dup_stdin
 * Returns: none
 *
 * Redirects stdin to the file specified in the argument after the '<' character in the input string
 * Doesn't modify the input
 * If an argument isn't found after the '<' character, then the failed flag in redirect is set to 1
 * Otherwise, stdin will be changed using redirect_stdin
 * ====================*/
void change_stdin(char *input, int *redirect, int *dup_stdin);

/*======== void restore_stdin() ==========
 * Inputs: int dup_stdin
 * Returns: none
 *
 * Closes the current stdin file descriptor and replaces it with the duplicate file descriptor created in
 * redirect_stdin
 * It will only be called if the stdin flag in the redirect variable is one
 * ====================*/
void restore_stdin(int dup_stdin);

/*======== void restore_stdout() ==========
 * Inputs: int dup_stdout
 * Returns: none
 *
 * Closes the current stdout file descriptor and replaces it with the duplicate file descriptor created in
 * redirect_stdout
 * It will only be called if the stdout flag in the redirect variable is one
 * ====================*/
void restore_stdout(int dup_stdout);

/*======== void restore_stderr() ==========
 * Inputs: int dup_stderr
 * Returns: none
 *
 * Closes the current stderr file descriptor and replaces it with the duplicate file descriptor created in
 * redirect_stderr
 * It will only be called if the stderr flag in the redirect variable is one
 * ====================*/
void restore_stderr(int dup_stderr);
#endif
