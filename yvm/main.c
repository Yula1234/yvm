#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "allocator.h"

typedef struct YulaVM {
	int* memory;
	int stack_base;
	int stack_head;
	int v0;
	int v1;
	int ip;
} YulaVM;

// yvm.nasm tools:
extern void init_yvm(YulaVM* yvm);
extern void yvm_push(YulaVM* yvm, int value);
extern int yvm_pop(YulaVM* yvm);
extern int yvm_mov_v0(YulaVM* yvm, int value);
extern int yvm_mov_v1(YulaVM* yvm, int value);
// end

#define INSTR_PUSH 0
#define INSTR_POP 1
#define INSTR_COUNT 1

#define REG_V0 0
#define REG_V1 1
#define REGS_COUNT 2

#define size_of_instr 2

#define YVM_MAGIC 89, 86, 77
#define YVM_MAGIC_LEN 3

static int FILE_SIZE;

static bool __is_valid_reg_operand(int operand) {
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
			else if(operand == REG_V1) {
				yvm_mov_v1(yvm, yvm_pop(yvm));
			}
			break;
		}
	}
	return true;
}

int get_file_size(FILE* file) {
	fseek(file, 0L, SEEK_END);
	int sz = ftell(file);
	return sz;
}

int get_file_size_wp(const char* path) {
	FILE* file = fopen(path, "rb");
	fseek(file, 0L, SEEK_END);
	int sz = ftell(file);
	fclose(file);
	return sz;
}

void read_bin_file(const char* path, int* buffer) {
	FILE* file = fopen(path, "rb");
	fread(buffer, sizeof(char), FILE_SIZE, file);
	fclose(file);
}

void write_bin_file(const char* path, int* buffer, int size) {
	FILE* file = fopen(path, "wb");
	fwrite(buffer, sizeof(char), size, file);
	fclose(file);
}

bool check_magic(int* buffer) {
	return (buffer[0] == 89 && buffer[1] == 86 && buffer[2] == 77);
}

void yvm_exec_code(YulaVM* yvm, int* buffer) {
	const int bufferSize = (FILE_SIZE - YVM_MAGIC_LEN) / 4;
	if(!check_magic(buffer)) {
		fputs("ERROR: invalid yvm bytecode signature\n", stderr);
		exit(1);
	}
	yvm->ip += YVM_MAGIC_LEN;
	while(yvm->ip < bufferSize && yvm_exec_intruction(yvm, buffer[yvm->ip], buffer[yvm->ip+1])) {
		yvm->ip += size_of_instr;
	}
}

void make_hard_coded_program(const char* path) {
	int arr[] = {YVM_MAGIC,
				INSTR_PUSH, 420,
				INSTR_POP, REG_V0};
	write_bin_file(path, arr, sizeof(arr));
}

void usage(FILE* stream) {
	fputs("Incorrect usage... Correct is:\n", stream);
	fputs("yvm <input.bin>\n", stream);
}

int main(int argc, const char* argv[]) {
	if(argc < 2) {
		usage(stderr);
		exit(1);
	}
	make_hard_coded_program(argv[1]);
	stack_base = (const uintptr_t*)__builtin_frame_address(0);
	
	YulaVM* _Yvm = allocator_alloc(sizeof(YulaVM));
	init_yvm(_Yvm);
	
	FILE_SIZE = get_file_size_wp(argv[1]);

	if(FILE_SIZE < 5) {
		fputs("ERROR: not yvm bytecode provided\n", stderr);
		exit(1);
	}

	int* buffer = allocator_alloc(FILE_SIZE);
	read_bin_file(argv[1], buffer);

	yvm_exec_code(_Yvm, buffer);

	allocator_free(_Yvm);
	allocator_free(buffer);
	allocator_collect();
	
	return 0;
}