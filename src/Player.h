#ifndef PLAYER_H_
#define PLAYER_H_

typedef struct Block_times Block_times;

enum control{
    forward = 1 << 0,
    back = 1 << 1,
    right = 1 << 2,
    left = 1 << 3,
    dodge = 1 << 4,
    run = 1 << 5,
    attack = 1 << 6,
    cast = 1 << 7,
    block = 1 << 8,
    action = 1 << 9,
    range_mode = 1 << 10,
    stunned = 1 << 11,
    map_look = 1 << 12,
    dodge_time = 1 << 13,
    disrupted = 1 << 14,
    stop_blade = 1 << 15,
    strike = 1 << 16
};
enum pc_effect{
    pc_effect_slow,
    pc_effect_hpregen,
    pc_effect_fpregen,
    pc_effect_weak,
    pc_effect_dodge,
    pc_no_effect
};

struct Blade{
    Placement placement;
    Impact impact;
    Uint16 hit_targets[MAX_HITS];
    Uint8 penetration;
    Uint8 hits;
    Placement step_shift;
    Sint8 key;
    Sint16 steps;
    Sint16 step;
    Uint8 chain;
    Uint8 chain_next;
    bool abide;
    bool loose;
    Uint16 idle_ticks;
    float charge;
};
struct Block_times{
    Sint16 fatigue;
    Sint16 armour;
    Sint16 shoot;
    Sint16 push;
    Sint16 dodge;
    Sint16 cast;
};
struct Player{
    Uint32 flags;
    SDL_FPoint position;
    Blade blade;
    Impact blade_attack;
    Impact range_attack;
    Segment *segment;
    Segment *last_seen_in;
    SDL_FRect attention_rect;
    float direction;
    float move_direction;
    float velocity;
    float max_velocity;
    float dodge_velocity_multipl;
    int hit_points;
    int fatigue_points;
    int magic_points;
    int max_hp;
    int max_fatigue;
    Block_times block_times;
    Armour armour;
    Armour max_armour;
    int coins;
    SDL_FPoint aim_position;
    Uint8 selected_scroll;
    Uint8 scrolls[SCROLLS_NUM];
    Uint8 scrolls_quick_access[QUICK_SCROLLS];
    Uint8 effects_num;
    Lasting_effect effects[MAX_PC_EFFECTS];
    struct help_data{
        unsigned int menu_position;
    }help_data;
};

void StopPlayerActions(Player *const);
void CreatePlayer(Player *const, const float, const float, const Config *const);
void SetPlayerPosition(Player *const, const float, const float);
void MovePlayer(Game_data *const, Player *const, const float, const float);
void UpdatePlayerNewSegment(World *const, Player *const);
bool PointInPlayer(const float, const float, Player *const);
bool CircleMeetsPlayer(const float, const float, const float, Player *const);
void DamagePlayer(Player *const, const Impact *const);
void HaltPlayer(Player *const);
void HitBarrier(Game_data *const gd, Player *const, const Impact *const);
float GetDirectionToPush(const SDL_FPoint *const, const SDL_FPoint *const);
void HealPlayer(Player *const, const int);
void UpdatePlayers(Game_data *const);
void UpdatePlayersEffects(Game_data *const);
void SetQuickScroll(Player *const, int);
void TeleportPlayer(Game_data *const, Player *const, const SDL_FPoint *const);

void AddPlayerEffect(Player *const, const Lasting_effect);
void AddOrUpdatePlayerEffect(Player *const, const Lasting_effect);
int PlayerHasEffect(Player *const, const int);

void MultiplayerUpdatePlayers(Game_data *const);
void ClientUpdatePlayers(Game_data *const);
void PlayerCastConsequences(Game_data *const, Player *const, const int, const Uint8);

#endif
