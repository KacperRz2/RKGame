#ifndef BEIGN_H_
#define BEIGN_H_

static void UpdateBeing(Being* const, Game_data* const);
float BeingSize(Being* const);
float BeingVelocity(Being* const);
int BeingMaxHitPoints(Being* const);
int BeingDamage(Being* const);

static void AddBeingToSegment(Segment* const, Being* const);
static void RemoveBeingFromSegment(Being* const);
void DestroyBeings(Beings_array* const);
void AddBeingToArray(Beings_array* const, const unsigned int, const float, const float, Segment* const);
static void DestroyBeingInArray(Beings_array* const, const unsigned int);
static void MoveBeing(Being* const, const float, const float);
static void SetBeingPosition(Being* const, const float, const float);
static void SetBeingPositionInNewSegment(Being* const, const float, const float, Segment* const);
static void MoveBeingX(Being* const, const float);
static void MoveBeingY(Being* const, const float);
static void MoveBeingToSegment(Being* const, Segment* const);
static void StartBeingWalk(Being* const, const int, const float, const float);
static void StartBeingWalkWithRandTurn(Being* const, const int, const float, const float);
static void StartBeingWalkWithRandTurn45Deg(Being* const, const int, const float, const float);
static bool BeingCollideWithBeing(Being* const, const float, const float, const float);
static bool ResolveBeingCollisionInNewSegment(Being* const, Segment* const, float* const, float* const, const float, const float);
static void TurnBeingWalk(Being* const);

static void UpdateBeingWalk(Being* const, World* const);
static void UpdateBeingShoot(Being* const, Projectiles_array* const);
static void MoveStrikingBeing(Being* const, float const, float const, float const, World* const);
static void MoveBackStrikingBeing(Being* const, float const, float const, float const, World* const);
static void SetBeingPositionIfAllowed(Being* const, float const, float const, World* const);
static void UpdateBeingStrike(Being* const, Player* const, float const, float const, float const, World* const);
static void UpdateBeingFollow(Being* const, float const, float const, float const, World* const);

static void ShiftHBlade(Blade_hostile* const, const Status_frame* const);
static Status_frame GetHBladeLocation(Being* const, float* const, float* const);
static void HaltBeing(Being* const, const int);

void UpdateBeings(Game_data* const);

bool DamageBeing(Being* const, int);
void ResetBeingBlade(Being* const);
void StunBeing(Being* const, const int);
void CatapultBeing(Being* const, const float, const float, const int);
static void UpdateBeingStunned(Being* const, World* const);
static void UpdateBeingFly(Being* const, World* const);

static void UpdateBeing0(Being* const, Game_data* const);
static void UpdateBeing1(Being* const, Game_data* const);

#endif