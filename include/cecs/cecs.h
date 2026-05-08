#ifndef CECS_H
#define CECS_H

#include <stddef.h>
#include <stdint.h>

#ifndef CECS_MAX_FUNS
#define CECS_MAX_FUNS 256
#endif
#ifndef CECS_MAX_COMPONENTS
#define CECS_MAX_COMPONENTS 256
#endif
// Per-query stack cost: 8 B * CECS_MAX_PER_TYPE * (1 + N_components) per call.
#ifndef CECS_MAX_PER_TYPE
#define CECS_MAX_PER_TYPE 1024
#endif
#ifndef CECS_MAX_ENTITIES
#define CECS_MAX_ENTITIES 1024
#endif

typedef uint32_t EntityId;
typedef void (*cecs_fn)(void);

extern cecs_fn cecs_internal_init_funs[CECS_MAX_FUNS];
extern unsigned cecs_internal_init_count;
extern cecs_fn cecs_internal_update_funs[CECS_MAX_FUNS];
extern unsigned cecs_internal_update_count;
extern cecs_fn cecs_internal_exit_funs[CECS_MAX_FUNS];
extern unsigned cecs_internal_exit_count;

EntityId cecs_internal_spawn(void);
void *cecs_internal_add(EntityId e, const char *type_name, size_t size);
void *cecs_internal_get(EntityId e, const char *type_name);
// Internal use only: out must be non-NULL; out_ents may be NULL.
unsigned cecs_internal_query(const char *type_name, void **out, EntityId *out_ents, unsigned cap);
void cecs_run(void);
void cecs_quit(void);

#define CECS_INTERNAL_PASTE2(a, b) a##b
#define CECS_INTERNAL_PASTE(a, b) CECS_INTERNAL_PASTE2(a, b)

// Macro DSL caps: spawn at 8 components, query at 4 components per call.
// query requires odd arity: count + N*(type, name).
#define CECS_INTERNAL_NARGS(...) \
  CECS_INTERNAL_NARGS_(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define CECS_INTERNAL_NARGS_(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N

#define CECS_INTERNAL_FE_1(M, T) M(T)
#define CECS_INTERNAL_FE_2(M, T, ...) M(T) CECS_INTERNAL_FE_1(M, __VA_ARGS__)
#define CECS_INTERNAL_FE_3(M, T, ...) M(T) CECS_INTERNAL_FE_2(M, __VA_ARGS__)
#define CECS_INTERNAL_FE_4(M, T, ...) M(T) CECS_INTERNAL_FE_3(M, __VA_ARGS__)
#define CECS_INTERNAL_FE_5(M, T, ...) M(T) CECS_INTERNAL_FE_4(M, __VA_ARGS__)
#define CECS_INTERNAL_FE_6(M, T, ...) M(T) CECS_INTERNAL_FE_5(M, __VA_ARGS__)
#define CECS_INTERNAL_FE_7(M, T, ...) M(T) CECS_INTERNAL_FE_6(M, __VA_ARGS__)
#define CECS_INTERNAL_FE_8(M, T, ...) M(T) CECS_INTERNAL_FE_7(M, __VA_ARGS__)
#define CECS_INTERNAL_FOREACH(M, ...) \
  CECS_INTERNAL_PASTE(CECS_INTERNAL_FE_, CECS_INTERNAL_NARGS(__VA_ARGS__))(M, __VA_ARGS__)

#define CECS_INTERNAL_EMIT(X) X;

#define component(T, ...) \
  (*(T *)cecs_internal_add(_e, #T, sizeof(T)) = (T){__VA_ARGS__})

#define spawn(...) do { \
    EntityId _e = cecs_internal_spawn(); \
    CECS_INTERNAL_FOREACH(CECS_INTERNAL_EMIT, __VA_ARGS__) \
  } while (0)

#define CECS_INTERNAL_Q_BUF(T, N) T *N[CECS_MAX_PER_TYPE];

#define query(...) CECS_INTERNAL_PASTE(CECS_INTERNAL_Q_, CECS_INTERNAL_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define CECS_INTERNAL_Q_3(C, T1, N1) \
  CECS_INTERNAL_Q_BUF(T1, N1) \
  unsigned C = cecs_internal_query(#T1, (void **)N1, NULL, CECS_MAX_PER_TYPE)

#define CECS_INTERNAL_Q_5(C, T1, N1, T2, N2) \
  CECS_INTERNAL_Q_BUF(T1, N1) CECS_INTERNAL_Q_BUF(T2, N2) \
  EntityId _##C##_ents[CECS_MAX_PER_TYPE]; \
  unsigned _##C##_raw = cecs_internal_query(#T1, (void **)N1, _##C##_ents, CECS_MAX_PER_TYPE); \
  unsigned C = 0; \
  for (unsigned _##C##_ci = 0; _##C##_ci < _##C##_raw; _##C##_ci++) { \
    T2 *_##N2##_p = (T2 *)cecs_internal_get(_##C##_ents[_##C##_ci], #T2); if (!_##N2##_p) continue; \
    N1[C] = N1[_##C##_ci]; \
    N2[C] = _##N2##_p; \
    C++; \
  }

#define CECS_INTERNAL_Q_7(C, T1, N1, T2, N2, T3, N3) \
  CECS_INTERNAL_Q_BUF(T1, N1) CECS_INTERNAL_Q_BUF(T2, N2) CECS_INTERNAL_Q_BUF(T3, N3) \
  EntityId _##C##_ents[CECS_MAX_PER_TYPE]; \
  unsigned _##C##_raw = cecs_internal_query(#T1, (void **)N1, _##C##_ents, CECS_MAX_PER_TYPE); \
  unsigned C = 0; \
  for (unsigned _##C##_ci = 0; _##C##_ci < _##C##_raw; _##C##_ci++) { \
    T2 *_##N2##_p = (T2 *)cecs_internal_get(_##C##_ents[_##C##_ci], #T2); if (!_##N2##_p) continue; \
    T3 *_##N3##_p = (T3 *)cecs_internal_get(_##C##_ents[_##C##_ci], #T3); if (!_##N3##_p) continue; \
    N1[C] = N1[_##C##_ci]; \
    N2[C] = _##N2##_p; \
    N3[C] = _##N3##_p; \
    C++; \
  }

#define CECS_INTERNAL_Q_9(C, T1, N1, T2, N2, T3, N3, T4, N4) \
  CECS_INTERNAL_Q_BUF(T1, N1) CECS_INTERNAL_Q_BUF(T2, N2) CECS_INTERNAL_Q_BUF(T3, N3) CECS_INTERNAL_Q_BUF(T4, N4) \
  EntityId _##C##_ents[CECS_MAX_PER_TYPE]; \
  unsigned _##C##_raw = cecs_internal_query(#T1, (void **)N1, _##C##_ents, CECS_MAX_PER_TYPE); \
  unsigned C = 0; \
  for (unsigned _##C##_ci = 0; _##C##_ci < _##C##_raw; _##C##_ci++) { \
    T2 *_##N2##_p = (T2 *)cecs_internal_get(_##C##_ents[_##C##_ci], #T2); if (!_##N2##_p) continue; \
    T3 *_##N3##_p = (T3 *)cecs_internal_get(_##C##_ents[_##C##_ci], #T3); if (!_##N3##_p) continue; \
    T4 *_##N4##_p = (T4 *)cecs_internal_get(_##C##_ents[_##C##_ci], #T4); if (!_##N4##_p) continue; \
    N1[C] = N1[_##C##_ci]; \
    N2[C] = _##N2##_p; \
    N3[C] = _##N3##_p; \
    N4[C] = _##N4##_p; \
    C++; \
  }

#define CECS_INTERNAL_DEFINE(_list, _count, _n, ...) \
  static void CECS_INTERNAL_PASTE(cecs_internal_fun_, _n)(void) { \
    __VA_ARGS__ \
  } \
  __attribute__((constructor)) \
  static void CECS_INTERNAL_PASTE(cecs_internal_reg_, _n)(void) { \
    _list[_count++] = CECS_INTERNAL_PASTE(cecs_internal_fun_, _n); \
  }

// Limitation: two cecs_* invocations on the same source line collide.
// __LINE__ chosen over __COUNTER__ to keep -Wpedantic clean under C23.
// cecs_init runs once at startup; cecs_update runs each tick until cecs_quit();
// cecs_exit runs once after the loop ends. Each may be defined multiple times.
#define cecs_init(...) \
  CECS_INTERNAL_DEFINE(cecs_internal_init_funs, cecs_internal_init_count, __LINE__, __VA_ARGS__)

#define cecs_update(...) \
  CECS_INTERNAL_DEFINE(cecs_internal_update_funs, cecs_internal_update_count, __LINE__, __VA_ARGS__)

#define cecs_exit(...) \
  CECS_INTERNAL_DEFINE(cecs_internal_exit_funs, cecs_internal_exit_count, __LINE__, __VA_ARGS__)

#endif // CECS_H

// Define CECS_IMPLEMENTATION in exactly one translation unit before including
// this header to compile the library implementation into that TU.
#ifdef CECS_IMPLEMENTATION
#ifndef CECS_IMPLEMENTATION_DEFINED
#define CECS_IMPLEMENTATION_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
  #include <windows.h>
  static void cecs_internal_sleep_seconds(unsigned s) { Sleep(s * 1000u); }
#else
  #include <unistd.h>
  static void cecs_internal_sleep_seconds(unsigned s) { sleep(s); }
#endif

#define CECS_INTERNAL_INVALID ((unsigned)-1)

cecs_fn cecs_internal_init_funs[CECS_MAX_FUNS];
unsigned cecs_internal_init_count = 0;
cecs_fn cecs_internal_update_funs[CECS_MAX_FUNS];
unsigned cecs_internal_update_count = 0;
cecs_fn cecs_internal_exit_funs[CECS_MAX_FUNS];
unsigned cecs_internal_exit_count = 0;
static int cecs_internal_should_quit = 0;

void cecs_quit(void) { cecs_internal_should_quit = 1; }

typedef struct {
  const char *type_name;
  size_t stride;
  unsigned count;
  void *data;
  unsigned *dense_to_entity;
  unsigned *sparse;
} CecsInternalPool;

static CecsInternalPool cecs_internal_pools[CECS_MAX_COMPONENTS];
static unsigned cecs_internal_pool_count = 0;
static unsigned cecs_internal_next_id = 0;

static CecsInternalPool *cecs_internal_find_pool(const char *type_name) {
  for (unsigned i = 0; i < cecs_internal_pool_count; i++) {
    if (strcmp(cecs_internal_pools[i].type_name, type_name) == 0) {
      return &cecs_internal_pools[i];
    }
  }
  return NULL;
}

static CecsInternalPool *cecs_internal_get_or_create_pool(const char *type_name, size_t stride) {
  CecsInternalPool *p = cecs_internal_find_pool(type_name);
  if (p) {
    if (p->stride != stride) {
      fprintf(stderr, "cecs: stride mismatch for type %s (have %zu, requested %zu)\n",
              type_name, p->stride, stride);
      exit(1);
    }
    return p;
  }
  if (cecs_internal_pool_count >= CECS_MAX_COMPONENTS) {
    fprintf(stderr, "cecs: too many component types (max %d)\n", CECS_MAX_COMPONENTS);
    exit(1);
  }
  p = &cecs_internal_pools[cecs_internal_pool_count++];
  p->type_name = type_name;
  p->stride = stride;
  p->count = 0;
  p->data = calloc(CECS_MAX_PER_TYPE, stride);
  p->dense_to_entity = calloc(CECS_MAX_PER_TYPE, sizeof(unsigned));
  p->sparse = malloc(CECS_MAX_ENTITIES * sizeof(unsigned));
  if (!p->data || !p->dense_to_entity || !p->sparse) {
    fprintf(stderr, "cecs: failed to allocate pool for %s\n", type_name);
    exit(1);
  }
  for (unsigned i = 0; i < CECS_MAX_ENTITIES; i++) {
    p->sparse[i] = CECS_INTERNAL_INVALID;
  }
  return p;
}

EntityId cecs_internal_spawn(void) {
  if (cecs_internal_next_id >= CECS_MAX_ENTITIES) {
    fprintf(stderr, "cecs: too many entities (max %d)\n", CECS_MAX_ENTITIES);
    exit(1);
  }
  return cecs_internal_next_id++;
}

void *cecs_internal_add(EntityId e, const char *type_name, size_t size) {
  CecsInternalPool *p = cecs_internal_get_or_create_pool(type_name, size);
  if (e >= CECS_MAX_ENTITIES) {
    fprintf(stderr, "cecs: entity id %u out of range\n", e);
    exit(1);
  }
  void *slot;
  if (p->sparse[e] != CECS_INTERNAL_INVALID) {
    slot = (char *)p->data + p->sparse[e] * p->stride;
  } else {
    if (p->count >= CECS_MAX_PER_TYPE) {
      fprintf(stderr, "cecs: pool for %s full (max %d)\n", type_name, CECS_MAX_PER_TYPE);
      exit(1);
    }
    unsigned idx = p->count++;
    p->sparse[e] = idx;
    p->dense_to_entity[idx] = e;
    slot = (char *)p->data + idx * p->stride;
  }
  memset(slot, 0, p->stride);
  return slot;
}

void *cecs_internal_get(EntityId e, const char *type_name) {
  CecsInternalPool *p = cecs_internal_find_pool(type_name);
  if (!p || e >= CECS_MAX_ENTITIES || p->sparse[e] == CECS_INTERNAL_INVALID) return NULL;
  return (char *)p->data + p->sparse[e] * p->stride;
}

unsigned cecs_internal_query(const char *type_name, void **out, EntityId *out_ents, unsigned cap) {
  CecsInternalPool *p = cecs_internal_find_pool(type_name);
  if (!p) return 0;
  unsigned n = 0;
  for (unsigned i = 0; i < p->count && n < cap; i++) {
    out[n] = (char *)p->data + i * p->stride;
    if (out_ents) out_ents[n] = p->dense_to_entity[i];
    n++;
  }
  return n;
}

void cecs_run(void) {
  setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
  for (unsigned i = 0; i < cecs_internal_init_count; i++) {
    cecs_internal_init_funs[i]();
  }
  while (!cecs_internal_should_quit) {
    for (unsigned i = 0; i < cecs_internal_update_count; i++) {
      cecs_internal_update_funs[i]();
    }
    cecs_internal_sleep_seconds(1);
  }
  for (unsigned i = 0; i < cecs_internal_exit_count; i++) {
    cecs_internal_exit_funs[i]();
  }
}

#endif // CECS_IMPLEMENTATION_DEFINED
#endif // CECS_IMPLEMENTATION

// Define CECS_CALLBACKS in the same TU as CECS_IMPLEMENTATION to have cecs
// provide main() for you. Pair with cecs_init / cecs_update / cecs_exit.
#ifdef CECS_CALLBACKS
#ifndef CECS_CALLBACKS_DEFINED
#define CECS_CALLBACKS_DEFINED
int main(int argc, char **argv) {
  (void)argc; (void)argv;
  cecs_run();
  return 0;
}
#endif // CECS_CALLBACKS_DEFINED
#endif // CECS_CALLBACKS
