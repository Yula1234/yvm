#ifndef __BINFILES_H__

#define __BINFILES_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static int FILE_SIZE;

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

void read_bin_file(const char* path, char* buffer) {
	FILE* file = fopen(path, "rb");
	fread(buffer, sizeof(char), FILE_SIZE, file);
	fclose(file);
}

void write_bin_file(const char* path, char* buffer, int size) {
	FILE* file = fopen(path, "wb");
	fwrite(buffer, sizeof(char), size, file);
	fclose(file);
}

#endif // __BINFILES_H__