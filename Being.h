#ifndef BEIGN_H_
#define BEIGN_H_

void addBeingToSegment(Segment*, Being*);
Being* createBeing(World*, const float, const float);
void removeBeingFromSegment(Being*);
void destroyBeing(Being*);
void destroyBeings(Beings_array*);
void addBeingToArray(Beings_array*, Being*);
void destroyBeingInArray(Beings_array*, const unsigned int);
void moveBeing(Being*, const float, const float);
void setBeingPosition(Being*, const float, const float);
void moveBeingX(Being*, const float);
void moveBeingY(Being*, const float);
void moveBeingToSegment(Being*, Segment*);
void startBeingWalk(Being*, const int, const float, const float);
void startBeingWalkWithRandTurn(Being*, const int, const float, const float);
void startBeingWalkWithRandTurn45Deg(Being*, const int, const float, const float);
bool resolveBeingCollisionInNewSegment(Being*, Segment*, float*, float*, const float, const float);
void turnBeingWalk(Being*);
void updateBeingWalk(Being*, World*);
void updateBeings(Beings_array*, World*, SDL_FPoint*);
void damageBeing(Being*, int);

#endif