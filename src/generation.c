#include "generation.h"
#include "config.h"
#include <stdio.h>
#include "cubes.h"
#include "chunk.h"
#include <stdlib.h>
#include <pthread.h>

Image world_image;
Color *world_image_colors;

void generate_chunk(int cx, int cy)
{
    int x, y, z, grass_level;

    grass_level = GRASS_LEVEL;

    for (x = 0; x < CHUNK_SIZE; x++)
    {
        for (y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (z = 0; z < CHUNK_SIZE; z++)
            {
                grass_level = world_image_colors[((cy * CHUNK_SIZE + z) * world_image.width) + cx * CHUNK_SIZE + x].r / 5;
                int cube;
                if (y == 0) cube = CUBE_BEDROCK;
                else if (y > grass_level) cube = CUBE_AIR;
                else if (y == grass_level) cube = CUBE_GRASS;
                else if (y > grass_level - 5) cube = CUBE_DIRT;
                else cube = CUBE_STONE;
                chunks[cx][cy][x][y][z] = cube;
            }
        }
    }
}

void *generate_chunk_thread(Vector2 *arg)
{
    generate_chunk(arg->x, arg->y);
    return NULL;
}

const float bumpiness = 3.0f;

void generate_world(long *generation_progress, long *generation_progress_max)
{
    long x, y;
    Texture2D world_image_tex;

    *generation_progress_max = 2;
    *generation_progress = 1;

    world_image = GenImagePerlinNoise(CHUNK_COUNT * CHUNK_SIZE, CHUNK_COUNT * CHUNK_SIZE, rand() % 4, rand() % 4, bumpiness);

    *generation_progress = 2;
    world_image_colors = LoadImageColors(world_image);

    //ExportImage(world_image, "world_image.png");

    *generation_progress_max = (long)CHUNK_COUNT * CHUNK_COUNT * CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * CHUNK_SIZE;

    pthread_t threads[CHUNK_COUNT][CHUNK_COUNT];
    Vector2 *vectors[CHUNK_COUNT][CHUNK_COUNT];

    for (x = 0; x < CHUNK_COUNT; x++)
    {
        *generation_progress = (x+1) * CHUNK_COUNT * CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * CHUNK_SIZE;
        for (y = 0; y < CHUNK_COUNT; y++)
        {
            Vector2 *vec = malloc(sizeof(Vector2));
            vec->x = x;
            vec->y = y;
            pthread_create(&(threads[x][y]), NULL, generate_chunk_thread, vec);
            vectors[x][y] = vec;
        }
    }

    for (x = 0; x < CHUNK_COUNT; x++)
    {
        for (y = 0; y < CHUNK_COUNT; y++)
        {
            pthread_join(threads[x][y], NULL);
            free(vectors[x][y]);
        }
    }

    UnloadImageColors(world_image_colors);
    UnloadImage(world_image);
}
