#include "generator.h"
#include "parser.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "icode.h"
#include "manager.h"

extern double* num_consts;
extern unsigned total_num_consts;
extern char** string_consts;
extern unsigned total_string_consts;
extern char** lib_funcs;
extern unsigned total_lib_funcs;
extern user_func** user_funcs;
extern unsigned total_user_funcs;
int curr_processed_quad = 0;

extern quad* quads;
extern FILE* yyout;

instruction** instructions = null;
unsigned int total_instr = 1;
unsigned int curr_instr = 1;

incomplete_jump* ij_head = null;
incomplete_jump* ij_tail = null;
unsigned ij_total = 0;

void add_incomplete_jump(unsigned instrNo, unsigned iaddress) {
    incomplete_jump* jump = malloc(sizeof(incomplete_jump));
    jump->instrNo = instrNo;
    jump->iaddress = iaddress;
    jump->next = null;

    if(!ij_head) {
        ij_head = jump;
        ij_tail = jump;
    }
    else {
        ij_tail->next = jump;
        ij_tail = jump;
    }
}

void patch_incomplete_jump() {
    incomplete_jump* temp = ij_head;

    while(temp) {
        instructions[temp->instrNo]->result->type = label_a;
        instructions[temp->instrNo]->result->val = (temp->iaddress == curr_quad) ? curr_instr : quads[temp->iaddress].taddress;
        temp = temp->next;
    }
}

void print_instrs();

void expand_instr() {
    assert(total_instr == curr_instr);
    instruction** i = malloc(NEW_INSTR_SIZE);
    if (instructions) {
        memcpy(i, instructions, CURR_INSTR_SIZE);
        free(instructions);
    }
    instructions = i;
    total_instr += EXPAND_SIZE;
}

void emit_instr(instruction* instr) {
    if (curr_instr == total_instr) expand_instr();
    instructions[curr_instr++] = instr;
}

void generate(vmopcode op, quad* quad) {
    instruction* t = malloc(sizeof(instruction));
    t->arg1 = malloc(sizeof(vmarg));
    t->arg2 = malloc(sizeof(vmarg));
    t->result = malloc(sizeof(vmarg));
    t->opcode = op;
    t->src_line = quad->line;

    make_operand(quad->arg1, t->arg1);
    make_operand(quad->arg2, t->arg2);

    if (!t->arg2->type && !t->arg2->val)
        t->arg2 = null;

    make_operand(quad->result, t->result);
    quad->taddress = curr_instr;
    emit_instr(t);
}

void make_operand(expr* e, vmarg* arg) {
    if (!e) return;
    switch (e->type) {
        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
        case assignexpr_e:
        case newtable_e: {
            assert(e->sym);
            arg->val = e->sym->offset;
            switch (e->sym->space) {
                case programvar:
                    arg->type = global_a;
                    break;
                case functionlocal:
                    arg->type = local_a;
                    break;
                case formalarg:
                    arg->type = formal_a;
                    break;
                default:
                    assert(0);
            }
            break;
        }
        case constbool_e: {
            arg->val = e->bool_const;
            arg->type = bool_a;
            break;
        }
        case conststring_e: {
            arg->val = consts_newstring(e->str_const);
            arg->type = string_a;
            break;
        }
        case constnum_e: {
            arg->val = consts_newnumber(e->num_const);
            arg->type = number_a;
            break;
        }
        case nil_e:
            arg->type = nil_a;
            break;
        case programfunc_e: {
            arg->type = userfunc_a;
            arg->val = userfuncs_newfunc(e->sym);
            break;
        }
        case libraryfunc_e: {
            arg->type = libfunc_a;
            arg->val = libfuncs_newused(e->sym->name);
            break;
        }
        default:
            assert(0);
    }
}

unsigned consts_newstring(char* s) {
    for (size_t i = 0; i < total_string_consts; i++) {
        if (strcmp(string_consts[i], s) == 0) return i;
    }

    total_string_consts++;

    if (!string_consts) string_consts = calloc(1, total_string_consts * sizeof(char *));
    else string_consts = realloc(string_consts, total_string_consts * sizeof(char *));

    string_consts[total_string_consts - 1] = s;

    return total_string_consts - 1;
}


unsigned consts_newnumber(double n) {
    for (size_t i = 0; i < total_num_consts; i++) {
        if (num_consts[i] == n) return i;
    }

    total_num_consts++;

    if (!num_consts) num_consts = calloc(1, total_num_consts * sizeof(double));
    else num_consts = realloc(num_consts, total_num_consts * sizeof(double));

    num_consts[total_num_consts - 1] = n;

    return total_num_consts - 1;
}


unsigned libfuncs_newused(char* s) {
    for (size_t i = 0; i < total_lib_funcs; i++) {
        if (strcmp(lib_funcs[i], s) == 0) return i;
    }

    total_lib_funcs++;

    if (!lib_funcs) lib_funcs = calloc(1, total_lib_funcs * sizeof(char *));
    else lib_funcs = realloc(lib_funcs, total_lib_funcs * sizeof(char *));

    lib_funcs[total_lib_funcs - 1] = s;

    return total_lib_funcs - 1;
}


unsigned userfuncs_newfunc(symbol* sym) {
    for (size_t i = 0; i < total_user_funcs; i++) {
        if (user_funcs[i]->address == sym->func_addr) return i;
    }

    total_user_funcs++;
    if (!user_funcs) user_funcs = calloc(1, total_user_funcs * sizeof(user_func*));
    else user_funcs = realloc(user_funcs, total_user_funcs * sizeof(user_func*));

    user_funcs[total_user_funcs - 1] = calloc(1, sizeof(user_func));
    user_funcs[total_user_funcs - 1]->address = sym->func_addr;
    user_funcs[total_user_funcs - 1]->localSize = sym->total_locals;
    user_funcs[total_user_funcs - 1]->id = sym->name;

    return total_user_funcs - 1;
}

typedef void (*generator_func_t)(quad*);

generator_func_t generators[] = {
    generate_assign,       generate_add,          generate_sub,
    generate_mul,          generate_div,          generate_mod,
    generate_uminus,       generate_and,          generate_or,
    generate_not,          generate_if_eq,        generate_if_noteq,
    generate_if_lesseq,    generate_if_greatereq, generate_if_less,
    generate_if_greater,   generate_jump,         generate_call,
    generate_param,        generate_ret,          generate_getretval,
    generate_funcstart,    generate_funcend,      generate_tablecreate,
    generate_tablegetelem, generate_tablesetelem,
};

void generate_all() {
    for (unsigned i = 1; i < curr_quad; ++i) {
        curr_processed_quad = i;
        (*generators[quads[i].op])(quads + i);
    }
    patch_incomplete_jump();
    /*print_instrs();*/
}

void generate_relational(vmopcode op, quad* quad) {
    instruction* t = calloc(1, sizeof(instruction));

    t->opcode = op;
    t->arg1 = calloc(1, sizeof(vmarg));
    t->arg2 = calloc(1, sizeof(vmarg));
    t->src_line = quad->line;

    make_operand(quad->arg1, t->arg1);
    make_operand(quad->arg2, t->arg2);

    if (op == jump_v) {
        t->arg1 = null;
        t->arg2 = null;
    }

    t->result = calloc(1, sizeof(vmarg));
    t->result->type = label_a;

    if (quad->label < curr_processed_quad) {
        t->result->val = quads[quad->label].taddress;
    }
    else {
        add_incomplete_jump(curr_instr, quad->label);
    }
    quad->taddress = curr_instr;
    emit_instr(t);
}

void generate_assign(quad* quad) {
    generate(assign_v, quad);
}


void generate_add(quad* quad) {
    generate(add_v, quad);
}


void generate_sub(quad* quad) {
    generate(sub_v, quad);
}


void generate_mul(quad* quad) {
    generate(mul_v, quad);
}


void generate_div(quad* quad) {
    generate(div_v, quad);
}


void generate_mod(quad* quad) {
    generate(mod_v, quad);
}


void generate_uminus(quad* quad) {
    instruction* t = malloc(sizeof(instruction));
    t->arg1 = malloc(sizeof(vmarg));
    t->arg2 = malloc(sizeof(vmarg));
    t->result = malloc(sizeof(vmarg));
    t->opcode = mul_v;
    t->src_line = quad->line;

    make_operand(quad->arg1, t->arg1);
    make_operand(manage_number(-1), t->arg2);
    make_operand(quad->result, t->result);
    quad->taddress = curr_instr;
    emit_instr(t);
}
void generate_and(quad* quad) {}
void generate_or(quad* quad) {}
void generate_not(quad* quad) {}
void generate_if_eq(quad* quad) {
    generate_relational(jeq_v, quad);
}

void generate_if_noteq(quad* quad) {
    generate_relational(jne_v, quad);
}

void generate_if_lesseq(quad* quad) {
    generate_relational(jle_v, quad);
}

void generate_if_greatereq(quad* quad) {
    generate_relational(jge_v, quad);
}

void generate_if_less(quad* quad) {
    generate_relational(jlt_v, quad);
}

void generate_if_greater(quad* quad) {
    generate_relational(jgt_v, quad);
}

void generate_jump(quad* quad) {
    generate_relational(jump_v, quad);
}

void generate_call(quad* quad) {
    quad->taddress = curr_instr;
    instruction* t = calloc(1, sizeof(instruction));
    t->opcode = call_v;
    t->result = calloc(1, sizeof(vmarg));
    t->src_line = quad->line;

    make_operand(quad->result, t->result);
    emit_instr(t);
}

void generate_param(quad* quad) {
    quad->taddress = curr_instr;
    instruction* t = calloc(1, sizeof(instruction));
    t->opcode = pusharg_v;
    t->result = calloc(1, sizeof(vmarg));
    t->src_line = quad->line;

    make_operand(quad->result, t->result);
    emit_instr(t);
}

// careful here ta allazoume edw pera ara prosoxh meta
void generate_ret(quad* quad) {
    quad->taddress = curr_instr;
    instruction* t = calloc(1, sizeof(instruction));
    t->opcode = assign_v;
    t->arg1 = calloc(1, sizeof(vmarg));
    t->result = calloc(1, sizeof(vmarg));
    t->src_line = quad->line;

    make_operand(quad->result, t->arg1);
    t->result->type = retval_a;
    emit_instr(t);
}

void generate_getretval(quad* quad){
    quad->taddress = curr_instr;
    instruction* t = calloc(1, sizeof(instruction));
    t->opcode = assign_v;
    t->arg1 = calloc(1, sizeof(vmarg));
    t->result = calloc(1, sizeof(vmarg));
    t->src_line = quad->line;

    make_operand(quad->result, t->result);
    t->arg1->type = retval_a;
    emit_instr(t);
}

void generate_funcstart(quad* quad) {
    quad->result->sym->func_addr = curr_instr;
    quad->taddress = curr_instr;

    instruction* t = calloc(1, sizeof(instruction));
    t->opcode = funcenter_v;
    t->result = calloc(1, sizeof(vmarg));
    t->src_line = quad->line;

    make_operand(quad->result, t->result);
    emit_instr(t);
}

void generate_funcend(quad* quad){
    quad->taddress = curr_instr;
    instruction* t = calloc(1, sizeof(instruction));
    t->opcode = funcexit_v;

    t->result = calloc(1, sizeof(vmarg));
    t->src_line = quad->line;

    make_operand(quad->result, t->result);
    emit_instr(t);
}
void generate_tablecreate(quad* quad) {
    generate(newtable_v, quad);
}


void generate_tablegetelem(quad* quad) {
    generate(tablegetelem_v, quad);
}


void generate_tablesetelem(quad* quad) {
    generate(tablesetelem_v, quad);
}

void print_instr_arg(vmarg* vma) {
    switch (vma->type) {
        case global_a:
        case formal_a:
        case local_a:
        case bool_a:
            fprintf(yyout, "%d:%-10d", vma->type, vma->val);
            break;
        case number_a:
            fprintf(yyout, "%d:%-10g", vma->type, num_consts[vma->val]);
            break;
        case nil_a:
            fprintf(yyout, "%-10d", vma->type);
            break;
        case string_a:
            fprintf(yyout, "%d:%-10s", vma->type, string_consts[vma->val]);
            break;
        case userfunc_a:
            fprintf(yyout, "%d:%-10s", vma->type, user_funcs[vma->val]->id);
            break;
        case libfunc_a:
            fprintf(yyout, "%d:%-10s", vma->type, lib_funcs[vma->val]);
            break;
        case label_a:
            fprintf(yyout, "%d:%-10d", vma->type, vma->val);
            break;
        default:
            break;
    }
}


void print_instrs() {
    int i = 1;
    fprintf(yyout, "\ninstr#\t%-15s%-10s%-10s%-10s%-10s%-10s\n", "opcode", "result", "arg1", "arg2", "label", "line");
    fprintf(yyout, "-------------------------------------------------------------------\n");
    for (i = 1; i < curr_instr; i++) {
        fprintf(yyout, "%d:\t", i);
        fprintf(yyout, "%-15d", instructions[i]->opcode);
        if (instructions[i]->result) {
            print_instr_arg(instructions[i]->result);
        }
        else fprintf(yyout, "%-10s", " ");
        if (instructions[i]->arg1) {
            print_instr_arg(instructions[i]->arg1);
        }
        else fprintf(yyout, "%-10s", " ");
        if (instructions[i]->arg2) {
            print_instr_arg(instructions[i]->arg2);
        }
        else fprintf(yyout, "%-10s", " ");
        fprintf(yyout, "\n");
    }
}

void write_abc_bin() {
    char* magicnumber = "340200501";
    FILE* fp = fopen("alpha_bin.abc", "wb");
    if (!fp) {
        exit(EXIT_FAILURE);
    }
    fwrite(magicnumber, strlen(magicnumber), 1, fp);

    fwrite(&total_string_consts, sizeof(total_string_consts), 1, fp);
    for (size_t i = 0; i < total_string_consts; i++) {
        int str_const_len = strlen(string_consts[i]);
        fwrite(&str_const_len, sizeof(int), 1, fp);
        fwrite(string_consts[i], strlen(string_consts[i]), 1, fp);
    }

    fwrite(&total_num_consts, sizeof(total_num_consts), 1, fp);
    for (size_t i = 0; i < total_num_consts; i++) {
        fwrite(&num_consts[i], sizeof(num_consts[i]), 1, fp);
    }

    fwrite(&total_user_funcs, sizeof(total_user_funcs), 1, fp);
    for (size_t i = 0; i < total_user_funcs; i++) {
        int func_id_len = strlen(user_funcs[i]->id);
        fwrite(&func_id_len, sizeof(int), 1, fp);
        fwrite(&user_funcs[i]->address, sizeof(int), 1, fp);
        fwrite(&user_funcs[i]->localSize, sizeof(int), 1, fp);
        fwrite(user_funcs[i]->id, strlen(user_funcs[i]->id), 1, fp);
    }

    fwrite(&total_lib_funcs, sizeof(total_lib_funcs), 1, fp);
    for (size_t i = 0; i < total_lib_funcs; i++) {
        int lib_func_len = strlen(lib_funcs[i]);
        fwrite(&lib_func_len, sizeof(int), 1, fp);
        fwrite(lib_funcs[i], strlen(lib_funcs[i]), 1, fp);
    }

    fwrite(&curr_instr, sizeof(curr_instr), 1, fp);
    for (size_t i = 1; i < curr_instr; i++) {
        fwrite(&instructions[i]->opcode, sizeof(vmarg_t), 1, fp);

        fwrite(&instructions[i]->result->type, sizeof(vmarg_t), 1, fp);
        fwrite(&instructions[i]->result->val, sizeof(int), 1, fp);
        int zero = 0;
        if (instructions[i]->arg1) {
            fwrite(&instructions[i]->arg1->type, sizeof(vmarg_t), 1, fp);
            fwrite(&instructions[i]->arg1->val, sizeof(int), 1, fp);
        }
        else {
            fwrite(&zero, sizeof(vmarg_t), 1, fp);
            fwrite(&zero, sizeof(int), 1, fp);
        }

        if (instructions[i]->arg2) {
            fwrite(&instructions[i]->arg2->type, sizeof(vmarg_t), 1, fp);
            fwrite(&instructions[i]->arg2->val, sizeof(int), 1, fp);
        }
        else {
            fwrite(&zero, sizeof(vmarg_t), 1, fp);
            fwrite(&zero, sizeof(int), 1, fp);
        }

        fwrite(&instructions[i]->src_line, sizeof(int), 1, fp);
    }
}

void write_abc_text() {
    char* magicnumber = "340200501";
    FILE* fp = fopen("alpha_text.abc", "w");
    if (!fp) {
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "%s\n", magicnumber);

    fprintf(fp, "# Total string consts:\n");
    fprintf(fp, "%d\n", total_string_consts);
    for (size_t i = 0; i < total_string_consts; i++) {
        fprintf(fp, "%lu %s\n", strlen(string_consts[i]), string_consts[i]);
    }

    fprintf(fp, "# Total number consts:\n");
    fprintf(fp, "%d\n", total_num_consts);
    for (size_t i = 0; i < total_num_consts; i++) {
        fprintf(fp, "%g\n", num_consts[i]);
    }

    fprintf(fp, "# Total user functions:\n");
    fprintf(fp, "%d\n", total_user_funcs);
    for (size_t i = 0; i < total_user_funcs; i++) {
        fprintf(fp, "%d %d %lu %s\n", user_funcs[i]->address, user_funcs[i]->localSize, strlen(user_funcs[i]->id), user_funcs[i]->id);
    }

    fprintf(fp, "# Total library functions:\n");
    fprintf(fp, "%d\n", total_lib_funcs);
    for (size_t i = 0; i < total_lib_funcs; i++) {
        fprintf(fp, "%lu %s\n", strlen(lib_funcs[i]), lib_funcs[i]);
    }

    fprintf(fp, "# Total instructions:\n");
    fprintf(fp, "%d\n", curr_instr);

    fprintf(fp, "#opcode %-10s%-10s%-10s\n", "result", "arg1", "arg2");
    for (size_t i = 1; i < curr_instr; i++) {
        fprintf(fp, "%d\t", instructions[i]->opcode);
        if (instructions[i]->result) {
            fprintf(fp, "%d:%d ", instructions[i]->result->type, instructions[i]->result->val);
        }
        else fprintf(fp, "%-10s", " ");
        if (instructions[i]->arg1) {
            fprintf(fp, "%d:%d ", instructions[i]->arg1->type, instructions[i]->arg1->val);
        }
        else fprintf(fp, "%-10s", " ");
        if (instructions[i]->arg2) {
            fprintf(fp, "%d:%d", instructions[i]->arg2->type, instructions[i]->arg2->val);
        }
        else fprintf(fp, "%-10s", " ");
        fprintf(fp, "\n");
    }
}
