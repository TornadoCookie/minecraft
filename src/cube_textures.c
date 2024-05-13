#include "cube_textures.h"
#include <raylib.h>

void init_cube_textures()
{
    Shader shader;
    int i;
    cube_mesh = GenMeshCube(1, 1, 1);

    shader = LoadShader("shaders/lighting_instancing.vs", "shaders/lighting.fs");
        shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(shader, "instanceTransform");
        int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);
    
    for (i = 0; i < CUBE_N_ENUMS; i++)
    {
        cube_textures[i] = LoadTexture(cube_texture_fnames[i]);
        cube_materials[i] = LoadMaterialDefault();
        cube_materials[i].shader = shader;
        cube_materials[i].maps[MATERIAL_MAP_DIFFUSE].color = cube_colors[i];
        cube_materials[i].maps[MATERIAL_MAP_DIFFUSE].texture = cube_textures[i];
    }
}
