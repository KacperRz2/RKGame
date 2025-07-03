#ifndef FUNCTION_H_
#define FUNCTION_H_

extern float si[];
extern float co[];

float RadToDeg(const float);
bool SetInBounds(float*, float*);
bool InBounds(SDL_FPoint*);
float sine(float);
float cosi(float);
float sineSafe(float);
float cosiSafe(float);

#endif