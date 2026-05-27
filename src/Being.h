#ifndef BEIGN_H_
#define BEIGN_H_

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

void HaltBeing(Being* const, const int);
static bool TypeIsAlly(const Uint8);
bool IsAlly(const Being* const);

void UpdateBeings(Game_data* const);
void UpdateBeingsEffects(Game_data* const);

bool DamageBeing(Being* const, const Impact* const, Being* const);
bool DamageAlly(Being* const, const Impact* const, Being* const);
void StunBeing(Being* const, const int);
void CatapultBeing(Being* const, const float, const float, const int);

void AddBeingEffect(Being* const, const Lasting_effect);
void AddOrUpdateBeingEffect(Being* const, const Lasting_effect);
void EndAllBeingEffects(Game_data* const, Being* const);
int BeingHasEffect(const Being* const, const unsigned int);
void SlowBeing(Game_data* const, Being* const, const int);
void SlowBeingEnd(Game_data* const, Being* const);
void CommanderAura(Game_data* const, Being* const, const int);
void CommanderIsNear(Game_data* const, Being* const, const int);
void CommanderIsNearEnd(Game_data* const, Being* const);
void OpeningPortal(Game_data* const, Being* const, const int);
void ThunderboltChain(Game_data* const, Being* const, const int);
void AllyLifetime(Game_data* const, Being* const, const int);
void BeingLastingEffectVoidEnd(Game_data* const, Being* const);
void AddBeingEffectVisual(Render_data *const, const Being *const, void (*AddFunc)(Visual_effects* const, const SDL_FPoint* const));
void ClientUpdateBeings(Game_data *const);

#endif
