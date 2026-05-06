#include <common.h>
#include <function.h>
#include <event.h>
#include <stdlib.h>
// render macros ==================================================================
#define texture(num)	            (*(rend_data->textures + num))
#define VIEWFINDER_CENTER	        (rend_data->viewfinder * 0.5F)
#define WINDOW_CENTER_Y		        (rend_data->window_h / 2)
#define VIEWFINDER_BEFORE_PC_PART   0.9375F
#define PLAYER_REND_Y               (rend_data->viewfinder * VIEWFINDER_BEFORE_PC_PART)
#define PLAYER_REND_X               (half(rend_data->viewfinder))
#define PLAYER_REND_Y_SHIFT	        (PLAYER_REND_Y - half(rend_data->viewfinder))
#define SIGHT_SQUARED   	        (VIEWFINDER_CENTER * VIEWFINDER_CENTER + (VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT) * (VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT))
#define SIGHT_BACK_SQUARED	        (VIEWFINDER_CENTER * VIEWFINDER_CENTER + (VIEWFINDER_CENTER - PLAYER_REND_Y_SHIFT) * (VIEWFINDER_CENTER - PLAYER_REND_Y_SHIFT))
#define WINDOW_START_W              1600
#define WINDOW_START_H              900
#define VIEWFINDER_SIZE             (rend_data->window_h - 16.0F)
#define GUN_SIGHT_SPREAD_MIN        8.0F
#define GUN_SIGHT_SPREAD_RANGE      (GUN_SIGHT_SPREAD_MIN * 4.0F)
#define GUN_SIGHT_MIN_DISTANCE      PLAYER_SIZE
#define MINIMAP_PC_SIZE             8.0F
#define MAP_KEY_SIZE                16.0F
#define SCROLL_TX_SIZE              128.0F
#define BARRIER_MAGNIFICATION       0.25F
#define SIDE_AREA_WIDTH             (rend_data->viewfinder_rect.x - FRAME_W * 2.0F)
#define FRAME_W                     rend_data->viewfinder_rect.y
#define BAR_H_CALC                  (SDL_floorf(rend_data->window_h / 32.0F))
#define AREAA_H                     (BAR_H * 6.0F + FRAME_W * 13.0F)
#define AREAA_Y                     (rend_data->window_h - AREAA_H - FRAME_W * 2.0F)
#define ICON_X                      (FRAME_W * 3.0F)
#define BAR_X                       ICON_X
#define BAR_W                       (rend_data->viewfinder_rect.x - FRAME_W * 6.0F)
#define SMALL_BAR_H                 (BAR_H * 0.5F)
#define RIGHT_AREA_X                (rend_data->viewfinder_rect.x + rend_data->viewfinder_rect.w + FRAME_W)
#define MP_BAR_H                    BAR_H
#define AREA_MP_Y                   (rend_data->window_h - MP_BAR_H * 2.0F - BAR_H - FRAME_W * 5.0F)
#define AREA_Q_SCROLLS_H            (SIDE_AREA_WIDTH - FRAME_W * 4.0F)
#define AREA_Q_SCROLLS_Y            (AREA_MP_Y - AREA_Q_SCROLLS_H - Q_SCROLL_SIZE - FRAME_W * 4.0F)
#define Q_SCROLL_SIZE_CALC          (SDL_floorf((AREA_Q_SCROLLS_H - FRAME_W * 4.0F) / 3.0F))
#define CHARS_ROW_HEIGHT            72.0F
#define TX_ICONS_IN_ROW             8
#define PORTRAIT_H                  ((float)(rend_data->window_h / 6))
#define KEY_ICON_FRAME_Y            ((float)(rend_data->window_h / 6) + BAR_H * 2.0F)
#define KEYS_NUM_X                  (ICON_X + BAR_H + FRAME_W * 2.0F)
#define KEYS_NUM_Y                  (KEY_ICON_FRAME_Y + FRAME_W + 1.0F)
#define COINS_NUM_Y                 (KEY_ICON_FRAME_Y + BAR_H + FRAME_W * 4.0F + 1.0F)
#define SCROLLS_MANAG_ICON_SIZE     ((rend_data->viewfinder - FRAME_W) / (float)ICONS_IN_VIEWF_ROW - FRAME_W)
#define SEGMENT_TX_SIZE             (SEGMENT_SIZE * 1.5F)
#define MAX_UNSEEN_SEG              (pow2((unsigned int)((VIEWFINDER_SIZE * 1.4142135623730950488016887242097F) / SEGMENT_SIZE)))
#define BEING_TEXURE_SIZE           128
#define PROJECTILE_TEXURE_SIZE      64
#define CREATION_POINT_SIZE         96.0F
#define VORTEX_ROTAT_SPEED          (1024.0F / (0x40000000ULL / FRAME_TIME))
#define TEXTURE_ROTA_SPEED          (512.0F / (0x40000000ULL / FRAME_TIME))
#define SCROLL_ICON_SIZE            (SCROLL_SIZE * (37.0F / 64.0F))
#define BIG_BURN_SIZE               320U
#define SMALL_BURN_SIZE             64U
#define BURN_EFFECT_TIME            (0x30000000ULL / FRAME_TIME)
#define BOLT_TICKS                  (0x8000000ULL / FRAME_TIME)
#define FLASH_TICKS                 (0x6000000ULL / FRAME_TIME)
#define FLASH_SIZE                  512
#define LIGHTING_TX_SIZE            64
#define VIEW_TX_FACTOR              2
#define SHOP_ICONS_NUM              7

#define BEINGS_TEXTURES             {\
                                        tx_being_weak,\
                                        tx_being_ordinary,\
                                        tx_being_ranger,\
                                        tx_being_elite,\
                                        tx_being_elite_ranger,\
                                        tx_being_commander,\
                                        tx_being_warlock,\
                                        tx_being_ally_ordinary,\
                                    }
#define BEINGS_WEAPON_TEXTURES      {\
                                        tx_being_blade,\
                                        tx_sword,\
                                        tx_weapon1,\
                                        tx_sword,\
                                        tx_weapon,\
                                        tx_sword,\
                                        tx_weapon,\
                                        tx_void\
                                    }
#define PROJECTILE_TEXTURES         {\
                                        tx_projectile,\
                                        tx_h_projectile,\
                                        tx_void\
                                    }
#define VISUAL_EFFECT_FUNCS         {\
                                        RenderVisualEffectsType0,\
                                        RenderVisualEffectsType1,\
                                        RenderBurnEffect,\
                                        RenderVisualEffectsTimer,\
                                        RenderVisualEffectsBolt,\
                                        RenderFlashEffect,\
                                    }
#define PC_RECT                     {\
                                        VIEWFINDER_CENTER - half(PLAYER_SIZE),\
                                        VIEWFINDER_CENTER - half(PLAYER_SIZE) + PLAYER_REND_Y_SHIFT,\
                                        (float)PLAYER_SIZE,\
                                        (float)PLAYER_SIZE\
                                    }
#define PC_SCROLL_RECT              {\
                                        VIEWFINDER_CENTER,\
                                        VIEWFINDER_CENTER - SCROLL_SIZE + PLAYER_REND_Y_SHIFT,\
                                        SCROLL_SIZE,\
                                        SCROLL_SIZE\
                                    }
#define PC_SCROLL_ICON_RECT         {\
                                        VIEWFINDER_CENTER + SCROLL_SIZE * (7.0F / 32.0F),\
                                        VIEWFINDER_CENTER + SCROLL_SIZE * (13.0F / 64.0F - 1.0F) + PLAYER_REND_Y_SHIFT,\
                                        SCROLL_ICON_SIZE,\
                                        SCROLL_ICON_SIZE\
                                    }
#define SRC_SCROLL_RECT             {\
                                        0.0F,\
                                        0.0F,\
                                        SCROLL_TX_SIZE,\
                                        SCROLL_TX_SIZE\
                                    }
#define SRC_UNR_SCROLL_RECT         {\
                                        128.0F,\
                                        0.0F,\
                                        SCROLL_TX_SIZE,\
                                        SCROLL_TX_SIZE\
                                    }
#define ICON_TX_SIZE                64.0F
#define ICON_TX_SIZE_INT            64
#define ICON_SCRL_0_Y               ICON_TX_SIZE
#define SRC_SCROLL_0_ICON_RECT      {\
                                        0.0F,\
                                        ICON_SCRL_0_Y,\
                                        ICON_TX_SIZE,\
                                        ICON_TX_SIZE\
                                    }
#define VIEWFINDER_FRAME            {\
                                        rend_data->viewfinder_rect.x - rend_data->viewfinder_rect.y,\
                                        0.0F,\
                                        rend_data->viewfinder_rect.w + rend_data->viewfinder_rect.y * 2.0F,\
                                        rend_data->viewfinder_rect.h + rend_data->viewfinder_rect.y * 2.0F\
                                    }
#define PORTRAIT_X                  (FRAME_W * 3.0F)
#define PORTRAIT_Y                  (FRAME_W * 3.0F)
#define PC_SHIELD_RECT              {\
                                        VIEWFINDER_CENTER - half(PLAYER_SIZE * 2.0F),\
                                        VIEWFINDER_CENTER - half(PLAYER_SIZE * 2.0F) + PLAYER_REND_Y_SHIFT,\
                                        (float)PLAYER_SIZE * 2.0F,\
                                        (float)PLAYER_SIZE * 2.0F\
                                    }
#define WEAPON_ROTATION_POINT       {\
                                        half(WEAPON_SIZE),\
                                        WEAPON_SIZE * BLADE_HANDLER_POSITION\
                                    }
#define FRAME_PART_0                ((SDL_FRect){8.0F, 0.0F, 8.0F, 8.0F})
#define ROW_0_CHARS                 14
#define ROW_1_CHARS                 (ROW_0_CHARS + 17)
#define ROW_2_CHARS                 (ROW_1_CHARS + 13)
#define ROW_3_CHARS                 (ROW_2_CHARS + 15)
#define ROW_4_CHARS                 (ROW_3_CHARS + 16)
#define ROW_5_CHARS                 (ROW_4_CHARS + 17)
#define CHARACTERS_X_AND_W          {\
                                        {0,41},{42,83-42}, {84,125-84},  {126,167-126},{168,174-168},{175,221-175},{222,269-222},{270,290-270},{291,344-291},{345,360-345},{361,402-361},{403,418-403},{419,463-419},{463,506-463},\
                                        {0,45},{46,88-46}, {89,130-89},  {131,172-131},{173,213-173},{214,256-214},{257,269-257},{270,282-270},{283,300-283},{301,348-301},{349,363-349},{364,378-364},{379,402-379},{403,426-403},{427,453-427},{454,492-454},{493,512-493},\
                                        {0,42},{43,109-43},{110,147-110},{148,192-148},{193,251-193},{252,298-252},{299,345-299},{346,357-346},{358,406-358},{407,435-407},{436,466-436},{467,500-467},{501,512-501},\
                                        {0,33},{34,49-34}, {50,77-50},   {78,135-78},  {136,183-136},{184,227-184},{228,274-228},{275,305-275},{306,349-306},{350,365-350},{366,391-366},{392,423-392},{424,451-424},{452,482-452},{483,512-483},\
                                        {0,33},{34,77-34}, {78,109-78},  {110,153-110},{154,194-154},{195,226-195},{227,258-227},{259,286-259},{287,319-287},{320,345-320},{346,372-346},{373,404-373},{405,440-405},{441,466-441},{467,495-467},{496,511-496},\
                                        {0,21},{22,54-22}, {55,83-55},   {84,117-84},  {118,145-118},{146,176-146},{177,204-177},{205,220-205},{221,243-221},{244,275-244},{276,327-276},{328,363-328},{364,392-364},{393,418-393},{419,440-419},{441,468-441},{469,512-469},\
                                        {0,25},{26,53-26}, {54,85-54},   {86,118-86},  {119,150-119},{151,182-151},{183,211-183},{212,232-212},{233,241-233},{242,262-242},{263,276-263},{277,314-277},{315,341-315},{342,374-342},{375,395-375},{396,432-396},{433,444-433},{445,457-445},{458,36}\
                                    }
#define FIRST_ICON_SRC_RECT         {\
                                        0.0F,\
                                        0.0F,\
                                        ICON_TX_SIZE,\
                                        ICON_TX_SIZE\
                                    }
#define VIEWFINDER_RECT             {\
                                        (int)(half(rend_data->window_w - rend_data->viewfinder)),\
                                        (int)(half(rend_data->window_h - rend_data->viewfinder)),\
                                        (int)(rend_data->viewfinder),\
                                        (int)(rend_data->viewfinder)\
                                    }
#define VISIBLE_RECT                {\
                                        0.0F,\
                                        0.0F,\
                                        rend_data->viewfinder,\
                                        rend_data->viewfinder\
                                    }
#define MOUSE_RECT                  {\
                                        rend_data->viewfinder_rect.x,\
                                        rend_data->viewfinder_rect.y,\
                                        rend_data->viewfinder_rect.w,\
                                        (int)(rend_data->viewfinder * VIEWFINDER_BEFORE_PC_PART - GUN_SIGHT_MIN_DISTANCE)\
                                    }
#define MANAGE_SCROLLS_RECT         {\
                                        rend_data->viewfinder_rect.x + half(FRAME_W),\
                                        rend_data->viewfinder_rect.y + icon_size + FRAME_W * 2.5F,\
                                        shift * ICONS_IN_VIEWF_ROW,\
                                        shift * 5\
                                    }
#define MENU_RECT                   {\
                                        rend_data->viewfinder_rect.x + FRAME_W,\
                                        rend_data->viewfinder_rect.y + icon_size + FRAME_W * 2.5F,\
                                        rend_data->viewfinder_rect.w - FRAME_W * 2.0F,\
                                        shift * OPTIONS_NUM\
                                    }
#define RGB_LOWER_RANK              ((Uint8[]){102, 63, 25})
#define RGB_MEDIUM_RANK             ((Uint8[]){223, 223, 223})
#define RGB_HIGH_RANK               ((Uint8[]){255, 191, 0})
#define PC_RGB_0                    ((Uint8[]){223, 223, 223})
#define PC_RGB_1                    RGB_HIGH_RANK
#define BEING0_RGB_0                ((Uint8[]){127, 255, 127})
#define BEING0_RGB_1                RGB_LOWER_RANK
#define BEING1_RGB_0                ((Uint8[]){0, 255, 0})
#define BEING1_RGB_1                RGB_LOWER_RANK
#define BEING2_RGB_0                ((Uint8[]){0, 255, 222})
#define BEING2_RGB_1                RGB_LOWER_RANK
#define BEING3_RGB_0                ((Uint8[]){0, 127, 0})
#define BEING3_RGB_1                RGB_MEDIUM_RANK
#define BEING4_RGB_0                ((Uint8[]){0, 127, 111})
#define BEING4_RGB_1                RGB_MEDIUM_RANK
#define BEING5_RGB_0                ((Uint8[]){0, 63, 0})
#define BEING5_RGB_1                RGB_HIGH_RANK
#define BEING6_RGB_0                ((Uint8[]){0, 63, 55})
#define BEING6_RGB_1                RGB_HIGH_RANK
#define ALLY0_RGB_0                 ((Uint8[]){179, 255, 0})
#define ALLY0_RGB_1                 RGB_MEDIUM_RANK
#define PROJECTILE0_RGBA            ((Uint8[]){255, 112, 0, 255})
#define PROJECTILE1_RGBA            ((Uint8[]){255, 255, 0, 255})
#define MP_TEXT_RGB                 0U, 208U, 255U
#define MP_COST_TEXT_RGB            255U, 0U, 127U
#define WHITE_RGB                   255U, 255U, 255U
#define BLACK_RGB                   0U, 0U, 0U
#define BOLT_RGB_F                  0xa.0p-4F, 0x9.0p-4F, 1.0F
#define DAMAGE_VIS_EFFECT(position) ((Visual_effect){\
                                        (SDL_FPoint)position,\
                                        (Uint16)    (0x10000000ULL / FRAME_TIME),\
                                        (Uint16)    (0x10000000ULL / FRAME_TIME),\
                                        (Uint16)    32U,\
                                        (Uint8)     tx_damage_test,\
                                        (Uint8)     visual_effect_t0\
                                    })
#define BONUS_VIS_EFFECT(position) ((Visual_effect){\
                                        (SDL_FPoint)position,\
                                        (Uint16)    (0x10000000ULL / FRAME_TIME),\
                                        (Uint16)    (0x10000000ULL / FRAME_TIME),\
                                        (Uint16)    16U,\
                                        (Uint8)     tx_bonus_effect,\
                                        (Uint8)     visual_effect_t0\
                                    })
#define CURSE_VIS_EFFECT(position) ((Visual_effect){\
                                        (SDL_FPoint)position,\
                                        (Uint16)    (0x10000000ULL / FRAME_TIME),\
                                        (Uint16)    (0x10000000ULL / FRAME_TIME),\
                                        (Uint16)    32U,\
                                        (Uint8)     tx_curse_effect,\
                                        (Uint8)     visual_effect_t0\
                                    })
#define DEAD_VIS_EFFECT(position)   ((Visual_effect){\
                                        (SDL_FPoint)position,\
                                        (Uint16)    (0x20000000ULL / FRAME_TIME),\
                                        (Uint16)    (0x20000000ULL / FRAME_TIME),\
                                        (Uint16)    64U,\
                                        (Uint8)     tx_damage_test,\
                                        (Uint8)     visual_effect_t0\
                                    })
	                                //position; ticks_left; start_ticks; size; type; tx_num;
#define PORTAL_VIS_EFFECT(position) ((Visual_effect){\
                                        (SDL_FPoint)position,\
                                        (Uint16)    ((0x40000000ULL / FRAME_TIME) / ((unsigned int)OPENING_PORTAL_TICKS / 128U)),\
                                        (Uint16)    ((0x40000000ULL / FRAME_TIME) / ((unsigned int)OPENING_PORTAL_TICKS / 128U)),\
                                        (Uint16)    128U,\
                                        (Uint8)     tx_creation_point,\
                                        (Uint8)     visual_effect_t1\
                                    })
#define PROJE_VIS_EFFECT(position)  ((Visual_effect){\
                                        (SDL_FPoint)position,\
                                        (Uint16)    (0x8000000ULL / FRAME_TIME),\
                                        (Uint16)    (0x8000000ULL / FRAME_TIME),\
                                        (Uint16)    16U,\
                                        (Uint8)     tx_creation_point,\
                                        (Uint8)     visual_effect_t0\
                                    })
#define BURN_VIS_EF(position, size) ((Visual_effect){\
                                        (SDL_FPoint)position,\
                                        (Uint16)    BURN_EFFECT_TIME,\
                                        (Uint16)    BURN_EFFECT_TIME,\
                                        (Uint16)    size,\
                                        (Uint8)     0U,\
                                        (Uint8)     visual_effect_t2\
                                    })
#define BURN_EF_TIM(posit, si, del) ((Visual_effect){\
                                        (SDL_FPoint)posit,\
                                        (Uint16)    del,\
                                        (Uint16)    BURN_EFFECT_TIME,\
                                        (Uint16)    si,\
                                        (Uint8)     0U,\
                                        (Uint8)     visual_effect_t2_timer\
                                    })
#define CIPHER_NUMS                 {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9}
#define HORDE_ALERT                 {H,o,r,d,a,exclam_m,char_end}
#define HORDE_ALERT_X               1.0F
#define HORDE_ALERT_Y               1.0F
#define HORDE_ALERT_SIZE            16.0F
#define MENU_TEXTS                  {\
                                        (Uint8[]){U,s,t,a,w,i,e,n,i,a},\
                                        (Uint8[]){C,o,f,n,i,j,sp,d,o,sp,p,u,n,k,t,u,sp,k,o,n,t,r,o,l,n,e,g,o},\
                                        (Uint8[]){K,o,n,t,y,n,u,u,j},\
                                        (Uint8[]){Z,a,p,i,s,z,sp,i,sp,w,y,j,d,ź},\
                                        (Uint8[]){W,y,j,d,ź},\
                                    }
#define MENU_TEXTS_SIZES       {\
                                        10U,\
                                        28U,\
                                        9U,\
                                        14U,\
                                        5U,\
                                    }
#define MAIN_MENU_TEXTS             {\
                                        (Uint8[]){N,o,w,a,sp,g,r,a},\
                                        (Uint8[]){W,c,z,y,t,a,j,sp,g,r,ę},\
                                        (Uint8[]){U,s,t,a,w,i,e,n,i,a},\
                                        (Uint8[]){A,u,t,o,r,z,y},\
                                        (Uint8[]){W,y,j,ś,c,i,e},\
                                    }
#define MAIN_MENU_TEXTS_SIZES       {\
                                        8U,\
                                        11U,\
                                        10U,\
                                        7U,\
                                        7U,\
                                    }
#define DEFEAT_TEXT                 {P,o,r,a,ż,k,a}
#define DEFEAT_TEXT_LEN             7U
#define VICTORY_TEXT                {S,u,k,c,e,s,exclam_m}
#define VICTORY_TEXT_LEN            7U


#define TEXTURE_FILES_NAMES         {\
                                        "img3",\
                                        "img4",\
                                        "img5",\
                                        "img6",\
                                        "img7",\
                                        "img9",\
                                        "imgA",\
                                        "imgB",\
                                        "imgC",\
                                        "imgD",\
                                        "imgE",\
                                        "imgF",\
                                        "img10",\
                                        "img11",\
                                        "img13",\
                                        "img15",\
                                        "img18",\
                                        "fnt",\
                                        "img19",\
                                        "img1A",\
                                        "img1B",\
                                        "img1C",\
                                        "img1D",\
                                        "img1E",\
                                        "img21",\
                                        "img22"\
                                    }

#define BEING_TEXTURE0_FILE_NAME    "img8"
#define BEING_TEXTURE1_FILE_NAME    "img12"
#define BACKGROUND_TX0_FILE_NAME    "img16"
#define BACKGROUND_TX1_FILE_NAME    "img17"
#define PROJECTILE_TX_FILE_NAME     "img2"
#define PC_TX0_FILE_NAME            "img0"
#define PC_TX1_FILE_NAME            "img14"
#define DODGE_TX_FILE_NAME          "img1F"
#define PLUS_UP_TX_FILE_NAME        "img20"
#define ICONS_TX_FILE_NAME          "img5"

static void RemoveVisalEffect(Visual_effects* const, const unsigned int);
static void RenderVisualEffects(Render_data* const, Game_data* const);
static void DrawBeings(Render_data* const);
static void DrawColouredThings(Render_data* const);
static void DrawBeing(Render_data* const, SDL_Texture**, SDL_Surface* const, SDL_Surface* const, const Uint8* const, const Uint8* const);
static void DrawColouredThing(Render_data* const, SDL_Texture**, SDL_Surface* const, const Uint8* const);
static void RenderHumanPlayerBlade(Render_data* const, Blade* const);
static void RenderProjectiles(Render_data* const, Game_data* const);
static void RenderBeing(Render_data* const, Game_data* const, Being* const, const unsigned int, const unsigned int);
static void RenderBeings(Render_data* const, Game_data* const, Segment** const, const unsigned int);
static void RenderMap(Render_data* const, const Game_data* const, Player* const);
static bool GetRenderPointFromTrue(Render_data* const, const float, const float, const Player* const, SDL_FPoint* const, World* const);
static bool GetRenderPointFromTrueWithKnownSegmentVisibility(Render_data* const, const float, const float, const Player* const, SDL_FPoint* const);
static void RenderPlayerStatus(Render_data* const, Player* const, const Game_data* const);
static void SetSineCosine(Render_data* const, Player* const);
static void RenderTerrain(Render_data* const, Game_data* const, Segment** const, unsigned int* const);
static bool GetExtendedRenderPointFromTrue(Render_data* const, const float, const float, const float, const Player* const, SDL_FPoint* const);
static void RenderGunSight(Render_data* const);
static void RenderDoors(Render_data* const, Game_data* const);
static bool GetRenderPointFromTrueShopEdition(Render_data* const, const float, const float, const float, const Player* const, SDL_FPoint* const, World* const);
static void RenderStaticThings(Render_data* const, Game_data* const);
static void RenderStaticThing(Render_data* const, const float, const float, Player* const, const float, const int, World* const);
static void RenderStaticThingRotating(Render_data* const, const float, const float, Player* const, const float, const int, World* const, const float);
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
static int GetMouseMenuPositionNum(const Render_data* const);
static SDL_FRect GetScrollTextureSrcRect(unsigned int);

enum inoc_position{
    ic_key,
    ic_coin,
    ic_arm,
    ic_hp,
    ic_fp,
    ic_directions,
    ic_mp
};

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

static void SetViewTexture(Render_data* const rend_data){
	texture(tx_view) = SDL_CreateTexture(rend_data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rend_data->viewfinder / VIEW_TX_FACTOR, rend_data->viewfinder / VIEW_TX_FACTOR);
	if(!texture(tx_view)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
		exit(-1);
	}
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_view));
	SDL_SetRenderScale(rend_data->renderer, 1.0F / (float)VIEW_TX_FACTOR, 1.0F / (float)VIEW_TX_FACTOR);
	SDL_SetTextureScaleMode(texture(tx_view), SDL_SCALEMODE_PIXELART);
}

static void DrawShopIcons(Render_data* const rend_data){
	SDL_Surface* base_surface = SDL_CreateSurface(ICON_TX_SIZE_INT * SHOP_ICONS_NUM, ICON_TX_SIZE_INT, SDL_PIXELFORMAT_RGBA8888);
	char* bmp_path = NULL;
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), ICONS_TX_FILE_NAME);
	SDL_Surface* surface = SDL_LoadBMP(bmp_path);
	SDL_free(bmp_path);
	SDL_Rect src_rect = {ICON_TX_SIZE_INT * 6, 0, ICON_TX_SIZE_INT, ICON_TX_SIZE_INT};
	SDL_Rect dst_rect = {0, 0, ICON_TX_SIZE_INT, ICON_TX_SIZE_INT};
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	dst_rect.x = ICON_TX_SIZE_INT;
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	dst_rect.x = ICON_TX_SIZE_INT * 2;
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	src_rect.x = 0;
	dst_rect.x = ICON_TX_SIZE_INT * 3;
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	dst_rect.x = ICON_TX_SIZE_INT * 4;
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	src_rect.x = ICON_TX_SIZE_INT * 2;
	dst_rect.x = ICON_TX_SIZE_INT * 5;
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	SDL_DestroySurface(surface);
	
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), DODGE_TX_FILE_NAME);
	surface = SDL_LoadBMP(bmp_path);
	SDL_free(bmp_path);
	dst_rect.x = ICON_TX_SIZE_INT * 6;
	SDL_BlitSurface(surface, NULL, base_surface, &dst_rect);
	SDL_DestroySurface(surface);
	
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), "fnt");
	surface = SDL_LoadBMP(bmp_path);
	SDL_free(bmp_path);
	SDL_FPoint char_x_and_w = GetCharacterXPositionAndWidth(n1);
	src_rect = (SDL_Rect){char_x_and_w.x, GetCharacterRow(n1) * CHARS_ROW_HEIGHT, char_x_and_w.y, CHARS_ROW_HEIGHT};
	dst_rect = (SDL_Rect){ICON_TX_SIZE_INT * 1, dst_rect.y, char_x_and_w.y / 2, CHARS_ROW_HEIGHT / 2};
	SDL_BlitSurface(surface, NULL, surface, NULL);
	SDL_BlitSurfaceScaled(surface, &src_rect, base_surface, &dst_rect, SDL_SCALEMODE_LINEAR);
	dst_rect.x += ICON_TX_SIZE_INT;
	SDL_BlitSurfaceScaled(surface, &src_rect, base_surface, &dst_rect, SDL_SCALEMODE_LINEAR);
	dst_rect.x += char_x_and_w.y / 2 - ICON_TX_SIZE_INT;
	char_x_and_w = GetCharacterXPositionAndWidth(n0);
	src_rect = (SDL_Rect){char_x_and_w.x, GetCharacterRow(n0) * CHARS_ROW_HEIGHT, char_x_and_w.y, CHARS_ROW_HEIGHT};
	dst_rect.w = char_x_and_w.y / 2;
	SDL_BlitSurfaceScaled(surface, &src_rect, base_surface, &dst_rect, SDL_SCALEMODE_LINEAR);
	dst_rect.x += ICON_TX_SIZE_INT;
	SDL_BlitSurfaceScaled(surface, &src_rect, base_surface, &dst_rect, SDL_SCALEMODE_LINEAR);
	dst_rect.x += char_x_and_w.y / 2;
	SDL_BlitSurfaceScaled(surface, &src_rect, base_surface, &dst_rect, SDL_SCALEMODE_LINEAR);

	char_x_and_w = GetCharacterXPositionAndWidth(s9);
	src_rect = (SDL_Rect){char_x_and_w.x, GetCharacterRow(s9) * CHARS_ROW_HEIGHT, char_x_and_w.y, CHARS_ROW_HEIGHT};
	dst_rect = (SDL_Rect){ICON_TX_SIZE_INT * 4 + 16, dst_rect.y - 8, char_x_and_w.y, CHARS_ROW_HEIGHT};
	SDL_BlitSurface(surface, NULL, surface, NULL);
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	SDL_DestroySurface(surface);

	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), PLUS_UP_TX_FILE_NAME);
	surface = SDL_LoadBMP(bmp_path);
	SDL_free(bmp_path);
	src_rect = (SDL_Rect){0, 0, 32, 32};
	dst_rect.x = ICON_TX_SIZE_INT / 2;
	dst_rect.y = ICON_TX_SIZE_INT / 2;
	SDL_SetSurfaceColorMod(surface, 0U, 255U, 0U);
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	dst_rect.x += ICON_TX_SIZE_INT;
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	dst_rect.x += ICON_TX_SIZE_INT;
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	dst_rect.x += ICON_TX_SIZE_INT;
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	dst_rect.x += ICON_TX_SIZE_INT;
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);

	src_rect.x = 32;
	dst_rect.x = ICON_TX_SIZE_INT * 5 + ICON_TX_SIZE_INT / 2;
	SDL_SetSurfaceColorMod(surface, 0U, 255U, 0U);
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	dst_rect.x += ICON_TX_SIZE_INT;
	SDL_BlitSurface(surface, &src_rect, base_surface, &dst_rect);
	SDL_DestroySurface(surface);
	
	texture(tx_shop_icons) = SDL_CreateTextureFromSurface(rend_data->renderer, base_surface);
	if(!texture(tx_shop_icons)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	SDL_DestroySurface(base_surface);
}

static void DrawTextures(Render_data* const rend_data){
	DrawBeings(rend_data);
	DrawColouredThings(rend_data);
	DrawShopIcons(rend_data);
	SDL_Surface* surface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA8888);
	SDL_WriteSurfacePixel(surface, 0, 0, 0U, 0U, 0U, 0U);
	texture(tx_void) = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!texture(tx_void)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError()); exit(-1);
	}
	SDL_DestroySurface(surface);
}

static inline void CreateTexruresFromFiles(Render_data* const rend_data){
	SDL_Surface* surface = NULL;
	char* bmp_path = NULL;
	const char* const texture_files[] = TEXTURE_FILES_NAMES;
	for(int i = 0; i < SDL_arraysize(texture_files); ++i){
		SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), *(texture_files + i));
		surface = SDL_LoadBMP(bmp_path);
		if(!surface){
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError()); exit(-1);
		}
		texture(i) = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
		if(!texture(i)){
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError()); exit(-1);
		}
		SDL_DestroySurface(surface);
		SDL_free(bmp_path);
	}
}

static inline void SetTexturesBlendAndScaleModes(Render_data* const rend_data){
	SDL_SetTextureBlendMode(texture(tx_lighting), SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(texture(tx_barrier), SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(texture(tx_bonus_effect), SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(texture(tx_bolt), SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(texture(tx_projectile), SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(texture(tx_h_projectile), SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(texture(tx_pixel), SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(texture(tx_void), SDL_BLENDMODE_ADD);
	SDL_SetTextureScaleMode(texture(tx_void), SDL_SCALEMODE_NEAREST);
	SDL_SetTextureScaleMode(texture(tx_icons), SDL_SCALEMODE_PIXELART);
	SDL_SetTextureScaleMode(texture(tx_shop_icons), SDL_SCALEMODE_PIXELART);
}

void GraphicsInitiation(Render_data* const rend_data){
	SDL_zeroa(rend_data->textures);
	if(!SDL_Init(SDL_INIT_VIDEO)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError()); exit(-1);
	}
	if(!SDL_CreateWindowAndRenderer("KacWindow", WINDOW_START_W, WINDOW_START_H, SDL_WINDOW_BORDERLESS, &rend_data->window, &rend_data->renderer)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError()); exit(-1);
	}
	CreateTexruresFromFiles(rend_data);
	texture(tx_map) = SDL_CreateTexture(rend_data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, BIG_SEGMENTS_X, BIG_SEGMENTS_X);
	texture(tx_lighting) = SDL_CreateTexture(rend_data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LIGHTING_TX_SIZE, LIGHTING_TX_SIZE);
	if(!(texture(tx_lighting) && texture(tx_map))){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError()); exit(-1);
	}
	DrawTextures(rend_data);
	SetMouseBarrier(rend_data);
	SetTexturesBlendAndScaleModes(rend_data);
	SDL_SetRenderDrawBlendMode(rend_data->renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(rend_data->renderer, NULL);
	ResetRenderData(rend_data);
}

extern inline void AddVisalEffect(Visual_effects* const ves, const Visual_effect* const ve){
	if(ves->num < MAX_VISUAL_EFFECTS_NUM){
		*(ves->array + ves->num++) = *ve;
	}else{
		*(ves->array + SDL_rand(MAX_VISUAL_EFFECTS_NUM)) = *ve;
	}
}

static inline void RemoveVisalEffect(Visual_effects* const ves, const unsigned int indx){
	if(indx != ves->num - 1U){
		*(ves->array + indx) = *(ves->array + ves->num - 1);
	}
	--ves->num;
}

static void RenderVisualEffectsType0(Visual_effect* const ve, Game_data* const gd){
	Render_data* const rend_data = gd->rend_data_ptr;
	SDL_FPoint rend_point;
	Segment* seg = GetSegmentUnsafe(&gd->world, ve->position.x, ve->position.y);
	if(!(seg && (seg->flags & segment_in_sight) && GetExtendedRenderPointFromTrue(rend_data, ve->position.x, ve->position.y, half(ve->data0.size), human(gd), &rend_point))){
		return;
	}
	if(ve->ticks_left < ve->data0.start_ticks * 3U / 4U){
		SDL_SetTextureAlphaModFloat(texture(ve->data0.tx_num), ve->ticks_left / (ve->data0.start_ticks * 0.75F));
	}else{
		SDL_SetTextureAlphaModFloat(texture(ve->data0.tx_num), 1.0F - ((float)ve->ticks_left - ve->data0.start_ticks * 0.75F) / (ve->data0.start_ticks * 0.5F));
	}
	const float size = ve->data0.size * (1.5F - ve->ticks_left / (float)ve->data0.start_ticks);
	SDL_RenderTexture(rend_data->renderer, texture(ve->data0.tx_num), NULL, &(SDL_FRect){
		rend_point.x - half(size),
		rend_point.y - half(size),
		size,
		size
	});
}

static void RenderVisualEffectsType1(Visual_effect* const ve, Game_data* const gd){
	Render_data* const rend_data = gd->rend_data_ptr;
	SDL_FPoint rend_point;
	Segment* seg = GetSegmentUnsafe(&gd->world, ve->position.x, ve->position.y);
	if(!(seg && (seg->flags & segment_in_sight) && GetExtendedRenderPointFromTrue(rend_data, ve->position.x, ve->position.y, half(ve->data0.size), human(gd), &rend_point))){
		return;
	}
	float size;
	if(ve->ticks_left < ve->data0.start_ticks / 2U){
		SDL_SetTextureAlphaModFloat(texture(ve->data0.tx_num), ve->ticks_left / (ve->data0.start_ticks * 0.5F));
		size = ve->data0.size * (ve->ticks_left / (ve->data0.start_ticks * 0.5F));
	}else{
		const float level = 2.0F - ve->ticks_left / (ve->data0.start_ticks * 0.5F);
		SDL_SetTextureAlphaModFloat(texture(ve->data0.tx_num), level);
		size = ve->data0.size * level;
	}
	SDL_RenderTextureRotated(rend_data->renderer, texture(ve->data0.tx_num), NULL, &(SDL_FRect){
		rend_point.x - half(size),
		rend_point.y - half(size),
		size,
		size
	}, (double)(rend_data->counter * VORTEX_ROTAT_SPEED), NULL, SDL_FLIP_NONE);
}

static void RenderBurnEffect(Visual_effect* const ve, Game_data* const gd){
	Render_data* const rend_data = gd->rend_data_ptr;
	SDL_FPoint rend_point;
	Segment* seg = GetSegmentUnsafe(&gd->world, ve->position.x, ve->position.y);
	if(!(seg && (seg->flags & segment_in_sight) && GetExtendedRenderPointFromTrue(rend_data, ve->position.x, ve->position.y, half(ve->data0.size), human(gd), &rend_point))){
		return;
	}
	float size;
	SDL_Texture* texture;
	if(ve->ticks_left > ve->data0.start_ticks - 2U){
		texture = texture(tx_pixel);
		SDL_SetTextureColorModFloat(texture, 0.625F, 0.5F, 1.0F);
		size = (float)ve->data0.size * ((float)ve->ticks_left / (float)ve->data0.start_ticks);
	}else if(ve->ticks_left >= ve->data0.start_ticks * 3U / 4U){
		texture = texture(tx_pixel);
		const float red = ((float)ve->ticks_left - ve->data0.start_ticks * 0.75F) / (ve->data0.start_ticks * 0.25F);
		SDL_SetTextureColorModFloat(texture, red, pow2(red) * 0.625F, 0.0F);
		size = (float)ve->data0.size * ((float)ve->ticks_left / (float)ve->data0.start_ticks);
	}else{
		if(ve->ticks_left == ve->data0.start_ticks * 3U / 4U - 1U && SDL_rand(2)){
			ve->ticks_left = 1U;
			return;
		}
		texture = texture(tx_pixel_blend);
		const float level = 1.0F - (float)ve->ticks_left / (ve->data0.start_ticks * 0.75F);
		SDL_SetTextureColorModFloat(texture, level, level, level);
		size = (float)ve->data0.size * (0.5F - (float)ve->ticks_left / (float)ve->data0.start_ticks * 0.5F);
	} 
	SDL_SetTextureAlphaModFloat(texture, (float)ve->ticks_left / (float)ve->data0.start_ticks * 0.75F);
	SDL_RenderTexture(rend_data->renderer, texture, NULL, &(SDL_FRect){
		rend_point.x - half(size),
		rend_point.y - half(size),
		size,
		size
	});
}

static void RenderVisualEffectsTimer(Visual_effect* const ve, Game_data* const gd){
	if(ve->ticks_left == 1U){
		ve->ticks_left = ve->data0.start_ticks + 1U;
		--(ve->type);
	}
}

static void RenderVisualEffectsBolt(Visual_effect* const ve, Game_data* const gd){
	Render_data* const rend_data = gd->rend_data_ptr;
	SDL_FPoint rend_point;
	SDL_FPoint rend_point1;
	Segment* seg = GetSegmentUnsafe(&gd->world, ve->position.x, ve->position.y);
	if(!(seg && (seg->flags & segment_in_sight) && GetExtendedRenderPointFromTrue(rend_data, ve->position.x, ve->position.y, SEGMENT_SIZE, human(gd), &rend_point)
		&& GetExtendedRenderPointFromTrue(rend_data, ve->data1.start_position.x, ve->data1.start_position.y, SEGMENT_SIZE, human(gd), &rend_point1))){
		return;
	}
	const Uint64 seed = pow2((uint_fast16_t)ve->position.x * (uint_fast16_t)ve->position.y * (uint_fast16_t)ve->data1.start_position.x);
	SDL_srand(seed + 1ULL);
	int count = 6 + SDL_rand(7);
	const float angle = ve->data1.angle / (float)UINT8_MAX * FULL_ANGLE - human(gd)->direction;
	const float sine_angle = SineSafe(angle);
	const float cosine_angle = CosiSafe(angle);
	const float parts = count - 1.0F;
	const float distance_x = rend_point.x - rend_point1.x;
	const float distance_y = rend_point.y - rend_point1.y;
	const float distance_squared = pow2(distance_x) + pow2(distance_y);
	const float x_part = distance_x / parts;
	const float y_part = distance_y / parts;
	const float shift0 = distance_squared / pow2(rend_data->viewfinder) * 64.0F;
	float shift_x = shift0 * cosine_angle;
	float shift_y = shift0 * sine_angle;
	const float width = (float)count * 1.5F;
	const float width_x = width * cosine_angle;
	const float width_y = width * sine_angle;
	const float width_step_x = width_x / (float)count * 0.75F;
	const float width_step_y = width_y / (float)count * 0.75F;
	if(SDL_rand(2)){
		shift_x = -shift_x;
		shift_y = -shift_y;
	}
	if(ve->ticks_left > BOLT_TICKS - (count - 2U)){
		count = BOLT_TICKS - ve->ticks_left + 2;
		int num_verts = count * 2;
		int num_indices = (count - 1) * 6;
		SDL_Vertex verts[num_verts];
		verts->tex_coord = (SDL_FPoint){0.0F, 0.875F};
		verts->color = (SDL_FColor)(SDL_FColor){BOLT_RGB_F, SDL_ALPHA_OPAQUE_FLOAT};
		(verts + 1)->tex_coord = (SDL_FPoint){1.0F, 0.875F};
		(verts + 1)->color = (SDL_FColor)(SDL_FColor){BOLT_RGB_F, SDL_ALPHA_OPAQUE_FLOAT};
		(verts + 2)->tex_coord = (SDL_FPoint){0.0F, 0.0F};
		(verts + 2)->color = (SDL_FColor){BOLT_RGB_F, SDL_ALPHA_OPAQUE_FLOAT};
		(verts + 3)->tex_coord = (SDL_FPoint){1.0F, 0.0F};
		(verts + 3)->color = (SDL_FColor){BOLT_RGB_F, SDL_ALPHA_OPAQUE_FLOAT};
		for(unsigned int i = 4U; i < num_verts; ++i){
			(verts + i)->tex_coord = (SDL_FPoint){1.0F * (i % 2U), 0.5F};
			(verts + i)->color = (SDL_FColor){BOLT_RGB_F, SDL_ALPHA_OPAQUE_FLOAT};
		}
		int indices[num_indices];
		for(int i = 0; i < (count - 1) * 2; ++i){
			for(int j = 0; j < 3; ++j){
				*(indices + i * 3 + j) = i + j;
			}
		}
		verts->position = (SDL_FPoint){
			rend_point1.x - width_x,
			rend_point1.y - width_y
		};
		(verts + 1)->position = (SDL_FPoint){
			rend_point1.x + width_x,
			rend_point1.y + width_y
		};
		float rand[count - 1];
		float rand1[count - 1];
		for(unsigned int i = 0U; i < count - 1; ++i){
			*(rand + i) = SDL_randf() * 0.75F + 0.25F;
			*(rand1 + i) = (SDL_randf() - 0.5F) * 0.875F;
		}
		for(unsigned int i = 1U; i < count; i += 2U){
			(verts + i * 2)->position = (SDL_FPoint){
				rend_point1.x + x_part * (i + *(rand1 + i - 1U)) + shift_x * *(rand + i - 1U) - (width_x - i * width_step_x),
				rend_point1.y + y_part * (i + *(rand1 + i - 1U)) + shift_y * *(rand + i - 1U) - (width_y - i * width_step_y)
			};
			(verts + i * 2 + 1)->position = (SDL_FPoint){
				rend_point1.x + x_part * (i + *(rand1 + i - 1U)) + shift_x * *(rand + i - 1U) + (width_x - i * width_step_x),
				rend_point1.y + y_part * (i + *(rand1 + i - 1U)) + shift_y * *(rand + i - 1U) + (width_y - i * width_step_y)
			};
		}
		for(unsigned int i = 2U; i < count; i += 2U){
			(verts + i * 2)->position = (SDL_FPoint){
				rend_point1.x + x_part * (i + *(rand1 + i - 1U)) - shift_x * *(rand + i - 1U) - (width_x - i * width_step_x),
				rend_point1.y + y_part * (i + *(rand1 + i - 1U)) - shift_y * *(rand + i - 1U) - (width_y - i * width_step_y)
			};
			(verts + i * 2 + 1)->position = (SDL_FPoint){
				rend_point1.x + x_part * (i + *(rand1 + i - 1U)) - shift_x * *(rand + i - 1U) + (width_x - i * width_step_x),
				rend_point1.y + y_part * (i + *(rand1 + i - 1U)) - shift_y * *(rand + i - 1U) + (width_y - i * width_step_y)
			};
		}
		SDL_RenderGeometry(rend_data->renderer, texture(tx_bolt), verts, num_verts, indices, num_indices);
	}else{
		float rand[count - 2];
		float rand1[count - 1];
		for(unsigned int i = 0U; i < count - 2; ++i){
			*(rand + i) = SDL_randf() * 0.75F + 0.25F;
			*(rand1 + i) = (SDL_randf() - 0.5F) * 0.875F;
		}
		SDL_SetRenderTarget(rend_data->renderer, texture(tx_lighting));
		const float level = (float)ve->ticks_left / (float)BOLT_TICKS * 0.5F;
		SDL_SetTextureAlphaModFloat(texture(tx_pixel), level);
		SDL_SetTextureColorModFloat(texture(tx_pixel), BOLT_RGB_F);
		const float size =  2048.0F + 1024.0F * SDL_randf();
		SDL_RenderTexture(rend_data->renderer, texture(tx_pixel), NULL, &(SDL_FRect){
			rend_point.x - half(size),
			rend_point.y - half(size),
			size,
			size
		});
		SDL_SetTextureAlphaModFloat(texture(tx_pixel), SDL_ALPHA_OPAQUE);
		const float size1 =  size * 0x1.0p-4F;
		SDL_RenderTexture(rend_data->renderer, texture(tx_pixel), NULL, &(SDL_FRect){
			rend_point.x - half(size1),
			rend_point.y - half(size1),
			size1,
			size1
		});
		SDL_SetRenderTarget(rend_data->renderer, texture(tx_view));
		const float alpha = ve->ticks_left / (float)BOLT_TICKS * (SDL_randf() + 1.0F);
		const int num_verts = count * 2;
		const int num_indices = (count - 1) * 6;
		SDL_Vertex verts[num_verts];
		verts->tex_coord = (SDL_FPoint){0.0F, 1.0F};
		verts->color = (SDL_FColor){BOLT_RGB_F, alpha};
		(verts + 1)->tex_coord = (SDL_FPoint){1.0F, 1.0F};
		(verts + 1)->color = (SDL_FColor){BOLT_RGB_F, alpha};
		(verts + 2)->tex_coord = (SDL_FPoint){0.0F, 0.0F};
		(verts + 2)->color = (SDL_FColor){BOLT_RGB_F, alpha};
		(verts + 3)->tex_coord = (SDL_FPoint){1.0F, 0.0F};
		(verts + 3)->color = (SDL_FColor){BOLT_RGB_F, alpha};
		for(unsigned int i = 4U; i < count * 2U - 2U; i += 2U){
			(verts + i)->tex_coord = (SDL_FPoint){0.0F, 0.5F};
			(verts + i)->color = (SDL_FColor){BOLT_RGB_F, alpha};
		}
		for(unsigned int i = 3U; i < count * 2U - 2U; i += 2U){
			(verts + i)->tex_coord = (SDL_FPoint){1.0F, 0.5F};
			(verts + i)->color = (SDL_FColor){BOLT_RGB_F, alpha};
		}
		(verts + count * 2 - 2)->tex_coord = (SDL_FPoint){0.0F, 0.0F};
		(verts + count * 2 - 2)->color = (SDL_FColor){BOLT_RGB_F, alpha};
		(verts + count * 2 - 1)->tex_coord = (SDL_FPoint){1.0F, 0.0F};
		(verts + count * 2 - 1)->color = (SDL_FColor){BOLT_RGB_F, alpha};
		for(unsigned int i = count * 2U; i < num_verts; ++i){
			(verts + i)->tex_coord = (SDL_FPoint){0.5F, 0.0F};
			(verts + i)->color = (SDL_FColor){BOLT_RGB_F, alpha};
		}
		int indices[num_indices];
		for(int i = 0; i < (count - 1) * 2; ++i){
			for(int j = 0; j < 3; ++j){
				*(indices + i * 3 + j) = i + j;
			}
		}
		verts->position = (SDL_FPoint){
			rend_point1.x - width_x,
			rend_point1.y - width_y
		};
		(verts + 1)->position = (SDL_FPoint){
			rend_point1.x + width_x,
			rend_point1.y + width_y
		};
		for(unsigned int i = 1U; i < count - 1U; i += 2U){
			(verts + i * 2)->position = (SDL_FPoint){
				rend_point1.x + x_part * (i + *(rand1 + i - 1U)) + shift_x * *(rand + i - 1U) - (width_x - i * width_step_x),
				rend_point1.y + y_part * (i + *(rand1 + i - 1U)) + shift_y * *(rand + i - 1U) - (width_y - i * width_step_y)
			};
			(verts + i * 2 + 1)->position = (SDL_FPoint){
				rend_point1.x + x_part * (i + *(rand1 + i - 1U)) + shift_x * *(rand + i - 1U) + (width_x - i * width_step_x),
				rend_point1.y + y_part * (i + *(rand1 + i - 1U)) + shift_y * *(rand + i - 1U) + (width_y - i * width_step_y)
			};
		}
		for(unsigned int i = 2U; i < count - 1U; i += 2U){
			(verts + i * 2)->position = (SDL_FPoint){
				rend_point1.x + x_part * (i + *(rand1 + i - 1U)) - shift_x * *(rand + i - 1U) - (width_x - i * width_step_x),
				rend_point1.y + y_part * (i + *(rand1 + i - 1U)) - shift_y * *(rand + i - 1U) - (width_y - i * width_step_y)
			};
			(verts + i * 2 + 1)->position = (SDL_FPoint){
				rend_point1.x + x_part * (i + *(rand1 + i - 1U)) - shift_x * *(rand + i - 1U) + (width_x - i * width_step_x),
				rend_point1.y + y_part * (i + *(rand1 + i - 1U)) - shift_y * *(rand + i - 1U) + (width_y - i * width_step_y)
			};
		}
		const float sh_last = 3.0F;
		const float sh_x_last = sh_last * cosine_angle;
		const float sh_y_last = sh_last * sine_angle;
		(verts + (count - 1) * 2)->position = (SDL_FPoint){
			rend_point.x - sh_x_last,
			rend_point.y - sh_y_last
		};
		(verts + (count - 1) * 2 + 1)->position = (SDL_FPoint){
			rend_point.x + sh_x_last,
			rend_point.y + sh_y_last
		};
		SDL_RenderGeometry(rend_data->renderer, texture(tx_bolt), verts, num_verts, indices, num_indices);
	}
	SDL_srand(0ULL);
}

static void RenderFlashEffect(Visual_effect* const ve, Game_data* const gd){
	Render_data* const rend_data = gd->rend_data_ptr;
	SDL_FPoint rend_point;
	Segment* seg = GetSegmentUnsafe(&gd->world, ve->position.x, ve->position.y);
	if(!(seg && (seg->flags & segment_in_sight) && GetExtendedRenderPointFromTrue(rend_data, ve->position.x, ve->position.y, half(ve->data0.size), human(gd), &rend_point))){
		return;
	}
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_lighting));
	SDL_SetTextureAlphaModFloat(texture(tx_pixel), ve->ticks_left / (float)FLASH_TICKS);
	SDL_SetTextureColorMod(texture(tx_pixel), ve->data2.r, ve->data2.g, ve->data2.b);
	SDL_RenderTexture(rend_data->renderer, texture(tx_pixel), NULL, &(SDL_FRect){
		rend_point.x - half(FLASH_SIZE),
		rend_point.y - half(FLASH_SIZE),
		FLASH_SIZE,
		FLASH_SIZE
	});
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_view));
}

static void RenderVisualEffects(Render_data* const rend_data, Game_data* const gd){
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_lighting));
	SDL_SetRenderDrawColor(rend_data->renderer, BLACK_RGB, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(rend_data->renderer);
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_view));
	void (*const render[])(Visual_effect* const, Game_data* const) = VISUAL_EFFECT_FUNCS;
	for(unsigned int i = 0U; i < rend_data->visual_effects.num; ++i){
		Visual_effect* ve = rend_data->visual_effects.array + i;
		(*(render + ve->type))(ve, gd);
		if(--ve->ticks_left < 1U){
			RemoveVisalEffect(&rend_data->visual_effects, i--);
		}
	}
}

extern inline void AddDamageVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &DAMAGE_VIS_EFFECT(*position));
}

extern inline void AddBonusVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &BONUS_VIS_EFFECT(*position));
}

extern inline void AddCurseVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &CURSE_VIS_EFFECT(*position));
}

extern inline void AddDeadVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &DEAD_VIS_EFFECT(*position));
}

extern inline void AddPortalVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &PORTAL_VIS_EFFECT(*position));
}

extern inline void AddProjectileVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &PROJE_VIS_EFFECT(*position));
}

extern inline void AddBigBurnVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &BURN_VIS_EF(*position, SDL_rand(BIG_BURN_SIZE / 2) + BIG_BURN_SIZE / 2 + 1U));
}

extern inline void AddSmallBurnVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &BURN_VIS_EF(*position, SDL_rand(SMALL_BURN_SIZE) + 1U));
}

extern inline void AddBoomVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position){
	AddVisalEffect(ves, &BURN_VIS_EF(*position, SDL_rand(BOOM_SIZE / 2) + BOOM_SIZE * 15U / 2U + 1U));
}

extern inline void AddBigBurnVisualEffectTimer(Visual_effects* const ves, const SDL_FPoint* const position, const unsigned int delay){
	AddVisalEffect(ves, &BURN_EF_TIM(*position, SDL_rand(BIG_BURN_SIZE / 2) + BIG_BURN_SIZE / 2 + 1U, delay));
}

extern inline void AddSmallBurnVisualEffectTimer(Visual_effects* const ves, const SDL_FPoint* const position, const unsigned int delay){
	AddVisalEffect(ves, &BURN_EF_TIM(*position, SDL_rand(SMALL_BURN_SIZE) + 1U, delay));
}

extern inline void AddBoomVisualEffectTimer(Visual_effects* const ves, const SDL_FPoint* const position, const unsigned int delay){
	AddVisalEffect(ves, &BURN_EF_TIM(*position, SDL_rand(BOOM_SIZE / 2) + BOOM_SIZE * 15U / 2U + 1U, delay));
}

extern inline void AddBoltVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position, const position16 start_position){
	Visual_effect ve = {
		*position,
		BOLT_TICKS
	};
	ve.data1.start_position = start_position;
	float angle = GetAngle(&(SDL_FPoint){start_position.x, start_position.y}, position);
	if(angle < 0.0F){
		angle += FULL_ANGLE;
	}
	ve.data1.angle = angle / FULL_ANGLE * UINT8_MAX;
	ve.type = visual_effect_bolt;
	AddVisalEffect(ves, &ve);
}

extern inline void AddSpellVisualEffect(Visual_effects* const ves, const SDL_FPoint* const position, const Uint8 r, const Uint8 g, const Uint8 b){
	Visual_effect ve = {
		*position,
		FLASH_TICKS
	};
	ve.data2.r = r;
	ve.data2.g = g;
	ve.data2.b = b;
	ve.type = visual_effect_t3;
	AddVisalEffect(ves, &ve);
}

static void DrawBeings(Render_data* const rend_data){
	SDL_Surface* surface = NULL;
	SDL_Surface* surface1 = NULL;
	char* bmp_path = NULL;
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), PC_TX0_FILE_NAME);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	SDL_free(bmp_path);
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), PC_TX1_FILE_NAME);
	surface1 = SDL_LoadBMP(bmp_path);
	if(!surface1){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	DrawBeing(rend_data, &texture(tx_pc), surface, surface1, PC_RGB_0, PC_RGB_1);
	SDL_DestroySurface(surface);
	SDL_DestroySurface(surface1);
	SDL_free(bmp_path);
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), BEING_TEXTURE0_FILE_NAME);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	SDL_free(bmp_path);
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), BEING_TEXTURE1_FILE_NAME);
	surface1 = SDL_LoadBMP(bmp_path);
	if(!surface1){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	DrawBeing(rend_data, &texture(tx_being_weak), surface, surface1, BEING0_RGB_0, BEING0_RGB_1);
	DrawBeing(rend_data, &texture(tx_being_ordinary), surface, surface1, BEING1_RGB_0, BEING1_RGB_1);
	DrawBeing(rend_data, &texture(tx_being_ranger), surface, surface1, BEING2_RGB_0, BEING2_RGB_1);
	DrawBeing(rend_data, &texture(tx_being_elite), surface, surface1, BEING3_RGB_0, BEING3_RGB_1);
	DrawBeing(rend_data, &texture(tx_being_elite_ranger), surface, surface1, BEING4_RGB_0, BEING4_RGB_1);
	DrawBeing(rend_data, &texture(tx_being_commander), surface, surface1, BEING5_RGB_0, BEING5_RGB_1);
	DrawBeing(rend_data, &texture(tx_being_warlock), surface, surface1, BEING6_RGB_0, BEING6_RGB_1);
	DrawBeing(rend_data, &texture(tx_being_ally_ordinary), surface, surface1, ALLY0_RGB_0, ALLY0_RGB_1);
	SDL_DestroySurface(surface);
	SDL_DestroySurface(surface1);
	SDL_free(bmp_path);
}

static void DrawColouredThings(Render_data* const rend_data){
	SDL_Surface* surface = NULL;
	char* bmp_path = NULL;
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), PROJECTILE_TX_FILE_NAME);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	DrawColouredThing(rend_data, &texture(tx_projectile), surface, PROJECTILE0_RGBA);
	DrawColouredThing(rend_data, &texture(tx_h_projectile), surface, PROJECTILE1_RGBA);
	SDL_DestroySurface(surface);
	SDL_free(bmp_path);
	surface = SDL_CreateSurface(3, 3, SDL_PIXELFORMAT_RGBA8888);
	SDL_WriteSurfacePixel(surface, 1, 1, 255U, 255U, 255U, 255U);
	texture(tx_pixel) = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	texture(tx_pixel_blend) = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!(texture(tx_pixel) && texture(tx_pixel_blend))){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	SDL_DestroySurface(surface);
}

static void DrawBeing(Render_data* const rend_data, SDL_Texture** being_tx, SDL_Surface* const sf_0, SDL_Surface* const sf_1, const Uint8* const RGB_0, const Uint8* const RGB_1){
	SDL_SetSurfaceColorMod(sf_0, *RGB_0, *(RGB_0 + 1), *(RGB_0 + 2));
	SDL_SetSurfaceColorMod(sf_1, *RGB_1, *(RGB_1 + 1), *(RGB_1 + 2));
	SDL_Surface* base_surface = SDL_CreateSurface(sf_0->w, sf_0->h, sf_0->format);
	SDL_BlitSurface(sf_0, NULL, base_surface, NULL);
	SDL_BlitSurface(sf_1, NULL, base_surface, NULL);
	*being_tx = SDL_CreateTextureFromSurface(rend_data->renderer, base_surface);
	SDL_DestroySurface(base_surface);
}

static void DrawColouredThing(Render_data* const rend_data, SDL_Texture** target, SDL_Surface* const sf, const Uint8* const RGBA){
	SDL_SetSurfaceColorMod(sf, *RGBA, *(RGBA + 1), *(RGBA + 2));
	SDL_SetSurfaceAlphaMod(sf, *(RGBA + 3));
	SDL_Surface* base_surface = SDL_CreateSurface(sf->w, sf->h, sf->format);
	SDL_BlitSurface(sf, NULL, base_surface, NULL);
	*target = SDL_CreateTextureFromSurface(rend_data->renderer, base_surface);
	SDL_DestroySurface(base_surface);
}

void ResetRenderData(Render_data* const rend_data){
	int w, h;
	SDL_GetWindowSizeInPixels(rend_data->window, &w, &h);
	rend_data->window_w = w;
	rend_data->window_h = h;
	rend_data->viewfinder = VIEWFINDER_SIZE;
	rend_data->viewfinder_rect = (SDL_Rect)VIEWFINDER_RECT;
	rend_data->visible_rect = (SDL_FRect)VISIBLE_RECT;
	SDL_WarpMouseInWindow(rend_data->window, half(rend_data->window_w), half(rend_data->window_h));
	rend_data->mouse_y = half(rend_data->window_h);
	SDL_SetWindowMouseRect(rend_data->window, &(SDL_Rect)MOUSE_RECT);
	DrawBackground(rend_data);
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_lighting));
	SDL_SetRenderScale(rend_data->renderer, LIGHTING_TX_SIZE / rend_data->viewfinder, LIGHTING_TX_SIZE / rend_data->viewfinder);
	SetViewTexture(rend_data);
	SDL_SetRenderTarget(rend_data->renderer, NULL);
}

static void RenderHumanPlayerBlade(Render_data* const rend_data, Blade* const blade){
	const SDL_FRect rect_blade = {
		(VIEWFINDER_CENTER - half(WEAPON_SIZE)) + blade->placement.position.x,
		(VIEWFINDER_CENTER - BLADE_LEN + PLAYER_REND_Y_SHIFT) - blade->placement.position.y,
		WEAPON_SIZE,
		WEAPON_SIZE
	};
	SDL_RenderTextureRotated(rend_data->renderer, texture(tx_pc_blade), NULL, &rect_blade, (double)RadToDeg(blade->placement.direction), &(SDL_FPoint)WEAPON_ROTATION_POINT, SDL_FLIP_NONE);
}

static void RenderProjectiles(Render_data* const rend_data, Game_data* const gd){
	const unsigned int projectile_textures[] = PROJECTILE_TEXTURES;
	for(Projectile* pr = gd->projectiles.array; pr != (gd->projectiles.array + gd->projectiles.num); ++pr){
		SDL_FPoint point;
		if(GetRenderPointFromTrue(rend_data, pr->position.x, pr->position.y, human(gd), &point, &gd->world)){
			const SDL_FRect rect = { 
				point.x - half(BULLET_SIZE),
				point.y - half(BULLET_SIZE),
				BULLET_SIZE,
				BULLET_SIZE
			};
			SDL_RenderTexture(rend_data->renderer, texture(*(projectile_textures + pr->type_id)), NULL, &rect);
		}
	}
}

static inline void RenderBeing(Render_data* const rend_data, Game_data* const gd, Being* const bg, const unsigned int tex_body, const unsigned int tex_weapon){
	SDL_FPoint point;
	if(GetRenderPointFromTrueWithKnownSegmentVisibility(rend_data, bg->position.x, bg->position.y, human(gd), &point)){
		const SDL_FRect rect = {
			point.x - half(BeingSize(bg)),
			point.y - half(BeingSize(bg)),
			BeingSize(bg),
			BeingSize(bg)
		};
		if(bg->status == being_fly && bg->rend_fly_help_data.ticks != 0){
			bg->direction = bg->rend_fly_help_data.start_angle + FULL_ANGLE * (1.0F - (float)bg->status_ticks_left / (float)bg->rend_fly_help_data.ticks);
		}else if(bg->status <= being_strike){
			bg->direction = arctan2(bg->position.y - bg->target.player->position.y, bg->position.x - bg->target.player->position.x) - SDL_PI_F * 0.5F;
		}else if(bg->status <= being_strike_being){
			bg->direction = arctan2(bg->position.y - bg->target.being->position.y, bg->position.x - bg->target.being->position.x) - SDL_PI_F * 0.5F;
		}else if(bg->status == being_walk && gd->enemy_morale > 0){
			bg->direction = arctan2(bg->position.y - human(gd)->position.y, bg->position.x - human(gd)->position.x) - SDL_PI_F * 0.5F;
		}else if(bg->status == being_search || (bg->status == being_walk && gd->enemy_morale <= 0)){
			bg->direction = arctan2(-bg->special_move_shift.y, -bg->special_move_shift.x) - SDL_PI_F * 0.5F;
		}
		const float being_direction = bg->direction - human(gd)->direction;
		const float sine = SineSafe(being_direction);
		const float cosine = CosiSafe(being_direction);
		const Placement weapon = GetBeingWeaponPlacement(bg);
		const SDL_FRect rect_blade = {
			point.x + (weapon.position.x * cosine + weapon.position.y * sine) - half(WEAPON_SIZE),
			point.y + (weapon.position.x * sine - weapon.position.y * cosine) - BLADE_LEN,
			WEAPON_SIZE,
			WEAPON_SIZE
		};
		SDL_RenderTextureRotated(rend_data->renderer, texture(tex_weapon), NULL, &rect_blade, (double)RadToDeg(weapon.direction + being_direction), &(SDL_FPoint)WEAPON_ROTATION_POINT, SDL_FLIP_NONE);
		SDL_RenderTextureRotated(rend_data->renderer, texture(tex_body), NULL, &rect, (double)RadToDeg(being_direction), NULL, SDL_FLIP_NONE);
		const int max_hp = BeingHP(bg);
		if(bg->hit_points < max_hp){
			SDL_SetTextureAlphaModFloat(texture(tx_damage_test), 1.0F - bg->hit_points / (float)max_hp);
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_damage_test), NULL, &rect, (double)(being_direction), NULL, SDL_FLIP_NONE);
		}
		if(bg->status == being_stunned){
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_stun), NULL, &rect, (double)(rend_data->counter * TEXTURE_ROTA_SPEED), NULL, SDL_FLIP_NONE);
		}
	}
}

static void RenderBeings(Render_data* const rend_data, Game_data* const gd, Segment** const segs, const unsigned int segs_num){
	const unsigned int beings_textures[] = BEINGS_TEXTURES;
	const unsigned int beings_weapon_textures[] = BEINGS_WEAPON_TEXTURES;
    for(unsigned int k = 0U; k < segs_num; ++k){
		const Segment* const seg = *(segs + k);
		for(unsigned int i = 0U; i < seg->ally_beings.num; ++i){
			Being* const bg = (gd->beings.array + *(seg->ally_beings.beings_ind + i));
			RenderBeing(rend_data, gd, bg, *(beings_textures + bg->type_id), *(beings_weapon_textures + bg->type_id));
		}
		for(unsigned int i = 0U; i < seg->beings.num; ++i){
			Being* const bg = (gd->beings.array + *(seg->beings.beings_ind + i));
			RenderBeing(rend_data, gd, bg, *(beings_textures + bg->type_id), *(beings_weapon_textures + bg->type_id));
		}
	}
}

static void RenderMap(Render_data* const rend_data, const Game_data* const gd, Player* const pc){
	const SDL_FRect pc_rect = {
		pc->position.x * (rend_data->viewfinder / WORLD_W) - half(MINIMAP_PC_SIZE),
		pc->position.y * (rend_data->viewfinder / WORLD_H) - half(MINIMAP_PC_SIZE),
		MINIMAP_PC_SIZE,
		MINIMAP_PC_SIZE
	};
	SDL_RenderTexture(rend_data->renderer, texture(tx_map), NULL, NULL);
	SDL_SetRenderDrawColor(rend_data->renderer, 255, 242, 0, 255);
	for(unsigned int i = 0U; i < gd->needed_keys; ++i){
		if(*(gd->keys_status + i) != key_location_unknown){
			const SDL_FRect rect = {
				(gd->world.key_locations + i)->x * (rend_data->viewfinder / 255.0F) - half(MAP_KEY_SIZE),
				(gd->world.key_locations + i)->y * (rend_data->viewfinder / 255.0F) - half(MAP_KEY_SIZE),
				MAP_KEY_SIZE,
				MAP_KEY_SIZE
			};
			SDL_RenderFillRect(rend_data->renderer, &rect);
			if(*(gd->keys_status + i) == key_owned){
				SDL_RenderTexture(rend_data->renderer, texture(tx_owned), NULL, &rect);
			}
		}
	}
	SDL_SetRenderDrawColor(rend_data->renderer, 192, 0, 0, 255);
	SDL_RenderFillRect(rend_data->renderer, &pc_rect);
}

static inline bool GetRenderPointFromTrue(Render_data* const rend_data, const float true_point_x, const float true_point_y, const Player* const human_player, SDL_FPoint* const rend_point, World* const w){
	const float dx = true_point_x - human_player->position.x;
	if(SDL_fabsf(dx) > rend_data->viewfinder) return false;
	const float dy = true_point_y - human_player->position.y;
	if(SDL_fabsf(dy) > rend_data->viewfinder) return false;
	if(!(GetSegmentUnsafe(w, true_point_x, true_point_y)->flags & segment_in_sight)) return false;
	*rend_point = (SDL_FPoint){
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	if(SDL_PointInRectFloat(rend_point, &rend_data->visible_rect)) return true;
	return false;
}

static inline bool GetRenderPointFromTrueShopEdition(Render_data* const rend_data, const float true_point_x, const float true_point_y, const float y_shift, const Player* const human_player, SDL_FPoint* const rend_point, World* const w){
	const float dx = true_point_x - human_player->position.x;
	if(SDL_fabsf(dx) > rend_data->viewfinder) return false;
	float dy = true_point_y - human_player->position.y;
	if(SDL_fabsf(dy) > rend_data->viewfinder) return false;
	if(!(GetSegmentUnsafe(w, true_point_x, true_point_y)->flags & segment_known)) return false;
	const SDL_FPoint point = {
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	dy = true_point_y + y_shift - human_player->position.y;
	*rend_point = (SDL_FPoint){
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	if(SDL_PointInRectFloat(&point, &rend_data->visible_rect)) return true;
	return false;
}

static inline bool GetRenderPointFromTrueWithKnownSegmentVisibility(Render_data* const rend_data, const float true_point_x, const float true_point_y, const Player* const human_player, SDL_FPoint* const rend_point){
	const float dx = true_point_x - human_player->position.x;
	const float dy = true_point_y - human_player->position.y;
	*rend_point = (SDL_FPoint){
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	if(SDL_PointInRectFloat(rend_point, &rend_data->visible_rect)) return true;
	return false;
}

static void RenderPlayerStatus(Render_data* const rend_data, Player* const pc, const Game_data* const gd){
	const float BAR_H = BAR_H_CALC;
	const SDL_FRect rect_armo = {
		BAR_X,
		AREAA_Y + BAR_H + FRAME_W * 2.0F,
		((1.0F - pc->armour.multipl) / (1.0F - pc->max_armour.multipl)) * BAR_W,
		SMALL_BAR_H
	};
	const SDL_FRect rect_marmo = {
		BAR_X,
		rect_armo.y + SMALL_BAR_H,
		((1.0F - pc->armour.magic_multipl) / (1.0F - pc->max_armour.magic_multipl)) * BAR_W,
		SMALL_BAR_H
	};
	const SDL_FRect rectHP = {
		BAR_X,
		rect_armo.y + BAR_H * 2.0F + FRAME_W * 5.0F,
		(float)pc->hit_points / (float)pc->max_hp * BAR_W,
		BAR_H
	};
	const SDL_FPoint MP_position = {
		RIGHT_AREA_X + FRAME_W * 2.0F,
		AREA_MP_Y + BAR_H + FRAME_W * 2.0F
	};
	const SDL_FRect rectFP = {
		BAR_X,
		rectHP.y + BAR_H * 2.0F + FRAME_W * 5.0F,
		(float)pc->fatigue_points / (float)pc->max_fatigue * BAR_W,
		BAR_H
	};
	SDL_SetTextureColorMod(texture(tx_bar), 128U, 128U, 128U);
	SDL_RenderTexture(rend_data->renderer, texture(tx_bar), NULL, &rect_armo);
	SDL_SetTextureColorMod(texture(tx_bar), 128U, 128U, 255U);
	SDL_RenderTexture(rend_data->renderer, texture(tx_bar), NULL, &rect_marmo);
	SDL_SetTextureColorMod(texture(tx_bar), 255U, 0U, 0U);
	SDL_RenderTexture(rend_data->renderer, texture(tx_bar), NULL, &rectHP);
	SDL_SetTextureColorMod(texture(tx_bar), 0U, 255U, 0U);
	SDL_RenderTexture(rend_data->renderer, texture(tx_bar), NULL, &rectFP);
	SDL_SetTextureColorMod(texture(tx_chars), MP_TEXT_RGB);
	RenderIntCentered(rend_data, MP_position.x, MP_position.y, MP_BAR_H, pc->magic_points, BAR_W);
	SDL_SetTextureColorMod(texture(tx_chars), MP_COST_TEXT_RGB);
	RenderIntCentered(rend_data, MP_position.x, MP_position.y + MP_BAR_H, MP_BAR_H, ScrollCost(pc->selected_scroll), BAR_W);
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	RenderIntCentered(rend_data, KEYS_NUM_X, KEYS_NUM_Y, BAR_H, gd->keys, BAR_W - BAR_H - FRAME_W * 2.0F);
	RenderIntCentered(rend_data, KEYS_NUM_X, COINS_NUM_Y, BAR_H, pc->coins, BAR_W - BAR_H - FRAME_W * 2.0F);
	if(pc->blade.charge != PC_BLADE_CHARGE_BASE){
		const SDL_FRect rect_cha = {
			half(rend_data->window_w) - half(PLAYER_SIZE),
			PLAYER_REND_Y + PLAYER_SIZE,
			PLAYER_SIZE,
			10.0F
		};
		const SDL_FRect rect_chb = {
			half(rend_data->window_w) - (half(PLAYER_SIZE) - 1.0F),
			PLAYER_REND_Y + PLAYER_SIZE + 1.0F,
			(1.0F - pc->blade.charge) * (rect_cha.w - 2.0F),
			8.0F
		};
		const unsigned int red = (unsigned int)((1.0F - pc->blade.charge) * 255.0F);
		const unsigned int green = (unsigned int)(pc->blade.charge * 255.0F);
		SDL_SetRenderDrawColor(rend_data->renderer, red / 4, green / 4, 0U, 255U);
		SDL_RenderRect(rend_data->renderer, &rect_cha);
		SDL_SetTextureColorMod(texture(tx_bar), red, green, 0U);
		SDL_RenderTexture(rend_data->renderer, texture(tx_bar), NULL, &rect_chb);
	}
}

void RenderMainMenu(Render_data* const rend_data, const unsigned int menu_position){
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 0U, 0U, 255U);
	SDL_RenderClear(rend_data->renderer);
	SDL_SetRenderViewport(rend_data->renderer, &rend_data->viewfinder_rect);
	const Uint8* texts[] = MAIN_MENU_TEXTS;
	const unsigned int texts_chars_num[] = MAIN_MENU_TEXTS_SIZES;
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float text_height = icon_size * 0.5F;
	SDL_FPoint dst_point_text = {
		icon_size,
		icon_size * 1.25F + FRAME_W * 3.0F
	};
	const float shift = icon_size + FRAME_W;
	const SDL_FRect menu_ptr_rect = {
		FRAME_W,
		icon_size + FRAME_W * 3.0F + shift * menu_position,
		rend_data->viewfinder_rect.w - FRAME_W * 2.0F,
		icon_size
	};
	for(unsigned int i = 0U; i < OPTIONS_NUM; ++i){
		RenderTextCentered(rend_data, 0.0F, dst_point_text.y, text_height, *(texts + i), *(texts_chars_num + i), rend_data->viewfinder);
		dst_point_text.y += shift;
	}
	SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &menu_ptr_rect);
	SDL_SetRenderViewport(rend_data->renderer, NULL);
	SDL_RenderPresent(rend_data->renderer);
}

static inline void RenderViewfinderBackground(Render_data* const rend_data){
	SDL_RenderTexture(rend_data->renderer, texture(tx_background), &(SDL_FRect){
		(float)rend_data->viewfinder_rect.x,
		(float)rend_data->viewfinder_rect.y,
		(float)rend_data->viewfinder_rect.w,
		(float)rend_data->viewfinder_rect.h
	}, NULL);
}

static inline void SetOtherPlayersRenderData(Render_data* const rend_data, Game_data* const gd, Player** players_to_rend, SDL_FPoint* players_rend_points, unsigned int* indx){
	Player* const pc = human(gd);
	for(unsigned int i = 0U; i < gd->champions.num; ++i){
		if(i == gd->human_indx){
			continue;
		}
		SDL_FPoint point;
		if(GetRenderPointFromTrue(rend_data, (gd->champions.array + i)->position.x, (gd->champions.array + i)->position.y, pc, &point, &gd->world)){
			*(players_to_rend + *indx) = gd->champions.array + i;
			*(players_rend_points + (*indx)++) = point;
		}
	}
}

void RenderGame(Render_data* const rend_data, Game_data* const gd, const int event_code){
	Player* const pc = human(gd);
	++rend_data->counter;
	SetSineCosine(rend_data, pc);
	RenderBackground(rend_data);
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_view));
	RenderViewfinderBackground(rend_data);
	Segment* beings_segs[MAX_UNSEEN_SEG];
	unsigned int beings_segs_num = 0U;
	RenderTerrain(rend_data, gd, beings_segs, &beings_segs_num);
	RenderStaticThings(rend_data, gd);
	if(!(pc->flags & range_mode)){
		RenderHumanPlayerBlade(rend_data, &pc->blade);
	}else if((pc->flags & attack)){
		RenderHumanPlayerScrollUnrolled(rend_data, pc->selected_scroll);
	}else{
		RenderHumanPlayerScroll(rend_data);
	}
	Player* players_to_rend[gd->champions.num - 1U];
	SDL_FPoint players_rend_points[gd->champions.num - 1U];
	unsigned int indx = 0U;
	SetOtherPlayersRenderData(rend_data, gd, players_to_rend, players_rend_points, &indx);
	RenderPlayersBladesAndScrolls(rend_data, pc->direction, players_to_rend, players_rend_points, indx);
	RenderBeings(rend_data, gd, beings_segs, beings_segs_num);
	RenderProjectiles(rend_data, gd);
	RenderHumanPlayer(rend_data);
	if(human(gd)->flags & stunned){
		SDL_RenderTextureRotated(rend_data->renderer, texture(tx_stun), NULL, &(SDL_FRect)PC_RECT, (double)(rend_data->counter * TEXTURE_ROTA_SPEED), NULL, SDL_FLIP_NONE);
	}
	RenderPlayers(rend_data, pc->direction, players_to_rend, players_rend_points, indx);
	if(pc->flags & block){
		RenderHumanPlayerBarrier(rend_data, pc);
	}
	RenderBarriers(rend_data, pc->direction, players_to_rend, players_rend_points, indx);
	RenderDoors(rend_data, gd);
	RenderVisualEffects(rend_data, gd);

	SDL_RenderTexture(rend_data->renderer, texture(tx_lighting), NULL, NULL);
	SDL_RenderTexture(rend_data->renderer, texture(tx_viewfinder), NULL, NULL);
	
	SDL_SetRenderTarget(rend_data->renderer, NULL);
	SDL_SetRenderViewport(rend_data->renderer, &rend_data->viewfinder_rect);
	SDL_RenderTexture(rend_data->renderer, texture(tx_view), NULL, NULL);

	if(event_code == event_menu){
		RenderMenu(rend_data, pc);
	}else if(event_code == event_manage_scrolls){
		RenderScrollsManagement(rend_data, pc);
	}else{
		RenderGunSight(rend_data);
		if(pc->flags & map_look){
			RenderMap(rend_data, gd, pc);
		}
	}
	if(gd->flags & gamef_horde_attack){
		RenderText(rend_data, HORDE_ALERT_X, HORDE_ALERT_Y, HORDE_ALERT_SIZE, (Uint8[])HORDE_ALERT);
	}
	SDL_SetRenderViewport(rend_data->renderer, NULL);
	
	RenderDirectionArrow(rend_data, RadToDeg(pc->direction));
	RenderPlayerStatus(rend_data, pc, gd);
	RenderQuickScrolls(rend_data, pc);
}

static inline void SetSineCosine(Render_data* const rend_data, Player* const pc){
	rend_data->sin_player_direction = SineUnsafe(pc->direction);
	rend_data->cos_player_direction = CosiUnsafe(pc->direction);
}

void ClearRenderData(Render_data* const rend_data){
	for(int i = 0; i < txs_num; ++i){
		SDL_DestroyTexture(texture(i));
	}
	SDL_DestroyRenderer(rend_data->renderer);
	SDL_DestroyWindow(rend_data->window);
	SDL_Quit();
}

static inline bool NeighbourSegKnown(World* const wld, Segment* const seg){
	Segment* seg1 = GetSegmentByIndxUnsafe(wld, seg->indx.x, seg->indx.y - 1U);
	if(seg1 && (seg1->flags & segment_known)){
		return true;
	}
	seg1 = GetSegmentByIndxUnsafe(wld, seg->indx.x + 1U, seg->indx.y);
	if(seg1 && (seg1->flags & segment_known)){
		return true;
	}
	seg1 = GetSegmentByIndxUnsafe(wld, seg->indx.x, seg->indx.y + 1U);
	if(seg1 && (seg1->flags & segment_known)){
		return true;
	}
	seg1 = GetSegmentByIndxUnsafe(wld, seg->indx.x - 1U, seg->indx.y);
	if(seg1 && (seg1->flags & segment_known)){
		return true;
	}
	return false;
}

static void RenderTerrain(Render_data* const rend_data, Game_data* const gd, Segment** const beings_segs, unsigned int* const beings_segs_num){
	const SDL_FPoint corner_first = {human(gd)->position.x - (rend_data->viewfinder + SEGMENT_SIZE), human(gd)->position.y - (rend_data->viewfinder + SEGMENT_SIZE)};
	const SDL_FPoint corner_last = {human(gd)->position.x + (rend_data->viewfinder + SEGMENT_SIZE), human(gd)->position.y + (rend_data->viewfinder + SEGMENT_SIZE)};
	SDL_FPoint point = corner_first;
	float shift_y = SDL_fmodf(point.y, SEGMENT_SIZE) + half(SEGMENT_SIZE);
	point.x -= SDL_fmodf(point.x, SEGMENT_SIZE) + half(SEGMENT_SIZE);
	point.y -= shift_y;
	SDL_FPoint unseen_seg_points[MAX_UNSEEN_SEG];
	unsigned int unseen_seg_num = 0U;
	unsigned int never_seen_seg_num = 0U;
	while(point.x < corner_last.x){
		while(point.y < corner_last.y){
			SDL_FPoint rend_point;
			if(GetExtendedRenderPointFromTrue(rend_data, point.x, point.y, SEGMENT_SIZE * SQRT2DIV2, human(gd), &rend_point)){
				if(point.x > 0.0F && point.y > 0.0F && point.x < WORLD_SIZE && point.y < WORLD_SIZE){
					Segment* seg = GetSegmentUnsafe(&gd->world, point.x, point.y);
					if(seg){
						if(SegmentInSight(&human(gd)->position, &point, seg, &gd->world)){
							seg->flags |= segment_in_sight | segment_known;
							if(seg->beings.num > 0U || seg->ally_beings.num > 0U){
								*(beings_segs + (*beings_segs_num)++) = seg;
							}
							SDL_RenderTextureRotated(rend_data->renderer, texture(tx_terrain), NULL, &(SDL_FRect){
								rend_point.x - half(SEGMENT_TX_SIZE),
								rend_point.y - half(SEGMENT_TX_SIZE),
								SEGMENT_TX_SIZE,
								SEGMENT_TX_SIZE
							}, -RadToDeg(human(gd)->direction), NULL, SDL_FLIP_NONE);
						}else{
							if(seg->flags & segment_known){
								*(unseen_seg_points + unseen_seg_num++) = (SDL_FPoint){rend_point.x - half(SEGMENT_TX_SIZE), rend_point.y - half(SEGMENT_TX_SIZE)};
							}else if(NeighbourSegKnown(&gd->world, seg)){
								*(unseen_seg_points + MAX_UNSEEN_SEG - 1U - never_seen_seg_num++) = (SDL_FPoint){rend_point.x - half(SEGMENT_TX_SIZE), rend_point.y - half(SEGMENT_TX_SIZE)};
							}
							seg->flags &= ~(segment_in_sight);
						}
					}
				}
			}
			point.y += SEGMENT_SIZE;
		}
		point.x += SEGMENT_SIZE;
		point.y = corner_first.y - shift_y;
	}
	SDL_SetTextureColorMod(texture(tx_terrain), 127, 111, 111);
	for(unsigned int i = 0U; i < unseen_seg_num; ++i){
		SDL_RenderTextureRotated(rend_data->renderer, texture(tx_terrain), NULL, &(SDL_FRect){
			(unseen_seg_points + i)->x,
			(unseen_seg_points + i)->y,
			SEGMENT_TX_SIZE,
			SEGMENT_TX_SIZE
		}, -RadToDeg(human(gd)->direction), NULL, SDL_FLIP_NONE);
	}
	SDL_SetTextureColorMod(texture(tx_terrain), 63, 55, 55);
	for(unsigned int i = 0U; i < never_seen_seg_num; ++i){
		SDL_RenderTextureRotated(rend_data->renderer, texture(tx_terrain), NULL, &(SDL_FRect){
			(unseen_seg_points + MAX_UNSEEN_SEG - 1U - i)->x,
			(unseen_seg_points + MAX_UNSEEN_SEG - 1U - i)->y,
			SEGMENT_TX_SIZE,
			SEGMENT_TX_SIZE
		}, -RadToDeg(human(gd)->direction), NULL, SDL_FLIP_NONE);
	}
	SDL_SetTextureColorMod(texture(tx_terrain), 255, 255, 255);
}

static inline bool GetExtendedRenderPointFromTrue(Render_data* const rend_data, const float true_point_x, const float true_point_y, const float extension, const Player* const pc, SDL_FPoint* const rend_point){
	const SDL_FRect extended_view = {
		rend_data->visible_rect.x - extension,
		rend_data->visible_rect.y - extension,
		rend_data->visible_rect.w + extension * 2.0F,
		rend_data->visible_rect.h + extension * 2.0F
	};
	const float dx = true_point_x - pc->position.x;
	if(SDL_fabsf(dx) > rend_data->viewfinder + extension) return false;
	const float dy = true_point_y - pc->position.y;
	if(SDL_fabsf(dy) > rend_data->viewfinder + extension) return false;
	*rend_point = (SDL_FPoint){
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	if(SDL_PointInRectFloat(rend_point, &extended_view)) return true;
	return false;
}

static void RenderGunSight(Render_data* const rend_data){
	const float spread = (PLAYER_REND_Y - rend_data->mouse_y) / (float)rend_data->viewfinder * GUN_SIGHT_SPREAD_RANGE + GUN_SIGHT_SPREAD_MIN;
	SDL_FRect rect = {
		VIEWFINDER_CENTER - half(GUN_SIGHT_SIZE),
		rend_data->mouse_y - half(GUN_SIGHT_SIZE) - spread,
		(float)GUN_SIGHT_SIZE,
		(float)GUN_SIGHT_SIZE
	};
	SDL_RenderTexture(rend_data->renderer, texture(tx_gunsightpart), NULL, &rect);
	rect.y += spread * 2.0F;
	SDL_RenderTexture(rend_data->renderer, texture(tx_gunsightpart), NULL, &rect);
	rect.x -= spread;
	rect.y = rend_data->mouse_y - half(GUN_SIGHT_SIZE);
	SDL_RenderTextureRotated(rend_data->renderer, texture(tx_gunsightpart), NULL, &rect, 90.0, NULL, SDL_FLIP_NONE);
	rect.x += spread * 2.0F;
	SDL_RenderTextureRotated(rend_data->renderer, texture(tx_gunsightpart), NULL, &rect, 90.0, NULL, SDL_FLIP_NONE);
}

static void RenderDoors(Render_data* const rend_data, Game_data* const gd){
	RenderStaticThing(rend_data, gd->world.door.x, gd->world.door.y, human(gd), DOOR_SIZE, tx_door, &gd->world);
	for(unsigned int i = 0U; i < SHOPS_NUM; ++i){
		SDL_FPoint point;
		if(GetRenderPointFromTrueShopEdition(rend_data, (gd->world.shops + i)->location.x, (gd->world.shops + i)->location.y, half(SHOP_SIZE) + 1.0F, human(gd), &point, &gd->world)){
			const SDL_FRect rect = {
				point.x - half(SHOP_SIZE),
				point.y - half(SHOP_SIZE),
				SHOP_SIZE,
				SHOP_SIZE
			};
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_door), NULL, &rect, (double)(-RadToDeg(human(gd)->direction)), NULL, SDL_FLIP_NONE);
		}
	}
	if(gd->flags & gamef_horde_attack){
		SDL_SetTextureAlphaModFloat(texture(tx_creation_point), 1.0F);
		for(unsigned int i = 0U; i < HORDE_ATTACK_POINTS; ++i){
			const int ticks_left = (gd->effects + HasEffect(gd->effects, gd->effects_num, game_effect_horde_attack))->ticks_left;
			if(ticks_left > HORDE_ATTACK_START_TICKS / 8 * 7){
				RenderStaticThingRotating(rend_data, (gd->horde_data.creation_points + i)->x, (gd->horde_data.creation_points + i)->y, human(gd), CREATION_POINT_SIZE * ((float)(HORDE_ATTACK_START_TICKS - ticks_left) / (HORDE_ATTACK_START_TICKS / 8)), tx_creation_point, &gd->world, VORTEX_ROTAT_SPEED);
			}else if(ticks_left < HORDE_ATTACK_START_TICKS / 8){
				RenderStaticThingRotating(rend_data, (gd->horde_data.creation_points + i)->x, (gd->horde_data.creation_points + i)->y, human(gd), CREATION_POINT_SIZE * ((float)ticks_left / (HORDE_ATTACK_START_TICKS / 8)), tx_creation_point, &gd->world, VORTEX_ROTAT_SPEED);
			}else{
				RenderStaticThingRotating(rend_data, (gd->horde_data.creation_points + i)->x, (gd->horde_data.creation_points + i)->y, human(gd), CREATION_POINT_SIZE, tx_creation_point, &gd->world, VORTEX_ROTAT_SPEED);
			}
		}
	}
}

static void RenderStaticThings(Render_data* const rend_data, Game_data* const gd){
	RenderStaticThing(rend_data, gd->world.portalA.x, gd->world.portalA.y, human(gd), DOOR_SIZE, tx_portal, &gd->world);
	RenderStaticThing(rend_data, gd->world.portalB.x, gd->world.portalB.y, human(gd), DOOR_SIZE, tx_portal, &gd->world);
	for(unsigned int i = 0U; i < gd->boxes.num; ++i){
		RenderStaticThing(rend_data, (gd->boxes.array + i)->location.x, (gd->boxes.array + i)->location.y, human(gd), BOX_SIZE, tx_box, &gd->world);
	}
}

static inline bool GetRenderPointFromTrueWithUnsighted(Render_data* const rend_data, const float true_point_x, const float true_point_y, const Player* const human_player, SDL_FPoint* const rend_point, World* const wld){
	const float dx = true_point_x - human_player->position.x;
	if(SDL_fabsf(dx) > rend_data->viewfinder) return false;
	const float dy = true_point_y - human_player->position.y;
	if(SDL_fabsf(dy) > rend_data->viewfinder) return false;
	if(!(GetSegmentUnsafe(wld, true_point_x, true_point_y)->flags & segment_known)) return false;
	*rend_point = (SDL_FPoint){
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	if(SDL_PointInRectFloat(rend_point, &rend_data->visible_rect)) return true;
	return false;
}

static void RenderStaticThing(Render_data* const rend_data, const float pos_x, const float pos_y, Player* const pc, const float size, const int tx_num, World* const wld){
	SDL_FPoint point;
	if(GetRenderPointFromTrueWithUnsighted(rend_data, pos_x, pos_y, pc, &point, wld)){
		const SDL_FRect rect = {
			point.x - half(size),
			point.y - half(size),
			size,
			size
		};
		SDL_RenderTextureRotated(rend_data->renderer, texture(tx_num), NULL, &rect, (double)(-RadToDeg(pc->direction)), NULL, SDL_FLIP_NONE);
	}
}

static void RenderStaticThingRotating(Render_data* const rend_data, const float pos_x, const float pos_y, Player* const p, const float size, const int tx_num, World* const w, const float speed){
	SDL_FPoint point;
	if(GetRenderPointFromTrueWithUnsighted(rend_data, pos_x, pos_y, p, &point, w)){
		const SDL_FRect rect = {
			point.x - half(size),
			point.y - half(size),
			size,
			size
		};
		SDL_RenderTextureRotated(rend_data->renderer, texture(tx_num), NULL, &rect, (double)(-RadToDeg(p->direction) + rend_data->counter * speed), NULL, SDL_FLIP_NONE);
	}
}

void DrawMap(Render_data* const rend_data, const World* const wld){
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_map));
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE / 2);
	SDL_RenderClear(rend_data->renderer);
	for(unsigned int c = 0U; c < BIG_SEGMENTS_X; ++c){
		for(unsigned int r = 0U; r < BIG_SEGMENTS_X; ++r){
			if(IsInUncoveredBigSeg(wld->plan, c, r)){
				SDL_SetRenderDrawColor(rend_data->renderer, SDL_MAX_UINT8, SDL_MAX_UINT8, SDL_MAX_UINT8, SDL_ALPHA_OPAQUE);
				SDL_RenderPoint(rend_data->renderer, c, r);
			}else if(IsInNoticedBigSeg(wld->plan, c, r)){
				SDL_SetRenderDrawColor(rend_data->renderer, SDL_MAX_UINT8 / 2, SDL_MAX_UINT8 / 2, SDL_MAX_UINT8 / 2, SDL_ALPHA_OPAQUE);
				SDL_RenderPoint(rend_data->renderer, c, r);
			}
		}
	}
	SDL_SetTextureScaleMode(texture(tx_map), SDL_SCALEMODE_NEAREST);
	SDL_SetRenderTarget(rend_data->renderer, NULL);
}

extern inline void UpdateMap(Render_data* const rend_data, const Uint8 red, const Uint8 green, const Uint8 blue, const unsigned int col, const unsigned int row){
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_map));
	SDL_SetRenderDrawColor(rend_data->renderer, red, green, blue, 255);
	SDL_RenderPoint(rend_data->renderer, col, row);
	SDL_SetRenderTarget(rend_data->renderer, NULL);
}

static inline void RenderHumanPlayer(Render_data* const rend_data){
	SDL_RenderTexture(rend_data->renderer, texture(tx_pc), NULL, &(SDL_FRect)PC_RECT);
}

static inline void RenderHumanPlayerBarrier(Render_data* const rend_data, const Player* const p){
	if(p->block_times.push < 1){
		SDL_RenderTexture(rend_data->renderer, texture(tx_barrier), NULL, &(SDL_FRect)PC_SHIELD_RECT);
	}else{
		const float size = BARRIER_SIZE + (float)p->block_times.push * BARRIER_MAGNIFICATION;
		SDL_RenderTexture(rend_data->renderer, texture(tx_barrier), NULL,
			&(SDL_FRect){
				VIEWFINDER_CENTER - half(size),
				VIEWFINDER_CENTER - half(size) + PLAYER_REND_Y_SHIFT,
				size,
				size
			}
		);
	}
}

static inline void RenderHumanPlayerScroll(Render_data* const rend_data){
	const SDL_FRect src_rect = SRC_SCROLL_RECT;
	SDL_RenderTexture(rend_data->renderer, texture(tx_scroll), &src_rect, &(SDL_FRect)PC_SCROLL_RECT);
}

static inline void RenderHumanPlayerScrollUnrolled(Render_data* const rend_data, unsigned int scroll_num){
	SDL_FRect src_rect = SRC_UNR_SCROLL_RECT;
	SDL_RenderTexture(rend_data->renderer, texture(tx_scroll), &src_rect, &(SDL_FRect)PC_SCROLL_RECT);
	src_rect = GetScrollTextureSrcRect(scroll_num);
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &src_rect, &(SDL_FRect)PC_SCROLL_ICON_RECT);
}

static void	RenderPlayersBladesAndScrolls(Render_data* const rend_data, const float human_player_direction, Player** plys, SDL_FPoint* points, const unsigned int num){
	SDL_FRect rect_blade = {
		0.0F, 0.0F,
		WEAPON_SIZE, WEAPON_SIZE
	};
	SDL_FRect rect_scroll = {
		0.0F, 0.0F,
		SCROLL_SIZE, SCROLL_SIZE
	};
	SDL_FRect rect_scroll_icon = {
		0.0F, 0.0F,
		SCROLL_ICON_SIZE, SCROLL_ICON_SIZE
	};
	for(unsigned int i = 0U; i < num; ++i){
		const float player_direction = (*(plys + i))->direction - human_player_direction;
		const float sine = SineSafe(player_direction);
		const float cosine = CosiSafe(player_direction);
		if(!((*(plys + i))->flags & range_mode)){
			rect_blade.x = (points + i)->x + ((*(plys + i))->blade.placement.position.x * cosine + (*(plys + i))->blade.placement.position.y * sine) - half(WEAPON_SIZE);
			rect_blade.y = (points + i)->y + ((*(plys + i))->blade.placement.position.x * sine - (*(plys + i))->blade.placement.position.y * cosine) - BLADE_LEN;
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_pc_blade), NULL, &rect_blade, (double)RadToDeg(player_direction + (*(plys + i))->blade.placement.direction), &(SDL_FPoint)WEAPON_ROTATION_POINT, SDL_FLIP_NONE);
		}else{
			rect_scroll.x = (points + i)->x + (half(SCROLL_SIZE) * cosine + half(SCROLL_SIZE) * sine) - half(SCROLL_SIZE);
			rect_scroll.y = (points + i)->y + (half(SCROLL_SIZE) * sine - half(SCROLL_SIZE) * cosine) - half(SCROLL_SIZE);
			if((*(plys + i))->flags & attack){
				rect_scroll_icon.x = (points + i)->x + (half(SCROLL_SIZE) * cosine + half(SCROLL_SIZE) * sine) - half(SCROLL_ICON_SIZE);
				rect_scroll_icon.y = (points + i)->y + (half(SCROLL_SIZE) * sine - half(SCROLL_SIZE) * cosine) - half(SCROLL_ICON_SIZE);
				SDL_FRect src_rect = SRC_UNR_SCROLL_RECT;
				SDL_RenderTextureRotated(rend_data->renderer, texture(tx_scroll), &src_rect, &rect_scroll, (double)RadToDeg(player_direction), NULL, SDL_FLIP_NONE);
				src_rect = GetScrollTextureSrcRect((*(plys + i))->selected_scroll);
				SDL_RenderTextureRotated(rend_data->renderer, texture(tx_icons), &src_rect, &rect_scroll_icon, (double)RadToDeg(player_direction), NULL, SDL_FLIP_NONE);
			}else{
				const SDL_FRect src_rect = SRC_SCROLL_RECT;
				SDL_RenderTextureRotated(rend_data->renderer, texture(tx_scroll), &src_rect, &rect_scroll, (double)RadToDeg(player_direction), NULL, SDL_FLIP_NONE);
			}
		}
	}
}

static void	RenderPlayers(Render_data* const rend_data, const float human_player_direction, Player** plys, SDL_FPoint* points, const unsigned int num){
	SDL_FRect rect_player = {
		0.0F,
		0.0F,
		(float)PLAYER_SIZE,
		(float)PLAYER_SIZE
	};
	for(unsigned int i = 0U; i < num; ++i){
		rect_player.x = (points + i)->x - half(PLAYER_SIZE);
		rect_player.y = (points + i)->y - half(PLAYER_SIZE);
		SDL_RenderTextureRotated(rend_data->renderer, texture(tx_pc), NULL, &rect_player, (double)RadToDeg((*(plys + i))->direction - human_player_direction), NULL, SDL_FLIP_NONE);
		if((*(plys + i))->flags & stunned){
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_stun), NULL, &rect_player, (double)(rend_data->counter * TEXTURE_ROTA_SPEED), NULL, SDL_FLIP_NONE);
		}
	}
}

static void	RenderBarriers(Render_data* const rend_data, const float human_player_direction, Player** plys, SDL_FPoint* points, const unsigned int num){
	SDL_FRect rect_barrier = {
		0.0F,
		0.0F,
		BARRIER_SIZE,
		BARRIER_SIZE
	};
	for(unsigned int i = 0U; i < num; ++i){
		if(!((*(plys + i))->flags & block)){
			continue;
		}
		rect_barrier.x = (points + i)->x - half(BARRIER_SIZE);
		rect_barrier.y = (points + i)->y - half(BARRIER_SIZE);
		if((*(plys + i))->block_times.push < 1){
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_barrier), NULL, &rect_barrier, (double)RadToDeg((*(plys + i))->direction - human_player_direction), NULL, SDL_FLIP_NONE);
		}else{
			const float size = BARRIER_SIZE + (float)(*(plys + i))->block_times.push * BARRIER_MAGNIFICATION;
			SDL_RenderTextureRotated(rend_data->renderer, texture(tx_barrier), NULL,
				&(SDL_FRect){
					(points + i)->x - half(size),
					(points + i)->y - half(size),
					size,
					size
				}, (double)RadToDeg((*(plys + i))->direction - human_player_direction), NULL, SDL_FLIP_NONE
			);
		}
	}
}

static inline Placement GetBeingWeaponPlacement(const Being* const b){
    if(b->status == being_strike || b->status == being_strike_being){
        if(b->status_ticks_left <= BEING_ATTACK_STEPS){
            const int step = BEING_ATTACK_STEPS - b->status_ticks_left;
			return GetWeaponPlacement(&(Placement)BEING_WEAPON_ATTACK_PLCMNT, &(Placement)BEING_WEAPON_BASE_PLCMNT, step, BEING_ATTACK_STEPS);
        }
        if(b->status_ticks_left <= BEING_ATTACK_STEPS * 2){
            const int step = BEING_ATTACK_STEPS * 2 - b->status_ticks_left;
            return GetWeaponPlacement(&(Placement)BEING_WEAPON_PREPARE_PLCMNT, &(Placement)BEING_WEAPON_ATTACK_PLCMNT, step, BEING_ATTACK_STEPS);
        }
        const int step = BEING_ATTACK_STEPS * 3 - b->status_ticks_left;
		return GetWeaponPlacement(&(Placement)BEING_WEAPON_BASE_PLCMNT, &(Placement)BEING_WEAPON_PREPARE_PLCMNT, step, BEING_ATTACK_STEPS);
    }else if(b->status == being_shoot || b->status == being_shoot_being){
		if(b->status_ticks_left < BEING_ATTACK_STEPS / 2){
			const int step = BEING_ATTACK_STEPS / 2 - b->status_ticks_left;
			return GetWeaponPlacement(&(Placement)BEING_WEAPON_ATTACK_PLCMNT, &(Placement)BEING_WEAPON_BASE_PLCMNT, step, BEING_ATTACK_STEPS / 2);
		}
		if(b->status_ticks_left < BEING_ATTACK_STEPS * 3){
        	return (Placement)BEING_WEAPON_ATTACK_PLCMNT;
		}
		if(b->status_ticks_left < (int)(BEING_ATTACK_STEPS * 4)){
			const int step = (int)(BEING_ATTACK_STEPS * 4) - b->status_ticks_left;
			return GetWeaponPlacement(&(Placement)BEING_WEAPON_BASE_PLCMNT, &(Placement)BEING_WEAPON_ATTACK_PLCMNT, step, BEING_ATTACK_STEPS);
		}
        return (Placement)BEING_WEAPON_BASE_PLCMNT;
    }else{
        return (Placement)BEING_WEAPON_BASE_PLCMNT;
    }
}

static inline Placement GetWeaponPlacement(Placement* const start, Placement* const end, const int step, const int steps){
	return (Placement){
		{
			start->position.x + (end->position.x - start->position.x) / (float)steps * step,
			start->position.y + (end->position.y - start->position.y) / (float)steps * step
		},
		start->direction + (end->direction - start->direction) / (float)steps * step
	};
}

static inline void RenderBackground(Render_data* const rend_data){
	SDL_FRect rect = {
		0.0F, 0.0F, rend_data->viewfinder_rect.x, rend_data->window_h
	};
	SDL_FRect rect1 = {
		rend_data->viewfinder_rect.x, 0.0F, rend_data->viewfinder, rend_data->viewfinder_rect.y
	};
	SDL_RenderTexture(rend_data->renderer, texture(tx_background), &rect, &rect);
	SDL_RenderTexture(rend_data->renderer, texture(tx_background), &rect1, &rect1);
	rect.x = rect.w + rend_data->viewfinder;
	rect1.y = rend_data->window_h - rend_data->viewfinder_rect.y;
	SDL_RenderTexture(rend_data->renderer, texture(tx_background), &rect, &rect);
	SDL_RenderTexture(rend_data->renderer, texture(tx_background), &rect1, &rect1);
}

static void RenderFrame(Render_data* const rend_data, SDL_Texture* const tx_backgr1, const SDL_FRect* const frame, const float width){
	const SDL_FRect rects[8] = {
		{
			frame->x,
			frame->y,
			width,
			width
		}, {
			frame->x + frame->w - width,
			frame->y,
			width,
			width
		}, {
			frame->x,
			frame->y + frame->h - width,
			width,
			width
		}, {
			frame->x + frame->w - width,
			frame->y + frame->h - width,
			width,
			width
		}, {//
			frame->x + width,
			frame->y,
			frame->w - width * 2.0F,
			width
		}, {
			frame->x + width,
			frame->y + frame->h - width,
			frame->w - width * 2.0F,
			width
		}, {
			frame->x,
			frame->y + width,
			width,
			frame->h - width * 2.0F
		}, {
			frame->x + frame->w - width,
			frame->y + width,
			width,
			frame->h - width * 2.0F
		}
	};
	const SDL_FRect src_rects[3] = {
		{0.0F, 0.0F, 8.0F, 8.0F},
		{8.0F, 0.0F, 8.0F, 8.0F},
		{0.0F, 8.0F, 8.0F, 8.0F}
	};
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects, rects);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects, rects + 1, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects, rects + 2, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects, rects + 3, 180.0, NULL, SDL_FLIP_NONE);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 1, rects + 4);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 1, rects + 5, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 6);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 7, 0.0, NULL, SDL_FLIP_HORIZONTAL);
}

static void RenderDirectionArrow(Render_data* const rend_data, const double rotation){
	SDL_RenderTextureRotated(rend_data->renderer, texture(tx_arrow), NULL, &(SDL_FRect){
		RIGHT_AREA_X + FRAME_W,
		FRAME_W * 2.0F,
		50.0F,
		50.0F
	}, rotation, NULL, SDL_FLIP_NONE);
}

static void RenderQuickScrolls(Render_data* const rend_data, const Player* const p){
	const float BAR_H = BAR_H_CALC;
	const float Q_SCROLL_SIZE = Q_SCROLL_SIZE_CALC;
	const float shift_scroll = Q_SCROLL_SIZE + FRAME_W * 2.5F;
	SDL_FRect scroll_rect = {
		RIGHT_AREA_X + FRAME_W * 1.5F,
		AREA_Q_SCROLLS_Y,
		Q_SCROLL_SIZE,
		Q_SCROLL_SIZE
	};
	SDL_SetTextureColorMod(texture(tx_chars), 255U, 0U, 0U);
	for(unsigned int i = 0U; i < 3U; ++i){
		scroll_rect.x = RIGHT_AREA_X + FRAME_W * 1.5F;
		for(unsigned int j = 0U; j < 3U; ++j){
			const unsigned int scroll_id = *(p->scrolls_quick_access + i * 3 + j);
			const int scrolls_num = *(p->scrolls + scroll_id);
			const SDL_FRect src_rect = GetScrollTextureSrcRect(scroll_id);
			if(scrolls_num < 1){
				SDL_SetTextureAlphaMod(texture(tx_icons), 31U);
				SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &src_rect, &scroll_rect);
			}else{
				SDL_SetTextureAlphaMod(texture(tx_icons), 255U);
				SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &src_rect, &scroll_rect);
				SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
				RenderIntFromRight(rend_data, scroll_rect.x + Q_SCROLL_SIZE - 1.0F, scroll_rect.y + Q_SCROLL_SIZE - 15.0F, 16.0F, scrolls_num);
				SDL_SetTextureColorMod(texture(tx_chars), 255U, 0U, 0U);
			}
			RenderInt(rend_data, scroll_rect.x + 1.0F, scroll_rect.y - 1.0F, 16.0F, (i * 3 + j + 2) % 10);
			scroll_rect.x += shift_scroll;
		}
		scroll_rect.y += shift_scroll;
	}
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	for(unsigned int i = 0U; i < 9U; ++i){
		if(p->selected_scroll == *(p->scrolls_quick_access + i)){
			SDL_SetRenderDrawColor(rend_data->renderer, 255, 0, 0, 255);
			SDL_RenderRect(rend_data->renderer, &(SDL_FRect){
				RIGHT_AREA_X + FRAME_W * 1.5F + shift_scroll * (i % 3),
				AREA_Q_SCROLLS_Y + shift_scroll * (i / 3),
				Q_SCROLL_SIZE,
				Q_SCROLL_SIZE
			});
			break;
		}
	}
}

static void RenderScrollsManagement(Render_data* const rend_data, const Player* const pc){
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 127);
	SDL_RenderFillRect(rend_data->renderer, NULL);
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float scrolls_num_text_height = icon_size * 0.25F;
	SDL_FRect dst_rect = {
		FRAME_W,
		icon_size + FRAME_W * 3.0F,
		icon_size,
		icon_size
	};
	const float shift = icon_size + FRAME_W;
	const SDL_FRect menu_ptr_rect = {
		FRAME_W + shift * (pc->help_data.menu_position % ICONS_IN_VIEWF_ROW),
		icon_size + FRAME_W * 3.0F + shift * (pc->help_data.menu_position / ICONS_IN_VIEWF_ROW),
		icon_size,
		icon_size
	};
	SDL_FRect quick_scroll_rect = {
		FRAME_W + shift * (*pc->scrolls_quick_access % ICONS_IN_VIEWF_ROW),
		icon_size + FRAME_W * 3.0F + shift * (*pc->scrolls_quick_access / ICONS_IN_VIEWF_ROW),
		icon_size,
		icon_size
	};
	const SDL_FRect selection_rect = {
		FRAME_W + shift * (pc->selected_scroll % ICONS_IN_VIEWF_ROW),
		icon_size + FRAME_W * 3.0F + shift * (pc->selected_scroll / ICONS_IN_VIEWF_ROW),
		icon_size,
		icon_size
	};
	for(unsigned int i = 0U; i < SCROLLS_NUM; ++i){
		const unsigned int col = i % ICONS_IN_VIEWF_ROW;
		const unsigned int row = i / ICONS_IN_VIEWF_ROW;
		dst_rect.x = FRAME_W + shift * col;
		dst_rect.y = icon_size + FRAME_W * 3.0F + shift * row;
		const int scrolls_num = *(pc->scrolls + i);
		const SDL_FRect src_rect = GetScrollTextureSrcRect(i);
		if(scrolls_num < 1){
			SDL_SetTextureAlphaMod(texture(tx_icons), 31U);
			SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &src_rect, &dst_rect);
		}else{
			SDL_SetTextureAlphaMod(texture(tx_icons), 255U);
			SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &src_rect, &dst_rect);
			const SDL_FPoint dst_point_scrolls_num = {
				4.0F + icon_size + shift * col,
				icon_size * 1.75F + FRAME_W * 3.0F + shift * row 
			};
			RenderIntFromRight(rend_data, dst_point_scrolls_num.x, dst_point_scrolls_num.y, scrolls_num_text_height, scrolls_num);
		}
	}
	SDL_SetTextureColorMod(texture(tx_menu_ptr), 31U, 127U, 255U);
	SDL_SetTextureColorMod(texture(tx_chars), 255U, 0U, 0U);
	for(unsigned int i = 0U; i < 9U; ++i){
		quick_scroll_rect.x = FRAME_W + shift * (*(pc->scrolls_quick_access + i) % ICONS_IN_VIEWF_ROW);
		quick_scroll_rect.y = icon_size + FRAME_W * 3.0F + shift * (*(pc->scrolls_quick_access + i) / ICONS_IN_VIEWF_ROW);
		SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &quick_scroll_rect);
		RenderInt(rend_data, quick_scroll_rect.x + 4.0F, quick_scroll_rect.y, scrolls_num_text_height, (i + 2) % 10);
	}
	SDL_SetTextureColorMod(texture(tx_menu_ptr), WHITE_RGB);
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &menu_ptr_rect);
	SDL_SetRenderDrawColor(rend_data->renderer, 255, 0, 0, 255);
	SDL_RenderRect(rend_data->renderer, &selection_rect);
}

static void RenderMenu(Render_data* const rend_data, const Player* const pc){
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 127);
	SDL_RenderFillRect(rend_data->renderer, NULL);
	const Uint8* texts[] = MENU_TEXTS;
	const unsigned int texts_chars_num[] = MENU_TEXTS_SIZES;
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float text_height = icon_size * 0.5F;
	SDL_FPoint dst_point_text = {
		icon_size,
		icon_size * 1.25F + FRAME_W * 3.0F
	};
	const float shift = icon_size + FRAME_W;
	const SDL_FRect menu_ptr_rect = {
		FRAME_W,
		icon_size + FRAME_W * 3.0F + shift * pc->help_data.menu_position,
		rend_data->viewfinder_rect.w - FRAME_W * 2.0F,
		icon_size
	};
	for(unsigned int i = 0U; i < OPTIONS_NUM; ++i){
		RenderTextCentered(rend_data, 0.0F, dst_point_text.y, text_height, *(texts + i), *(texts_chars_num + i), rend_data->viewfinder);
		dst_point_text.y += shift;
	}
	SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &menu_ptr_rect);
}

static void DrawBackground(Render_data* const rend_data){
	SDL_Surface* surface = NULL;
	char* bmp_path = NULL;
	if(texture(tx_background) != NULL){
		SDL_DestroyTexture(texture(tx_background));
	}
	texture(tx_background) = SDL_CreateTexture(rend_data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rend_data->window_w, rend_data->window_h);
	if(!(texture(tx_background))){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
		exit(-1);
	}
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), BACKGROUND_TX0_FILE_NAME);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	SDL_Texture* tx_backgr0 = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!tx_backgr0){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	SDL_DestroySurface(surface);
	SDL_free(bmp_path);
	SDL_asprintf(&bmp_path, "%sdata/%s.bmp", SDL_GetBasePath(), BACKGROUND_TX1_FILE_NAME);
	surface = SDL_LoadBMP(bmp_path);
	if(!surface){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
		exit(-1);
	}
	SDL_Texture* tx_backgr1 = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!tx_backgr1){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	SDL_DestroySurface(surface);
	SDL_free(bmp_path);
	SDL_SetRenderTarget(rend_data->renderer, texture(tx_background));
	const SDL_FRect nesw_rect = {
		RIGHT_AREA_X + FRAME_W,
		FRAME_W * 2.0F,
		50.0F,
		50.0F
	};
	const SDL_FRect rects[16] = {
		{
			FRAME_W,
			FRAME_W,
			SIDE_AREA_WIDTH,
			rend_data->window_h - FRAME_W * 2.0
		}, {
			RIGHT_AREA_X,
			FRAME_W,
			SIDE_AREA_WIDTH,
			rend_data->window_h - FRAME_W * 2.0
		}, {//
			SIDE_AREA_WIDTH,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			RIGHT_AREA_X,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			SIDE_AREA_WIDTH,
			rend_data->viewfinder_rect.y + rend_data->viewfinder_rect.h,
			FRAME_W,
			FRAME_W
		}, {
			RIGHT_AREA_X,
			rend_data->viewfinder_rect.y + rend_data->viewfinder_rect.h,
			FRAME_W,
			FRAME_W
		}, {//
			0.0F,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			0.0F + rend_data->window_w - FRAME_W,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			0.0F,
			0.0F + rend_data->window_h - FRAME_W,
			FRAME_W,
			FRAME_W
		}, {
			0.0F + rend_data->window_w - FRAME_W,
			0.0F + rend_data->window_h - FRAME_W,
			FRAME_W,
			FRAME_W
		}, {//
			FRAME_W,
			0.0F,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {
			RIGHT_AREA_X + FRAME_W,
			0.0F,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {
			FRAME_W,
			rend_data->window_h - FRAME_W,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {
			RIGHT_AREA_X + FRAME_W,
			rend_data->window_h - FRAME_W,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {//
			0.0F,
			FRAME_W,
			FRAME_W,
			rend_data->window_h - FRAME_W * 2.0F
		}, {
			rend_data->window_w - FRAME_W,
			FRAME_W,
			FRAME_W,
			rend_data->window_h - FRAME_W * 2.0F
		}
	};
	const SDL_FRect src_rects[6] = {
		{0.0F, 0.0F, 512.0F, 1024.0F},
		{512.0F, 0.0F, 512.0F, 1024.0F},
		{0.0F, 0.0F, 8.0F, 8.0F},
		{8.0F, 0.0F, 8.0F, 8.0F},
		{0.0F, 8.0F, 8.0F, 8.0F},
		{8.0F, 8.0F, 8.0F, 8.0F}
	};
	const float BAR_H = BAR_H_CALC;
	const float Q_SCROLL_SIZE = Q_SCROLL_SIZE_CALC;
	SDL_FRect frames[] = {
		{
			ICON_X - FRAME_W,
			AREAA_Y - FRAME_W,
			BAR_H + FRAME_W * 2.0F,
			BAR_H + FRAME_W * 2.0F
		}, {
			BAR_X - FRAME_W,
			AREAA_Y + BAR_H + FRAME_W,
			BAR_W + FRAME_W * 2.0F,
			BAR_H + FRAME_W * 2.0F
		}, {
			RIGHT_AREA_X + FRAME_W,
			AREA_MP_Y - FRAME_W,
			BAR_H + FRAME_W * 2.0F,
			BAR_H + FRAME_W * 2.0F
		}, {
			RIGHT_AREA_X + FRAME_W,
			AREA_MP_Y + BAR_H + FRAME_W,
			BAR_W + FRAME_W * 2.0F,
			MP_BAR_H * 2.0F + FRAME_W * 2.0F
		}, {
			RIGHT_AREA_X + FRAME_W * 0.5F,
			AREA_Q_SCROLLS_Y - FRAME_W,
			Q_SCROLL_SIZE + FRAME_W * 2.0F,
			Q_SCROLL_SIZE + FRAME_W * 2.0F
		}, {
			nesw_rect.x - FRAME_W,
			nesw_rect.y - FRAME_W,
			nesw_rect.w + FRAME_W * 2.0F,
			nesw_rect.h + FRAME_W * 2.0F
		}, {
			ICON_X + BAR_H + FRAME_W,
			KEY_ICON_FRAME_Y,
			BAR_W - BAR_H,
			BAR_H + FRAME_W * 2.0F
		}
	};
	SDL_FRect empty_areas[] = {
		{
			ICON_X,
			AREAA_Y,
			BAR_H,
			BAR_H
		}, {
			RIGHT_AREA_X + FRAME_W * 2.0F,
			AREA_MP_Y,
			BAR_H,
			BAR_H
		}, {
			RIGHT_AREA_X + FRAME_W * 2.0F,
			AREA_MP_Y + BAR_H + FRAME_W * 2.0F,
			BAR_W,
			MP_BAR_H
		}, {
			RIGHT_AREA_X + FRAME_W * 1.5F,
			AREA_Q_SCROLLS_Y,
			Q_SCROLL_SIZE,
			Q_SCROLL_SIZE
		}
	};
	const SDL_FRect empty_bar_rects[] = {
		{
			BAR_X,
			AREAA_Y + BAR_H + FRAME_W * 2.0F,
			BAR_W,
			SMALL_BAR_H
		}, {
			BAR_X,
			AREAA_Y + BAR_H + FRAME_W * 2.0F + SMALL_BAR_H,
			BAR_W,
			SMALL_BAR_H
		}, {
			BAR_X,
			AREAA_Y + BAR_H * 3.0F + FRAME_W * 7.0F,
			BAR_W,
			BAR_H
		}, {
			BAR_X,
			AREAA_Y + BAR_H * 5.0F + FRAME_W * 12.0F,
			BAR_W,
			BAR_H
		}, {
			BAR_X,
			KEY_ICON_FRAME_Y + FRAME_W,
			BAR_W ,
			BAR_H
		}, {
			BAR_X,
			KEY_ICON_FRAME_Y + FRAME_W * 4.0F + BAR_H,
			BAR_W,
			BAR_H
		}
	};
	SDL_FRect icon_scr_rect = FIRST_ICON_SRC_RECT;
	SDL_RenderTexture(rend_data->renderer, tx_backgr0, src_rects, rects);
	SDL_RenderTexture(rend_data->renderer, tx_backgr0, src_rects + 1, rects + 1);

	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 2, 180.0, NULL, SDL_FLIP_NONE);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 3, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 4, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 2, rects + 5);

	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 5, rects + 6);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 5, rects + 7, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 5, rects + 8, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 5, rects + 9, 180.0, NULL, SDL_FLIP_NONE);

	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 3, rects + 10, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 3, rects + 11, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 3, rects + 12);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 3, rects + 13);

	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, src_rects + 4, rects + 14, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTexture(rend_data->renderer, tx_backgr1, src_rects + 4, rects + 15);

	RenderFrame(rend_data, tx_backgr1, &(SDL_FRect)VIEWFINDER_FRAME, FRAME_W);
	RenderPortrait(rend_data, tx_backgr1);
	const float shift_bar = BAR_H * 2.0F + FRAME_W * 5.0F;
	const float shift_scroll = Q_SCROLL_SIZE + FRAME_W * 2.5F;
	icon_scr_rect.x = ICON_TX_SIZE * ic_arm;
	for(unsigned int i = 0U; i < 3U; ++i){
		RenderFrame(rend_data, tx_backgr1, frames, FRAME_W);
		RenderFrame(rend_data, tx_backgr1, frames + 1, FRAME_W);
		SDL_SetTextureColorMod(tx_backgr1, 127U, 127U, 127U);
		SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_areas, 0.0F, NULL, SDL_FLIP_VERTICAL);
		SDL_SetTextureColorMod(tx_backgr1, WHITE_RGB);
		SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &icon_scr_rect, empty_areas);
		icon_scr_rect.x += ICON_TX_SIZE;
		frames->y += shift_bar;
		(frames + 1)->y += shift_bar;
		empty_areas->y += shift_bar;
	}
	SDL_SetTextureColorMod(tx_backgr1, 127U, 127U, 127U);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_bar_rects + 4, 0.0F, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_bar_rects + 5, 0.0F, NULL, SDL_FLIP_VERTICAL);
	empty_areas->y = KEY_ICON_FRAME_Y + FRAME_W;
	icon_scr_rect.x = ICON_TX_SIZE * ic_key;
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &icon_scr_rect, empty_areas);
	empty_areas->y += BAR_H + FRAME_W * 3.0F;
	icon_scr_rect.x = ICON_TX_SIZE * ic_coin;
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &icon_scr_rect, empty_areas);
	SDL_SetTextureColorMod(tx_backgr1, WHITE_RGB);
	frames->y = KEY_ICON_FRAME_Y;
	RenderFrame(rend_data, tx_backgr1, frames, FRAME_W);
	RenderFrame(rend_data, tx_backgr1, frames + 6, FRAME_W);
	(frames + 6)->y += BAR_H + FRAME_W * 3.0F;
	RenderFrame(rend_data, tx_backgr1, frames + 6, FRAME_W);
	frames->y +=  BAR_H + FRAME_W * 3.0F;
	RenderFrame(rend_data, tx_backgr1, frames, FRAME_W);
	RenderFrame(rend_data, tx_backgr1, frames + 2, FRAME_W);
	RenderFrame(rend_data, tx_backgr1, frames + 3, FRAME_W);
	RenderFrame(rend_data, tx_backgr1, frames + 5, FRAME_W);
	SDL_SetTextureColorMod(tx_backgr1, 127U, 127U, 127U);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_areas + 1, 0.0F, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_areas + 2, 0.0F, NULL, SDL_FLIP_VERTICAL);
	(empty_areas + 2)->y += MP_BAR_H;
	SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_areas + 2, 0.0F, NULL, SDL_FLIP_VERTICAL);
	SDL_SetTextureColorMod(tx_backgr1, WHITE_RGB);
	icon_scr_rect.x = ICON_TX_SIZE * ic_mp;
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &icon_scr_rect, empty_areas + 1);
	for(unsigned int i = 0U; i < 3U; ++i){
		(frames + 4)->x = RIGHT_AREA_X + FRAME_W * 0.5F;
		(empty_areas + 3)->x = RIGHT_AREA_X + FRAME_W * 1.5F;
		for(unsigned int j = 0U; j < 3U; ++j){
			RenderFrame(rend_data, tx_backgr1, frames + 4, FRAME_W);
			SDL_SetTextureColorMod(tx_backgr1, 127U, 127U, 127U);
			SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_areas + 3, 0.0F, NULL, SDL_FLIP_VERTICAL);
			SDL_SetTextureColorMod(tx_backgr1, WHITE_RGB);
			(frames + 4)->x += shift_scroll;
			(empty_areas + 3)->x += shift_scroll;
		}
		(frames + 4)->y += shift_scroll;
		(empty_areas + 3)->y += shift_scroll;
	}
	for(unsigned int i = 0U; i < 4U; ++i){
		SDL_RenderTextureRotated(rend_data->renderer, tx_backgr1, &FRAME_PART_0, empty_bar_rects + i, 0.0F, NULL, SDL_FLIP_VERTICAL);
	}
	SDL_SetTextureColorMod(tx_backgr0, 15U, 0U, 0U);
	SDL_RenderTexture(rend_data->renderer, tx_backgr0, NULL, &(SDL_FRect){
		rend_data->viewfinder_rect.x,
		rend_data->viewfinder_rect.y,
		rend_data->viewfinder_rect.w,
		rend_data->viewfinder_rect.h
	});
	SDL_SetTextureColorMod(tx_backgr0, WHITE_RGB);
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(rend_data->renderer, &nesw_rect);
	icon_scr_rect.x = ICON_TX_SIZE * ic_directions;
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &icon_scr_rect, &nesw_rect);
	SDL_DestroyTexture(tx_backgr0);
	SDL_DestroyTexture(tx_backgr1);
	surface = SDL_RenderReadPixels(rend_data->renderer, NULL);
	SDL_DestroyTexture(texture(tx_background));
	texture(tx_background) = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	if(!texture(tx_background)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		exit(-1);
	}
	SDL_DestroySurface(surface);
}

static void RenderPortrait(Render_data* const rend_data, SDL_Texture* const tx_backgr1){
	const SDL_FRect portrait_frame_rect = {
		SIDE_AREA_WIDTH < PORTRAIT_H ? PORTRAIT_Y - FRAME_W : FRAME_W + half(SIDE_AREA_WIDTH) - half(PORTRAIT_H),
		PORTRAIT_Y - FRAME_W,
		SIDE_AREA_WIDTH < PORTRAIT_H ? SIDE_AREA_WIDTH - FRAME_W * 2.0F : PORTRAIT_H,
		SIDE_AREA_WIDTH < PORTRAIT_H ? SIDE_AREA_WIDTH - FRAME_W * 2.0F : PORTRAIT_H
	};
	const SDL_FRect portrait_rect = {
		portrait_frame_rect.x + FRAME_W,
		portrait_frame_rect.y + FRAME_W,
		portrait_frame_rect.w - FRAME_W * 2.0F,
		portrait_frame_rect.h - FRAME_W * 2.0F,
	};
	RenderFrame(rend_data, tx_backgr1, &portrait_frame_rect, FRAME_W);
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(rend_data->renderer, &portrait_rect);
	SDL_RenderTexture(rend_data->renderer, texture(tx_portrait), NULL, &portrait_rect);
}

static SDL_FPoint GetCharacterXPositionAndWidth(const int character_num){
	const SDL_FPoint x_positions[] = CHARACTERS_X_AND_W;
	return *(x_positions + character_num);
}

static inline unsigned int GetCharacterRow(const int character_num){
	if(character_num >= ROW_5_CHARS){
		return 6U;
	}
	if(character_num >= ROW_4_CHARS){
		return 5U;
	}
	if(character_num >= ROW_3_CHARS){
		return 4U;
	}
	if(character_num >= ROW_2_CHARS){
		return 3U;
	}
	if(character_num >= ROW_1_CHARS){
		return 2U;
	}
	if(character_num >= ROW_0_CHARS){
		return 1U;
	}
	return 0U;
}

static void RenderText(Render_data* const rend_data, float x, const float y, const float size, const Uint8* char_nums){
	const float size_factor = size / CHARS_ROW_HEIGHT;
	do{
		const SDL_FPoint x_and_width = GetCharacterXPositionAndWidth(*char_nums);
		const float char_width = x_and_width.y * size_factor;
		SDL_RenderTexture(rend_data->renderer, texture(tx_chars), &(SDL_FRect){
			x_and_width.x,
			GetCharacterRow(*char_nums) * CHARS_ROW_HEIGHT,
			x_and_width.y,
			CHARS_ROW_HEIGHT
		}, &(SDL_FRect){
			x,
			y,
			char_width,
			CHARS_ROW_HEIGHT * size_factor
		});
		x += char_width + 1.0F;
		++char_nums;
	}while(*char_nums != char_end);
}

static void RenderTextWithKnownCharsXAndWidth(Render_data* const rend_data, float x, const float y, const float size_factor, const Uint8* const char_nums, const unsigned int chars_num, const SDL_FPoint* const chars_x_and_width){
	for(unsigned int i = 0U; i < chars_num; ++i){
		const SDL_FPoint x_and_width = *(chars_x_and_width + i);
		const float char_width = x_and_width.y * size_factor;
		SDL_RenderTexture(rend_data->renderer, texture(tx_chars), &(SDL_FRect){
			x_and_width.x,
			GetCharacterRow(*(char_nums + i)) * CHARS_ROW_HEIGHT,
			x_and_width.y,
			CHARS_ROW_HEIGHT
		}, &(SDL_FRect){
			x,
			y,
			char_width,
			CHARS_ROW_HEIGHT * size_factor
		});
		x += char_width + 1.0F;
	}
}

static void RenderTextCentered(Render_data* const rend_data, float x, const float y, const float size, const Uint8* char_nums, const unsigned int chars_num, const float space){
	SDL_FPoint chars_x_and_width[chars_num];
	float width = -1.0F;
	const float size_factor = size / CHARS_ROW_HEIGHT;
	for(unsigned int i = 0U; i < chars_num; ++i){
		*(chars_x_and_width + i) = GetCharacterXPositionAndWidth(*(char_nums + i));
		width += (chars_x_and_width + i)->y * size_factor + 1.0F;
	}
	x += half(space - width);
	RenderTextWithKnownCharsXAndWidth(rend_data, x, y, size_factor, char_nums, chars_num, chars_x_and_width);
}

static void RenderTextFromRight(Render_data* const rend_data, float x, const float y, const float size, const Uint8* char_nums, const unsigned int chars_num){
	SDL_FPoint chars_x_and_width[chars_num];
	float width = -1.0F;
	const float size_factor = size / CHARS_ROW_HEIGHT;
	for(unsigned int i = 0U; i < chars_num; ++i){
		*(chars_x_and_width + i) = GetCharacterXPositionAndWidth(*(char_nums + i));
		width += (chars_x_and_width + i)->y * size_factor + 1.0F;
	}
	x -= width;
	RenderTextWithKnownCharsXAndWidth(rend_data, x, y, size_factor, char_nums, chars_num, chars_x_and_width);
}

static void RenderInt(Render_data* const rend_data, const float x, const float y, const float size, int num){
	const Uint8 cipher_nums[] = CIPHER_NUMS;
	unsigned int text_len = 1U;
	int tmp = 10;
	while(num >= tmp){
		++text_len;
		tmp *= 10;
	}
	Uint8 char_nums[text_len + 1U];
	for(unsigned int i = 0U; i < text_len; ++i){
		tmp /= 10;
		unsigned int cipher = num / tmp;
		*(char_nums + i) = *(cipher_nums + cipher);
		num %= tmp;
	}
	*(char_nums + text_len) = char_end;
	RenderText(rend_data, x, y, size, char_nums);
}

static void RenderIntCentered(Render_data* const rend_data, const float x, const float y, const float size, int num, const float space){
	const Uint8 cipher_nums[] = CIPHER_NUMS;
	unsigned int text_len = 1U;
	int tmp = 10;
	while(num >= tmp){
		++text_len;
		tmp *= 10;
	}
	Uint8 char_nums[text_len];
	for(unsigned int i = 0U; i < text_len; ++i){
		tmp /= 10;
		unsigned int cipher = num / tmp;
		*(char_nums + i) = *(cipher_nums + cipher);
		num %= tmp;
	}
	RenderTextCentered(rend_data, x, y, size, char_nums, text_len, space);
}

static void RenderSignedIntCentered(Render_data* const rend_data, const float x, const float y, const float size, int num, const float space){
	const Uint8 cipher_nums[] = CIPHER_NUMS;
	Uint8 sign;
	if(num < 0){
		num = -num;
		sign = s_minus;
	}else{
		sign = s_plus;
	}
	unsigned int text_len = 2U;
	int tmp = 10;
	while(num >= tmp){
		++text_len;
		tmp *= 10;
	}
	Uint8 char_nums[text_len + 1U];
	*(char_nums) = sign;
	for(unsigned int i = 1U; i < text_len; ++i){
		tmp /= 10;
		unsigned int cipher = num / tmp;
		*(char_nums + i) = *(cipher_nums + cipher);
		num %= tmp;
	}
	RenderTextCentered(rend_data, x, y, size, char_nums, text_len, space);
}

static void RenderIntFromRight(Render_data* const rend_data, const float x, const float y, const float size, int num){
	const Uint8 cipher_nums[] = CIPHER_NUMS;
	unsigned int text_len = 1U;
	int tmp = 10;
	while(num >= tmp){
		++text_len;
		tmp *= 10;
	}
	Uint8 char_nums[text_len];
	for(unsigned int i = 0U; i < text_len; ++i){
		tmp /= 10;
		unsigned int cipher = num / tmp;
		*(char_nums + i) = *(cipher_nums + cipher);
		num %= tmp;
	}
	RenderTextFromRight(rend_data, x, y, size, char_nums, text_len);
}

extern inline void ResetTextTextureAlpha(Render_data* const rend_data){
	SDL_SetTextureAlphaMod(texture(tx_chars), 255U);
}

void SetSelectetScrollMouseSelection(const Render_data* const rend_data, Player* const p){
	const int num = GetMouseScrollManagPositionNum(rend_data);
	if(num >= 0){
		p->selected_scroll = num;
	}
}

void SetPointedScrollMouseSelection(const Render_data* const rend_data, Player* const pc){
	const int num = GetMouseScrollManagPositionNum(rend_data);
	if(num >= 0){
		pc->help_data.menu_position = num;
	}
}

unsigned int GetMouseShopSelection(const Render_data* const rend_data){
	const float width = (float)rend_data->window_w / SHOP_COLS;
	const float height = (float)rend_data->window_h / SHOP_ROWS;
	SDL_FPoint mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);
	mouse.x -= half(width);
	if(mouse.x <= 0.0F || mouse.x >= (float)rend_data->window_w - width){
		return SHOP_POSITIONS;
	}
	const unsigned int col = (unsigned int)(mouse.x / width);
	const unsigned int row = (unsigned int)(mouse.y / height);
	if(col != SHOP_SIDE_COLS + 1U && col != SHOP_SIDE_COLS + 3U){
		if(
			(row < FIRST_SHOP_ROW || (row == SHOP_ROWS - 1U && col != SHOP_SIDE_COLS + 2U))
		 	|| (col == SHOP_SIDE_COLS || (col == SHOP_SIDE_COLS + 2U && row < SHOP_ROWS - 1U) || col == SHOP_SIDE_COLS + 4U)
			|| (row < FIRST_SHOP_ROW + 1U && col > SHOP_SIDE_COLS + 4U)
		){
			return SHOP_POSITIONS;
		}
	}
	return col + row * SHOP_COLS;
}

void SetPointedOptionMouseSelection(const Render_data* const rend_data, unsigned int* const menu_position){
	const int num = GetMouseMenuPositionNum(rend_data);
	if(num >= 0){
		*menu_position = num;
	}
}

int GetMouseScrollManagPositionNum(const Render_data* const rend_data){
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float shift = icon_size + FRAME_W;
	const SDL_FRect manage_scrolls_rect = MANAGE_SCROLLS_RECT;
	SDL_FPoint mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);
	if(SDL_PointInRectFloat(&mouse, &manage_scrolls_rect)){
		const unsigned int num = (unsigned int)((mouse.x - manage_scrolls_rect.x) / shift) + (unsigned int)((mouse.y - manage_scrolls_rect.y) / shift) * ICONS_IN_VIEWF_ROW;
		if(num < SCROLLS_NUM){
			return num;
		}
		return SCROLLS_NUM - 1U;
	}
	return NOT_FOUND;
}

static int GetMouseMenuPositionNum(const Render_data* const rend_data){
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float shift = icon_size + FRAME_W;
	const SDL_FRect menu_rect = MENU_RECT;
	SDL_FPoint mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);
	if(SDL_PointInRectFloat(&mouse, &menu_rect)){
		const unsigned int num = (unsigned int)((mouse.y - menu_rect.y) / shift);
		if(num < OPTIONS_NUM){
			return num;
		}
		return OPTIONS_NUM - 1U;
	}
	return NOT_FOUND;
}

void RenderDefeatedScreen(Render_data* const rend_data){
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 0U, 0U, 255U);
	SDL_RenderClear(rend_data->renderer);
	SDL_SetTextureColorMod(texture(tx_chars), 255U, 0U, 0U);
	const float text_height = rend_data->window_h * 0.125F;
	RenderTextCentered(rend_data, 0.0F, WINDOW_CENTER_Y - half(text_height), text_height, (Uint8[])DEFEAT_TEXT, DEFEAT_TEXT_LEN, rend_data->window_w);
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	SDL_RenderPresent(rend_data->renderer);
}

void RenderVictoryScreen(Render_data* const rend_data){
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 0U, 0U, 255U);
	SDL_RenderClear(rend_data->renderer);
	SDL_SetTextureColorMod(texture(tx_chars), 0U, 255U, 0U);
	const float text_height = rend_data->window_h * 0.125F;
	RenderTextCentered(rend_data, 0.0F, WINDOW_CENTER_Y - half(text_height), text_height, (Uint8[])VICTORY_TEXT, VICTORY_TEXT_LEN, rend_data->window_w);
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	SDL_RenderPresent(rend_data->renderer);
}

extern inline void SetMouseBarrier(Render_data* const rend_data){
	SDL_SetWindowMouseRect(rend_data->window, &(SDL_Rect)MOUSE_RECT);
}

static inline SDL_FRect GetScrollTextureSrcRect(unsigned int scroll_num){
	return (SDL_FRect){
		(scroll_num % TX_ICONS_IN_ROW) * ICON_TX_SIZE,
		ICON_SCRL_0_Y + (scroll_num / TX_ICONS_IN_ROW) * ICON_TX_SIZE,
		ICON_TX_SIZE,
		ICON_TX_SIZE
	};
}

static inline SDL_FRect GetShopItemTextureSrcRect(unsigned int item_num){
	return (SDL_FRect){
		(item_num - shop_item_invalid) * ICON_TX_SIZE,
		0.0F,
		ICON_TX_SIZE,
		ICON_TX_SIZE
	};
}

static void RenderShopElement(Render_data* const rend_data, const float x, const float y, const float size, const int price, const unsigned int texture_num, const SDL_FRect* const src_rect){
	const float texture_x = x + size * 0x1.0p-4F;
	const float texture_size = size * 0x7.0p-3F;
	const float text_y = y + size * 0x3.0p-2F;
	SDL_RenderTexture(rend_data->renderer, texture(texture_num), src_rect, &(SDL_FRect){texture_x, y, texture_size, texture_size});
	RenderIntCentered(rend_data, x, text_y, size * 0x1.0p-2F, price, size);
}

void RenderShop(Render_data* const rend_data, const Player* const pc, const Shop* const sh, const Uint8* const items_to_sell, const unsigned int items_to_sell_num, const Uint8* const items_to_get, const unsigned int items_to_get_num, const int profit){
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 0U, 0U, 255U);
	SDL_RenderClear(rend_data->renderer);
	const float width = (float)rend_data->window_w / SHOP_COLS;
	const float height = (float)rend_data->window_h / SHOP_ROWS;
	for(unsigned int i = 0U; i < scroll_empty; ++i){
		if(*(pc->scrolls + i) > 0U){
			SDL_FRect src_rect = GetScrollTextureSrcRect(i);
			RenderShopElement(rend_data, width * (0.5F + (i % SHOP_SIDE_COLS)), height * ((i / SHOP_SIDE_COLS) + FIRST_SHOP_ROW), width, ItemPrice(i) / SELL_DIVIDER, tx_icons, &src_rect);
		}
	}
	for(unsigned int i = 0U; i < items_to_sell_num; ++i){
		SDL_FRect src_rect = GetScrollTextureSrcRect(*(items_to_sell + i));
		RenderShopElement(rend_data, width * (0.5F + SHOP_SIDE_COLS + 1.0F), height * i, width, ItemPrice(*(items_to_sell + i)) / SELL_DIVIDER, tx_icons, &src_rect);
	}
	for(unsigned int i = 0U; i < items_to_get_num; ++i){
		if(*(items_to_get + i) < shop_item_invalid){
			SDL_FRect src_rect = GetScrollTextureSrcRect(*(items_to_get + i));
			RenderShopElement(rend_data, width * (0.5F + SHOP_SIDE_COLS + 3.0F), height * i, width, ItemPrice(*(items_to_get + i)), tx_icons, &src_rect);
		}else{
			SDL_FRect src_rect = GetShopItemTextureSrcRect(*(items_to_get + i));
			RenderShopElement(rend_data, width * (0.5F + SHOP_SIDE_COLS + 3.0F), height * i, width, ItemPrice(*(items_to_get + i)), tx_shop_icons, &src_rect);
		}
	}
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &(SDL_FRect){ICON_TX_SIZE * ic_mp, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE}, &(SDL_FRect){
		width * 0.5F,
		height * (FIRST_SHOP_ROW - 1.0F),
		width,
		width
	});
	SDL_RenderTexture(rend_data->renderer, texture(tx_icons), &(SDL_FRect){ICON_TX_SIZE * ic_coin, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE}, &(SDL_FRect){
		width * (0.5F + SHOP_SIDE_COLS - 3.0F),
		height * (FIRST_SHOP_ROW - 1.0F),
		width,
		width
	});
	SDL_SetTextureColorMod(texture(tx_chars), MP_TEXT_RGB);
	RenderIntCentered(rend_data, width * 1.5F, height * (FIRST_SHOP_ROW - 1.0F) + width * 0.25F, half(width), pc->magic_points, width * 2.0F);
	SDL_SetTextureColorMod(texture(tx_chars), WHITE_RGB);
	RenderIntCentered(rend_data, width * (0.5F + SHOP_SIDE_COLS - 2.0F), height * (FIRST_SHOP_ROW - 1.0F) + width * 0.25F, half(width), pc->coins, width * 2.0F);
	if(profit){
		RenderSignedIntCentered(rend_data, 0.0F, height * (SHOP_ROWS - 1.0F), height, profit, rend_data->window_w);
	}else if(items_to_sell_num){
		RenderTextCentered(rend_data, 0.0F, height * (SHOP_ROWS - 1.0F), height, (Uint8[]){O,K}, 2U, rend_data->window_w);
	}
	for(unsigned int i = 0U; i < SHOP_SCROLLS_NUM; ++i){
		if(*(sh->scrolls + i) < scroll_empty){
			SDL_FRect src_rect = GetScrollTextureSrcRect(*(sh->scrolls + i));
			RenderShopElement(rend_data, width * (0.5F + SHOP_COLS - SHOP_SCROLLS_COLS - 1.0F + (i % SHOP_SCROLLS_COLS)), height * (FIRST_SHOP_ROW + 1.0F + (i / SHOP_SCROLLS_COLS)), width, ItemPrice(*(sh->scrolls + i)), tx_icons, &src_rect);
		}
	}
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS - 1.0F)), height * (FIRST_SHOP_ROW + 1.0F), width, ItemPrice(shop_item_mp10), tx_shop_icons, &(SDL_FRect){ICON_TX_SIZE * 1.0F, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE});
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS - 1.0F)), height * (FIRST_SHOP_ROW + 2.0F), width, ItemPrice(shop_item_mp100), tx_shop_icons, &(SDL_FRect){ICON_TX_SIZE * 2.0F, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE});
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS)), height * (FIRST_SHOP_ROW + 1.0F), width, ItemPrice(shop_item_key), tx_shop_icons, &(SDL_FRect){ICON_TX_SIZE * 3.0F, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE});
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS + 1.0F)), height * (FIRST_SHOP_ROW + 1.0F), width, ItemPrice(shop_item_key_location), tx_shop_icons, &(SDL_FRect){ICON_TX_SIZE * 4.0F, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE});
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS - 1.0F)), height * (FIRST_SHOP_ROW + 5.0F), width, ItemPrice(shop_item_armour), tx_shop_icons, &(SDL_FRect){ICON_TX_SIZE * 5.0F, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE});
	RenderShopElement(rend_data, width * (0.5F + (SHOP_COLS - SHOP_SIDE_COLS)), height * (FIRST_SHOP_ROW + 5.0F), width, ItemPrice(shop_item_dodge_speed), tx_shop_icons, &(SDL_FRect){ICON_TX_SIZE * 6.0F, 0.0F, ICON_TX_SIZE, ICON_TX_SIZE});
	if(pc->help_data.menu_position < SHOP_POSITIONS){
		const unsigned int row = pc->help_data.menu_position / SHOP_COLS;
		if(row == SHOP_ROWS - 1U){
			if(profit || items_to_sell_num){
				SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &(SDL_FRect){
					width * (0.5F + SHOP_SIDE_COLS + 1U),
					height * row,
					width * 3.0F,
					height
				});
			}
		}else{
			const SDL_FRect menu_ptr_rect = {
				width * (0.5F + (pc->help_data.menu_position % SHOP_COLS)),
				height * row,
				width,
				width
			};
			SDL_RenderTexture(rend_data->renderer, texture(tx_menu_ptr), NULL, &menu_ptr_rect);
		}
	}
	SDL_FRect rects[6] = {
		{width * 0.5F, height * 2.0F, width * 7.0F, height * 7.0F},
		{width * 8.5F, height * 0.0F, width * 1.0F, height * 9.0F},
		{width * 10.5F, height * 0.0F, width * 1.0F, height * 9.0F},
		{width * 12.5F, height * 2.0F, width * 3.0F, height * 7.0F},
		{width * 16.5F, height * 2.0F, width * 3.0F, height * 7.0F},
		{width * 8.5F, height * 9.0F, width * 3.0F, height * 1.0F}
	};
	SDL_SetRenderDrawColor(rend_data->renderer, WHITE_RGB, 255U);
	SDL_RenderRects(rend_data->renderer, rects, 6);
	SDL_RenderPresent(rend_data->renderer);
}

void ToggleFullscreen(Render_data* const rend_data){
	const bool fullscreen = (SDL_GetWindowFlags(rend_data->window) & SDL_WINDOW_FULLSCREEN);
	if(fullscreen){
		SDL_SetWindowFullscreen(rend_data->window, false);
	}else{
		SDL_SetWindowFullscreen(rend_data->window, true);
	}
	SDL_SyncWindow(rend_data->window);
	SDL_DestroyTexture(texture(tx_view));
	ResetRenderData(rend_data);
}

extern inline SDL_FPoint GetMouseWorldPosition(const Game_data* const gd){
	const Render_data* const rend_data = gd->rend_data_ptr;
	const float distance = PLAYER_REND_Y - rend_data->mouse_y;
	return (SDL_FPoint){
		human(gd)->position.x + rend_data->sin_player_direction * distance,
		human(gd)->position.y - rend_data->cos_player_direction * distance
	};
}
