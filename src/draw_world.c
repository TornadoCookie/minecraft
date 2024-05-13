#include "draw_world.h"
#include "config.h"
#include "cubes.h"
#include <raymath.h>
#include "chunk.h"
#include "cube_textures.h"
#include <stdlib.h>

Matrix *draw_chunk_transforms[CUBE_N_ENUMS];
bool chunk_transforms_generated;
int transform_lengths[CUBE_N_ENUMS];

void generate_matrix_transforms(int cx, int cy)
{
    int x, y, z, i, cube;
    Vector3 cube_pos;
    if (chunk_transforms_generated) return;
    chunk_transforms_generated = true;
    
    for (i = 0; i < CUBE_N_ENUMS; i++)
    {
        transform_lengths[i] = 0;
    }

    for (x = 0; x < CHUNK_SIZE; x++)
    {
        for (y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (z = 0; z < CHUNK_SIZE; z++)
            {
                if (!chunks[cx][cy][x][y][z]) continue;
                cube = chunks[cx][cy][x][y][z];
                draw_chunk_transforms[cube][transform_lengths[cube]] = MatrixTranslate(cx * CHUNK_SIZE + x, y-GRASS_LEVEL, cy * CHUNK_SIZE + z);
                transform_lengths[cube]++;
            }
        }
    }
}

void draw_chunk(int cx, int cy, Vector3 picked_cube, bool we_have_picked_cube, int draw_lines, Vector3 our_position)
{
    int x, y, z, cube, i;
    Color cube_color;
    Vector3 cube_pos;

    /*printf("draw_chunk %d %d\n", cx, cy);*/
    /*
    for (x = 0; x < CHUNK_SIZE; x++)
    {
        for (y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (z = 0; z < CHUNK_SIZE; z++)
            {/*
                cube_color = cube_colors[cube];
                cube_pos = (Vector3){cx * CHUNK_SIZE + x, y-GRASS_LEVEL, cy * CHUNK_SIZE + z};
                if (fuzzy_eq(cube_pos.y, our_position.y, 10)) continue;

                if (we_have_picked_cube && Vector3_Equals(picked_cube, (Vector3){x,y,z})) cube_color = WHITE;

                DrawCube(cube_pos, 1, 1, 1, cube_color);
                if (draw_lines) DrawCubeWires(cube_pos, 1, 1, 1, BLACK);*//*
            }
        }
    }/*
    DrawModel(chunk_models[cx][cy], (Vector3){cx * CHUNK_SIZE, 0, cy * CHUNK_SIZE}, 1.0f, WHITE);
    if (draw_lines)
    {
        DrawModelWires(chunk_models[cx][cy], (Vector3){cx * CHUNK_SIZE, 0, cy * CHUNK_SIZE}, 1.0f, BLACK);
    }*/

    generate_matrix_transforms(cx, cy);
    for (i = 1; i < CUBE_N_ENUMS; i++)
    {
        DrawMeshInstanced(cube_mesh, cube_materials[i], draw_chunk_transforms[i], transform_lengths[i]);
    }
    if (we_have_picked_cube)
    {
        DrawCubeWires(picked_cube, 1, 1, 1, BLACK);
    }

}

#define fuzzy_eq(x, y, epsilon) (abs((x) - (y)) < (epsilon))

void draw_world(Vector3 our_position, int draw_lines, Camera camera, Vector3 picked_cube)
{
    int x, y;
    Vector2 plr_chunk = get_plr_chunk(our_position);
    for (x = 0; x < CHUNK_COUNT; x++)
    {
        if (!fuzzy_eq(x, plr_chunk.x, RENDER_DISTANCE)) continue;
        for (y = 0; y < CHUNK_COUNT; y++)
        {
            if (!fuzzy_eq(y, plr_chunk.y, RENDER_DISTANCE)) continue;
            draw_chunk(x, y, picked_cube, false, draw_lines, our_position);
        }
    }
    DrawCube((Vector3){(CHUNK_COUNT * CHUNK_SIZE) / 2, 0, (CHUNK_COUNT * CHUNK_SIZE) / 2}, 1, 100, 1, WHITE);
    DrawCubeWires(Vector3Add(picked_cube, (Vector3){plr_chunk.x * CHUNK_SIZE, -GRASS_LEVEL, plr_chunk.y * CHUNK_SIZE}), 1, 1, 1, BLACK);
}

void init_chunk_transforms()
{
    int i;

    for (i = 0; i < CUBE_N_ENUMS; i++)
    {
        draw_chunk_transforms[i] = malloc(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE * sizeof(Matrix));
        transform_lengths[i] = 0;
    }
}

void free_chunk_transforms()
{
    int i;

    for (i = 0; i < CUBE_N_ENUMS; i++)
    {
        free(draw_chunk_transforms[i]);
    }
}
