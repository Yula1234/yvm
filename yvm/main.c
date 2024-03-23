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
	Arena arena = create_arena(KiB(32));
	YulaVM* _Yvm = (YulaVM*)arena_alloc(&arena, sizeof(YulaVM));
	init_yvm(_Yvm, 512);
	FILE_SIZE = get_file_size_wp(argv[1]);
	if(FILE_SIZE < 5) {
		fputs("ERROR: not yvm bytecode provided\n", stderr);
		exit(1);
	}
	int* buffer = (int*)arena_alloc(&arena, FILE_SIZE);
	read_bin_file(argv[1], buffer);
	destroy_arena(&arena);
	return 0;
}