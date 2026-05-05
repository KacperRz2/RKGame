#ifndef BEIGN_H_
#define BEIGN_H_

typedef struct Being_type Being_type;

enum being_status{
    being_follow,
    being_shoot,
    being_strike,
    being_attack_being,
    being_follow_being,
    being_shoot_being,
    being_strike_being,
    being_walk,
    being_search,
    being_fly,
    being_idle,
    being_stunned,
    being_in_void,
    being_dead
};
enum being_type{
    being_weak,
    being_ordinary,
    being_ranger,
    being_elite,
    being_elite_ranger,
    being_commander,
    being_warlock,
    ally_ordinary
};
enum being_effect{
    being_effect_slow,
    being_effect_commander,
    being_effect_bonus,
    being_effect_open_portal,
    being_effect_burn,
    being_effect_thunderbolt,
    being_effect_ally_lifetime,
    being_no_effect
};

struct Being_type{
    float size;
    float velocity;
    int hit_points;
    Armour armour;
    Impact impact;
    void (*update)(Being* const, Game_data* const);
};
struct Being{
    SDL_FPoint position;
    float direction;
    float velocity;
    int hit_points;
    Segment* segment;
    union target{
        Player* player;
        Being* being;
    }target;
    position16 target_last_seen_at;
    Uint8 indx;
    Uint8 type_id;
    Sint8 status;
    int status_ticks_left;
    Armour armour;
    Impact impact;
    SDL_FPoint special_move_shift;
    Uint16 main_indx;
    Uint8 effects_num;
    Lasting_effect effects[MAX_BEING_EFFECTS];
    struct rend_fly_help_data{
        _Float16 start_angle;
        Sint16 ticks;
    }rend_fly_help_data;
};

static void UpdateBeing(Being* const, Game_data* const);
float BeingSize(const Being* const);
int BeingHP(const Being* const);

unsigned int GetRandomBeingId();
void AddBeingToSegment(Segment* const, Being* const, Segment_beings* const);
void RemoveBeingFromSegment(Being* const, Segment_beings* const, Being* const);
void DestroyBeings(Beings_array* const);
void TryCreateIdleBeing(Game_data* const, const Uint8, const float, const float, Player* const);
bool TryCreateHostileBeing(Game_data* const, const Uint8, const float, const float, Player* const);
void AddIdleBeingToArray(Beings_array* const, const Uint8, const float, const float, Segment* const, Player* const);
Being* AddIdleAllyToArray(Beings_array* const, const Uint8, const float, const float, Segment* const, Player* const);
void AddHordeBeingToArray(Beings_array* const, const Uint8, const float, const float, Segment* const, Player* const);
static Being* AddBeingToArray(Beings_array* const, const Uint8, const float, const float, Player* const);
static void SetBeingTypeData(Being* const);
static void GetBeingDistances(const Being* const, const SDL_FPoint* const, float* const, float* const, float* const);
static void MoveBeing(Being* const, const float, const float);
static void SetBeingPosition(Being* const, const float, const float);
static void SetBeingPositionInNewSegment(Being* const, const float, const float, Segment* const, Being* const);
static void SetAllyPositionInNewSegment(Being* const, const float, const float, Segment* const, Being* const);
static void MoveBeingX(Being* const, const float);
static void MoveBeingY(Being* const, const float);
static bool CircleMeetsBeing(const float, const float, const float, Being* const);
static void MoveBeingToSegment(Being* const, Segment* const, Being* const);
static void MoveAllyToSegment(Being* const, Segment* const, Being* const);
static void TurnBlockedBeing(Being* const, Game_data* const);
static void TurnBlockedBeingWalk(Being* const, Game_data* const);

static void StartBeingWalk(Being* const, const int, const float, const float);
static void StartBeingRandWalk(Being* const, const int);
static void StartBeingWalkWithRandTurn(Being* const, const int, const float, const float);
static void StartBeingWalkWithRandTurn45Deg(Being* const, const int, const float, const float);
static void StartBeingSearch(Being* const, const int);
static void StartBeingFollow(Being* const, const int, float const, float const, float const);

static bool BeingCollideWithBeing(Being* const, const float, const float, const float);
static bool ResolveBeingCollisionInNewSegment(Game_data* const, Being* const, Segment* const, float* const, float* const, const float, const float);
static void TurnBeingWalk(Being* const);

static void UpdateBeingWalk(Being* const, Game_data* const);
static void UpdateBeingShoot(Being* const, Game_data* const, const SDL_FPoint* const, Segment* const);
static void UpdateWarlockShoot(Being* const, Game_data* const, const SDL_FPoint* const, Segment* const);
static void UpdateBeingStrike(Being* const, Player* const, float const, float const, float const, Game_data* const);
static void UpdateBeingStrikeAlly(Being* const, float const, float const, float const, Game_data* const);
static void UpdateBeingSearch(Being* const, Game_data* const);
static void UpdateBeingFollow(Being* const, float const, float const, float const, Game_data* const);
static void UpdateBeingStunned(Being* const);
static void UpdateBeingFly(Being* const, Game_data* const);
static void UpdateBeingAttackAlly(Being* const b, Game_data* const gd, const float attack_range);

static void MoveStrikingBeing(Being* const, float const, float const, float const, Game_data* const, float const);
static void MoveBackStrikingBeing(Being* const, float const, float const, float const, Game_data* const);
static void SetBeingPositionIfAllowed(Being* const, float const, float const, Game_data* const);

static SDL_FPoint GetHBladeAttackHittingPoint(Being* const, const float, const float);
void HaltBeing(Being* const, const int);
static bool TypeIsAlly(const Uint8);
bool IsAlly(const Being* const);

void UpdateBeings(Game_data* const);
void UpdateBeingsEffects(Game_data* const);

bool DamageBeing(Being* const, const Impact* const, Being* const);
bool DamageAlly(Being* const, const Impact* const, Being* const);
void StunBeing(Being* const, const int);
void CatapultBeing(Being* const, const float, const float, const int);
static void FindTargetForBeing(Being* const, Players* const);
static bool IdleBeingCollision(Being* const, Game_data* const);
static bool FindAllyTarget(Being* const, Game_data* const);
static void TurnBlockedAlly(Being* const, Game_data* const);
static bool AllyNear(Being* const, Game_data* const);
static Being* GetOtherBeingNearPlayer(Being* const, Game_data* const);
static void MovePlayerIfTooClose(const float, const float, const float, const Being* const, Game_data* const);
static void BeingFlee(Being* const, Game_data* const);

static void StartAllyFollow(Being* const, const int);
static void UpdateAllyFollow(Being* const, Game_data* const);
static void UpdateAllyWalk(Being* const, Game_data* const);
static void UpdateAllySearch(Being* const, Game_data* const);

void AddBeingEffect(Being* const, const Lasting_effect);
void AddOrUpdateBeingEffect(Being* const, const Lasting_effect);
void EndAllBeingEffects(Game_data* const, Being* const);
static void RemoveBeingEffect(Being* const, const int);
static void UpdateBeingEffects(Game_data* const, Being* const);
static void UpdateBeingEffect(Game_data* const, Being* const, const int);
int BeingHasEffect(Being* const, const unsigned int);
void SlowBeing(Game_data* const, Being* const, const int);
void SlowBeingEnd(Game_data* const, Being* const);
void CommanderAura(Game_data* const, Being* const, const int);
void CommanderIsNear(Game_data* const, Being* const, const int);
void CommanderIsNearEnd(Game_data* const, Being* const);
void OpeningPortal(Game_data* const, Being* const, const int);
static void Burn(Game_data* const, Being* const, const int);
void ThunderboltChain(Game_data* const, Being* const, const int);
void AllyLifetime(Game_data* const, Being* const, const int);
void BeingLastingEffectVoidEnd(Game_data* const, Being* const);

static void UpdateBeingOrdinary(Being* const, Game_data* const);
static void UpdateBeingRanger(Being* const, Game_data* const);
static void UpdateBeingCommander(Being* const, Game_data* const);
static void UpdateBeingWarlock(Being* const, Game_data* const);
static void UpdateAlly0(Being* const, Game_data* const);

#endif
