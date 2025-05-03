#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>

extern inline float radToDeg(const float angle) {
	return angle * 180.0F / SDL_PI_F;
}

extern inline bool setInBounds(float* x, float* y) {
    bool set = false;
    if (*x > BOUNDS_R - 1.0F) {
        *x = BOUNDS_R - 2.0F;
        set = true;
    }
    else if (*x < BOUNDS_L) {
        *x = BOUNDS_L + 1.0F;
        set = true;
    }
    if (*y > BOUNDS_D - 1.0F) {
        *y = BOUNDS_D - 2.0F;
        set = true;
    }
    else if (*y < BOUNDS_U) {
        *y = BOUNDS_U + 1.0F;
        set = true;
    }
    return set;
}

extern inline bool inBounds(SDL_FPoint* point) {
    if (point->x < BOUNDS_R && point->x > BOUNDS_L && point->y > BOUNDS_U && point->y < BOUNDS_D) {
        return true;
    }
    return false;
}
//inline float distancePowTwo(const float x, const float y) {
//	return SDL_powf(WINDOW_CENTER_X - x, 2.0F) + SDL_powf(WINDOW_CENTER_Y + PLAYER_REND_Y_SHIFT - y, 2.0F);
//}