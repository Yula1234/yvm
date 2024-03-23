#include <stdio.h>
#include <stdlib.h>
#include "allocator.h"

typedef struct YulaVM {
	int* memory;
	int stack_base;
	int stack_head;
	int v0;
	int v1;
} YulaVM;

// yvm.nasm tools:
extern void init_yvm(YulaVM* yvm);
extern void yvm_push(YulaVM* yvm, int value);
extern int yvm_pop(YulaVM* yvm);
extern int yvm_mov_v0(YulaVM* yvm, int value);
// end

#define INSTR_PUSH 0
#define INSTR_POP 1
#define INSTR_COUNT 1

#define REG_V0 0
#define REG_V1 1
#define REGS_COUNT 2

inline bool __is_valid_reg_operand(int operand) {
	return (operand > -1 && operand < REGS_COUNT);
}

bool yvm_exec_intruction(YulaVM* yvm, int instr, int operand) {
	if(instr < INSTR_PUSH && instr > INSTR_COUNT) {
		return false;
	}
	switch(instr) {
		case INSTR_PUSH:
		{
			yvm_push(yvm, operand);
			break;
		}
		case INSTR_POP:
		{
			if(!__is_valid_reg_operand(operand)) {
				return false;
			}
			if(operand == REG_V0) {
				yvm_mov_v0(yvm, yvm_pop(yvm));
			}
		}
	}
	return true;
}

int main() {
	stack_base = (const uintptr_t*)__builtin_frame_address(0);
	YulaVM* _Yvm = allocator_alloc(sizeof(YulaVM));
	init_yvm(_Yvm);
	yvm_exec_intruction(_Yvm, INSTR_PUSH, 100);
	yvm_exec_intruction(_Yvm, INSTR_POP, REG_V0);
	printf("v0 = %d\n", _Yvm->v0);
	allocator_free(_Yvm);
	allocator_collect();
	return 0;
}