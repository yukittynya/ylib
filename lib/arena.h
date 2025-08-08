/*
 *
 *  Inspired by tsoding's implementation at https://github.com/tsoding/arena
 *  Original work licensed under MIT License
 *
 *  Usage:
 *
 *      #define USING_ARENA
 *      #include "arena.h"
 *
 */

#ifndef ARENA_H
#define ARENA_H 

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_CAPACITY (8 * 1024) 

#define arena_array(arena, type, count) \
    (type*) arena_alloc(arena, sizeof(type) * (count)) 

#define arena_array_zero(arena, type, count) \
    (type*) arena_memset(arena_alloc(arena, sizeof(type) * (count)), 0, sizeof(type) * (count)) 

typedef struct Block {
    struct Block* next;
    size_t usage;
    size_t capacity;
    uintptr_t data[];
} Block;

typedef struct {
    Block* start;
    Block* end;
} Arena;

Block* new_block(size_t size);
void free_block(Block* block);

void* arena_alloc(Arena* arena, size_t size);
void* arena_realloc(Arena* arena, void* ptr, size_t old_size, size_t new_size);
void* arena_memset(void* ptr, int value, size_t len);
void* arena_memcpy(void* dest, const void* src, size_t len);
char* arena_strdup(Arena* arena, const char* str);

void arena_reset(Arena* arena);
void arena_free(Arena* arena); 

size_t total_capacity(Arena* arena);
size_t total_usage(Arena* arena); 

#ifdef USING_ARENA

static inline size_t align_size(size_t size) {
    return (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
}

static inline size_t _strlen(const char* str) {
    size_t len = 0;
    while (*str++) len++;
    return len;
}

Block* new_block(size_t size) {
    size_t capacity = DEFAULT_CAPACITY;

    while (size > capacity * sizeof(uintptr_t)) capacity *= 2;

    size_t bytes = capacity * sizeof(uintptr_t);
    size_t total_size = sizeof(Block) + bytes;
    Block* block = (Block*) malloc(total_size);
    assert(block);

    block -> next = NULL;
    block -> usage =  0;
    block -> capacity = bytes;

    return block;
}

void free_block(Block* block) {
    free(block);
}

void* arena_alloc(Arena* arena, size_t size) {
    size = align_size(size);

    if (arena -> end == NULL && arena -> start == NULL) {
        arena -> end = new_block(size);
        arena -> start = arena -> end;
    }

    while (arena -> end -> usage + size > arena -> end -> capacity && arena -> end -> next != NULL) {
        arena -> end = arena -> end -> next;
    } 

    if (arena -> end -> usage + size > arena -> end -> capacity) {
            Block* block = new_block(size);
            arena -> end -> next = block;
            arena -> end = block;
    }

    void* result = (char*) &arena -> end -> data + arena -> end -> usage;
    arena -> end -> usage += size;

    return result;
}

void* arena_realloc(Arena* arena, void* ptr, size_t old_size, size_t new_size) {
    if (new_size <= old_size) return ptr;

    void* result = arena_alloc(arena, new_size);
    char* new = (char*) result;
    char* old = (char*) ptr;

    for (size_t i = 0; i < old_size; i++) {
        new[i] = old[i];
    }

    for (size_t i = old_size; i < new_size; i++) {
        new[i] = 0;
    }

    return result;
}

void* arena_memset(void* ptr, int value, size_t len) {
    char* p = (char*) ptr;
    char byte_value = (char) value;

    while (len > 0) {
        *p++ = byte_value;
        len--;
    }

    return ptr;
} 

void* arena_memcpy(void* dest, const void* src, size_t len) {
    char* d = dest;
    const char* s = src;

    while (len > 0) {
        *d++ = *s++;
        len--;
    }

    return dest;
}

char* arena_strdup(Arena* arena, const char* str) {
    size_t len = _strlen(str);
    char* duplicate = (char*) arena_alloc(arena, len + 1);

    arena_memcpy(duplicate, str, len + 1);
    duplicate[len] = '\0';

    return duplicate;
}

void arena_reset(Arena* arena) {
    for (Block* block = arena -> start; block != NULL; block = block -> next) {
        block -> usage = 0;
    }
    arena -> end = arena -> start;
}

void arena_free(Arena* arena) {
    Block* block = arena -> start;

    while (block != NULL) {
        Block* previous = block;
        block = block -> next;
        free_block(previous);
    }

    arena -> start = NULL;
    arena -> end = NULL;
}

size_t total_capacity(Arena* arena) {
    Block* current = arena -> start;
    size_t total = 0;

    while (current != NULL) {
        total += current -> capacity;
        current = current -> next;
    }
    
    return total;
}

size_t total_usage(Arena* arena) {
    Block* current = arena -> start;
    size_t total = 0;

    while (current != NULL) {
        total += current -> usage;
        current = current -> next;
    }
    
    return total;
}

#endif

#endif // !ARENA_H
