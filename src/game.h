#ifndef GAME_H_
#define GAME_H_

typedef struct Box_element Box_element;
typedef struct Box Box;
typedef struct Boxes Boxes;
typedef struct Announcement Announcement;

enum menu_option{
    menu_start,
    menu_load,
    menu_settings,
    menu_credits,
    menu_quit,
    menu_unknown,
    menu_escape,
    menu_multipl_host,
    menu_multipl
};
enum box_content_type{
    box_mp,
    box_coins,
    box_scroll,
    box_key,
    box_map,
    box_clear
};
enum game_effect{
    game_effect_horde_attack,
    game_effect_1,
    game_no_effect
};
enum game_flags{
    gamef_horde_attack = 1 << 0,
    gamef_morale_break = 1 << 1,
    gamef_multiplayer = 1 << 2,
    gamef_client_dodge = 1 << 3,
    gamef_client_cast = 1 << 4,
    gamef_is_serv_host = 1 << 5
};
enum game_update{
    update_ok,
    update_defeated,
    update_victory,
    update_shop
};
enum shop_item{
    shop_item_invalid = scroll_empty,
    shop_item_mp10,
    shop_item_mp100,
    shop_item_key,
    shop_item_key_location,
    shop_item_armour,
    shop_item_dodge_speed
};
enum check_queue{
    check_0,
    check_1,
    check_2,
    check_3,
    check_4,
    check_5,
    checks_num
};
enum option{
    opt_exit = -1,
    opt_void,
    opt_select
};
enum announcement_id{
    annncmnt_destroy_box,
    annncmnt_load,
    annncmnt_update_mp_and_coin,
    annncmnt_update_scroll,
    annncmnt_update_key,
    annncmnt_damage,
    annncmnt_explosion,
    annncmnt_kill,
    annncmnt_portal,
    annncmnt_bolt,
    annncmnt_cast,
    annncmnt_spell
};

struct Beings_array{
    Being* array;
    Uint16* indices;
    Uint16 num;
};
struct Projectiles_array{
    Projectile* array;
    Uint16 num;
};
struct Players{
    Player* array;
    Uint8 num;
};
struct Box_element{
    Uint8 type;
    Uint16 value;
};
struct Box{
    SDL_FPoint location;
    Box_element elements[BOX_SLOTS];
};
struct Boxes{
    Box* array;
    Uint16 num;
};
struct Announcement{
    union{
        SDL_FPoint position;
        struct{
            position16 pos16b0;
            position16 pos16b1;
        };
        struct{
            Uint32 data32b0;
            Uint32 data32b1;
        };
        Uint64 data64b;
        struct{
            position16 pos16b;
            Uint8 data8b0;
            Uint8 data8b1;
        };
        struct{
            int data32b01;
            int data32b11;
        };
    };
    Uint8 id;
};
struct Game_data{
    Uint32 flags;
    Players champions;
    Beings_array beings;
    Projectiles_array projectiles;
    World world;
    Boxes boxes;
    unsigned int host_indx;
    unsigned int keys;
    unsigned int needed_keys;
    int enemy_morale;
    Uint8 *keys_status;
    unsigned int effects_num;
    Lasting_effect effects[MAX_GAME_EFFECTS];
    struct{
        Announcement *unimportant;
        Announcement *important;
        Uint8 unimportant_num;
        Uint8 important_num;
    }announcements;
    union horde_data{
        SDL_FPoint creation_points[HORDE_ATTACK_POINTS];
        unsigned int ticks_from_attack;
    }horde_data;
    Render_data* rend_data_ptr;
    SDL_Event* ev_ptr;
    Uint64 seed;
};

int MainMenuLoop(SDL_Event* const, Render_data* const);
void GameLoop(Game_data* const);
void CreditsLoop(SDL_Event* const, Render_data *const);
void SetGameData(Game_data* const);
void ClearGameData(Game_data* const);
void AddToBox(Box* const, const unsigned int, const int, const unsigned int);
int GetNearbyBoxIndx(const Boxes* const, const SDL_FPoint* const, const float);
int CalculateDamage(const Impact* const, const Armour* const);
float CalculateStunPower(const Impact* const, const Armour* const);
int ItemPrice(const unsigned int);

int HasEffect(Lasting_effect* const, const unsigned int, const unsigned int);
void AddLastingEffect(Lasting_effect* const, const Lasting_effect, const int);
void RemoveLastingEffect(Lasting_effect* const, const int, const int);

void SaveGame(const Game_data* const, const char *const);
void LoadGame(Game_data* const, const char *const);

void HostGameLoop(Game_data* const);
void ClientGameLoop(Game_data* const);
void AddAnnouncement(Game_data *const, const Uint8, const void *const);

#endif
