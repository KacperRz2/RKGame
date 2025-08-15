#ifndef HEADER_H_
#define HEADER_H_

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <macros.h>

#include <types.h>
#include <enum.h>
#include <render.h>
#include <game.h>

float si[ANGLE_PARTS];
float co[ANGLE_PARTS];
const Being_type test_being = {
    TEST_BEING_SIZE,
    TEST_BEING_VELOCITY,
    TEST_BEING_HP,
    TEST_BEING_DMG_CLOSE,
    TEST_BEING_DMG_FAR
};

#endif