#ifndef LISPY_H
  #define LISPY_H
  
  #include "mpc.h"
  
  struct lval;
  struct lenv;
  
  typedef struct lval lval;
  typedef struct lenv lenv;
  
  enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR, LVAL_STR };
  
  /* Parsers */
  mpc_parser_t* Number;
  mpc_parser_t* String;
  mpc_parser_t* Comment;
  mpc_parser_t* Symbol;
  mpc_parser_t* Sexpr;
  mpc_parser_t* Qexpr;
  mpc_parser_t* Expr;
  mpc_parser_t* Lispy;
  
  typedef lval*(*lbuiltin)(lenv*, lval*);
  
  /* struct to represent all lispy value types */
  struct lval 
  {
    int type;
    
    /* Identity */
    long num;
    char *err;
    char *sym;
    char *str; 
    
    /* Function */
    lbuiltin builtin;
    lenv* env;
    lval* formals;
    lval* body;
    
    /* Expression */
    int count;
    lval** cell;
  };
  
  /* struct to represent lispy environment */
  struct lenv 
  {
    lenv* par;
    int count;
    char** syms;
    lval** vals;
  };

  /* function prototypes */
  
  lval* lval_read(mpc_ast_t* t);
  
  /* lval constructors */
  lval* lval_num(long x);
  lval* lval_err(char* fmt, ...);
  lval* lval_sym(char* s);
  lval* lval_str(char* s);
  lval* lval_builtin(lbuiltin func);
  lval* lval_lambda(lval* formals, lval* body);
  lval* lval_sexpr(void);
  lval* lval_qexpr(void);
  void lval_del(lval* v);
  lval* lval_copy(lval* v);
  lval* lval_add(lval* v, lval* x);
  lval* lval_join(lval* x, lval* y);
  lval* lval_pop(lval* v, int i);
  lval* lval_take(lval* v, int i);
  
  /* lval print */
  void lval_print(lval *v);
  void lval_println(lval *v);
  void lval_print_str(lval *v);
  void lval_print_expr(lval* v, char open, char close);
  
  /* lenv constructors */
  lenv* lenv_new(void);
  void lenv_del(lenv* e);
  lenv* lenv_copy(lenv* e);
  
  char* ltype_name(int t);
  
  /* builtin operations */
  lval* builtin_gt(lenv* e, lval* a);
  lval* builtin_lt(lenv* e, lval* a);
  lval* builtin_ge(lenv* e, lval* a);
  lval* builtin_le(lenv* e, lval* a);
  lval* builtin_eq(lenv* e, lval* a);
  lval* builtin_ne(lenv* e, lval* a);
  lval* builtin_if(lenv* e, lval* a);
  lval* builtin_load(lenv* e, lval* a);
  lval* builtin_print(lenv* e, lval* a);
  lval* builtin_error(lenv* e, lval* a);
  lval* builtin_ord(lenv* e, lval* a, char* op);
  lval* builtin_cmp(lenv* e, lval* a, char* op);
  
  int lval_eq(lval* x, lval* y);
  
  #define LASSERT(args, cond, fmt, ...) \
    if (!(cond)) { lval* err = lval_err(fmt, ##__VA_ARGS__); lval_del(args); return err; }
  
  #define LASSERT_TYPE(func, args, index, expect) \
    LASSERT(args, args->cell[index]->type == expect, \
      "Function '%s' passed incorrect type for argument %i. " \
      "Got %s, Expected %s.", \
      func, index, ltype_name(args->cell[index]->type), ltype_name(expect))
  
  #define LASSERT_NUM(func, args, num) \
    LASSERT(args, args->count == num, \
      "Function '%s' passed incorrect number of arguments. " \
      "Got %i, Expected %i.", \
      func, args->count, num)
  
  #define LASSERT_NOT_EMPTY(func, args, index) \
    LASSERT(args, args->cell[index]->count != 0, \
      "Function '%s' passed {} for argument %i.", func, index);
#endif
