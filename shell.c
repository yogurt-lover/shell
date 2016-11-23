#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define NRML "\x1B[0m"
#define CYAN "\x1B[36m"
#define BOLD "\x1B[1m"

void process() {

  //prompt
  char prompt[500];
  getcwd(prompt, sizeof(prompt));
  printf("%s%s%s%s$ ", CYAN, BOLD, prompt, NRML);
  
  char raw_input[100];
  fgets(raw_input, sizeof(raw_input), stdin);

  char *s1 = raw_input;
  while (s1) {
    char *single_input = strsep(&s1, ";");
    
    char *args[100];
    char *pch = strtok(single_input, " \n\r\t");

    // New version separates on any whitespace
    // Fills args[] with the arguments from single_input
    int i;
    for (i = 0; pch; i++) {
      args[i] = pch;
      pch = strtok(NULL, " \n\r\t");
    }
    args[i] = pch;

    // Print args for debugging
    printf("----------------------------\n");
    printf("ARGUMENTS\n");
    int k = 0;
    for (; k < i; k++) {
      printf("arg: @%s@\n", args[k]);
    }
    printf("----------------------------\n");


    // Execute the command
    int f = fork();
    if (f == 0) {

      // cd
      if (!strcmp(args[0], "cd")) {
	if (args[1]) {
	  if (chdir(args[1])) {
	    printf("%s\n", strerror(errno));
	  }
	}
	else {
	  printf("This should go to the HOME path, but it hasn't been implemented yet :(\n");
	}
      }

      // system binaries
      else if (execvp(args[0], args) == -1) {
	printf("%s: command not found\n", args[0]);
      }
    }
    else {
      wait(NULL);
    }
    
  }
}

int main() {
  while (1) {
    process();
  }
  return 0;
}
  
