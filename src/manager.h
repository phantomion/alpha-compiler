#ifndef MANAGER_H
#define MANAGER_H

#include "utilities.h"
#include "parser.h"
#include "icode.h"

void manage_lvalue(expr* id);
expr* manage_global_var(char* id);
expr* manage_var(char *id);
expr* manage_local_var(char *id);
void manage_function(char *id);
void manage_anonymous_function();
void manage_function_exit();
expr* manage_args(char *id);
#endif /* MANAGER_H */
