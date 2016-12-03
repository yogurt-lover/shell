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
 *
 * ====================*/
void redirect_stdout(int *redirect, int *dup_stdout);

/*======== void redirect_stderrt() ==========
 * Inputs: int *redirect
           int *dup_stderr
 * Returns: none
 *
 *
 * ====================*/
void redirect_stderr(int *redirect, int *dup_stderr);

/*======== void redirect_stdin() ==========
 * Inputs: int *redirect
           int *dup_stdin
 * Returns: none
 *
 *
 * ====================*/
void redirect_stdin(int *redirect, int *dup_stdin);

/*======== void change_stdout() ==========
 * Inputs: char *input
           int *redirect
           int *dup_stdout
           int *dup_stderr
 * Returns: none
 *
 *
 * ====================*/
void change_stdout(char *input, int *redirect, int *dup_stdout, int *dup_stderr);

/*======== void change_stdin() ==========
 * Inputs: char *input
           int *redirect
           int *dup_stdin
 * Returns: none
 *
 *
 * ====================*/
void change_stdin(char *input, int *redirect, int *dup_stdin);

/*======== void restore_stdin() ==========
 * Inputs: int dup_stdin
 * Returns: none
 *
 *
 * ====================*/
void restore_stdin(int dup_stdin);

/*======== void restore_stdout() ==========
 * Inputs: int dup_stdout
 * Returns: none
 *
 *
 * ====================*/
void restore_stdout(int dup_stdout);

/*======== void restore_stderr() ==========
 * Inputs: int dup_stderr
 * Returns: none
 *
 *
 * ====================*/
void restore_stderr(int dup_stderr);
#endif
