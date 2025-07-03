#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <function.h>

extern inline float RadToDeg(const float angle) {
	return angle * 180.0F / SDL_PI_F;
}

extern inline bool SetInBounds(float* x, float* y) {
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

extern inline bool InBounds(SDL_FPoint* point) {
    if (point->x < BOUNDS_R && point->x > BOUNDS_L && point->y > BOUNDS_U && point->y < BOUNDS_D) {
        return true;
    }
    return false;
}

extern inline float sine(float a){
    static const float tmp = (float)ANGLE_PARTS * 0.5F / SDL_PI_F;
    int angle_indx = (int)(a * tmp);
	// if(angle_indx < 0) exit(-1); 
	// else if(angle_indx >= ANGLE_PARTS) exit(-1);
    return *(si + angle_indx);
}

extern inline float cosi(float a){
    static const float tmp = (float)ANGLE_PARTS * 0.5F / SDL_PI_F;
    int angle_indx = (int)(a * tmp);
	// if(angle_indx < 0) exit(-1); 
	// else if(angle_indx >= ANGLE_PARTS) exit(-1);
    return *(co + angle_indx);
}
extern inline float sineSafe(float a){
    static const float tmp = (float)ANGLE_PARTS * 0.5F / SDL_PI_F;
    int angle_indx = (int)(a * tmp);
	if(angle_indx < 0) angle_indx += ANGLE_PARTS; 
	else if(angle_indx >= ANGLE_PARTS) angle_indx -= ANGLE_PARTS;
    return *(si + angle_indx);
}

extern inline float cosiSafe(float a){
    static const float tmp = (float)ANGLE_PARTS * 0.5F / SDL_PI_F;
    int angle_indx = (int)(a * tmp);
	if(angle_indx < 0) angle_indx += ANGLE_PARTS; 
	else if(angle_indx >= ANGLE_PARTS) angle_indx -= ANGLE_PARTS;
    return *(co + angle_indx);
}
//inline float distancePowTwo(const float x, const float y) {
//	return SDL_powf(WINDOW_CENTER_X - x, 2.0F) + SDL_powf(WINDOW_CENTER_Y + PLAYER_REND_Y_SHIFT - y, 2.0F);
//}