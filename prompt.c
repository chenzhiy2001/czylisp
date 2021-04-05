#include <stdio.h>
#include<stdlib.h>

#ifdef _WIN32 //如果是windows系统，直接用一般的fgets就行了
#include<string.h>

static char buffer[2048];

// Fake readline function
char *readline(char *prompt){
  fputs(prompt,stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1]='\0';
  return cpy;
}
// fake add histtory
void add_history(Char* unused){};

#else
#ifdef __linux__
#include<editline/readline.h>
#include<editline/history.h>
#endif

#ifdef __MACH__
#include<editline/readline.h>
#endif
#endif

//static char input[2048]; 
int main(int argc, char **argv)
{
  puts("czylisp Version June");
  puts("Press Ctrl+c to Exit");
  while (1)
  {
    char *input = readline("czylisp> ");
    add_history(input);
    printf("No you're a %s\n", input);
    free(input);
  }
  return 0;
}
