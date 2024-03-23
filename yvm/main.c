#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "yvm.h"
#include "binfiles.h"
#include "arena.h"

void usage(FILE* stream) {
	fputs("Incorrect usage... Correct is:\n", stream);
	fputs("yvm <input.bin>\n", stream);
}

void make_hard_coded_prog() {
	Instr in = {INSTR_PUSH, 32};
	write_bin_file("test.bin", (char*)&in, sizeof(Instr));
}

int main(int argc, const char* argv[]) {
	if(argc < 2) {
		usage(stderr);
		exit(1);
	}
	make_hard_coded_prog();
	YulaVM* _Yvm = malloc(sizeof(YulaVM));
	init_yvm(_Yvm, YVM_MEM_CAPACITY);
	FILE_SIZE = get_file_size_wp(argv[1]);
	Instr* buffer = (Instr*)malloc(FILE_SIZE);
	read_bin_file(argv[1], (char*)buffer);

	yvm_load_bytecode(_Yvm, buffer, FILE_SIZE / sizeof(Instr));
	yvm_exec_prog(_Yvm);

	free(_Yvm);
	free(buffer);
	return 0;
}