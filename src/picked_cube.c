#include "picked_cube.h"
#include <raymath.h>
#include "chunk.h"
#include "config.h"
#include <rlgl.h>

Ray GetMouseRayFast(Vector2 mouse, Camera camera)
{
    Ray ray = { 0 };

    // Calculate view matrix from camera look at
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);

    Matrix matProj = MatrixIdentity();

    if (camera.projection == CAMERA_PERSPECTIVE)
    {
        // Calculate projection matrix from perspective
        matProj = MatrixPerspective(camera.fovy*DEG2RAD, ((double)GetScreenWidth()/(double)GetScreenHeight()), RL_CULL_DISTANCE_NEAR, RL_CULL_DISTANCE_FAR);
    }

    // Unproject far/near points
    Vector3 nearPoint = Vector3Unproject((Vector3){ 0.0f, 0.0f, 0.0f }, matProj, matView);
    Vector3 farPoint = Vector3Unproject((Vector3){ 0.0f, 0.0f, 1.0f }, matProj, matView);

    // Calculate normalized direction vector
    Vector3 direction = Vector3Normalize(Vector3Subtract(farPoint, nearPoint));

    if (camera.projection == CAMERA_PERSPECTIVE) ray.position = camera.position;

    // Apply calculated vectors to ray
    ray.direction = direction;

    return ray;
}

Vector3 get_picked_cube(Vector3 our_position, Camera camera)
{
    int x, y, z, cube;
    Ray mouse_ray;
    RayCollision mouse_ray_collision;
    Vector3 cube_pos;
    float best_ray_distance = __FLT_MAX__;
    Vector3 ret;
    Vector2 plr_chunk = get_plr_chunk(our_position);

/*
    for (x = 0; x < WORLD_SIZE; x++)
    {
        for (y = 0; y < WORLD_HEIGHT; y++)
        {
            for (z = 0; z < WORLD_SIZE; z++)
            {
                mouse_ray = GetMouseRay((Vector2){RESOLUTION_X / 2, RESOLUTION_Y / 2}, camera);
                mouse_ray_collision = GetRayCollisionBox(mouse_ray, (BoundingBox){Vector3_Add(cube_poses[x][y][z], (Vector3){-0.5, -0.5, -0.5}), Vector3_Add(cube_poses[x][y][z], (Vector3){0.5, 0.5, 0.5})});
                if (!mouse_ray_collision.hit || mouse_ray_collision.distance > best_ray_distance) continue;
                ret = (Vector3){x, y, z};
                best_ray_distance = mouse_ray_collision.distance;
            }
        }
    }*/


    for (x = 0; x < CHUNK_SIZE; x++)
    {
        for (y = 0; y < CHUNK_HEIGHT; y++)
        {
            for (z = 0; z < CHUNK_SIZE; z++)
            {
                if (!(chunks[(int)plr_chunk.x][(int)plr_chunk.y][x][y][z])) continue;
                mouse_ray = GetMouseRayFast((Vector2){RESOLUTION_X / 2, RESOLUTION_Y / 2}, camera);
                cube_pos = (Vector3){CHUNK_SIZE * plr_chunk.x + x, y-GRASS_LEVEL, CHUNK_SIZE * plr_chunk.y + z};
                mouse_ray_collision = GetRayCollisionBox(mouse_ray, (BoundingBox){Vector3Add(cube_pos, (Vector3){-0.5, -0.5, -0.5}), Vector3Add(cube_pos, (Vector3){0.5, 0.5, 0.5})});
                if (!mouse_ray_collision.hit || mouse_ray_collision.distance > best_ray_distance) continue;
                ret = (Vector3){x, y, z};
                best_ray_distance = mouse_ray_collision.distance;
            }
        }
    }

    return ret;
}
