#ifndef RENDER_H_
#define RENDER_H_

typedef struct Visual_effect Visual_effect;
typedef struct Visual_effects Visual_effects;

enum visual_effect_type{
    visual_effect_t0,
    visual_effect_t1,
    visual_effect_t2,
    visual_effect_t2_timer,
    visual_effect_bolt,
    visual_effect_t3
};
enum character{
    Ż,Ź,Ś,Ć,s0,Ń,Ó,J,Q,s1,Z,s2,Ą,L,
    Ę,Ł,S,C,D,E,s3,s4,j,T,s5,s6,s7,s8,f,B,s9,
    V,W,s10,G,H,N,U,exclam_m,s12,n3,n8,b,dot,
    h,l,I,M,O,P,R,Y,s14,ł,n1,n2,n6,n7,a,
    k,A,F,K,X,ź,ż,n0,n4,n5,n9,d,ń,ć,ó,i,
    ś,q,g,p,y,ą,ę,s15,t,z,m,n,o,r,s,v,w,
    c,e,u,x,s16,s17,s_plus,s19,s20,s21,s22,s23,s24,s25,s_minus,s27,s28,s29,sp,
    char_end
};
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
static void RemoveVisalEffect(Visual_effects* const, const unsigned int);
static void RenderVisualEffects(Render_data* const, Game_data* const);
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

static void DrawBeings(Render_data* const);
static void DrawColouredThings(Render_data* const);
static void DrawBeing(Render_data* const, SDL_Texture**, SDL_Surface* const, SDL_Surface* const, const Uint8* const, const Uint8* const);
static void DrawColouredThing(Render_data* const, SDL_Texture**, SDL_Surface* const, const Uint8* const);
void ResetRenderData(Render_data* const);
static void RenderHumanPlayerBlade(Render_data* const, Blade* const);
static void RenderProjectiles(Render_data* const, Game_data* const);
static void RenderBeing(Render_data* const, Game_data* const, Being* const, const unsigned int, const unsigned int);
static void RenderBeings(Render_data* const, Game_data* const, Segment** const, const unsigned int);
static void RenderMap(Render_data* const, const Game_data* const, Player* const);
static bool GetRenderPointFromTrue(Render_data* const, const float, const float, const Player* const, SDL_FPoint* const, World* const);
static bool GetRenderPointFromTrueWithKnownSegmentVisibility(Render_data* const, const float, const float, const Player* const, SDL_FPoint* const);
static void RenderPlayerStatus(Render_data* const, Player* const, const Game_data* const);
void RenderMainMenu(Render_data* const, const unsigned int);
void RenderGame(Render_data* const, Game_data* const, const int);
static void SetSineCosine(Render_data* const, Player* const);
void ClearRenderData(Render_data* const);
static void RenderTerrain(Render_data* const, Game_data* const, Segment** const, unsigned int* const);
static bool GetExtendedRenderPointFromTrue(Render_data* const, const float, const float, const float, const Player* const, SDL_FPoint* const);
static void RenderGunSight(Render_data* const);
static void RenderDoors(Render_data* const, Game_data* const);
static bool GetRenderPointFromTrueShopEdition(Render_data* const, const float, const float, const float, const Player* const, SDL_FPoint* const, World* const);
static void RenderStaticThings(Render_data* const, Game_data* const);
static void RenderStaticThing(Render_data* const, const float, const float, Player* const, const float, const int, World* const);
static void RenderStaticThingRotating(Render_data* const, const float, const float, Player* const, const float, const int, World* const, const float);
void DrawMap(Render_data* const, const World* const);
void UpdateMap(Render_data* const, const Uint8, const Uint8, const Uint8, const unsigned int, const unsigned int);
static void RenderHumanPlayer(Render_data* const);
static void RenderHumanPlayerBarrier(Render_data* const, const Player* const);
static void RenderHumanPlayerScroll(Render_data* const);
static void RenderHumanPlayerScrollUnrolled(Render_data* const, unsigned int);
static void	RenderPlayersBladesAndScrolls(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static void	RenderPlayers(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static void	RenderBarriers(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static Placement GetBeingWeaponPlacement(const Being* const);
static Placement GetWeaponPlacement(Placement* const, Placement* const, const int, const int);
static void RenderBackground(Render_data* const);
static void RenderFrame(Render_data* const, SDL_Texture* const, const SDL_FRect* const, const float);
static void RenderDirectionArrow(Render_data* const, const double);
static void RenderQuickScrolls(Render_data* const, const Player* const);
static void RenderScrollsManagement(Render_data* const, const Player* const);
static void RenderMenu(Render_data* const, const Player* const);
static void DrawBackground(Render_data* const);
static void RenderPortrait(Render_data* const, SDL_Texture* const);
static SDL_FPoint GetCharacterXPositionAndWidth(const int);
static unsigned int GetCharacterRow(const int);
static void RenderText(Render_data* const, float, const float, const float, const Uint8*);
static void RenderTextCentered(Render_data* const, float, const float, const float, const Uint8*, const unsigned int, const float);
static void RenderInt(Render_data* const, const float, const float, const float, int);
static void RenderIntCentered(Render_data* const, const float, const float, const float, int, const float);
static void RenderTextFromRight(Render_data* const, float, const float, const float, const Uint8*, const unsigned int);
static void RenderIntFromRight(Render_data* const, const float, const float, const float, int);
void ResetTextTextureAlpha(Render_data* const);
void SetSelectetScrollMouseSelection(const Render_data* const, Player* const);
void SetPointedScrollMouseSelection(const Render_data* const, Player* const);
unsigned int GetMouseShopSelection(const Render_data* const);
void SetPointedOptionMouseSelection(const Render_data* const, unsigned int* const);
int GetMouseScrollManagPositionNum(const Render_data* const);
static int GetMouseMenuPositionNum(const Render_data* const);
void RenderDefeatedScreen(Render_data* const);
void RenderVictoryScreen(Render_data* const);
void SetMouseBarrier(Render_data* const);
static SDL_FRect GetScrollTextureSrcRect(unsigned int);
void RenderShop(Render_data* const, const Player* const, const Shop* const, const Uint8* const, const unsigned int, const Uint8* const, const unsigned int, const int);
void ToggleFullscreen(Render_data* const);
SDL_FPoint GetMouseWorldPosition(const Game_data* const);

#endif
