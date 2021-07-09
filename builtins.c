#include "lispy.h"

lval* builtin_lambda(lenv* e, lval* a)
{
  /* Check there are two elements, both of which are Q Expressions */
  LASSERT_NUM("\\", a, 2);
  LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
  LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);
  
    /* Check first Q-Expression contains only Symbols */
  for (int i = 0; i < a->cell[0]->count; i++) 
  {
    LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
      "Cannot define non-symbol. Got %s, Expected %s.",
      ltype_name(a->cell[0]->cell[i]->type),ltype_name(LVAL_SYM));
  }
  
  /* Pop first two elements and create a lambda value from it */
  lval* formals = lval_pop(a, 0);
  lval* body    = lval_pop(a, 0);
  lval_del(a);
  
  return lval_lambda(formals, body);
}

lval* builtin_list(lenv* e, lval* a)
{
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_head(lenv* e, lval* a)
{
  LASSERT_NUM("head", a, 1);
  LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("head", a, 0);
  
  lval* v = lval_take(a, 0);
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lenv* e, lval* a)
{
  LASSERT_NUM("tail", a, 1);
  LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("tail", a, 0);
  
  lval* v = lval_take(a, 0);
  lval_del(lval_pop(v, 0));
  lval_del(a);
  return v;
}

lval* builtin_eval(lenv* e, lval* a)
{
  LASSERT_NUM("eval", a, 1);
  LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);
  
  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  lval_del(a);
  
  return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a)
{
  for (int i = 0; i < a->count; i++) 
  {
    LASSERT_TYPE("join", a, i, LVAL_QEXPR);
  }
  
  lval* x = lval_pop(a, 0);
  
  for(int i = 0;i < a->count; i++)
  {
    lval* y = lval_pop(a, 0);
    lval_join(x, y);
  }
  
  lval_del(a);
  return x;
}

lval* builtin_op(lenv* e, lval* a, char* op)
{
  for(int i = 0; i < a->count; i++)
  {
    LASSERT_TYPE(op, a, i, LVAL_NUM);
  }
  
  lval* x = lval_pop(a, 0);
  
  if(strcmp(op, "-") == 0 && a->count == 1) 
  {
    x->num = -x->num; 
  }
  
  while(a->count > 0)
  {
    lval *y = lval_pop(a, 0);
    if (strcmp(op, "+") == 0) { x->num += y->num; }
    if (strcmp(op, "-") == 0) { x->num -= y->num; }
    if (strcmp(op, "*") == 0) { x->num *= y->num; }
    if (strcmp(op, "%") == 0) { x->num %= y->num; }
    if (strcmp(op, "/") == 0) {
      if (y->num == 0) {
        lval_del(x); lval_del(y);
        x = lval_err("Division By Zero.");
        break;
      }
      x->num /= y->num;
    }
    
    lval_del(y);
  }
  
  lval_del(a);
  return x;
}

lval* builtin_var(lenv* e, lval* a, char* func)
{
  LASSERT_TYPE(func, a, 0, LVAL_QEXPR);
  
  /* Get the first expression and check that it does not contain symbols */
  lval* syms = a->cell[0];
  for (int i = 0; i < syms->count; i++) {
    LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
      "Function '%s' cannot define non-symbol. "
      "Got %s, Expected %s.", func, 
      ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
  }
  
  /* Check that the values passed are exactly equal in number to the variable names */
  LASSERT(a, (syms->count == a->count-1),
    "Function '%s' passed too many arguments for symbols. "
    "Got %i, Expected %i.", func, syms->count, a->count-1);
    
  for (int i = 0; i < syms->count; i++) {
    /* If 'def' define in globally. If 'put' define in locally */
    if (strcmp(func, "def") == 0) {
      lenv_def(e, syms->cell[i], a->cell[i+1]);
    }
    
    if (strcmp(func, "=")   == 0) {
      lenv_put(e, syms->cell[i], a->cell[i+1]);
    } 
  }
  
  lval_del(a);
  return lval_sexpr();
}

lval* builtin_ord(lenv* e, lval* a, char* op)
{
  LASSERT_NUM(op, a, 2);
  LASSERT_TYPE(op, a, 0, LVAL_NUM);
  LASSERT_TYPE(op, a, 1, LVAL_NUM);
  
  int r;
  if (strcmp(op, ">")  == 0) {
    r = (a->cell[0]->num >  a->cell[1]->num);
  }
  if (strcmp(op, "<")  == 0) {
    r = (a->cell[0]->num <  a->cell[1]->num);
  }
  if (strcmp(op, ">=") == 0) {
    r = (a->cell[0]->num >= a->cell[1]->num);
  }
  if (strcmp(op, "<=") == 0) {
    r = (a->cell[0]->num <= a->cell[1]->num);
  }
  lval_del(a);
  return lval_num(r);
}

int lval_eq(lval* x, lval* y)
{
  if(x->type != y->type) return 0;
  
  switch(x->type)
  {
    case LVAL_NUM   : 
      return x->num == y->num;
    case LVAL_ERR   :
      return strcmp(x->err, y->err) == 0;
    case LVAL_SYM   :
      return strcmp(x->err, y->err) == 0;
    case LVAL_STR: 
      return (strcmp(x->str, y->str) == 0);
    case LVAL_FUN   :
      if(x->builtin || y->builtin) return x->builtin == y->builtin;
      
      return lval_eq(x->formals, y->formals) && lval_eq(x->body, y->body);
    case LVAL_QEXPR :
    case LVAL_SEXPR :
      if (x->count != y->count) { return 0; }
      for(int i = 0; i < x->count; i++)
      {
         if (!lval_eq(x->cell[i], y->cell[i])) { return 0; }
      }
      return 1;
  }
  
  return 0;
}

lval* builtin_cmp(lenv* e, lval* a, char* op)
{
  LASSERT_NUM(op, a, 2);
  int r = 0;
  if(strcmp(op, "==") == 0)
    r = lval_eq(a->cell[0], a->cell[1]);
  else if (strcmp(op, "!=") == 0)
    r = !lval_eq(a->cell[0], a->cell[1]);
    
  lval_del(a);
  return lval_num(r);
}

lval* builtin_if(lenv* e, lval* a)
{
  LASSERT_NUM("if", a, 3);
  LASSERT_TYPE("if", a, 0, LVAL_NUM);
  LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
  LASSERT_TYPE("if", a, 2, LVAL_QEXPR);
  
  lval* x = NULL;
  a->cell[1]->type = LVAL_SEXPR;
  a->cell[2]->type = LVAL_SEXPR;
  
  if(a->cell[0]->num)
  {
    x = lval_eval(e, lval_pop(a, 1));
  }
  else
  {
    x = lval_eval(e, lval_pop(a, 2));
  }
  lval_del(a);
  return x;
}

lval* builtin_load(lenv* e, lval* a) {
  LASSERT_NUM("load", a, 1);
  LASSERT_TYPE("load", a, 0, LVAL_STR);
  
  mpc_result_t r;
  if (mpc_parse_contents(a->cell[0]->str, Lispy, &r)) {
    lval* expr = lval_read(r.output);
    mpc_ast_delete(r.output);

    while (expr->count) {
      lval* v = lval_pop(expr, 0);
      lval* x = lval_eval(e, v);
      lval_println(v);
      if (x->type == LVAL_ERR) { lval_println(x); }
      lval_del(x);
    }
    
    lval_del(expr);    
    lval_del(a);
    return lval_sexpr();
    
  } else {
    char* err_msg = mpc_err_string(r.error);
    mpc_err_delete(r.error);
    
    lval* err = lval_err("Could not load Library %s", err_msg);
    free(err_msg);
    lval_del(a);
    return err;
  }
}

lval* builtin_print(lenv* e, lval* a) 
{
  for (int i = 0; i < a->count; i++) 
  {
    lval_print(a->cell[i]); putchar(' ');
  }
  putchar('\n');
  lval_del(a);

  return lval_sexpr();
}

lval* builtin_error(lenv* e, lval* a) 
{
  LASSERT_NUM("error", a, 1);
  LASSERT_TYPE("error", a, 0, LVAL_STR);
  
  lval* err = lval_err(a->cell[0]->str);
  lval_del(a);
  return err;
}

lval* builtin_def(lenv* e, lval* a) { return builtin_var(e, a, "def"); }
lval* builtin_put(lenv* e, lval* a) { return builtin_var(e, a, "="); }
lval* builtin_add(lenv* e, lval* a) { return builtin_op(e, a, "+"); }
lval* builtin_sub(lenv* e, lval* a) { return builtin_op(e, a, "-"); }
lval* builtin_mul(lenv* e, lval* a) { return builtin_op(e, a, "*"); }
lval* builtin_div(lenv* e, lval* a) { return builtin_op(e, a, "/"); }
lval* builtin_mod(lenv* e, lval* a) { return builtin_op(e, a, "%"); }
lval* builtin_gt(lenv* e, lval* a)  { return builtin_ord(e, a, ">"); }
lval* builtin_lt(lenv* e, lval* a)  { return builtin_ord(e, a, "<"); }
lval* builtin_ge(lenv* e, lval* a)  { return builtin_ord(e, a, ">="); }
lval* builtin_le(lenv* e, lval* a)  { return builtin_ord(e, a, "<="); }
lval* builtin_eq(lenv* e, lval* a)  { return builtin_cmp(e, a, "=="); }
lval* builtin_ne(lenv* e, lval* a)  { return builtin_cmp(e, a, "!="); }

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) 
{
  lval* k = lval_sym(name);
  lval* v = lval_builtin(func);
  lenv_put(e, k, v);
  lval_del(k); lval_del(v);
}

void lenv_add_builtins(lenv* e) 
{
  /* Variable Functions */
  lenv_add_builtin(e, "\\",  builtin_lambda); 
  lenv_add_builtin(e, "def", builtin_def);
  lenv_add_builtin(e, "=",   builtin_put);
  /* List Functions */
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "join", builtin_join); 
  /* Mathematical Functions */
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);
  lenv_add_builtin(e, "%", builtin_mod);
  /* Conditional Evaluation */
  lenv_add_builtin(e, "if", builtin_if);
  lenv_add_builtin(e, "==", builtin_eq);
  lenv_add_builtin(e, "!=", builtin_ne);
  lenv_add_builtin(e, ">",  builtin_gt);
  lenv_add_builtin(e, "<",  builtin_lt);
  lenv_add_builtin(e, ">=", builtin_ge);
  lenv_add_builtin(e, "<=", builtin_le);
  /* String Functions */
  lenv_add_builtin(e, "load", builtin_load);
  lenv_add_builtin(e, "error", builtin_error);
  lenv_add_builtin(e, "print", builtin_print);
}
