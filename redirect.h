#ifndef _REDIRECTH_
#define _REDIRECTH_
void redirect_stdout(int *redirect, int *dup_stdout);
void redirect_stderr(int *redirect, int *dup_stderr);
void redirect_stdin(int *redirect, int *dup_stdin);
void change_stdout(char *input, int *redirect, int *dup_stdout, int *dup_stderr);
void change_stdin(char *input, int *redirect, int *dup_stdin);
void restore_stdin(int dup_stdin);
void restore_stdout(int dup_stdout);
void restore_stderr(int dup_stderr);
#endif
