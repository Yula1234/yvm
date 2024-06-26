#ifndef __YVM_H__

#define __YVM_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "arena.h"

typedef enum {
	INSTR_PUSH = 0,
	INSTR_POP = 1,
	INSTR_SYSCALL = 2,
	INSTR_MOV_V0 = 3,
	INSTR_MOV_V1 = 4,
	INSTR_JMP = 5,
	INSTR_ADD = 6,
	INSTR_SUB = 7,
	INSTR_MUL = 8,
	INSTR_DIV = 9,
	INSTR_RPUSH = 10,
	INSTR_PUSH_IP = 11,
	INSTR_PUSH_BP = 12,
	INSTR_PUSH_SP = 13,
	INSTR_JMP_ONSTACK = 14,
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

typedef enum __sycall_no_ {
	__syscall_dump_state = 0,
	__syscall_dump_v1 = 1,
	__syscall_exit = 2,
} __sycall_no_;

Err __invoke_syscall(YulaVM* yvm) {
	int __syscall_no = yvm->v0;
	if(__syscall_no == __syscall_dump_state) {
		dump_yvm_state(yvm, stdout);
		return ERR_OK;
	}
	if(__syscall_no == __syscall_dump_v1) {
		printf("%d\n", yvm->v1);
		return ERR_OK;
	}
	if(__syscall_no == __syscall_exit) {
		free(yvm->memory);
		free(yvm);
		exit(yvm->v1);
	}
	return ERR_ILLEGAL_SYSCALL_NO;
}

const char* inst_as_cstr(InstrType type) {
	switch(type) {
	case INSTR_PUSH:
		return "push";
	case INSTR_POP:
		return "pop";
	case INSTR_MOV_V0:
		return "mov v0";
	case INSTR_SYSCALL:
		return "syscall";
	case INSTR_MOV_V1:
		return "mov v1";
	case INSTR_JMP:
		return "jmp";
	case INSTR_ADD:
		return "add";
	case INSTR_SUB:
		return "sub";
	case INSTR_MUL:
		return "mul";
	case INSTR_DIV:
		return "div";
	case INSTR_RPUSH:
		return "rpush";
	case INSTR_PUSH_IP:
		return "sip";
	case INSTR_PUSH_BP:
		return "bpush";
	case INSTR_PUSH_SP:
		return "spush";
	case INSTR_JMP_ONSTACK:
		return "sjmp";
	default:
		return "UNKOWN";
	}
	return "UNKOWN";
}

void dump_instr(Instr in) {
	printf("instr(type=`%s`, operand=%d)\n", inst_as_cstr(in.type), in.operand);
}

Err yvm_push(YulaVM* yvm, int value) {
	int _value = value;
	int* __value = &_value;
	if(yvm->stack_head >= YVM_MEM_CAPACITY) {
		return ERR_STACK_OVERFLOW;
	}
	uint8_t* sp = &yvm->memory[yvm->stack_head];
	memcpy(sp, __value, 4);
	*sp = value;
	yvm->stack_head += 4;
	return ERR_OK;
}

bool yvm_can_pop(YulaVM* yvm) {
	if(yvm->stack_base <= yvm->stack_head) {
		return true;
	}
	return false;
}

Err yvm_pop(YulaVM* yvm, int* to) {
	if(!yvm_can_pop(yvm)) {
		return ERR_STACK_UNDERFLOW;
	}
	yvm->stack_head -= 4;
	*to = *(int*)&yvm->memory[yvm->stack_head];
	return ERR_OK;
}

const char* __reg_no_to_cstr(int reg) {
	if(reg == REG_V0) {
		return "v0";
	}
	if(reg == REG_V1) {
		return "v1";
	}
	return "v0";
}

void __process_debug_cstate(YulaVM* yvm) {
	// printf("(ydb) info [v0: %d, v1: %d, ip: %d]\n", yvm->v0, yvm->v1, yvm->ip);
	Instr cur_inst = yvm->code[yvm->ip];
	if(cur_inst.type == INSTR_RPUSH)        printf("(ydb) rpush %s", __reg_no_to_cstr(cur_inst.operand));
	else if(cur_inst.type == INSTR_MOV_V0)  printf("(ydb) mov v0, %d", cur_inst.operand);
	else if(cur_inst.type == INSTR_MOV_V1)  printf("(ydb) mov v1, %d", cur_inst.operand);
	else if(cur_inst.type == INSTR_POP)     printf("(ydb) pop %s", __reg_no_to_cstr(cur_inst.operand));
	else if(cur_inst.type == INSTR_ADD)     printf("(ydb) add");
	else if(cur_inst.type == INSTR_SYSCALL) printf("(ydb) syscall");
	else printf("(ydb) %s %d", inst_as_cstr(cur_inst.type), cur_inst.operand);
	if(yvm->ip == 0 && cur_inst.type == INSTR_JMP) {
		fputs(" (jump to entry)", stdout);
	}
	if(cur_inst.type == INSTR_SYSCALL) {
		if(yvm->v0 == 0)      printf(" (dump state)");
		else if(yvm->v0 == 1) printf(" (dump v1)");
		else if(yvm->v0 == 2) printf(" (exit)");
		else printf(" WARNING: unkown syscall_no");
	}
}

Err yvm_exec_instr(YulaVM* yvm, bool debug) {
	Instr cur_inst = yvm->code[yvm->ip];
	if(debug) {
		__process_debug_cstate(yvm);
		getc(stdin);
	}
	switch(cur_inst.type) {
		case INSTR_PUSH:
		{
			Err e = yvm_push(yvm, cur_inst.operand);
			yvm->ip += 1;
			return e;
			break;
		}
		case INSTR_PUSH_IP:
		{
			yvm->ip += 1;
			Err e = yvm_push(yvm, yvm->ip);
			return e;
			break;
		}
		case INSTR_PUSH_BP:
		{
			Err e = yvm_push(yvm, yvm->stack_base);
			yvm->ip += 1;
			return e;
			break;
		}
		case INSTR_PUSH_SP:
		{
			Err e = yvm_push(yvm, yvm->stack_head);
			yvm->ip += 1;
			return e;
			break;
		}
		case INSTR_JMP_ONSTACK:
		{
			int addr;
			if(!yvm_can_pop(yvm)) {
				return ERR_STACK_UNDERFLOW;
			}
			yvm_pop(yvm, &addr);
			yvm->ip = addr;
			break;
		}
		case INSTR_RPUSH:
		{
			Err e = yvm_push(yvm, *__find_reg(yvm, cur_inst.operand));
			yvm->ip += 1;
			return e;
			break;
		}
		case INSTR_POP:
		{
			Err e = yvm_pop(yvm, __find_reg(yvm, cur_inst.operand));
			yvm->ip += 1;
			return e;
			break;
		}
		case INSTR_MOV_V0:
		{	
			yvm->v0 = cur_inst.operand;
			yvm->ip += 1;
			break;
		}
		case INSTR_MOV_V1:
		{	
			yvm->v1 = cur_inst.operand;
			yvm->ip += 1;
			break;
		}
		case INSTR_SYSCALL:
		{
			yvm->ip += 1;
			return __invoke_syscall(yvm);
			break;
		}
		case INSTR_JMP:
		{
			yvm->ip = cur_inst.operand;
			break;
		}
		case INSTR_ADD:
		{
			int one;
			int two;
			if(!yvm_can_pop(yvm)) {
				return ERR_STACK_UNDERFLOW;
			}
			yvm_pop(yvm, &two);
			if(!yvm_can_pop(yvm)) {
				return ERR_STACK_UNDERFLOW;
			}
			yvm_pop(yvm, &one);
			yvm_push(yvm, one + two);
			yvm->ip += 1;
			break;
		}
		case INSTR_SUB:
		{
			int one;
			int two;
			if(!yvm_can_pop(yvm)) {
				return ERR_STACK_UNDERFLOW;
			}
			yvm_pop(yvm, &two);
			if(!yvm_can_pop(yvm)) {
				return ERR_STACK_UNDERFLOW;
			}
			yvm_pop(yvm, &one);
			yvm_push(yvm, one - two);
			yvm->ip += 1;
			break;
		}
		case INSTR_MUL:
		{
			int one;
			int two;
			if(!yvm_can_pop(yvm)) {
				return ERR_STACK_UNDERFLOW;
			}
			yvm_pop(yvm, &two);
			if(!yvm_can_pop(yvm)) {
				return ERR_STACK_UNDERFLOW;
			}
			yvm_pop(yvm, &one);
			yvm_push(yvm, one * two);
			yvm->ip += 1;
			break;
		}
		case INSTR_DIV:
		{
			int one;
			int two;
			if(!yvm_can_pop(yvm)) {
				return ERR_STACK_UNDERFLOW;
			}
			yvm_pop(yvm, &two);
			if(!yvm_can_pop(yvm)) {
				return ERR_STACK_UNDERFLOW;
			}
			yvm_pop(yvm, &one);
			yvm_push(yvm, one / two);
			yvm->ip += 1;
			break;
		}
		default:
			return ERR_ILLEGAL_INST;
	}
	return ERR_OK;
}

void yvm_exec_prog(YulaVM* yvm, bool debug) {
	if(debug) {
		printf("start debuging...\n");
		printf("for execute next instruction press `enter`");
		getc(stdin);
	}
	for(;yvm->ip < yvm->code_size;) {
		Err e = yvm_exec_instr(yvm, debug);
		if(e != ERR_OK) {
			fprintf(stderr, "SIGNAL: %s\n", err_as_cstr(e));
			err_destroy_yvm(yvm);
		}
	}
	if(debug) {
		printf("(ydb) end");
		getc(stdin);
	}
}

void yvm_load_bytecode(YulaVM* yvm, Instr* buffer, size_t size, char* magic) {
	size_t i = 0;
	Instr* buf = buffer;
	if(magic[0] != 'Y' && magic[1] != 'M') {
		fputs("ERROR: not yvm bytecode provided\n", stderr);
		exit(1);
	}
	for(;i < size;++i) {
		yvm->code[i] = buf[i];
	}
	yvm->code_size = (int)i;
}

#endif // __YVM_H__