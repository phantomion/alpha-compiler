#ifndef MANAGER_H
#define MANAGER_H

#include "utilities.h"
#include "parser.h"
#include "icode.h"

extern unsigned int curr_quad;

void manage_lvalue(expr* id);
expr* manage_global_var(char* id);
expr* manage_var(char *id);
expr* manage_local_var(char *id);
void manage_function(char *id);
void manage_anonymous_function();
void manage_function_exit();
expr* manage_add(expr* arg1, expr* arg2);
expr* manage_sub(expr* arg1, expr* arg2);
expr* manage_mul(expr* arg1, expr* arg2);
expr* manage_div(expr* arg1, expr* arg2);
expr* manage_mod(expr* arg1, expr* arg2);
expr* manage_uminus(expr* ex);
expr* manage_not(expr* ex);
expr* manage_pre_inc(expr* ex);
expr* manage_post_inc(expr* ex);
expr* manage_pre_dec(expr* ex);
expr* manage_post_dec(expr* ex);
expr* manage_args(char *id);
expr* manage_real(double val);
expr* manage_bool(short int val);
expr* manage_nil();
expr* manage_string(char *val);
expr* manage_number(int val);
expr* manage_assignexpr(expr* lvalue, expr* ex);
#endif /* MANAGER_H */
