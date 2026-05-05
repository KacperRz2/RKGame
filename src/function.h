#ifndef FUNCTION_H_
#define FUNCTION_H_

float RadToDeg(const float);
float SineUnsafe(const float);
float CosiUnsafe(const float);
float SineSafe(const float);
float CosiSafe(const float);
float arctan2(const float, const float);
float GetAngle(const SDL_FPoint* const, const SDL_FPoint* const);
void GetShift(const SDL_FPoint* const, const SDL_FPoint* const, const float, float*, float*);
void GetShiftFromAngle(const float, const float, float*, float*);
void SetSineCosineArrays();
float GetDistanceSquared(const SDL_FPoint* const, const SDL_FPoint* const);

#endif
