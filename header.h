#ifndef HEADER_H_
#define HEADER_H_

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <macros.h>

#include <types.h>
#include <function.h>
#include <Player.h>
#include <World.h>
#include <Being.h>
#include <Projectile.h>
#include <render.h>
#include <event.h>
#include <enum.h>

float si[ANGLE_PARTS];
float co[ANGLE_PARTS];
World world;
Render_data rend_data;
const Being_type test_being = {
    PLAYER_VELOCITY * 2.5F,
    100,
    2,
    1
};

#endif