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

extern cecs_fn cecs_internal_startup_funs[CECS_MAX_FUNS];
extern unsigned cecs_internal_startup_count;
extern cecs_fn cecs_internal_update_funs[CECS_MAX_FUNS];
extern unsigned cecs_internal_update_count;

EntityId cecs_internal_spawn(void);
void *cecs_internal_add(EntityId e, const char *type_name, size_t size);
void *cecs_internal_get(EntityId e, const char *type_name);
// Internal use only: out must be non-NULL; out_ents may be NULL.
unsigned cecs_internal_query(const char *type_name, void **out, EntityId *out_ents, unsigned cap);
void cecs_run(void);

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

// Limitation: two startup/update invocations on the same source line collide.
// __LINE__ chosen over __COUNTER__ to keep -Wpedantic clean under C23.
#define startup(...) \
  CECS_INTERNAL_DEFINE(cecs_internal_startup_funs, cecs_internal_startup_count, __LINE__, __VA_ARGS__)

#define update(...) \
  CECS_INTERNAL_DEFINE(cecs_internal_update_funs, cecs_internal_update_count, __LINE__, __VA_ARGS__)

#endif
