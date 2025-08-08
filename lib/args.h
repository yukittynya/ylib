/*
*
*   args parser :3
*
*/

#ifndef ARGS_H
#define ARGS_H

#include "arena.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char* literal;
    bool expects_input;
} ValidFlag;

typedef struct {
    const char* literal;
    const char* input;
} Arg;

typedef struct {
    Arg* args;
    int count;
} ParseResult;

/*
*
*   Define your flags here! 
*
*   {flag, expects input}
*   E.g. {"-h", false}
*
*/

static const ValidFlag valid_flags[] = {
    {"-h", false},
    {"--help", false},
    {"-a", true}, 
    {"-add", true}, 
};

static inline size_t valid_arg_count() {
    return sizeof(valid_flags) / sizeof(Arg);
}

static bool is_valid_arg(const char* arg) {
    for (int i = 0; i < valid_arg_count(); i++) {
        if (strcmp(arg, valid_flags[i].literal) == 0) return true;
    }
    return false;
}

static ParseResult parse_args(Arena* arena, int count, char** args);

static inline ParseResult parse_args(Arena* arena, int count, char** args) {
    Arg* result = arena_array(arena, Arg, count);

    int index = 0;
    int i = 1;
    while (i < count) {
        const char* literal = args[i];
        bool found_match = false;

        for (int k = 0; k < valid_arg_count(); k++) {
            if (strcmp(literal, valid_flags[k].literal) == 0) {
                if (!valid_flags[k].expects_input) {
                    result[index++] = (Arg){.literal = arena_strdup(arena, literal), .input = NULL};
                    found_match = true;
                    break;
                } else {
                    if (i + 1 >= count) {
                        printf("\e[1merror:\e[0m expected input!\n");
                        arena_free(arena);
                        exit(1);
                    }

                    if (is_valid_arg(args[i + 1])) {
                        printf("\e[1merror:\e[0m expected input, but found flag!\n");
                        arena_free(arena);
                        exit(1);
                    }

                    result[index++] = (Arg) {
                        .literal = arena_strdup(arena, literal),
                        .input = arena_strdup(arena, args[i + 1]),
                    };

                    i++;
                    found_match = true;
                    break;
                }
            }
        }

        if (!found_match) {
            printf("\e[1munknown arg:\e[0m %s\n", literal);
        }

        i++;
    }

    return (ParseResult) {.args = result, .count = index};
}

#endif // !ARGS_H
