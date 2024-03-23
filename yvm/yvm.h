#ifndef __YVM_H__

#define __YVM_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "arena.h"

typedef enum {
	INSTR_PUSH,
	INSTR_POP,
	INSTR_SYSCALL,
	INSTR_MOV_V0,
} InstrType;

typedef struct Instr {
	InstrType type;
	int operand;
} Instr;

#define YVM_CODE_CAPACITY 12232
#define YVM_MEM_CAPACITY 64000
#define YVM_DEF_STACK_LOC 21000

typedef struct YulaVM {
	uint8_t* memory;
	int stack_base;
	int stack_head;
	int v0;
	int v1;
	Instr code[YVM_CODE_CAPACITY];
	int code_size;
	int ip;
} YulaVM;

void dump_yvm_state(YulaVM* yvm, FILE* stream) {
	fprintf(stream, "dump(YVM_STATE) {\n");
	fprintf(stream, "    stack_addr: %p,\n", (void*)yvm->memory);
	fprintf(stream, "    bp: %d,\n", yvm->stack_base);
	fprintf(stream, "    sp: %d,\n", yvm->stack_head);
	fprintf(stream, "    ip: %d,\n", yvm->ip);
	fprintf(stream, "    code_size: %d,\n", yvm->code_size);
	fprintf(stream, "    registers {\n");
	fprintf(stream, "        v0: %d,\n", yvm->v0);
	fprintf(stream, "        v1: %d\n", yvm->v1);
	fprintf(stream, "    }\n");
	fprintf(stream, "}\n");
}

void init_yvm(YulaVM* yvm, int memory_size) {
	yvm->memory = malloc(memory_size);
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
	ERR_ILLEGAL_SYSCALL_NO,
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
	case ERR_ILLEGAL_SYSCALL_NO:
		return "illegal syscall";
	default:
		fputs("error unreacheable at err_as_cstr(...)\n", stderr);
		exit(1);
	}
}

#define REG_V0 0
#define REG_V1 1

int* __find_reg(YulaVM* yvm, int reg) {
	if(reg == REG_V0) {
		return &(yvm->v0);
	}
	if(reg == REG_V1) {
		return &(yvm->v1);
	}
	return &(yvm->v0);
}

Err __invoke_syscall(YulaVM* yvm) {
	int __syscall_no = yvm->v0;
	if(__syscall_no == 0) {
		dump_yvm_state(yvm, stdout);
		return ERR_OK;
	}
	return ERR_ILLEGAL_SYSCALL_NO;
}

Err yvm_exec_instr(YulaVM* yvm) {
	Instr cur_inst = yvm->code[yvm->ip];
	switch(cur_inst.type) {
		case INSTR_PUSH:
		{
			if(yvm->stack_head >= YVM_MEM_CAPACITY) {
				return ERR_STACK_OVERFLOW;
			}
			*(int*)(&(yvm->memory[yvm->stack_head])) = cur_inst.operand;
			yvm->stack_head += 4;
			break;
		}
		case INSTR_POP:
		{
			if(yvm->stack_base <= yvm->stack_head) {
				return ERR_STACK_UNDERFLOW;
			}
			int* reg = __find_reg(yvm, cur_inst.operand);
			*reg = yvm->memory[yvm->stack_head];
			yvm->stack_head -= 4;
			break;
		}
		case INSTR_MOV_V0:
		{	
			yvm->v0 = cur_inst.operand;
			break;
		}
		case INSTR_SYSCALL:
		{
			return __invoke_syscall(yvm);
			break;
		}
		default:
			fprintf(stderr, "illegal instruction %d\n", cur_inst.type);
			dump_yvm_state(yvm, stderr);
			err_destroy_yvm(yvm);
			break;
	}
	return ERR_OK;
}

const char* inst_as_cstr(InstrType type) {
	switch(type) {
	case INSTR_PUSH:
		return "PUSH";
	default:
		return "UNKOWN";
	}
	return "UNKOWN";
}

void dump_instr(Instr in) {
	printf("instr(type=`%s`, operand=%d)\n", inst_as_cstr(in.type), in.operand);
}

void yvm_exec_prog(YulaVM* yvm) {
	for(;yvm->ip < yvm->code_size;yvm->ip++) {
		Err e = yvm_exec_instr(yvm);
		if(e != ERR_OK) {
			fprintf(stderr, "SIGNAL: %s\n", err_as_cstr(e));
			err_destroy_yvm(yvm);
		}
	}
}

void yvm_load_bytecode(YulaVM* yvm, Instr* buffer, size_t size) {
	size_t i = 0;
	Instr* buf = buffer;
	char* cbuf = (char*)buf;
	if(cbuf[0] != 'Y' && cbuf[1] != 'M') {
		fputs("ERROR: not yvm bytecode provided\n", stderr);
	}
	for(;i < size;++i) {
		if(i != 0) {
			yvm->code[i] = buf[i];
		}
	}
	yvm->code_size = (int)i;
}

#endif // __YVM_H__