#ifndef PLAYER_H_
#define PLAYER_H_

void CreatePlayer(Player*, const float, const float);
void SetPlayerPosition(Player*, const float, const float);
void MovePlayer(Player*, const float, const float);
void UpdatePlayerMove(Player*);
void UpdatePlayerDirection(Player*);
void UpdatePlayerPoints(Player*);
void UpdatePlayer(Player*, Projectiles_array*);
void ShiftBlade(Blade*, Status_frame*);
void SetBladePosition(Blade*, const Status_frame*);
void SetShiftToBase(Blade*, Status_frame*, const unsigned int);
void SetShiftToPosition(Blade*, Status_frame*, const Status_frame*, const unsigned int);
Status_frame GetBladeLocation(Player*, float*, float*);
bool BladeHitsBeing(Blade*, Status_frame*, Being*, SDL_FPoint*);
bool UnleashDestruction(Player*);
void UpdatePlayerBlade(Player*);

#endif