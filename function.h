#ifndef FUNCTION_H_
#define FUNCTION_H_

extern float si[];
extern float co[];

float RadToDeg(const float);
float sine(const float);
float cosi(const float);
float SineSafe(const float);
float CosiSafe(const float);
float arctan2(const float, const float);
void GetShift(const SDL_FPoint* const, const SDL_FPoint* const, const float, float*, float*);
void GetShiftFromAngle(const float, const float, float*, float*);

#endif