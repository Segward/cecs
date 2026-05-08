#include <stdio.h>
#include <cecs/cecs.h>

typedef struct { float x; float y; } Position;
typedef struct { float w; float h; } Size;
typedef struct { char *name; } Player;
typedef struct { char *name; } Camera;

startup({
  spawn(
    component(Position, .x = 1.0f),
    component(Size, .w = 10.0f, .h = 10.0f),
    component(Player, .name = "p1")
  );

  spawn(
    component(Position, .x = 5.0f),
    component(Size, .w = 10.0f, .h = 10.0f),
    component(Player, .name = "p2")
  );

  spawn(
    component(Position, .x = 0.0f, .y = 0.0f),
    component(Camera, .name = "main")
  );
})

update({
  query(count, Position, pos, Player, ply);
  for (unsigned i = 0; i < count; i++) {
    printf("player %s\n", ply[i]->name);
    printf("position %.1f %.1f\n", pos[i]->x, pos[i]->y);
    pos[i]->x += 1.0f;
  }

  query(cam_count, Position, cam_pos, Camera, cam);
  for (unsigned i = 0; i < cam_count; i++) {
    printf("camera %s\n", cam[i]->name);
    printf("position %.1f %.1f\n", cam_pos[i]->x, cam_pos[i]->y);
  }
})

update({
  query(count, Size, size, Player, ply);
  for (unsigned i = 0; i < count; i++) {
    printf("player %s\n", ply[i]->name);
    printf("size %.1f %.1f\n", size[i]->w, size[i]->h);
    size[i]->h += 2.0f;
  }
})

int main(void) {
  cecs_run();
  return 0;
}
