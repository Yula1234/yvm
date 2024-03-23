#ifndef __ARENA_H__

#define __ARENA_H__

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

typedef struct Arena Arena;

#define KiB(bytes) ((size_t) bytes << 10)
#define MiB(bytes) ((size_t) bytes << 20)
#define GiB(bytes) ((size_t) bytes << 30)

typedef struct Arena {
	char *memory;
	size_t size;
	size_t used;
} Arena;

Arena create_arena(size_t size) {
	Arena arena = {0};
	arena.memory = malloc(size);
	arena.size = size;
	return arena;
}

void destroy_arena(Arena *arena) {
	free(arena->memory);
	arena = NULL;
}

void *arena_alloc(Arena *arena, size_t size) {
	assert(arena->size >= arena->used + size);
	char *allocated = arena->memory + arena->used;
	arena->used += size;
	return allocated;
}

void arena_free(Arena *arena, size_t size) {
	assert(arena->used - size >= 0);
	arena->used -= size;
}

void clear_arena(Arena *arena) {
	arena->used = 0;
}

#endif // __ARENA_H__