#ifndef BEIGN_H_
#define BEIGN_H_

void AddBeingToSegment(Segment*, Being*);
Being* CreateBeing(World*, const float, const float);
void RemoveBeingFromSegment(Being*);
void DestroyBeing(Being*);
void DestroyBeings(Beings_array*);
void AddBeingToArray(Beings_array*, Being*);
void DestroyBeingInArray(Beings_array*, const unsigned int);
void MoveBeing(Being*, const float, const float);
void SetBeingPosition(Being*, const float, const float);
void MoveBeingX(Being*, const float);
void MoveBeingY(Being*, const float);
void MoveBeingToSegment(Being*, Segment*);
void StartBeingWalk(Being*, const int, const float, const float);
void StartBeingWalkWithRandTurn(Being*, const int, const float, const float);
void StartBeingWalkWithRandTurn45Deg(Being*, const int, const float, const float);
bool ResolveBeingCollisionInNewSegment(Being*, Segment*, float*, float*, const float, const float);
void TurnBeingWalk(Being*);
void UpdateBeingWalk(Being*, World*);
void UpdateBeings(Beings_array*, World*, SDL_FPoint*);
void DamageBeing(Being*, int);

#endif