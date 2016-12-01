#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "prompt.h"

#define NRML "\x1B[0m"
#define CYAN "\x1B[36m"
#define BOLD "\x1B[1m"

char *get_home_dir() {
	return getenv("HOME");
}
char *get_username() {
	return getenv("USER");
}
char *get_hostname() {
	char *hostname = (char *)malloc(256*sizeof(char));
	hostname[255] = '\0';
	gethostname(hostname, 255);
	return hostname;
}
char *get_cwd() {
	char *cwd = (char *)malloc(256*sizeof(char));
	cwd[255] = '\0';
	getcwd(cwd, 255);
	return cwd;
}

void print_prompt() {
	char *cwd = get_cwd();
	char *username = get_username();
	char *hostname = get_hostname();
	char *home = get_home_dir();
	char *dir = cwd;
	char in_home_dir = 0;
	if (strstr(cwd, home)) {
		dir += strlen(home);
		in_home_dir = '~';
	}
	printf("%s%s%s@%s:%c%s%s\n$ ", CYAN, BOLD, username, hostname, in_home_dir, dir, NRML );
	free(cwd);
	free(hostname);
}
