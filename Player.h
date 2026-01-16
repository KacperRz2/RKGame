#ifndef PLAYER_H_
#define PLAYER_H_

void CreatePlayer(Player* const, const float, const float);
void SetPlayerPosition(Player* const, const float, const float);
void MovePlayer(Game_data* const, Player* const, const float, const float);
void UpdatePlayerNewSegment(World* const, Player* const);
static void UpdatePlayerMove(Game_data* const, const unsigned int);
static void UpdatePlayerDirection(Player* const);
static void UpdatePlayerPoints(Player* const);
static void UpdatePlayer(Game_data* const, const unsigned int);
static void ShiftBlade(Blade* const, Placement*);
static void SetBladePosition(Blade* const, const Placement*);
static void SetShiftToBase(Blade* const, Placement* const, const unsigned int);
static void SetBladePositionToBase(Blade* const);
static void SetShiftToPosition(Blade*, Placement* const, const Placement* const, const unsigned int);
static Placement GetBladeLocation(Player* const);
static bool BladeHitsBeing(Blade* const, Being* const, SDL_FPoint* const);
static bool UnleashDestruction(Game_data* const, const unsigned int);
static void UpdatePlayerBlade(Game_data* const, const unsigned int);
bool PointInPlayer(const float, const float, Player* const);
bool CircleMeetsPlayer(const float, const float, const float, Player* const);
void DamagePlayer(Player* const, const Impact* const);
static void UpdatePlayerFire(Game_data* const, const unsigned int);
static void UpdatePlayerPush(Game_data* const, const unsigned int);
void HaltPlayer(Player* const);
void HitBarrier(Player* const, const Impact* const);
float GetDirectionToPush(const SDL_FPoint* const, const SDL_FPoint* const);
static void UpdatePlayerCast(Game_data* const, const unsigned int);
void HealPlayer(Player* const p, const int);
static void UpdatePlayerHitPoints(Player* const);
static void BlockPlayerFatigue(Player* const, const int);
static void BlockPlayerArmourRegen(Player* const, const int);
static void PlayerGainArmour(Player* const, const float);
void UpdatePlayers(Game_data* const);
void UpdatePlayersEffects(Game_data* const);
static void UpdateCPUPlayerFlags(Game_data* const, const unsigned int);
static Being* BeingNear(Segment* s, Game_data* const);
void SetQuickScroll(Player* const, int);
static void StunPlayer(Player* const, float);

void AddPlayerEffect(Player* const, const Lasting_effect);
void AddOrUpdatePlayerEffect(Player* const, const Lasting_effect);
static void RemovePlayerEffect(Player* const, const int);
static void UpdatePlayerEffects(Game_data* const, const unsigned int);
static void UpdatePlayerEffect(Game_data* const, Player* const, const int);
int PlayerHasEffect(Player* const, const int);
void SlowPlayer(Game_data* const, Player* const, const int);
void PlayerHPRegeneration(Game_data* const, Player* const, const int);
void PlayerWeakness(Game_data* const, Player* const, const int);
void PlayerDodge(Game_data* const, Player* const, const int);

#endif