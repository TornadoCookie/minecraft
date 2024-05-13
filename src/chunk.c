#include "chunk.h"
#include <raylib.h>

Vector2 get_plr_chunk(Vector3 our_position)
{
    return (Vector2){(int)(our_position.x / CHUNK_SIZE), (int)(our_position.z / CHUNK_SIZE)};
}
