#include <cecs/cecs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
  #include <windows.h>
  static void cecs_sleep_seconds(unsigned s) { Sleep(s * 1000u); }
#else
  #include <unistd.h>
  static void cecs_sleep_seconds(unsigned s) { sleep(s); }
#endif

#define CECS_INVALID ((unsigned)-1)

cecs_fn cecs_internal_startup_funs[CECS_MAX_FUNS];
unsigned cecs_internal_startup_count = 0;
cecs_fn cecs_internal_update_funs[CECS_MAX_FUNS];
unsigned cecs_internal_update_count = 0;

typedef struct {
  const char *type_name;
  size_t stride;
  unsigned count;
  void *data;
  unsigned *dense_to_entity;
  unsigned *sparse;
} CecsPool;

static CecsPool cecs_internal_pools[CECS_MAX_COMPONENTS];
static unsigned cecs_internal_pool_count = 0;
static unsigned cecs_internal_next_id = 0;

static CecsPool *cecs_internal_find_pool(const char *type_name) {
  for (unsigned i = 0; i < cecs_internal_pool_count; i++) {
    if (strcmp(cecs_internal_pools[i].type_name, type_name) == 0) {
      return &cecs_internal_pools[i];
    }
  }
  return NULL;
}

static CecsPool *cecs_internal_get_or_create_pool(const char *type_name, size_t stride) {
  CecsPool *p = cecs_internal_find_pool(type_name);
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
    p->sparse[i] = CECS_INVALID;
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
  CecsPool *p = cecs_internal_get_or_create_pool(type_name, size);
  if (e >= CECS_MAX_ENTITIES) {
    fprintf(stderr, "cecs: entity id %u out of range\n", e);
    exit(1);
  }
  void *slot;
  if (p->sparse[e] != CECS_INVALID) {
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
  CecsPool *p = cecs_internal_find_pool(type_name);
  if (!p || e >= CECS_MAX_ENTITIES || p->sparse[e] == CECS_INVALID) return NULL;
  return (char *)p->data + p->sparse[e] * p->stride;
}

unsigned cecs_internal_query(const char *type_name, void **out, EntityId *out_ents, unsigned cap) {
  CecsPool *p = cecs_internal_find_pool(type_name);
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
  for (unsigned i = 0; i < cecs_internal_startup_count; i++) {
    cecs_internal_startup_funs[i]();
  }
  while (1) {
    for (unsigned i = 0; i < cecs_internal_update_count; i++) {
      cecs_internal_update_funs[i]();
    }
    cecs_sleep_seconds(1);
  }
}
