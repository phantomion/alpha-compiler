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
expr* manage_args(char *id);
expr* manage_real(double val);
expr* manage_bool(short int val);
expr* manage_nil();
expr* manage_string(char *val);
expr* manage_number(int val);
expr* manage_assignexpr(expr* lvalue, expr* ex);
expr* manage_logical(expr* arg1, expr* arg2, iopcode op);
expr* manage_less(expr* arg1, expr* arg2);
expr* manage_lesseq(expr* arg1, expr* arg2);
expr* manage_greater(expr* arg1, expr* arg2);
expr* manage_greatereq(expr* arg1, expr* arg2);
expr* manage_eq(expr* arg1, expr* arg2);
expr* manage_neq(expr* arg1, expr* arg2);
expr* manage_or(expr* arg1, expr* arg2);
expr* manage_and(expr* arg1, expr* arg2);
void print_quads();
#endif /* MANAGER_H */
