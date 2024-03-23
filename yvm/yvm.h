#ifndef __YVM_H__

#define __YVM_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "arena.h"

typedef enum {
	INSTR_PUSH,
} InstrType;

typedef struct Instr {
	InstrType type;
	int operand;
};

#define YVM_CODE_CAPACITY 12232
#define YVM_MEM_CAPACITY 64000
#define YVM_DEF_STACK_LOC 21000

typedef struct YulaVM {
	int* memory;
	int stack_base;
	int stack_head;
	int v0;
	int v1;
	Instr code[YVM_CODE_CAPACITY];
	int ip;
} YulaVM;

void dump_yvm_state(YulaVM* yvm, FILE* stream) {
	fprintf(stream, "dump(YVM_STATE) {\n");
	fprintf(stream, "    stack_addr: %p,\n", (void*)yvm->memory);
	fprintf(stream, "    bp: %d,\n", yvm->stack_base);
	fprintf(stream, "    sp: %d,\n", yvm->stack_head);
	fprintf(stream, "    ip: %d,\n", yvm->ip);
	fprintf(stream, "    registers {\n");
	fprintf(stream, "        v0: %d,\n", yvm->v0);
	fprintf(stream, "        v1: %d\n", yvm->v1);
	fprintf(stream, "    }\n");
	fprintf(stream, "}\n");
}

void init_yvm(YulaVM* yvm, int memory_size, Arena* arena) {
	yvm->memory = arena_alloc(arena, memory_size);
	yvm->ip = 0;
	yvm->stack_base = YVM_DEF_STACK_LOC;
	yvm->stack_head = YVM_DEF_STACK_LOC;
	yvm->v0 = 0;
	yvm->v1 = 0;
}

void err_destroy_yvm(YulaVM* yvm) {
	free(yvm->memory);
	free(yvm);
	exit(1);
}

typedef enum Err {
	ERR_OK,
	ERR_STACK_UNDERFLOW,
	ERR_STACK_OVERFLOW,
	ERR_ILLEGAL_INST,
} Err;

const char* err_as_cstr(Err e) {
	switch(e) {
	case ERR_OK:
		return "ok";
	case ERR_STACK_UNDERFLOW:
		return "stack underflow";
	case ERR_STACK_OVERFLOW:
		return "stack overflow";
	case ERR_ILLEGAL_INST:
		return "illegal instruction";
	default:
		fputs("error unreacheable at err_as_cstr(...)\n", stderr);
		exit(1);
	}
}

Err yvm_exec_instr(YulaVM* yvm) {
	Instr cur_inst = yvm->code[yvm->ip];
	switch(cur_inst.type) {
		case INSTR_PUSH:
			break;
		default:
			fprintf(stderr, "illegal instruction\n");
			dump_yvm_state(yvm);
			err_destroy_yvm(yvm);
			break;
	}
}

#endif // __YVM_H__