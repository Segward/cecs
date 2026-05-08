#ifndef CECS_H
#define CECS_H

#include <stddef.h>

#define CECS_MAX_FUNS 256
#define CECS_MAX_COMPONENTS 256
#define CECS_MAX_PER_TYPE 1024
#define CECS_MAX_ENTITIES 1024

typedef unsigned EntityId;
typedef void (*cecs_fun_t)(void);

extern cecs_fun_t _cecs_startup_funs[CECS_MAX_FUNS];
extern unsigned _cecs_startup_count;
extern cecs_fun_t _cecs_update_funs[CECS_MAX_FUNS];
extern unsigned _cecs_update_count;

EntityId _cecs_spawn(void);
void *_cecs_add(EntityId e, const char *type_name, size_t size);
void *_cecs_get(EntityId e, const char *type_name);
unsigned _cecs_query(const char *type_name, void **out, EntityId *out_ents, unsigned cap);
void cecs_run(void);

#define _CECS_PASTE2(a, b) a##b
#define _CECS_PASTE(a, b) _CECS_PASTE2(a, b)

#define _CECS_NARGS(...) \
  _CECS_NARGS_(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define _CECS_NARGS_(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N

#define _CECS_FE_1(M, T) M(T)
#define _CECS_FE_2(M, T, ...) M(T) _CECS_FE_1(M, __VA_ARGS__)
#define _CECS_FE_3(M, T, ...) M(T) _CECS_FE_2(M, __VA_ARGS__)
#define _CECS_FE_4(M, T, ...) M(T) _CECS_FE_3(M, __VA_ARGS__)
#define _CECS_FE_5(M, T, ...) M(T) _CECS_FE_4(M, __VA_ARGS__)
#define _CECS_FE_6(M, T, ...) M(T) _CECS_FE_5(M, __VA_ARGS__)
#define _CECS_FE_7(M, T, ...) M(T) _CECS_FE_6(M, __VA_ARGS__)
#define _CECS_FE_8(M, T, ...) M(T) _CECS_FE_7(M, __VA_ARGS__)
#define _CECS_FOREACH(M, ...) \
  _CECS_PASTE(_CECS_FE_, _CECS_NARGS(__VA_ARGS__))(M, __VA_ARGS__)

#define _CECS_EMIT(X) X;

#define component(T, ...) \
  (*(T *)_cecs_add(_e, #T, sizeof(T)) = (T){__VA_ARGS__})

#define spawn(...) do { \
    EntityId _e = _cecs_spawn(); \
    _CECS_FOREACH(_CECS_EMIT, __VA_ARGS__) \
  } while (0)

#define _CECS_Q_BUF(T, N) T *N[CECS_MAX_PER_TYPE];

#define query(...) _CECS_PASTE(_CECS_Q_, _CECS_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define _CECS_Q_3(C, T1, N1) \
  _CECS_Q_BUF(T1, N1) \
  EntityId _##C##_ents[CECS_MAX_PER_TYPE]; \
  unsigned C = _cecs_query(#T1, (void **)N1, _##C##_ents, CECS_MAX_PER_TYPE)

#define _CECS_Q_5(C, T1, N1, T2, N2) \
  _CECS_Q_BUF(T1, N1) _CECS_Q_BUF(T2, N2) \
  EntityId _##C##_ents[CECS_MAX_PER_TYPE]; \
  unsigned _##C##_raw = _cecs_query(#T1, (void **)N1, _##C##_ents, CECS_MAX_PER_TYPE); \
  unsigned C = 0; \
  for (unsigned _##C##_ci = 0; _##C##_ci < _##C##_raw; _##C##_ci++) { \
    T2 *_##N2##_p = (T2 *)_cecs_get(_##C##_ents[_##C##_ci], #T2); if (!_##N2##_p) continue; \
    N1[C] = N1[_##C##_ci]; \
    N2[C] = _##N2##_p; \
    C++; \
  }

#define _CECS_Q_7(C, T1, N1, T2, N2, T3, N3) \
  _CECS_Q_BUF(T1, N1) _CECS_Q_BUF(T2, N2) _CECS_Q_BUF(T3, N3) \
  EntityId _##C##_ents[CECS_MAX_PER_TYPE]; \
  unsigned _##C##_raw = _cecs_query(#T1, (void **)N1, _##C##_ents, CECS_MAX_PER_TYPE); \
  unsigned C = 0; \
  for (unsigned _##C##_ci = 0; _##C##_ci < _##C##_raw; _##C##_ci++) { \
    T2 *_##N2##_p = (T2 *)_cecs_get(_##C##_ents[_##C##_ci], #T2); if (!_##N2##_p) continue; \
    T3 *_##N3##_p = (T3 *)_cecs_get(_##C##_ents[_##C##_ci], #T3); if (!_##N3##_p) continue; \
    N1[C] = N1[_##C##_ci]; \
    N2[C] = _##N2##_p; \
    N3[C] = _##N3##_p; \
    C++; \
  }

#define _CECS_Q_9(C, T1, N1, T2, N2, T3, N3, T4, N4) \
  _CECS_Q_BUF(T1, N1) _CECS_Q_BUF(T2, N2) _CECS_Q_BUF(T3, N3) _CECS_Q_BUF(T4, N4) \
  EntityId _##C##_ents[CECS_MAX_PER_TYPE]; \
  unsigned _##C##_raw = _cecs_query(#T1, (void **)N1, _##C##_ents, CECS_MAX_PER_TYPE); \
  unsigned C = 0; \
  for (unsigned _##C##_ci = 0; _##C##_ci < _##C##_raw; _##C##_ci++) { \
    T2 *_##N2##_p = (T2 *)_cecs_get(_##C##_ents[_##C##_ci], #T2); if (!_##N2##_p) continue; \
    T3 *_##N3##_p = (T3 *)_cecs_get(_##C##_ents[_##C##_ci], #T3); if (!_##N3##_p) continue; \
    T4 *_##N4##_p = (T4 *)_cecs_get(_##C##_ents[_##C##_ci], #T4); if (!_##N4##_p) continue; \
    N1[C] = N1[_##C##_ci]; \
    N2[C] = _##N2##_p; \
    N3[C] = _##N3##_p; \
    N4[C] = _##N4##_p; \
    C++; \
  }

#define _CECS_DEFINE(_list, _count, _n, ...) \
  static void _CECS_PASTE(_cecs_fun_, _n)(void) { \
    __VA_ARGS__ \
  } \
  __attribute__((constructor)) \
  static void _CECS_PASTE(_cecs_reg_, _n)(void) { \
    _list[_count++] = _CECS_PASTE(_cecs_fun_, _n); \
  }

#define startup(...) \
  _CECS_DEFINE(_cecs_startup_funs, _cecs_startup_count, __LINE__, __VA_ARGS__)

#define update(...) \
  _CECS_DEFINE(_cecs_update_funs, _cecs_update_count, __LINE__, __VA_ARGS__)

#endif
