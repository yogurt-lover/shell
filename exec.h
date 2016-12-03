#ifndef _EXECH_
#define _EXECH_
/*======== void cd_def() ==========
Inputs: char **args
Returns: none

Changes directory of current process
If unsuccessful, print error message
If no arguments after "cd", then goes to home directory
 * ====================*/
void cd_def(char **args);

/*======== void exec_coreutil() ==========
Inputs: char **args
Returns: none

Executes command in $PATH
If execution fails, print error message
 * ====================*/
void exec_coreutil(char **args);

/*======== void exec_coreutil() ==========
Inputs: char **args, int *status
Returns: none

Tests for special commands such as "cd" or "exit" and runs cd_def
and sets exit status respectively.
Forks, and runs exec_coreutil for all else.
 * ====================*/
void exec_single(char **args, int *status);
#endif
