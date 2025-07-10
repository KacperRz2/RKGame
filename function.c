#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <function.h>

extern inline float RadToDeg(const float angle) {
	return angle * (180.0F / SDL_PI_F);
}

extern inline float sine(float a){
    static const float tmp = (float)ANGLE_PARTS * 0.5F / SDL_PI_F;
    return *(si + (unsigned int)(a * tmp));
}

extern inline float cosi(float a){
    static const float tmp = (float)ANGLE_PARTS * 0.5F / SDL_PI_F;
    return *(co + (unsigned int)(a * tmp));
}
extern inline float SineSafe(float a){
    static const float tmp = (float)ANGLE_PARTS * 0.5F / SDL_PI_F;
    return *(si + (unsigned int)(a * tmp) % ANGLE_PARTS);
}

extern inline float CosiSafe(float a){
    static const float tmp = (float)ANGLE_PARTS * 0.5F / SDL_PI_F;
    return *(co + (unsigned int)(a * tmp) % ANGLE_PARTS);
}