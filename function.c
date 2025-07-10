#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <function.h>

extern inline float RadToDeg(const float angle) {
	return angle * (180.0F / SDL_PI_F);
}

extern inline float sine(float a){
    return *(si + (unsigned int)(a * RAD_TO_MINE));
}

extern inline float cosi(float a){
    return *(co + (unsigned int)(a * RAD_TO_MINE));
}
extern inline float SineSafe(float a){
    return *(si + (unsigned int)(a * RAD_TO_MINE) % ANGLE_PARTS);
}

extern inline float CosiSafe(float a){
    return *(co + (unsigned int)(a * RAD_TO_MINE) % ANGLE_PARTS);
}