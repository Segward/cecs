#ifndef CECS_H
#define CECS_H

#include <stddef.h>

#define CECS_MAX_FUNS       256
#define CECS_MAX_COMPONENTS 256
#define CECS_MAX_PER_TYPE   1024
#define CECS_MAX_ENTITIES   1024

typedef unsigned EntityId;
typedef void (*cecs_fun_t)(void);

extern cecs_fun_t _cecs_startup_funs[CECS_MAX_FUNS];
extern unsigned   _cecs_startup_count;
extern cecs_fun_t _cecs_update_funs[CECS_MAX_FUNS];
extern unsigned   _cecs_update_count;

EntityId _cecs_spawn(void);
void    *_cecs_add(EntityId e, const char *type_name, size_t size);
void    *_cecs_get(EntityId e, const char *type_name);
int      _cecs_has(EntityId e, const char *type_name);
unsigned _cecs_query(const char *type_name, void **out, EntityId *out_ents, unsigned cap);
void    *_cecs_query_one(const char *type_name);
void     cecs_run(void);

#define _CECS_PASTE2(a, b) a##b
#define _CECS_PASTE(a, b)  _CECS_PASTE2(a, b)

#define _CECS_NARGS(...) \
  _CECS_NARGS_(__VA_ARGS__, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define _CECS_NARGS_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, N, ...) N

#define _CECS_FE_1(M, T)       M(T)
#define _CECS_FE_2(M, T, ...)  M(T) _CECS_FE_1(M, __VA_ARGS__)
#define _CECS_FE_3(M, T, ...)  M(T) _CECS_FE_2(M, __VA_ARGS__)
#define _CECS_FE_4(M, T, ...)  M(T) _CECS_FE_3(M, __VA_ARGS__)
#define _CECS_FE_5(M, T, ...)  M(T) _CECS_FE_4(M, __VA_ARGS__)
#define _CECS_FE_6(M, T, ...)  M(T) _CECS_FE_5(M, __VA_ARGS__)
#define _CECS_FE_7(M, T, ...)  M(T) _CECS_FE_6(M, __VA_ARGS__)
#define _CECS_FE_8(M, T, ...)  M(T) _CECS_FE_7(M, __VA_ARGS__)
#define _CECS_FE_9(M, T, ...)  M(T) _CECS_FE_8(M, __VA_ARGS__)
#define _CECS_FE_10(M, T, ...) M(T) _CECS_FE_9(M, __VA_ARGS__)
#define _CECS_FE_11(M, T, ...) M(T) _CECS_FE_10(M, __VA_ARGS__)
#define _CECS_FE_12(M, T, ...) M(T) _CECS_FE_11(M, __VA_ARGS__)
#define _CECS_FE_13(M, T, ...) M(T) _CECS_FE_12(M, __VA_ARGS__)
#define _CECS_FE_14(M, T, ...) M(T) _CECS_FE_13(M, __VA_ARGS__)
#define _CECS_FE_15(M, T, ...) M(T) _CECS_FE_14(M, __VA_ARGS__)
#define _CECS_FE_16(M, T, ...) M(T) _CECS_FE_15(M, __VA_ARGS__)
#define _CECS_FOREACH(M, ...) \
  _CECS_PASTE(_CECS_FE_, _CECS_NARGS(__VA_ARGS__))(M, __VA_ARGS__)

#define _CECS_EMIT(X) X;

#define component(T, ...) \
  (*(T *)_cecs_add(_e, #T, sizeof(T)) = (T){__VA_ARGS__})

#define spawn(...) do { \
    EntityId _e = _cecs_spawn(); \
    _CECS_FOREACH(_CECS_EMIT, __VA_ARGS__) \
  } while (0)

#define add(E, T) ((T *)_cecs_add((E), #T, sizeof(T)))
#define get(E, T) ((T *)_cecs_get((E), #T))
#define has(E, T) (_cecs_has((E), #T))

#define single(TYPE, NAME) \
  for (TYPE *NAME = (TYPE *)_cecs_query_one(#TYPE); NAME != NULL; NAME = NULL)

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

#define _CECS_Q_11(C, T1, N1, T2, N2, T3, N3, T4, N4, T5, N5) \
  _CECS_Q_BUF(T1, N1) _CECS_Q_BUF(T2, N2) _CECS_Q_BUF(T3, N3) _CECS_Q_BUF(T4, N4) _CECS_Q_BUF(T5, N5) \
  EntityId _##C##_ents[CECS_MAX_PER_TYPE]; \
  unsigned _##C##_raw = _cecs_query(#T1, (void **)N1, _##C##_ents, CECS_MAX_PER_TYPE); \
  unsigned C = 0; \
  for (unsigned _##C##_ci = 0; _##C##_ci < _##C##_raw; _##C##_ci++) { \
    T2 *_##N2##_p = (T2 *)_cecs_get(_##C##_ents[_##C##_ci], #T2); if (!_##N2##_p) continue; \
    T3 *_##N3##_p = (T3 *)_cecs_get(_##C##_ents[_##C##_ci], #T3); if (!_##N3##_p) continue; \
    T4 *_##N4##_p = (T4 *)_cecs_get(_##C##_ents[_##C##_ci], #T4); if (!_##N4##_p) continue; \
    T5 *_##N5##_p = (T5 *)_cecs_get(_##C##_ents[_##C##_ci], #T5); if (!_##N5##_p) continue; \
    N1[C] = N1[_##C##_ci]; \
    N2[C] = _##N2##_p; \
    N3[C] = _##N3##_p; \
    N4[C] = _##N4##_p; \
    N5[C] = _##N5##_p; \
    C++; \
  }

#define _CECS_Q_13(C, T1, N1, T2, N2, T3, N3, T4, N4, T5, N5, T6, N6) \
  _CECS_Q_BUF(T1, N1) _CECS_Q_BUF(T2, N2) _CECS_Q_BUF(T3, N3) _CECS_Q_BUF(T4, N4) _CECS_Q_BUF(T5, N5) _CECS_Q_BUF(T6, N6) \
  EntityId _##C##_ents[CECS_MAX_PER_TYPE]; \
  unsigned _##C##_raw = _cecs_query(#T1, (void **)N1, _##C##_ents, CECS_MAX_PER_TYPE); \
  unsigned C = 0; \
  for (unsigned _##C##_ci = 0; _##C##_ci < _##C##_raw; _##C##_ci++) { \
    T2 *_##N2##_p = (T2 *)_cecs_get(_##C##_ents[_##C##_ci], #T2); if (!_##N2##_p) continue; \
    T3 *_##N3##_p = (T3 *)_cecs_get(_##C##_ents[_##C##_ci], #T3); if (!_##N3##_p) continue; \
    T4 *_##N4##_p = (T4 *)_cecs_get(_##C##_ents[_##C##_ci], #T4); if (!_##N4##_p) continue; \
    T5 *_##N5##_p = (T5 *)_cecs_get(_##C##_ents[_##C##_ci], #T5); if (!_##N5##_p) continue; \
    T6 *_##N6##_p = (T6 *)_cecs_get(_##C##_ents[_##C##_ci], #T6); if (!_##N6##_p) continue; \
    N1[C] = N1[_##C##_ci]; \
    N2[C] = _##N2##_p; \
    N3[C] = _##N3##_p; \
    N4[C] = _##N4##_p; \
    N5[C] = _##N5##_p; \
    N6[C] = _##N6##_p; \
    C++; \
  }

#define _CECS_Q_15(C, T1, N1, T2, N2, T3, N3, T4, N4, T5, N5, T6, N6, T7, N7) \
  _CECS_Q_BUF(T1, N1) _CECS_Q_BUF(T2, N2) _CECS_Q_BUF(T3, N3) _CECS_Q_BUF(T4, N4) _CECS_Q_BUF(T5, N5) _CECS_Q_BUF(T6, N6) _CECS_Q_BUF(T7, N7) \
  EntityId _##C##_ents[CECS_MAX_PER_TYPE]; \
  unsigned _##C##_raw = _cecs_query(#T1, (void **)N1, _##C##_ents, CECS_MAX_PER_TYPE); \
  unsigned C = 0; \
  for (unsigned _##C##_ci = 0; _##C##_ci < _##C##_raw; _##C##_ci++) { \
    T2 *_##N2##_p = (T2 *)_cecs_get(_##C##_ents[_##C##_ci], #T2); if (!_##N2##_p) continue; \
    T3 *_##N3##_p = (T3 *)_cecs_get(_##C##_ents[_##C##_ci], #T3); if (!_##N3##_p) continue; \
    T4 *_##N4##_p = (T4 *)_cecs_get(_##C##_ents[_##C##_ci], #T4); if (!_##N4##_p) continue; \
    T5 *_##N5##_p = (T5 *)_cecs_get(_##C##_ents[_##C##_ci], #T5); if (!_##N5##_p) continue; \
    T6 *_##N6##_p = (T6 *)_cecs_get(_##C##_ents[_##C##_ci], #T6); if (!_##N6##_p) continue; \
    T7 *_##N7##_p = (T7 *)_cecs_get(_##C##_ents[_##C##_ci], #T7); if (!_##N7##_p) continue; \
    N1[C] = N1[_##C##_ci]; \
    N2[C] = _##N2##_p; \
    N3[C] = _##N3##_p; \
    N4[C] = _##N4##_p; \
    N5[C] = _##N5##_p; \
    N6[C] = _##N6##_p; \
    N7[C] = _##N7##_p; \
    C++; \
  }

#define _CECS_Q_17(C, T1, N1, T2, N2, T3, N3, T4, N4, T5, N5, T6, N6, T7, N7, T8, N8) \
  _CECS_Q_BUF(T1, N1) _CECS_Q_BUF(T2, N2) _CECS_Q_BUF(T3, N3) _CECS_Q_BUF(T4, N4) _CECS_Q_BUF(T5, N5) _CECS_Q_BUF(T6, N6) _CECS_Q_BUF(T7, N7) _CECS_Q_BUF(T8, N8) \
  EntityId _##C##_ents[CECS_MAX_PER_TYPE]; \
  unsigned _##C##_raw = _cecs_query(#T1, (void **)N1, _##C##_ents, CECS_MAX_PER_TYPE); \
  unsigned C = 0; \
  for (unsigned _##C##_ci = 0; _##C##_ci < _##C##_raw; _##C##_ci++) { \
    T2 *_##N2##_p = (T2 *)_cecs_get(_##C##_ents[_##C##_ci], #T2); if (!_##N2##_p) continue; \
    T3 *_##N3##_p = (T3 *)_cecs_get(_##C##_ents[_##C##_ci], #T3); if (!_##N3##_p) continue; \
    T4 *_##N4##_p = (T4 *)_cecs_get(_##C##_ents[_##C##_ci], #T4); if (!_##N4##_p) continue; \
    T5 *_##N5##_p = (T5 *)_cecs_get(_##C##_ents[_##C##_ci], #T5); if (!_##N5##_p) continue; \
    T6 *_##N6##_p = (T6 *)_cecs_get(_##C##_ents[_##C##_ci], #T6); if (!_##N6##_p) continue; \
    T7 *_##N7##_p = (T7 *)_cecs_get(_##C##_ents[_##C##_ci], #T7); if (!_##N7##_p) continue; \
    T8 *_##N8##_p = (T8 *)_cecs_get(_##C##_ents[_##C##_ci], #T8); if (!_##N8##_p) continue; \
    N1[C] = N1[_##C##_ci]; \
    N2[C] = _##N2##_p; \
    N3[C] = _##N3##_p; \
    N4[C] = _##N4##_p; \
    N5[C] = _##N5##_p; \
    N6[C] = _##N6##_p; \
    N7[C] = _##N7##_p; \
    N8[C] = _##N8##_p; \
    C++; \
  }

#define stream(C, ARR, ITEM, ...) \
  for (unsigned _##ITEM##_i = 0; _##ITEM##_i < (C); _##ITEM##_i++) { \
    __typeof__((ARR)[0]) ITEM = (ARR)[_##ITEM##_i]; \
    __VA_ARGS__ \
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
