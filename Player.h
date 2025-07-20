#ifndef PLAYER_H_
#define PLAYER_H_

void CreatePlayer(Player* const, const float, const float);
void SetPlayerPosition(Player* const, const float, const float);
void MovePlayer(Game_data* const, const float, const float);
void UpdatePlayerMove(Game_data* const);
void UpdatePlayerDirection(Player* const);
void UpdatePlayerPoints(Player* const);
void UpdatePlayer(Game_data* const);
void ShiftBlade(Blade* const, Status_frame*);
void SetBladePosition(Blade* const, const Status_frame*);
void SetShiftToBase(Blade* const, Status_frame* const, const unsigned int);
void SetShiftToPosition(Blade*, Status_frame* const, const Status_frame* const, const unsigned int);
Status_frame GetBladeLocation(Player* const, float* const, float* const);
bool BladeHitsBeing(Blade* const, Status_frame* const, Being* const, SDL_FPoint* const);
bool UnleashDestruction(Game_data* const);
void UpdatePlayerBlade(Game_data* const);
bool PointInPlayer(const float, const float, Player*);
void DamagePlayer(Player* const, const int);

#endif