#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define PROMPT "$ "


void process() {

  char raw_input[100];
  printf(PROMPT);
  fgets(raw_input, sizeof(raw_input), stdin);

  char *s1 = raw_input;
  while (s1) {
    char *single_input = strsep(&s1, ";");
    
    char *args[100];
    char *pch = strtok(single_input, " \n\r\t");

    /* Old version
    int i = 0;
    while (s2) {
      char *arg = strsep(&s2, " ");
      if (strlen(arg)) {
	args[i] = arg;
	i++;
      }
    }
    args[i] = 0;
    */

    // New version separates on any whitespace
    // Fills args[] with the arguments from single_input
    int i;
    for (i = 0; pch; i++) {
      args[i] = pch;
      pch = strtok(NULL, " \n\r\t");
    }
    args[i] = pch;

    /* Print first 5 args
    int k = 0;
    for (; k< 5; k++) {
      printf("arg: \"%s\"\n", args[i]);
    }
    */

    // Execute the command
    int f = fork();
    if (f == 0) {
      if (execvp(args[0], args) == -1) {
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
  
