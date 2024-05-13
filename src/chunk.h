#ifndef _CHUNK_
#define _CHUNK_

#include "config.h"
#include <raylib.h>

typedef unsigned char Chunk[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];

extern Chunk chunks[CHUNK_COUNT][CHUNK_COUNT];

Vector2 get_plr_chunk(Vector3 our_position);

#endif
