#ifndef PLAYER_H_
#define PLAYER_H_

Player* CreatePlayer(const float, const float);
void SetPlayerPosition(Player*, const float, const float);
void MovePlayer(Player*, const float, const float);
// void SetPlayerInBounds(Player*);
void UpdatePlayerMove(Player*);
void UpdatePlayerDirection(Player*);
void DestroyPlayer(Player*);
void UpdatePlayerPoints(Player*);
void UpdatePlayer(Player*, Projectiles_array*);
void ShiftBlade(Blade*, Status_frame*);
void SetBladePosition(Blade*, const Status_frame*);
// void SetShift(Status_frame*, const Status_frame*, const unsigned int);
// void SetShiftToBase(Status_frame*, const Status_frame*, const unsigned int);
void SetShiftToBase(Blade*, Status_frame*, const unsigned int);
void SetShiftToPosition(Blade*, Status_frame*, const Status_frame*, const unsigned int);
Status_frame GetBladeLocation(Player*);
bool BladeHitsBeing(Blade*, Status_frame*, Being*);
bool UnleashDestruction(Player*);
// void ClearBlade(Blade*);
void UpdatePlayerBlade(Player*);

#endif