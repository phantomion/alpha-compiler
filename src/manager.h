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
expr* manage_args(char *id);
expr* manage_real(double val);
expr* manage_bool(short int val);
expr* manage_nil();
expr* manage_string(char *val);
expr* manage_number(int val);
expr* manage_assignexpr(expr* lvalue, expr* ex);
#endif /* MANAGER_H */
