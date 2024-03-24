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

int main(int argc, const char* argv[]) {
	if(argc < 2) {
		usage(stderr);
		exit(1);
	}
	YulaVM* _Yvm = malloc(sizeof(YulaVM));
	init_yvm(_Yvm, YVM_MEM_CAPACITY);
	FILE_SIZE = get_file_size_wp(argv[1]);
	Instr* buffer = (Instr*)malloc(FILE_SIZE);
	read_bin_file(argv[1], (char*)buffer);

	yvm_load_bytecode(_Yvm, buffer, FILE_SIZE / sizeof(Instr), tmp_buf);
	yvm_exec_prog(_Yvm);

	free(_Yvm);
	free(buffer);
	return 0;
}