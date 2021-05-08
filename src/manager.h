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
expr* manage_function(char *id);
expr* manage_function_exit();
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
unsigned int manage_ifprefix(expr* ex);
void manage_ifstmt(unsigned int qq);
unsigned int manage_elseprefix();
void manage_ifelse(unsigned int ifp_quad, unsigned int elsep_quad);
expr* manage_logical(expr* arg1, expr* arg2, iopcode op);
expr* manage_less(expr* arg1, expr* arg2);
expr* manage_lesseq(expr* arg1, expr* arg2);
expr* manage_greater(expr* arg1, expr* arg2);
expr* manage_greatereq(expr* arg1, expr* arg2);
expr* manage_eq(expr* arg1, expr* arg2);
expr* manage_neq(expr* arg1, expr* arg2);
expr* manage_or(expr* arg1, expr* arg2);
expr* manage_and(expr* arg1, expr* arg2);
expr* manage_member_item(expr* lv, char* name);
expr* manage_array_item(expr* lv, expr* ex);
void print_quads();
char* new_anonymous_function();
#endif /* MANAGER_H */
