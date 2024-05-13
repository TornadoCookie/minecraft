#include <raylib.h>
#include <math.h> 
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <raymath.h>
#include <string.h>
#include <rlgl.h>
#include <time.h>
#include "config.h"
#include "cubes.h"
#include "chunk.h"
#include "generation.h"
#include "cube_textures.h"
#include "draw_world.h"
#include "picked_cube.h"
#include "clouds.h"
#include <pthread.h>

int RESOLUTION_X = 640, RESOLUTION_Y = 480;
bool in_fullscreen = false;

#define TEXTURE_SIZE 16

const Color cube_colors[CUBE_N_ENUMS] = {
    [CUBE_AIR] = SKYBLUE,
    [CUBE_GRASS] = GREEN,
    [CUBE_STONE] = GRAY,
    [CUBE_DIRT] = BROWN,
    [CUBE_BEDROCK] = BLACK
};

Chunk chunks[CHUNK_COUNT][CHUNK_COUNT];

Texture cube_textures[CUBE_N_ENUMS];
const char *cube_texture_fnames[CUBE_N_ENUMS] = {
    [CUBE_AIR] = "assets/cube_air.png",
    [CUBE_GRASS] = "assets/cube_grass.png",
    [CUBE_STONE] = "assets/cube_stone.png",
    [CUBE_DIRT] = "assets/cube_dirt.png",
    [CUBE_BEDROCK] = "assets/cube_bedrock.png"
};

Material cube_materials[CUBE_N_ENUMS];
Mesh cube_mesh;

Vector3 get_cube_position(int cx, int cy, int x, int y, int z)
{
    return (Vector3){cx * CHUNK_SIZE + x, y-GRASS_LEVEL, cy * CHUNK_SIZE + z};
}

void init_textures()
{
    init_cube_textures();
    init_cloud_textures();
}

Vector3 TranslateFromMatrix(Matrix matrix)
{
    return (Vector3){matrix.m12, matrix.m13, matrix.m14};
}

bool generation_done = false;
long generation_progress;
long generation_progress_max;

void *generation_thread(void *__unused)
{
    InitWindow(640, 480, "Loading minecraft...");
    while (!generation_done && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GRAY);
        DrawText(TextFormat("Generating world... (%ld/%ld)", generation_progress, generation_progress_max), 0, 0, 20, WHITE);
        DrawRectangle(10, 30, 500, 20, BLACK);
        DrawRectangle(10, 30, ((float)generation_progress / generation_progress_max) * 500, 20, WHITE);
        EndDrawing();
    }
    if (WindowShouldClose())
    {
        exit(EXIT_SUCCESS);
    }
    CloseWindow();
}

int main()
{
    Camera camera = {0};
    int cameraMode = CAMERA_FIRST_PERSON, ambientLoc, prev_resolution_x = RESOLUTION_X, prev_resolution_y = RESOLUTION_Y;
    Vector3 picked_cube;
    Vector2 current_chunk;
    Shader shader;
    float deltaTime;
    float speed_multiplier = 1.0f;
    clock_t frame_start;
    bool cursor_enabled = false;
    pthread_t new_thread;
    
    srand(time(NULL));

    pthread_create(&new_thread, NULL, generation_thread, NULL);
    generate_world(&generation_progress, &generation_progress_max);
    generation_done = true;
    pthread_join(new_thread, NULL);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(RESOLUTION_X, RESOLUTION_Y, "Minecraft");

    init_textures();

    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    /* Camera position */
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      /* Camera looking at point */
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          /* Camera up vector (rotation towards target) */
    camera.fovy = 60.0f;                                /* Camera field-of-view Y */
    camera.projection = CAMERA_PERSPECTIVE;

    DisableCursor(); 
    //SetTargetFPS(60);

    init_chunk_transforms();

    while (!WindowShouldClose())
    {
        frame_start = clock();
        if (IsKeyPressed(KEY_V))
        {
            cameraMode = cameraMode == CAMERA_FIRST_PERSON ? CAMERA_FREE : CAMERA_FIRST_PERSON;
        }
        if (IsKeyPressed(KEY_F11))
        {
            ToggleBorderlessWindowed();
            if (IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE))
            {
                prev_resolution_x = RESOLUTION_X;
                prev_resolution_y = RESOLUTION_Y;
            }
            else
            {
                RESOLUTION_X = prev_resolution_x;
                RESOLUTION_Y = prev_resolution_y;
            }
        }
        if (IsKeyPressed(KEY_F2))
        {
            if (cursor_enabled)
            {
                cursor_enabled = false;
                DisableCursor();
            }
            else
            {
                cursor_enabled = true;
                EnableCursor();
            }
            
        }
        if (IsKeyDown(KEY_LEFT_SHIFT))
            speed_multiplier = 2.0f;
        else
            speed_multiplier = 1.0f;

        current_chunk = get_plr_chunk(camera.position);

        if (cameraMode == CAMERA_FIRST_PERSON)
        {
UpdateCameraPro(&camera,
            (Vector3){
                (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))*0.05f * speed_multiplier * deltaTime -      // Move forward-backward
                (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*0.05f * deltaTime * speed_multiplier,    
                (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*0.05f * deltaTime * speed_multiplier -   // Move right-left
                (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*0.05f * deltaTime * speed_multiplier,
                0.0f                                                // Move up-down
            },
            (Vector3){
                GetMouseDelta().x*0.1f * deltaTime,                            // Rotation: yaw
                GetMouseDelta().y*0.1f * deltaTime,                            // Rotation: pitch
                0.0f                                                // Rotation: roll
            },
            GetMouseWheelMove()*2.0f * deltaTime);                              // Move to target (zoom)
        }
        else
        {
            UpdateCamera(&camera, cameraMode);
        }
                RESOLUTION_X = GetScreenWidth();
                RESOLUTION_Y = GetScreenHeight();
        float camera_pos[3] = {camera.position.x, camera.position.y, camera.position.z};
        BeginDrawing();
        ClearBackground(SKYBLUE);
        BeginMode3D(camera);
        picked_cube = get_picked_cube(camera.position, camera);
        current_chunk = get_plr_chunk(camera.position);
        draw_world(camera.position, 0, camera, picked_cube);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            chunks[(int)current_chunk.x][(int)current_chunk.y][(int)picked_cube.x][(int)picked_cube.y][(int)picked_cube.z] = CUBE_AIR;
        }
        draw_clouds();
        EndMode3D();
        DrawFPS(0,0);
        if (cameraMode == CAMERA_FREE)
        {
            DrawText("noclip ON", 0, 20, 20, BLACK);
        }
        DrawPixel(RESOLUTION_X / 2, RESOLUTION_Y / 2, PINK);
        DrawRectangle(RESOLUTION_X / 2 - 3, RESOLUTION_Y / 2 - 6, 6, 12, PINK);
        DrawRectangle(RESOLUTION_X / 2 - 6, RESOLUTION_Y / 2 - 3, 12, 6, PINK);
        EndDrawing();
        deltaTime = ((float)clock() - (float)frame_start) / 10000;
    }

    free_chunk_transforms();
    
    return 0;
}
