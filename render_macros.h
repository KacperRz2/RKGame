#ifndef RENDER_MACROS_H_
#define RENDER_MACROS_H_

#define texture(num)	            (*(rend_data->textures + num))
#define VIEWFINDER_CENTER	        (rend_data->viewfinder * 0.5F)
#define WINDOW_CENTER_Y		        (rend_data->window_h / 2)
#define VIEWFINDER_BEFORE_PC_PART   0.9375F
#define PLAYER_REND_Y               (rend_data->viewfinder * VIEWFINDER_BEFORE_PC_PART)
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
                                        tx_pc_blade,\
                                        tx_weapon1,\
                                        tx_pc_blade,\
                                        tx_weapon,\
                                        tx_pc_blade,\
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
                                        RenderVisualEffectsType2,\
                                        RenderVisualEffectsTimer\
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
                                        half(BLADE_SIZE),\
                                        BLADE_SIZE * BLADE_HANDLER_POSITION\
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
#define PC_RGB_0                    ((Uint8[]){255, 0, 0})
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
#define DAMAGE_VIS_EFFECT(position) ((Visual_effect){\
                                        position,\
                                        (0x10000000ULL / FRAME_TIME),\
                                        (0x10000000ULL / FRAME_TIME),\
                                        32U,\
                                        visual_effect_t0,\
                                        tx_damage_test\
                                    })
#define BONUS_VIS_EFFECT(position) ((Visual_effect){\
                                        position,\
                                        (0x10000000ULL / FRAME_TIME),\
                                        (0x10000000ULL / FRAME_TIME),\
                                        16U,\
                                        visual_effect_t0,\
                                        tx_bonus_effect\
                                    })
#define CURSE_VIS_EFFECT(position) ((Visual_effect){\
                                        position,\
                                        (0x10000000ULL / FRAME_TIME),\
                                        (0x10000000ULL / FRAME_TIME),\
                                        32U,\
                                        visual_effect_t0,\
                                        tx_curse_effect\
                                    })
#define DEAD_VIS_EFFECT(position)   ((Visual_effect){\
                                        position,\
                                        (0x20000000ULL / FRAME_TIME),\
                                        (0x20000000ULL / FRAME_TIME),\
                                        64U,\
                                        visual_effect_t0,\
                                        tx_damage_test\
                                    })
	                                //position; ticks_left; start_ticks; size; type; tx_num;
#define PORTAL_VIS_EFFECT(position) ((Visual_effect){\
                                        position,\
                                        ((0x40000000ULL / FRAME_TIME) / ((unsigned int)OPENING_PORTAL_TICKS / 128U)),\
                                        ((0x40000000ULL / FRAME_TIME) / ((unsigned int)OPENING_PORTAL_TICKS / 128U)),\
                                        128U,\
                                        visual_effect_t1,\
                                        tx_creation_point\
                                    })
#define PROJE_VIS_EFFECT(position)  ((Visual_effect){\
                                        position,\
                                        (0x8000000ULL / FRAME_TIME),\
                                        (0x8000000ULL / FRAME_TIME),\
                                        16U,\
                                        visual_effect_t0,\
                                        tx_creation_point\
                                    })
#define BURN_VIS_EF(position, size) ((Visual_effect){\
                                        position,\
                                        (0x40000000ULL / FRAME_TIME),\
                                        (0x40000000ULL / FRAME_TIME),\
                                        size,\
                                        visual_effect_t2,\
                                        tx_pixel\
                                    })
#define BURN_EF_TIM(posit, si, del) ((Visual_effect){\
                                        posit,\
                                        del,\
                                        (0x40000000ULL / FRAME_TIME),\
                                        si,\
                                        visual_effect_t2_timer,\
                                        tx_pixel\
                                    })
#define CIPHER_NUMS                 {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9}
#define HORDE_ALERT                 {H,o,r,d,a,exclam_m,char_end}
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
                                        "img1",\
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
                                        "void"\
                                    }

#define BEING_TEXTURE0_FILE_NAME    "img8"
#define BEING_TEXTURE1_FILE_NAME    "img12"
#define BACKGROUND_TX0_FILE_NAME    "img16"
#define BACKGROUND_TX1_FILE_NAME    "img17"
#define PROJECTILE_TX_FILE_NAME     "img2"
#define PC_TX0_FILE_NAME            "img0"
#define PC_TX1_FILE_NAME            "img14"

enum inoc_position{
    ic_key,
    ic_coin,
    ic_arm,
    ic_hp,
    ic_fp,
    ic_directions,
    ic_mp
};

#endif