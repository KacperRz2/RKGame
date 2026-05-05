#include <common.h>

float si[ANGLE_PARTS];
float co[ANGLE_PARTS];

extern inline float RadToDeg(const float angle){
	return angle * (180.0F / SDL_PI_F);
}

extern inline float SineUnsafe(const float a){
    return *(si + (unsigned int)(a * RAD_TO_MINE));
}

extern inline float CosiUnsafe(const float a){
    return *(co + (unsigned int)(a * RAD_TO_MINE));
}
extern inline float SineSafe(const float a){
    return *(si + (unsigned int)(a * RAD_TO_MINE) % ANGLE_PARTS);
}

extern inline float CosiSafe(const float a){
    return *(co + (unsigned int)(a * RAD_TO_MINE) % ANGLE_PARTS);
}

extern inline float arctan2(const float y, const float x){
    const float abs_x = SDL_fabsf(x);
    const float abs_y = SDL_fabsf(y);
    const float a = (abs_x > abs_y) ? (abs_y / abs_x) : (abs_x / abs_y);
    float approx = a * SDL_PI_F * 0.25F;
    if(abs_y > abs_x){
        approx = SDL_PI_F * 0.5F - approx;
    }
    if(x < 0.0F){
        approx = SDL_PI_F - approx;
    }
    if(y < 0.0F){
        approx = -approx;
    }
    return approx;
}

extern inline float GetAngle(const SDL_FPoint* const a, const SDL_FPoint* const b){
    return arctan2(b->y - a->y, b->x - a->x) + SDL_PI_F * 0.5F;
}

extern inline void GetShift(const SDL_FPoint* const a, const SDL_FPoint* const b, const float velocity, float* x, float* y){
    const float distance_x = b->x - a->x;
    const float distance_y = b->y - a->y;
    const float distance = SDL_sqrtf(pow2(distance_x) + pow2(distance_y));
    const float velocity_xy = distance / velocity;
    *x = distance_x / velocity_xy;
    *y = distance_y / velocity_xy;
}

extern inline void GetShiftFromAngle(const float angle, const float velocity, float* x, float* y){
    *x = SineSafe(angle) * velocity;
    *y = -CosiSafe(angle) * velocity;
}

void SetSineCosineArrays(){
	for(unsigned int i = 0U; i < ANGLE_PARTS; ++i){
		*(si + i) = SDL_sinf(MIN_ANGLE * i);
		*(co + i) = SDL_cosf(MIN_ANGLE * i);
	}
}

extern inline float GetDistanceSquared(const SDL_FPoint* const point0, const SDL_FPoint* const point1){
    return pow2(point0->x - point1->x) + pow2(point0->y - point1->y);
}
