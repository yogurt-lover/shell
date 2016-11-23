#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define NRML "\x1B[0m"
#define CYAN "\x1B[36m"
#define BOLD "\x1B[1m"

// signals
// tab bug (due to prompt)
// resizing buffers
// redirection < | >

void print_prompt() {
  char prompt[256];
  getcwd(prompt, 256);
  printf("%s%s%s%s$ ", CYAN, BOLD, prompt, NRML);
}

char *read_raw() {
  char *buff = (char *)malloc(256*sizeof(char));

  /*
  char c;
  int counter = 0;

  c = getchar();
  for (; c != '\n' && c != EOF; counter++) {
    buff[counter] = c;
    c = getchar();
  }
  buff[counter] = '\0';
  return buff;
  */
  
  fgets(buff, 256, stdin);
  return buff;
}

char **get_args(char *input, int *num_inputs) {
  char **args = (char **)malloc(256*sizeof(char*));
  char *arg = strtok(input, " \n\r\t");

  int counter;
  for (counter = 0; arg; counter++) {
    args[counter] = arg;
    //printf("~~~%s~~~\n", args[counter]);
    arg = strtok(NULL, " \n\r\t");
  }
  args[counter] = arg;
  *num_inputs = counter;

  return args;
}

int execute(char **args) {
  // cd (might put this someplace else)
  if (!strcmp(args[0], "cd")) {
    if (args[1]) {
      if (chdir(args[1])) {
	printf("%s\n", strerror(errno));
      }
    }
    else {
      chdir("/\n"); //change this to home directory 
    }
    return 1;
  }
  
  // exit (this too)
  if (!strcmp(args[0], "exit")) {
    return 0;
  }
  
  // system binaries
  int f = fork();
  if (f == 0) {
    if (execvp(args[0], args) == -1) {
      printf("%s: command not found\n", args[0]);
    }
  }
  else {
    wait(NULL);
  }
  return 1;
}

int process() {
  int status;
  char *raw_input;
  
  print_prompt(); 
  raw_input = read_raw();
  //printf("raw: ~~~%s~~~\n", raw_input);
  
  char *s = raw_input;
  while (s) {
    int num_inputs;
    char *single_input;
    char **args;

    single_input = strsep(&s, ";");
    args = get_args(single_input, &num_inputs);

    /*
    printf("----------------------------\n");
    printf("ARGUMENTS\n");
    int k = 0;
    for (; k < num_inputs; k++) {
      printf("arg: ~~~%s~~~\n", args[k]);
    }
    printf("----------------------------\n");
    */
    
    status = execute(args);
    free(args);
  }
  free(raw_input);
  return status;
}

int main() {
  int status = 1;
  while (status) {
    status = process();
    //printf("status: %d\n", status);
  }
  return 0;
}
  
