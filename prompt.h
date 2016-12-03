#ifndef _PROMPTH_
#define _PROMPTH_

/*======== char *get_home_dir() ==========
 * Inputs: none
 * Returns: Home directory string
 *
 * Get environment variable "HOME"
 * ====================*/
char *get_home_dir();

/*======== char *get_username() =========
 * Inputs: none
 * Returns: User string
 * 
 * Get environment variable "USER"
 * ====================*/
char *get_username();

/*======== char *get_hostname() =========
 * Inputs: none
 * Returns: Hostname string
 * 
 * Dynamically allocates buffer with gethostname()
 * ====================*/
char *get_hostname();

/*======== char *get_cwd() =========
 * Inputs: none
 * Returns: CWD string
 * 
 * Dynamically allocates buffer with getcwd()
 * ====================*/
char *get_cwd();

/*======== void print_prompt() =========
 * Inputs: none
 * Returns: none
 * 
 * Prints shell prompt and frees cwd and hostname memory
 * ====================*/
void print_prompt();
#endif
