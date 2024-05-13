#ifndef _CUBE_TEXTURES_
#define _CUBE_TEXTURES_

#include "cubes.h"

extern Texture cube_textures[CUBE_N_ENUMS];
extern const char *cube_texture_fnames[CUBE_N_ENUMS];
extern Material cube_materials[CUBE_N_ENUMS];
extern Mesh cube_mesh;

void init_cube_textures();

#endif
