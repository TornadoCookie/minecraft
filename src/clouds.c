#include "clouds.h"
#include <raylib.h>
#include "config.h"
#include <raymath.h>
#include <rlgl.h>

Texture cloud_texture;
Material cloud_material;
Mesh cloud_mesh;

void init_cloud_textures()
{
    cloud_texture = LoadTexture("assets/clouds.png");
    cloud_mesh = GenMeshPlane(CHUNK_COUNT * CHUNK_SIZE, CHUNK_COUNT * CHUNK_SIZE, 1, 1);
    cloud_material = LoadMaterialDefault();
    cloud_material.maps[MATERIAL_MAP_DIFFUSE].texture = cloud_texture;
}

void draw_clouds()
{
    rlDisableBackfaceCulling();
    DrawMesh(cloud_mesh, cloud_material, MatrixTranslate((CHUNK_COUNT * CHUNK_SIZE) / 2, 100, (CHUNK_COUNT * CHUNK_SIZE) / 2));
    rlEnableBackfaceCulling();
}
