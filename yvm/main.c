#include <stdio.h>
#include <stdlib.h>
#include "allocator.h"

typedef struct YulaVM {
	int* memory;
	int stack_base;
	int stack_head;
} YulaVM;

// yvm.nasm tools:
void init_yvm(YulaVM*);
// end

int main() {
	YulaVM* _Yvm = allocator_alloc(sizeof(YulaVM));
	init_yvm(_Yvm);
	printf("_Yvm::stack_head = %d\n", _Yvm->stack_head);
	allocator_collect();
	return 0;
}