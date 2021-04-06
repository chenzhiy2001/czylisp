#include <stdio.h>
#include<stdlib.h>
#include"mpc.h"

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

long eval_op(long x, char *op, long y){//TODO 适配多个字符的op的情况
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  return 114514;
}
long eval(mpc_ast_t *t){
  if(strstr(t->tag, "number")){ //TODO 用专用的tag查找方法来提升性能
    return atoi(t->contents);
  }

  char *op = t->children[1]->contents;

  long x = eval(t->children[2]);

  int i = 3;
  while(strstr(t->children[i]->tag,"expr")){
    x = eval_op(x,op,eval(t->children[i]));
    i++;
  }//递归函数里面用循环，非常邪恶
  return x;
}
//static char input[2048]; 
int main(int argc, char **argv)
{
  // create some parsers
  mpc_parser_t* number = mpc_new("number");
  mpc_parser_t* operator = mpc_new("operator");
  mpc_parser_t* expr = mpc_new("expr");
  mpc_parser_t* czylisp = mpc_new("czylisp");
  mpc_parser_t* blyat = mpc_new("blyat");

  mpca_lang(MPCA_LANG_DEFAULT,
  "\
    number:/-?[0-9]+/;\
    operator:'+'|'-'|'*'|'/';\
    expr:<number>|'('<operator><expr>+')'|<blyat>;\
    czylisp:/^/<operator><expr>+/$/;\
    blyat:/blyat/;\
  ",
  number, operator, expr, czylisp, blyat);
  puts("czylisp Version June 6th 2021");
  puts("Press ctrl+c to Exit");
  while (1)
  {
    char *input = readline("czylisp> ");
    add_history(input);
//    printf("No you're a %s\n", input);
    mpc_result_t r;

    if(mpc_parse("<stdin>",input,czylisp,&r)){
//      mpc_ast_print(r.output);
      long result = eval(r.output);
      printf("%li\n",result);
      mpc_ast_delete(r.output);
    }else{
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    free(input);
  }
  mpc_cleanup(5, number, operator, expr, blyat, czylisp);
   return 0;
} 
