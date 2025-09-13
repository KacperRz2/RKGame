#ifndef PLAYER_H_
#define PLAYER_H_

void CreatePlayer(Player* const, const float, const float);
void SetPlayerPosition(Player* const, const float, const float);
void MovePlayer(Game_data* const, const float, const float);
static void UpdatePlayerMove(Game_data* const);
static void UpdatePlayerDirection(Player* const);
static void UpdatePlayerPoints(Player* const);
void UpdatePlayer(Game_data* const);
static void ShiftBlade(Blade* const, Status_frame*);
static void SetBladePosition(Blade* const, const Status_frame*);
static void SetShiftToBase(Blade* const, Status_frame* const, const unsigned int);
static inline void SetBladePositionToBase(Blade* const);
static void SetShiftToPosition(Blade*, Status_frame* const, const Status_frame* const, const unsigned int);
static Status_frame GetBladeLocation(Player* const, float* const, float* const);
static bool BladeHitsBeing(Blade* const, Status_frame* const, Being* const, SDL_FPoint* const);
static bool UnleashDestruction(Game_data* const);
static void UpdatePlayerBlade(Game_data* const);
bool PointInPlayer(const float, const float, Player* const);
void DamagePlayer(Player* const, const int);
static void UpdatePlayerFire(Game_data* const);
static void UpdatePlayerPush(Game_data* const);
void HaltPlayer(Player* const);
void HitBarrier(Player* const, const int);
static float GetDirectionToPush(SDL_FPoint* const, SDL_FPoint* const);

#endif