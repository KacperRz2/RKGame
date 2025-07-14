#ifndef BEIGN_H_
#define BEIGN_H_

void AddBeingToSegment(Segment* const, Being* const);
void RemoveBeingFromSegment(Being* const);
void DestroyBeings(Beings_array* const);
void AddBeingToArray(Beings_array* const, const float, const float);
void DestroyBeingInArray(Beings_array* const, const unsigned int);
void MoveBeing(Being* const, const float, const float);
void SetBeingPosition(Being* const, const float, const float);
void MoveBeingX(Being* const, const float);
void MoveBeingY(Being* const, const float);
void MoveBeingToSegment(Being* const, Segment* const);
void StartBeingWalk(Being* const, const int, const float, const float);
void StartBeingWalkWithRandTurn(Being* const, const int, const float, const float);
void StartBeingWalkWithRandTurn45Deg(Being* const, const int, const float, const float);
bool CollideWithBeing(Being* const, const float, const float);
bool ResolveBeingCollisionInNewSegment(Being* const, Segment* const, float* const, float* const, const float, const float);
void TurnBeingWalk(Being* const);
void UpdateBeingWalk(Being* const);
void ShiftHBlade(Blade_hostile* const, const Status_frame* const);
Status_frame GetHBladeLocation(Being* const, float* const, float* const);
void HaltBeing(Being* const, const int); // const int ticks < 0!
void UpdateBeings(Beings_array* const, Player* const, Segment* const, Projectiles_h_array* const);
// void UpdateAndRenderBeings(Beings_array* const, Player* const, Segment* const, SDL_Renderer* const, SDL_Texture* const);
void DamageBeing(Being* const, int);

#endif