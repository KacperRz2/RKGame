#ifndef MACROS_H_
#define MACROS_H_

#define SQRT2DIV2			        (1.4142135623730950488016887242097F * 0.5F)
#define FULL_ANGLE			        (SDL_PI_F * 2.0F)
#define pow2(x)                     ((x) * (x))
#define half(x)                     ((x) * (0.5F))
#define VIEWFINDER_CENTER	        (rend_data->viewfinder * 0.5F)
#define WINDOW_CENTER_Y		        (rend_data->window_h / 2)
#define VIEWFINDER_BEFORE_PC_PART   0.9375F
#define PLAYER_REND_Y               (rend_data->viewfinder * VIEWFINDER_BEFORE_PC_PART)
#define PLAYER_REND_Y_SHIFT	        (PLAYER_REND_Y - half(rend_data->viewfinder))
#define BIG_SEGMENTS_X              29U
#define BIG_SEGMENT_SEGMENTS_X      16U
#define SEGMENTS_X			        (BIG_SEGMENT_SEGMENTS_X * BIG_SEGMENTS_X + 2U)
#define SEGMENTS_Y			        SEGMENTS_X
#define SEGMENT_SIZE		        128.0F
#define WORLD_W				        (SEGMENT_SIZE * SEGMENTS_X)
#define WORLD_H				        WORLD_W
#define WORLD_SIZE			        WORLD_W
#define PLAYER_SIZE			        32
#define GUN_SIGHT_SIZE		        16
#define BULLET_SIZE			        16
#define DECELERATION		        0.93F
#define TICK_TIME			        0x200000ULL
#define TICK_TIME_MS		        (TICK_TIME / 1000000ULL)
#define ACCELERATION		        6.25e-2F
#define PLAYER_VELOCITY		        0x0.Ap+0F
#define RUN_VELOCITY		        (PLAYER_VELOCITY * 128.0F)
#define ROTATION_SPEED		        3.90625e-3F
#define TEXTURE_FILES_NUM	        21
#define TEXTURE_TARGET_NUM	        0
#define TEXTURE_CREATED_NUM	        1
#define TEXTURES_NUM		        (TEXTURE_FILES_NUM + TEXTURE_TARGET_NUM + TEXTURE_CREATED_NUM)
#define SIGHT_SQUARED   	        (VIEWFINDER_CENTER * VIEWFINDER_CENTER + (VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT) * (VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT))
#define SIGHT_BACK_SQUARED	        (VIEWFINDER_CENTER * VIEWFINDER_CENTER + (VIEWFINDER_CENTER - PLAYER_REND_Y_SHIFT) * (VIEWFINDER_CENTER - PLAYER_REND_Y_SHIFT))
#define FRAME_TIME			        0x400000ULL
#define FRAME_TIME_MS		        (FRAME_TIME / 1000000ULL)
#define MAX_PROJECTILES_NUM	        0x1000U
#define MAX_SEGM_BEINGS		        16
#define MAX_BEINGS_NUM		        0x8000U
#define ANGLE_PARTS		            512
#define MIN_ANGLE                   (2.0F * SDL_PI_F / (float)ANGLE_PARTS)
#define BLADE_DAMAGE                50
#define BLADE_PENETRATION           25U
#define BLADE_SIZE                  64.0F
#define RANGE_SEGMENTS              ((unsigned int)(7000.0F / SEGMENT_SIZE))
#define PROJECTILE_RAN_SEG          ((unsigned int)(2000.0F / SEGMENT_SIZE))
#define MAX_HITS                    25U
#define RAD_TO_MINE                 ((float)ANGLE_PARTS * 0.5F / SDL_PI_F)
#define RANGE                       700.0F
#define BEING_ATTACK_STEPS          64

#define BEING_RELOAD                512
#define BEING_DEFAULT_LEFT_TICKS    32
#define BLADE_BASE_X                16.0F
#define BLADE_BASE_Y                (-8.0F)
#define BLADE_BASE_DIRECTION_BEING  0.0F
#define BEING_WALK_TICKS            128
#define IDLE_BEING_ACTION_DISTANCE  900.0F
#define BEING_ATTACK_DISTANCE       140.0F
#define BEING_SHOOT_DISTANCE        700.0F
#define BEING_RELOAD_TICKS          128
#define BEING_STUN_DURATION         256
#define PROJECTILE_VELOCITY         4.0F
#define BEING_HALT_DISTANCE         70.0F
#define BEING_MIN_DISTANCE          64.0F
#define CHECK_COLLISION_DISTANCE    768.0F
#define UPDATE_BEINGS_INTERVAL      1U
#define WINDOW_START_W              1600
#define WINDOW_START_H              900
#define BLADE_BASE_DIRECTION_PC     (SDL_PI_F * 0.45F)
#define PC_HP                       30000
#define PC_FATIGUE                  1000
#define PC_MAGIC                    0x10
#define PC_M_MAGIC                  0x10000000
#define MOVING_BACK_VELO_MODI       0.93F
#define PC_DODGE_FATIG              100
#define PC_DODGE_FATIG_BLOCK_TIME   50
#define PC_DODGE_VELOCITY           10.0F
#define PC_PUSH_FATIG               100
#define PC_PUSH_FATIG_BLOCK_TIME    50
#define PC_PUSH_RELOAD              256
#define PC_PUSH_REACH               (PLAYER_SIZE * 2.0F)
#define PC_CAST_RELOAD              512
#define PC_FAILURE_FATIG_BLOCK_TIME 2
#define PC_FAILURE_VELOCITY         0.5F
#define PC_FATIGUE_GAIN_INTERVAL    4U
#define TEST_DAMAGE                 100
#define TEST_PENETRATION            3U
#define PC_BLADE_CHECKPOINTS        2U
#define PC_BLADE_CHARGE_BASE        0xF.Fp-4F
#define PC_BLADE_CHARGE_MODIFIER    0xF.F8p-4F
#define PC_BLADE_DAMAGE_BASE        1200.0F
#define PC_BLADE_MAX_IDLE_TICKS     192U
#define PC_BLADE_FIRST_MOVE_STEPS   128
#define PC_BLADE_RETURN_STEPS       384
#define PC_BLADE_TO_NEXT_STEPS      192
#define PC_BLADE_STRIKE_STEPS       32
#define PC_BLADE_AFTER_BLOCK_STEPS  512
#define PC_BLADE_BOUNCE_ANGLE       0.375F
#define VIEWFINDER_WINDOW_H_PART    0.984375F
#define VIEWFINDER_SIZE             (rend_data->window_h * VIEWFINDER_WINDOW_H_PART)
#define BLADE_HANDLER_POSITION      0.85F
#define PC_SHOOT_RELOAD             64
#define GUN_SIGHT_SPREAD_MIN        8.0F
#define GUN_SIGHT_SPREAD_RANGE      (GUN_SIGHT_SPREAD_MIN * 4.0F)
#define GUN_SIGHT_MIN_DISTANCE      PLAYER_SIZE
#define MAX_START_BEINGS_NUM        (MAX_BEINGS_NUM / 0x1U)
#define TEST_BEING_VELOCITY         (PLAYER_VELOCITY * 2.5F)
#define TEST_BEING_DMG_CLOSE        2
#define TEST_BEING_DMG_FAR          1
#define DOOR_SIZE                   (SEGMENT_SIZE * 0.5F)
#define PC_START_COINS              123
#define MINIMAP_PC_SIZE             8.0F
#define KEYS_NUM                    8U
#define BOXES_NUM                   0x200U
#define BOX_SIZE                    32.0F
#define BOX_SLOTS                   8
#define BLOCK_COST                  16
#define PC_BLOCK_FATIG_BLOCK_TIME   50
#define DEFAULT_FLY_VELOCITY        0.5F
#define QUICK_SCROLLS               9U
#define SCROLLS_NUM                 8U
#define SCROLL_SIZE                 32.0F
#define BEING_TYPES_NUM             2U
#define MAX_BEING_EFFECTS           8
#define MAX_PC_EFFECTS              8
#define MAX_GAME_EFFECTS            8
#define SCROLL_TX_SIZE              128.0F

#define KEY_MOVE_FORWARD            SDL_SCANCODE_W
#define KEY_MOVE_BACK               SDL_SCANCODE_S
#define KEY_MOVE_RIGHT              SDL_SCANCODE_D
#define KEY_MOVE_LEFT               SDL_SCANCODE_A
#define KEY_DODGE                   SDL_SCANCODE_SPACE
#define KEY_RUN                     SDL_SCANCODE_LALT
#define KEY_ACTION                  SDL_SCANCODE_E
#define KEY_SWITCH_RANGE            SDL_SCANCODE_Q
#define KEY_TMP                     SDL_SCANCODE_H
#define BUTTON_ATTACK               SDL_BUTTON_LEFT
#define BUTTON_BLOCK                SDL_BUTTON_RIGHT

#define SCR_COSTS                   {\
                                        8U,\
                                        2U,\
                                        8U,\
                                        8U,\
                                        8U,\
                                        8U,\
                                        8U,\
                                        0U\
                                    }
#define SCR_EFFECTS                 {\
                                        effect0,\
                                        effect1,\
                                        effect2,\
                                        effect3,\
                                        effect4,\
                                        effect5,\
                                        effect6,\
                                        EffectEmpty\
                                    }
#define BEING_TYPES                 {\
                                        {\
                                            32,\
                                            PLAYER_VELOCITY * 2.5F,\
                                            1000,\
                                            {0, 1.0F, 1.0F, 1.5F},\
                                            {20, 0.0F, 0, 0.0F},\
                                            UpdateBeing0\
                                        },\
                                        {\
                                            48,\
                                            PLAYER_VELOCITY * 1.5F,\
                                            2000,\
                                            {10, 0.75F, 0.25F, 0.5F},\
                                            {20, 0.5F, 10, 0.0F},\
                                            UpdateBeing1\
                                        }\
                                    }
#define PROJECTILES_UPDATE_FUNC     {\
                                        UpdatePCProjectile,\
                                        UpdateHostileProjectile,\
                                        UpdateSpecialProjectile\
                                    }
#define BEING_WEAPON_BASE_PLCMNT    {{BLADE_BASE_X, BLADE_BASE_Y}, BLADE_BASE_DIRECTION_BEING}
#define PC_BLADE_BASE_PLCMNT        {{BLADE_BASE_X, BLADE_BASE_Y}, BLADE_BASE_DIRECTION_PC}
#define PC_BLADE_IMPACT             {500, 0.0F, 0, 0.0F}
#define PC_ARMOUR                   {0, 0.875F, 0.0F, 0.25F}
#define BEINGS_TEXTURES             {\
                                        tx_being,\
                                        tx_being1\
                                    }
#define BEINGS_WEAPON_TEXTURES      {\
                                        tx_being_blade,\
                                        tx_weapon\
                                    }
#define PROJECTILE_TEXTURES         {\
                                        tx_projectile,\
                                        tx_h_projectile,\
                                        tx_h_projectile\
                                    }
#define PC_BLADE_FRAMES0	        {\
                                        {{16.0F, -8.0F}, SDL_PI_F * 0.55F},\
                                        {{6.0F, 8.0F}, SDL_PI_F * 0.25F},\
                                        {{-4.0F, 10.0F}, 0.0F},\
                                        {{-4.0F, 10.0F}, SDL_PI_F * -0.416F},\
                                        {{-14.0F, 0.0F}, SDL_PI_F * -0.55F}\
                                    }
#define PC_BLADE_FRAMES1	        {\
                                        {{-14.0F, 0.0F}, SDL_PI_F * -0.65F},\
                                        {{-4.0F, 10.0F}, SDL_PI_F * -0.38F},\
                                        {{6.0F, 8.0F}, SDL_PI_F * -0.138F},\
                                        {{16.0F, -8.0F}, SDL_PI_F * 0.45F},\
                                        {{16.0F, -8.0F}, SDL_PI_F * 0.55F}\
                                    }
#define PC_BLADE_FRAMES2	        {\
                                        {{16.0F, -8.0F}, SDL_PI_F * 0.45F},\
                                        {{16.0F, 0.0}, 0.0F},\
                                        {{-1.0F, 24.0F}, 0.0F},\
                                        {{0.0F, 25.0F}, 0.0F}\
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
                                        "img13.bmp"\
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
#define WORLD_BASE              {\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0,\
                                    0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0,\
                                    0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,\
                                    1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1,\
                                    0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0,\
                                    0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0,\
                                    0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\
                                }
#define WORLD_SMALL_BASE        {\
                                    0, 0, 1, 1, 1, 0, 0,\
                                    0, 1, 1, 1, 1, 1, 0,\
                                    1, 1, 1, 1, 1, 1, 1,\
                                    1, 1, 1, 2, 1, 1, 1,\
                                    1, 1, 1, 1, 1, 1, 1,\
                                    0, 1, 1, 1, 1, 1, 0,\
                                    0, 0, 1, 1, 1, 0, 0,\
                                }
#define BEING_BLD_SHIFT_PREPATE {{(20.0F - 16.0F) / BEING_ATTACK_STEPS, (-16.0F - -8.0F) / BEING_ATTACK_STEPS}, (0.5F - 0.0F) / BEING_ATTACK_STEPS}
#define BEING_BLD_SHIFT_ATTACK  {{(0.0F - 20.0F) / BEING_ATTACK_STEPS, (24.0F - -16.0F) / BEING_ATTACK_STEPS}, (0.0F - 0.5F) / BEING_ATTACK_STEPS}
#define BEING_BLD_SHIFT_RESET   {{(BLADE_BASE_X - 0.0F) / BEING_ATTACK_STEPS, (BLADE_BASE_Y - 24.0F) / BEING_ATTACK_STEPS}, (BLADE_BASE_DIRECTION_BEING - 0.0F) / BEING_ATTACK_STEPS}

#endif