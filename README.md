# cecs

A tiny entity-component-system for C, distributed as a single header.

## Usage

In **one** translation unit:

```c
#define CECS_IMPLEMENTATION   // compile the implementation here
#define CECS_CALLBACKS        // optional: cecs provides main()
#include <cecs/cecs.h>
```

Other TUs include `<cecs/cecs.h>` plain.

Define your world with the callback macros:

```c
cecs_init({   spawn(component(Position, .x = 1.0f), component(Player, .name = "p1")); })
cecs_update({ query(n, Position, pos, Player, pl); /* ... */ })
cecs_exit({   /* cleanup */ })
```

`cecs_init` runs once at start, `cecs_update` each tick, `cecs_exit` once after `cecs_quit()` ends the loop. Each macro can be used multiple times.

Without `CECS_CALLBACKS`, write your own `main()` and call `cecs_run()`.

## Build the demo

```sh
cmake -S . -B build
cmake --build build
./build/cecs
```

`src/main.c` is a small demo using the callback style.
