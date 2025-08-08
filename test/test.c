#define USING_ARENA
#define USING_ARGS

#include "../lib/arena.h"
#include "../lib/args.h"

#include <stdio.h>

static Arena arena = {0};

int main(int argc, char *argv[]) {
    ParseResult parsed = parse_args(&arena, argc, argv);

    for (int i = 0; i < parsed.count; i++) {
        if (parsed.args[i].input) {
            printf("\e[1mflag:\e[0m %s \e[1minput:\e[0m %s\n", parsed.args[i].literal, parsed.args[i].input);
        } else {
            printf("\e[1mflag:\e[0m %s\n", parsed.args[i].literal);
        }
    }

    arena_free(&arena);
    return EXIT_SUCCESS;
}
