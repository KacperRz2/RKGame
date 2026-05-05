#ifndef RENDER_H_
#define RENDER_H_

typedef struct Visual_effect Visual_effect;
typedef struct Visual_effects Visual_effects;

enum texture_num{
    tx_viewfinder,
    tx_pc_blade,
    tx_icons,
    tx_arrow,
    tx_being_blade,
    tx_terrain,
    tx_gunsightpart,
    tx_damage_test,
    tx_portal,
    tx_door,
    tx_box,
    tx_barrier,
    tx_stun,
    tx_scroll,
    tx_weapon,
    tx_creation_point,
    tx_bar,
    tx_chars,
    tx_menu_ptr,
    tx_weapon1,
    tx_bonus_effect,
    tx_curse_effect,
    tx_owned,
    tx_bolt,
    tx_portrait,
    tx_sword,
    tx_void,
    tx_pc,
    tx_being_weak,
    tx_being_ordinary,
    tx_being_ranger,
    tx_being_elite,
    tx_being_elite_ranger,
    tx_being_commander,
    tx_being_warlock,
    tx_being_ally_ordinary,
    tx_projectile,
    tx_h_projectile,
    tx_shop_icons,
    tx_pixel,
    tx_pixel_blend,
    tx_map,
    tx_background,
    tx_lighting,
    tx_view,
    txs_num
};

struct Visual_effect{
    SDL_FPoint position;
    Uint16 ticks_left;
    union{
        struct{
            Uint16 start_ticks;
            Uint16 size;
            Uint8 tx_num;
        }data0;
        struct{
            position16 start_position;
            Uint8 angle;
        }data1;
        struct{
            Uint8 r;
            Uint8 g;
            Uint8 b;
        }data2;
    };
    Uint8 type;
};
struct Visual_effects{
    Visual_effect* array;
    Uint16 num;
};
struct Render_data{
    float viewfinder;
    SDL_FRect visible_rect;
    SDL_Rect viewfinder_rect;
    Uint16 window_w;
    Uint16 window_h;
    float sin_player_direction;
    float cos_player_direction;
    unsigned int counter;
    float mouse_y;
    Visual_effects visual_effects;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* textures[txs_num];
};

void GraphicsInitiation(Render_data* const);

void AddVisalEffect(Visual_effects* const, const Visual_effect* const);
void AddDamageVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddBonusVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddCurseVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddDeadVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddPortalVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddProjectileVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddBigBurnVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddSmallBurnVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddBoomVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddBigBurnVisualEffectTimer(Visual_effects* const, const SDL_FPoint* const, const unsigned int);
void AddSmallBurnVisualEffectTimer(Visual_effects* const, const SDL_FPoint* const, const unsigned int);
void AddBoomVisualEffectTimer(Visual_effects* const, const SDL_FPoint* const, const unsigned int);
void AddBoltVisualEffect(Visual_effects* const, const SDL_FPoint* const, const position16);
void AddSpellVisualEffect(Visual_effects* const, const SDL_FPoint* const, const Uint8, const Uint8, const Uint8);

void ResetRenderData(Render_data* const);
void RenderMainMenu(Render_data* const, const unsigned int);
void RenderGame(Render_data* const, Game_data* const, const int);
void ClearRenderData(Render_data* const);
void DrawMap(Render_data* const, const World* const);
void UpdateMap(Render_data* const, const Uint8, const Uint8, const Uint8, const unsigned int, const unsigned int);
void ResetTextTextureAlpha(Render_data* const);
void SetSelectetScrollMouseSelection(const Render_data* const, Player* const);
void SetPointedScrollMouseSelection(const Render_data* const, Player* const);
unsigned int GetMouseShopSelection(const Render_data* const);
void SetPointedOptionMouseSelection(const Render_data* const, unsigned int* const);
int GetMouseScrollManagPositionNum(const Render_data* const);
void RenderDefeatedScreen(Render_data* const);
void RenderVictoryScreen(Render_data* const);
void SetMouseBarrier(Render_data* const);
void RenderShop(Render_data* const, const Player* const, const Shop* const, const Uint8* const, const unsigned int, const Uint8* const, const unsigned int, const int);
void ToggleFullscreen(Render_data* const);
SDL_FPoint GetMouseWorldPosition(const Game_data* const);

#endif
