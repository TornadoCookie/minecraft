#ifndef _DRAW_WORLD_
#define _DRAW_WORLD_

#include <raylib.h>

void draw_world(Vector3 our_position, int draw_lines, Camera camera, Vector3 picked_cube);

void init_chunk_transforms();
void free_chunk_transforms();

#endif
