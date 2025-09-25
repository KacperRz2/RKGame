#ifndef BEIGN_H_
#define BEIGN_H_

static void UpdateBeing(Being* const, Game_data* const);
float BeingSize(Being* const);

static void AddBeingToSegment(Segment* const, Being* const, Segment_beings* const);
static void RemoveBeingFromSegment(Being* const, Segment_beings* const, Being* const);
void DestroyBeings(Beings_array* const);
void AddBeingToArray(Beings_array* const, const Uint8, const float, const float, Segment* const, Player* const);
static void MoveBeing(Being* const, const float, const float);
static void SetBeingPosition(Being* const, const float, const float);
static void SetBeingPositionInNewSegment(Being* const, const float, const float, Segment* const, Being* const);
static void SetAllyPositionInNewSegment(Being* const, const float, const float, Segment* const, Being* const);
static void MoveBeingX(Being* const, const float);
static void MoveBeingY(Being* const, const float);
static void MoveBeingToSegment(Being* const, Segment* const, Being* const);
static void MoveAllyToSegment(Being* const, Segment* const, Being* const);
static void StartBeingWalk(Being* const, const int, const float, const float);
static void StartBeingRandWalk(Being* const, const int);
static void StartBeingWalkWithRandTurn(Being* const, const int, const float, const float);
static void StartBeingWalkWithRandTurn45Deg(Being* const, const int, const float, const float);
static bool BeingCollideWithBeing(Being* const, const float, const float, const float);
static bool ResolveBeingCollisionInNewSegment(Being* const, Segment* const, float* const, float* const, const float, const float, Being* const);
static void TurnBeingWalk(Being* const);

static void UpdateBeingWalk(Being* const, Game_data* const);
static void UpdateBeingShoot(Being* const, Projectiles_array* const);
static void MoveStrikingBeing(Being* const, float const, float const, float const, Game_data* const);
static void MoveBackStrikingBeing(Being* const, float const, float const, float const, Game_data* const);
static void SetBeingPositionIfAllowed(Being* const, float const, float const, Game_data* const);
static void UpdateBeingStrike(Being* const, Player* const, float const, float const, float const, Game_data* const);
static void StartBeingFollow(Being* const, const int, float const, float const, float const);
static void UpdateBeingFollow(Being* const, float const, float const, float const, Game_data* const);

static SDL_FPoint GetHBladeAttackHittingPoint(Being* const, const float, const float);
static void HaltBeing(Being* const, const int);
static bool IsAlly(const Being* const);

void UpdateBeings(Game_data* const);

bool DamageBeing(Being* const, const Impact* const, Being* const);
bool DamageAlly(Being* const, const Impact* const, Being* const);
void StunBeing(Being* const, const int);
void CatapultBeing(Being* const, const float, const float, const int);
static void UpdateBeingStunned(Being* const);
static void UpdateBeingFly(Being* const, Game_data* const);
static void FindTargetForBeing(Being* const, Players* const);
static bool IdleBeingCollision(Being* const, Game_data* const);
static bool FindAllyTargetFar(Being* const, Game_data* const, const unsigned int);
static bool FindAllyTarget(Being* const, Game_data* const);
static void UpdateAllyFollow(Being* const, Game_data* const);
static void UpdateAllyWalk(Being* const, Game_data* const);

static void UpdateBeing0(Being* const, Game_data* const);
static void UpdateBeing1(Being* const, Game_data* const);
static void UpdateAlly0(Being* const, Game_data* const);

#endif