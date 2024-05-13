#ifndef _CUBES_
#define _CUBES_

#include <raylib.h>

enum Cube {
    CUBE_AIR,
    CUBE_GRASS,
    CUBE_STONE,
    CUBE_DIRT,
    CUBE_BEDROCK,
    CUBE_N_ENUMS
};

enum CubeNormal {
    CN_TOP,
    CN_BOTTOM,
    CN_LEFT,
    CN_RIGHT,
    CN_FRONT,
    CN_BACK,
    CN_N_ENUMS
};

extern const Color cube_colors[CUBE_N_ENUMS];

#endif
