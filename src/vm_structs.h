#ifndef VM_STRUCTS_H
#define VM_STRUCTS_H
#define null NULL

typedef enum vmopcode {
    assign_v, add_v, sub_v,
    mul_v, div_v, mod_v,
    uminus_v, and_v, or_v,
    not_v, jeq_v, jne_v, jump_v,
    jle_v, jge_v, jlt_v,
    jgt_v, call_v, pusharg_v,
    funcenter_v, funcexit_v, newtable_v,
    tablegetelem_v, tablesetelem_v, nop_v,
}vmopcode;

typedef enum vmarg_t {
    label_a,
    global_a,
    formal_a,
    local_a,
    number_a,
    string_a,
    bool_a,
    nil_a,
    userfunc_a,
    libfunc_a,
    retval_a
}vmarg_t;

typedef struct vmarg {
    vmarg_t type;
    unsigned val;
}vmarg;

typedef struct instruction {
    vmopcode opcode;
    vmarg* result;
    vmarg* arg1;
    vmarg* arg2;
    unsigned src_line;
}instruction;

typedef struct user_func {
    unsigned address;
    unsigned localSize;
    char* id;
}user_func;

#endif // !VM_STRUCTS_H
