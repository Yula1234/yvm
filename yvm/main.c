#include <stdio.h>
#include <stdlib.h>
#include "allocator.h"

typedef struct YulaVM {
	int* memory;
	int stack_base;
	int stack_head;
} YulaVM;

// yvm.nasm tools:
void init_yvm(YulaVM* yvm);
void yvm_push(YulaVM* yvm, int value);
int yvm_pop(YulaVM* yvm);
// end

int main() {
	YulaVM* _Yvm = allocator_alloc(sizeof(YulaVM));
	init_yvm(_Yvm);
	allocator_collect();
	return 0;
}