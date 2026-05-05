#ifndef GAME_H_
#define GAME_H_

typedef struct Box_element Box_element;
typedef struct Box Box;
typedef struct Boxes Boxes;

enum menu_option{
    menu_start,
    menu_load,
    menu_settings,
    menu_credits,
    menu_quit,
    menu_unknown
};
enum menu_position{
    menu_p_settings,
    menu_p_load,
    menu_p_continue,
    menu_p_save,
    menu_p_quit
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
    gamef_morale_break = 1 << 1
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
struct Game_data{
    unsigned int flags;
    Players champions;
    Beings_array beings;
    Projectiles_array projectiles;
    World world;
    Boxes boxes;
    unsigned int human_indx;
    unsigned int keys;
    unsigned int needed_keys;
    int enemy_morale;
    Uint8* keys_status;
    unsigned int effects_num;
    Lasting_effect effects[MAX_GAME_EFFECTS];
    union horde_data{
        SDL_FPoint creation_points[HORDE_ATTACK_POINTS];
        unsigned int ticks_from_attack;
    }horde_data;
    Render_data* rend_data_ptr;
    SDL_Event* ev_ptr;
    Uint64 seed;
};

static void NoticeBigSeg(Game_data* const, const unsigned int, const unsigned int);
static void PlayerInUncoveredBigSeg(Game_data* const);
int MainMenuLoop(SDL_Event* const, Render_data* const);
void GameLoop(Game_data* const);
static void EndLoop(SDL_Event* const, Game_data* const, const int);
void SetGameData(Game_data* const);
void ClearGameData(Game_data* const);
static int RareEventsService(Game_data* const);
void AddToBox(Box* const, const unsigned int, const int, const unsigned int);
static void DestroyBoxes(Boxes* const);
int GetNearbyBoxIndx(const Boxes* const, const SDL_FPoint* const, const float);
static void LootBox(Game_data* const, const unsigned int);
static void DestroyBoxInArray(Boxes* const, unsigned int);
static int GetNearbyShopIndx(Game_data* const);
static void EnterShop(Game_data* const, Player* const, const unsigned int);
int CalculateDamage(const Impact* const, const Armour* const);
float CalculateStunPower(const Impact* const, const Armour* const);
static int UpdateGame(Game_data* const);
int ActivateMenuOption(const unsigned int, Game_data* const);
int ItemPrice(const unsigned int);
static SDL_FPoint GetBeingCreationPoint(Game_data* const, const float);

static void UpdateEffects(Game_data* const);
int HasEffect(Lasting_effect* const, const unsigned int, const unsigned int);
void AddLastingEffect(Lasting_effect* const, const Lasting_effect, const int);
void RemoveLastingEffect(Lasting_effect* const, const int, const int);
static void AddGameEffect(Game_data* const, const Lasting_effect);
static void RemoveGameEffect(Game_data* const, const int);
static void UpdateGameEffects(Game_data* const);
static void UpdateGameEffect(Game_data* const, const int);
static void HordeAttack(Game_data* const, const int);

void SaveGame(const Game_data* const);
void LoadGame(Game_data* const);

#endif
