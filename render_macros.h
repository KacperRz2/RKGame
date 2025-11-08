#ifndef RENDER_MACROS_H_
#define RENDER_MACROS_H_

#define VIEWFINDER_CENTER	        (rend_data->viewfinder * 0.5F)
#define WINDOW_CENTER_Y		        (rend_data->window_h / 2)
#define VIEWFINDER_BEFORE_PC_PART   0.9375F
#define PLAYER_REND_Y               (rend_data->viewfinder * VIEWFINDER_BEFORE_PC_PART)
#define PLAYER_REND_Y_SHIFT	        (PLAYER_REND_Y - half(rend_data->viewfinder))
#define TEXTURE_FILES_NUM	        29
#define TEXTURE_CREATED_NUM	        2
#define SIGHT_SQUARED   	        (VIEWFINDER_CENTER * VIEWFINDER_CENTER + (VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT) * (VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT))
#define SIGHT_BACK_SQUARED	        (VIEWFINDER_CENTER * VIEWFINDER_CENTER + (VIEWFINDER_CENTER - PLAYER_REND_Y_SHIFT) * (VIEWFINDER_CENTER - PLAYER_REND_Y_SHIFT))
#define WINDOW_START_W              1600
#define WINDOW_START_H              900
#define VIEWFINDER_SIZE             (rend_data->window_h - 16.0F)
#define GUN_SIGHT_SPREAD_MIN        8.0F
#define GUN_SIGHT_SPREAD_RANGE      (GUN_SIGHT_SPREAD_MIN * 4.0F)
#define GUN_SIGHT_MIN_DISTANCE      PLAYER_SIZE
#define MINIMAP_PC_SIZE             8.0F
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
#define KEYS_NUM_X                  (ICON_X + BAR_H + FRAME_W * 2.0F + 4.0F)
#define KEYS_NUM_Y                  (KEY_ICON_FRAME_Y + FRAME_W + 1.0F)
#define COINS_NUM_Y                 (KEY_ICON_FRAME_Y + BAR_H * 1.25F + FRAME_W * 4.0F + 1.0F)
#define SCROLLS_MANAG_ICON_SIZE     ((rend_data->viewfinder - FRAME_W) / (float)ICONS_IN_VIEWF_ROW - FRAME_W)
#define SEGMENT_TX_SIZE             (SEGMENT_SIZE * 1.5F)
#define MAX_UNSEEN_SEG              (pow2((unsigned int)(VIEWFINDER_SIZE / SEGMENT_SIZE) + 1U))

#define BEINGS_TEXTURES             {\
                                        tx_being,\
                                        tx_being1,\
                                        tx_being2\
                                    }
#define BEINGS_WEAPON_TEXTURES      {\
                                        tx_being_blade,\
                                        tx_weapon,\
                                        tx_void\
                                    }
#define PROJECTILE_TEXTURES         {\
                                        tx_projectile,\
                                        tx_h_projectile,\
                                        tx_h_projectile\
                                    }
#define TEXTURE_FILES_NAMES         {\
                                        "img0.bmp",\
                                        "img1.bmp",\
                                        "img2.bmp",\
                                        "being.bmp",\
                                        "img3.bmp",\
                                        "img4.bmp",\
                                        "img5.bmp",\
                                        "img6.bmp",\
                                        "img7.bmp",\
                                        "img8.bmp",\
                                        "img9.bmp",\
                                        "imgA.bmp",\
                                        "imgB.bmp",\
                                        "imgC.bmp",\
                                        "imgD.bmp",\
                                        "imgE.bmp",\
                                        "imgF.bmp",\
                                        "img10.bmp",\
                                        "img11.bmp",\
                                        "img12.bmp",\
                                        "img13.bmp",\
                                        "img14.bmp",\
                                        "img15.bmp",\
                                        "img16.bmp",\
                                        "img17.bmp",\
                                        "img18.bmp",\
                                        "font.bmp",\
                                        "img19.bmp",\
                                        "void.bmp"\
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
#define SRC_SCROLL_RECT             {\
                                        0.0F,\
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
#define FRAME_PART_0            ((SDL_FRect){8.0F, 0.0F, 8.0F, 8.0F})
#define ROW_0_CHARS             14
#define ROW_1_CHARS             (ROW_0_CHARS + 17)
#define ROW_2_CHARS             (ROW_1_CHARS + 13)
#define ROW_3_CHARS             (ROW_2_CHARS + 15)
#define ROW_4_CHARS             (ROW_3_CHARS + 16)
#define ROW_5_CHARS             (ROW_4_CHARS + 17)
#define CHARACTERS_X_AND_W      {\
                                    {0,41},{42,83-42}, {84,125-84},  {126,167-126},{168,174-168},{175,221-175},{222,269-222},{270,290-270},{291,344-291},{345,360-345},{361,402-361},{403,418-403},{419,463-419},{463,506-463},\
                                    {0,45},{46,88-46}, {89,130-89},  {131,172-131},{173,213-173},{214,256-214},{257,269-257},{270,282-270},{283,300-283},{301,348-301},{349,363-349},{364,378-364},{379,402-379},{403,426-403},{427,453-427},{454,492-454},{493,512-493},\
                                    {0,42},{43,109-43},{110,147-110},{148,192-148},{193,251-193},{252,298-252},{299,345-299},{346,357-346},{358,406-358},{407,435-407},{436,466-436},{467,500-467},{501,512-501},\
                                    {0,33},{34,49-34}, {50,77-50},   {78,135-78},  {136,183-136},{184,227-184},{228,274-228},{275,305-275},{306,349-306},{350,365-350},{366,391-366},{392,423-392},{424,451-424},{452,482-452},{483,512-483},\
                                    {0,33},{34,77-34}, {78,109-78},  {110,153-110},{154,194-154},{195,226-195},{227,258-227},{259,286-259},{287,319-287},{320,345-320},{346,372-346},{373,404-373},{405,440-405},{441,466-441},{467,495-467},{496,511-496},\
                                    {0,21},{22,54-22}, {55,83-55},   {84,117-84},  {118,145-118},{146,176-146},{177,204-177},{205,220-205},{221,243-221},{244,275-244},{276,327-276},{328,363-328},{364,392-364},{393,418-393},{419,440-419},{441,468-441},{469,512-469},\
                                    {0,25},{26,53-26}, {54,85-54},   {86,118-86},  {119,150-119},{151,182-151},{183,211-183},{212,232-212},{233,241-233},{242,262-242},{263,276-263},{277,314-277},{315,341-315},{342,374-342},{375,395-375},{396,432-396},{433,444-433},{445,457-445},{458,36}\
                                }
#define FIRST_ICON_SRC_RECT     {\
                                    0.0F,\
                                    0.0F,\
                                    ICON_TX_SIZE,\
                                    ICON_TX_SIZE\
                                }
#define VIEWFINDER_RECT         {\
                                    (int)(half(rend_data->window_w - rend_data->viewfinder)),\
                                    (int)(half(rend_data->window_h - rend_data->viewfinder)),\
                                    (int)(rend_data->viewfinder),\
                                    (int)(rend_data->viewfinder)\
                                }
#define VISIBLE_RECT            {\
                                    0.0F,\
                                    0.0F,\
                                    rend_data->viewfinder,\
                                    rend_data->viewfinder\
                                }
#define MOUSE_RECT              {\
                                    rend_data->viewfinder_rect.x,\
                                    rend_data->viewfinder_rect.y,\
                                    rend_data->viewfinder_rect.w,\
                                    (int)(rend_data->viewfinder * VIEWFINDER_BEFORE_PC_PART - GUN_SIGHT_MIN_DISTANCE)\
                                }
#define MANAGE_SCROLLS_RECT     {\
                                    rend_data->viewfinder_rect.x + half(FRAME_W),\
                                    rend_data->viewfinder_rect.y + icon_size + FRAME_W * 2.5F,\
                                    shift * ICONS_IN_VIEWF_ROW,\
                                    shift * 5\
                                }
#define CIPHER_NUMS             {n0,n1,n2,n3,n4,n5,n6,n7,n8,n9}
#define TEST_TEXT               {T,e,s,t,sp,t,e,k,s,t,dot,char_end}
#define MENU_TEXT               {N,a,c,i,ś,n,i,j,sp,S,P,A,C,E,char_end}
#define HORDE_ALERT             {H,o,r,d,a,exclam_m,char_end}

enum texture_num{
    tx_pc,
    tx_icon,
    tx_projectile,
    tx_being,
    tx_viewfinder,
    tx_pc_blade,
    tx_icons,
    tx_arrow,
    tx_being_blade,
    tx_h_projectile,
    tx_terrain,
    tx_gunsightpart,
    tx_wall,
    tx_portal,
    tx_door,
    tx_box,
    tx_barrier,
    tx_stun,
    tx_scroll,
    tx_being1,
    tx_weapon,
    tx_being2,
    tx_terrain1,
    tx_backgr0,
    tx_backgr1,
    tx_bar,
    tx_chars,
    tx_menu_ptr,
    tx_void,
    tx_map,
    tx_background
};

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