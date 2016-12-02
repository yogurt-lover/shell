#ifndef _EXECH_
#define _EXECH_
/* void cd_def(char **args)
Inputs: char **args
Returns: none

Changes directory of current process
If unsuccessful, print error message
If no arguments after "cd", then goes to home directory
*/

void cd_def(char **args);

/* void exec_coreutil(char **args)
Inputs: char **args
Returns: none

Executes command in $PATH
If execution fails, print error message
*/
void exec_coreutil(char **args);
/* void exec_single(char **args, int *status)
Inputs: char **args, int *status
Returns: none


*/
void exec_single(char **args, int *status);
#endif
