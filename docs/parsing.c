/// 这个是期中答辩时演示的代码
/* Makefile 
# FILENAME = parsing

# build:
# 	@cc -std=c99 -Wall $(FILENAME).c mpc.c -ledit -lm -o $(FILENAME)
# build-windows:
# 	@cc -std=c99 -Wall $(FILENAME).c mpc.c -o $(FILENAME)


# run:build
# 	@./$(FILENAME)
*/
#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32 //如果是windows系统，直接用一般的fgets就行了
#include <string.h>

static char buffer[2048];

// Fake readline function
char *readline(char *prompt)
{
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char *cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}
// fake add histtory
void add_history(Char *unused){};

#else
#ifdef __linux__
#include <editline/readline.h>
#include <editline/history.h>
#endif

#ifdef __MACH__
#include <editline/readline.h>
#endif
#endif

enum
{
  LVAL_NUM,
  LVAL_ERR
};
enum
{
  LERR_DIV_ZERO,
  LERR_BAD_OP,
  LERR_BAD_NUM
};

typedef struct
{
  int type;
  long num;
  int err;
  /* data */
} lval;

lval lval_num(long x)
{
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v; //TODO 赋值v.err
}
lval lval_err(int x)
{
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

void lval_print(lval v)
{
  switch (v.type)
  {
  case LVAL_NUM:
    printf("%li", v.num);
    break;
  case LVAL_ERR:
    if (v.err == LERR_DIV_ZERO)
    {
      printf("Error：Division By Zero!");
    }
    if (v.err == LERR_BAD_OP)
    {
      printf("Errrr: Invalid Operator!");
    }
    if (v.err == LERR_BAD_NUM)
    {
      printf("Errrrr: Invalid Number!");
    }
    break;
  } //TODO default?
}

void lval_println(lval v)
{
  lval_print(v);
  putchar('\n');
}

lval eval_op(lval x, char *op, lval y)
{ //TODO 适配多个字符的op的情况
  if (x.type == LVAL_ERR)
  {
    return x;
  }
  if (y.type == LVAL_ERR)
  {
    return y;
  }

  if (strcmp(op, "+") == 0)
  {
    return lval_num(x.num + y.num);
  }
  if (strcmp(op, "-") == 0)
  {
    return lval_num(x.num - y.num);
  }
  if (strcmp(op, "*") == 0)
  {
    return lval_num(x.num * y.num);
  }
  if (strcmp(op, "/") == 0)
  {

    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
  }
  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t *t)
{
  if (strstr(t->tag, "number"))
  { //TODO 用专用的tag查找方法来提升性能
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno!=ERANGE?lval_num(x):lval_err(LERR_BAD_NUM);
  }

  char *op = t->children[1]->contents;

  lval x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr"))
  {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  } //递归函数里面用循环，非常邪恶
  return x;
}
//static char input[2048];
int main(int argc, char **argv)
{
  // create some parsers
  mpc_parser_t *number = mpc_new("number");
  mpc_parser_t *operator= mpc_new("operator");
  mpc_parser_t *expr = mpc_new("expr");
  mpc_parser_t *czylisp = mpc_new("czylisp");
  mpc_parser_t *blyat = mpc_new("blyat");

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

    if (mpc_parse("<stdin>", input, czylisp, &r))
    {
      //      mpc_ast_print(r.output);
      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    }
    else
    {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    free(input);
  }
  mpc_cleanup(5, number, operator, expr, blyat, czylisp);
  return 0;
}
