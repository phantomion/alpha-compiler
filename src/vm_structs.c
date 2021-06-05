#include "vm_structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double* num_consts = null;
unsigned total_num_consts = 0;
char** string_consts = null;
unsigned total_string_consts = 0;
char** lib_funcs = null;
unsigned total_lib_funcs = 0;
user_func** user_funcs = null;
unsigned total_user_funcs = 0;
