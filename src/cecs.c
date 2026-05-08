#include <cecs/cecs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CECS_INVALID ((unsigned)-1)

cecs_fun_t _cecs_startup_funs[CECS_MAX_FUNS];
unsigned   _cecs_startup_count = 0;
cecs_fun_t _cecs_update_funs[CECS_MAX_FUNS];
unsigned   _cecs_update_count = 0;

typedef struct {
  const char *type_name;
  size_t      stride;
  unsigned    count;
  void       *data;
  unsigned   *dense_to_entity;
  unsigned   *sparse;
} _cecs_pool_t;

static _cecs_pool_t _cecs_pools[CECS_MAX_COMPONENTS];
static unsigned     _cecs_pool_count = 0;
static unsigned     _cecs_next_id = 0;

static _cecs_pool_t *_cecs_find_pool(const char *type_name) {
  for (unsigned i = 0; i < _cecs_pool_count; i++) {
    if (strcmp(_cecs_pools[i].type_name, type_name) == 0) {
      return &_cecs_pools[i];
    }
  }
  return NULL;
}

static _cecs_pool_t *_cecs_get_or_create_pool(const char *type_name, size_t stride) {
  _cecs_pool_t *p = _cecs_find_pool(type_name);
  if (p) return p;
  if (_cecs_pool_count >= CECS_MAX_COMPONENTS) {
    fprintf(stderr, "cecs: too many component types (max %d)\n", CECS_MAX_COMPONENTS);
    exit(1);
  }
  p = &_cecs_pools[_cecs_pool_count++];
  p->type_name       = type_name;
  p->stride          = stride;
  p->count           = 0;
  p->data            = calloc(CECS_MAX_PER_TYPE, stride);
  p->dense_to_entity = calloc(CECS_MAX_PER_TYPE, sizeof(unsigned));
  p->sparse          = malloc(CECS_MAX_ENTITIES * sizeof(unsigned));
  if (!p->data || !p->dense_to_entity || !p->sparse) {
    fprintf(stderr, "cecs: failed to allocate pool for %s\n", type_name);
    exit(1);
  }
  for (unsigned i = 0; i < CECS_MAX_ENTITIES; i++) {
    p->sparse[i] = CECS_INVALID;
  }
  return p;
}

EntityId _cecs_spawn(void) {
  if (_cecs_next_id >= CECS_MAX_ENTITIES) {
    fprintf(stderr, "cecs: too many entities (max %d)\n", CECS_MAX_ENTITIES);
    exit(1);
  }
  return _cecs_next_id++;
}

void *_cecs_add(EntityId e, const char *type_name, size_t size) {
  _cecs_pool_t *p = _cecs_get_or_create_pool(type_name, size);
  if (e >= CECS_MAX_ENTITIES) {
    fprintf(stderr, "cecs: entity id %u out of range\n", e);
    exit(1);
  }
  if (p->sparse[e] != CECS_INVALID) {
    return (char *)p->data + p->sparse[e] * p->stride;
  }
  if (p->count >= CECS_MAX_PER_TYPE) {
    fprintf(stderr, "cecs: pool for %s full (max %d)\n", type_name, CECS_MAX_PER_TYPE);
    exit(1);
  }
  unsigned idx = p->count++;
  p->sparse[e] = idx;
  p->dense_to_entity[idx] = e;
  void *slot = (char *)p->data + idx * p->stride;
  memset(slot, 0, p->stride);
  return slot;
}

void *_cecs_get(EntityId e, const char *type_name) {
  _cecs_pool_t *p = _cecs_find_pool(type_name);
  if (!p || e >= CECS_MAX_ENTITIES || p->sparse[e] == CECS_INVALID) return NULL;
  return (char *)p->data + p->sparse[e] * p->stride;
}

unsigned _cecs_query(const char *type_name, void **out, EntityId *out_ents, unsigned cap) {
  _cecs_pool_t *p = _cecs_find_pool(type_name);
  if (!p) return 0;
  unsigned n = 0;
  for (unsigned i = 0; i < p->count && n < cap; i++) {
    out[n] = (char *)p->data + i * p->stride;
    out_ents[n] = p->dense_to_entity[i];
    n++;
  }
  return n;
}

void cecs_run(void) {
  setvbuf(stdout, NULL, _IOLBF, 0);
  for (unsigned i = 0; i < _cecs_startup_count; i++) {
    _cecs_startup_funs[i]();
  }
  while (1) {
    for (unsigned i = 0; i < _cecs_update_count; i++) {
      _cecs_update_funs[i]();
    }
    sleep(1);
  }
}
