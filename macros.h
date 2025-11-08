#ifndef MACROS_H_
#define MACROS_H_

#define SQRT2DIV2			        (1.4142135623730950488016887242097F * 0.5F)
#define FULL_ANGLE			        (SDL_PI_F * 2.0F)
#define pow2(x)                     ((x) * (x))
#define half(x)                     ((x) * (0.5F))
#define human(game_data_ptr)        (game_data_ptr->champions.array + game_data_ptr->human_indx)
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
#define RUN_MULTIPL		            2.0F
#define BLOCK_VELOCITY_MULTIP		0.5F
#define ROTATION_SPEED		        3.90625e-3F
#define FRAME_TIME			        0x400000ULL
#define FRAME_TIME_MS		        (FRAME_TIME / 1000000ULL)
#define MAX_PROJECTILES_NUM	        0x4000U
#define MAX_SEGM_BEINGS		        0x10U
#define MAX_BEINGS_NUM		        0x4000U
#define MAX_PLAYERS_NUM		        0x4U
#define START_PLAYERS_NUM		    0x1U
#define ANGLE_PARTS		            512
#define MIN_ANGLE                   (2.0F * SDL_PI_F / (float)ANGLE_PARTS)
#define BLADE_SIZE                  64.0F
#define BEING_HIT_CIRCLE_DIAMET     16.0F
#define RANGE_SEGMENTS              ((unsigned int)(7000.0F / SEGMENT_SIZE))
#define PROJECTILE_RAN_SEG          ((unsigned int)(2000.0F / SEGMENT_SIZE))
#define MAX_HITS                    16U
#define BLADE_PENETRATION           MAX_HITS
#define RAD_TO_MINE                 ((float)ANGLE_PARTS * 0.5F / SDL_PI_F)
#define RANGE                       700.0F
#define BEING_ATTACK_STEPS          64
#define TEXTURES_NUM		        31

#define BEING_RELOAD                512
#define BEING_DEFAULT_LEFT_TICKS    32
#define NAP_TICKS                   512
#define BLADE_BASE_X                16.0F
#define BLADE_BASE_Y                (-8.0F)
#define BLADE_BASE_DIRECTION_BEING  0.0F
#define BEING_WALK_TICKS            128
#define IDLE_BEING_ACTION_DISTANCE  900.0F
#define BEING_ATTACK_DISTANCE       140.0F
#define BEING_SHOOT_DISTANCE        700.0F
#define BEING_FOCUS_DISTANCE        256.0F
#define BEING_RELOAD_TICKS          128
#define BEING_STUN_DURATION         256
#define PROJECTILE_VELOCITY         4.0F
#define BEING_HALT_DISTANCE         70.0F
#define BEING_MIN_DISTANCE          64.0F
#define CHECK_COLLISION_DISTANCE    768.0F
#define UPDATE_BEINGS_INTERVAL      1U
#define BLADE_BASE_DIRECTION_PC     (SDL_PI_F * 0.45F)
#define PC_HP                       3000
#define PC_FATIGUE                  1000
#define PC_MAGIC                    0x10
#define PC_M_MAGIC                  0x10000000
#define MOVING_BACK_VELO_MODI       0.93F
#define PC_DODGE_FATIG              100
#define PC_DODGE_FATIG_BLOCK_TIME   50
#define PC_DODGE_VELOCITY           10.0F
#define PC_DODGE_RELOAD             128
#define PC_PUSH_FATIG               100
#define PC_PUSH_FATIG_BLOCK_TIME    50
#define PC_PUSH_RELOAD              256
#define PC_PUSH_REACH               (PLAYER_SIZE * 3.0F)
#define PC_CAST_RELOAD              256
#define PC_FAILURE_FATIG_BLOCK_TIME 2
#define PC_FAILURE_VELOCITY         0.5F
#define PC_FATIGUE_GAIN_INTERVAL    3
#define PC_ARMOUR_REGEN_BLOCK       0x2000
#define PC_ARMOUR_GAIN_INTERVAL     0x100
#define TEST_DAMAGE                 100
#define TEST_PENETRATION            3U
#define PC_BLADE_CHECKPOINTS        2U
#define PC_BLADE_CHARGE_BASE        0xF.Fp-4F
#define PC_BLADE_CHARGE_MODIFIER    0xF.F8p-4F
#define PC_BLADE_MAX_IDLE_TICKS     192U
#define PC_BLADE_FIRST_MOVE_STEPS   128
#define PC_BLADE_RETURN_STEPS       384
#define PC_BLADE_TO_NEXT_STEPS      192
#define PC_BLADE_STRIKE_STEPS       32
#define PC_BLADE_AFTER_BLOCK_STEPS  512
#define PC_BLADE_BOUNCE_ANGLE       0.375F
#define BLADE_HANDLER_POSITION      0.85F
#define PC_SHOOT_RELOAD             64
#define MAX_START_BEINGS_NUM        (MAX_BEINGS_NUM / 0x40U)
#define TEST_BEING_VELOCITY         (PLAYER_VELOCITY * 2.5F)
#define TEST_BEING_DMG_CLOSE        2
#define TEST_BEING_DMG_FAR          1
#define DOOR_SIZE                   (SEGMENT_SIZE * 0.5F)
#define PC_START_COINS              123
#define KEYS_NUM                    7U
#define BOXES_NUM                   0x200U
#define BOX_SIZE                    32.0F
#define BOX_SLOTS                   8
#define BLOCK_COST                  16
#define PC_BLOCK_FATIG_BLOCK_TIME   50
#define DEFAULT_FLY_VELOCITY        0.5F
#define QUICK_SCROLLS               9U
#define SCROLLS_NUM                 35U
#define SCROLL_SIZE                 32.0F
#define BEING_TYPES_NUM             2U
#define MAX_BEING_EFFECTS           8
#define MAX_PC_EFFECTS              8
#define MAX_GAME_EFFECTS            8
#define BARRIER_SIZE                ((float)PLAYER_SIZE * 2.0F)
#define WEAPON_ATTACK_Y             24.0F
#define MAX_PC_ARMOUR_ABSORP        200.0F
#define ICONS_IN_VIEWF_ROW          7
#define BOX_MAX_COINS               256U
#define BOX_MAX_MP                  32U
#define MAX_KEYS                    16U
#define MAX_MAPS                    32U
#define SMALL_PLAN_SIZE             (BIG_SEGMENTS_X / 4)
#define HORDE_ATTACK_START_TICKS    0x1000
#define HORDE_ATTACK_POINTS         8
#define BIG_SEGMENT_SIZE            (SEGMENT_SIZE * BIG_SEGMENT_SEGMENTS_X)
#define HUGE_SEGMENTS_X             7
#define HUGE_SEGMENT_SIZE           ((WORLD_SIZE - SEGMENT_SIZE * 2.0F) / (float)HUGE_SEGMENTS_X)
#define MAX_POPULATION_NUM          0x20

#define KEY_MOVE_FORWARD            SDL_SCANCODE_W
#define KEY_MOVE_BACK               SDL_SCANCODE_S
#define KEY_MOVE_RIGHT              SDL_SCANCODE_D
#define KEY_MOVE_LEFT               SDL_SCANCODE_A
#define KEY_DODGE                   SDL_SCANCODE_SPACE
#define KEY_RUN                     SDL_SCANCODE_LALT
#define KEY_ACTION                  SDL_SCANCODE_E
#define KEY_SWITCH_RANGE            SDL_SCANCODE_Q
#define KEY_MANAGE_SCROLLS          SDL_SCANCODE_I
#define KEY_TMP                     SDL_SCANCODE_PERIOD
#define KEY_TMP1                    SDL_SCANCODE_H
#define KEY_SELECT                  SDL_SCANCODE_E
#define KEY_FULLSCR                 SDL_SCANCODE_F11
#define KEY_WINDOWED                SDL_SCANCODE_F12
#define BUTTON_ATTACK               SDL_BUTTON_LEFT
#define BUTTON_BLOCK                SDL_BUTTON_RIGHT

#define SCR_COSTS                   {\
                                        1U,\
                                        1U,\
                                        1U,\
                                        1U,\
                                        1U,\
                                        1U,\
                                        1U,1U,1U,1U,1U,1U,1U,1U,1U,1U,\
                                        1U,1U,1U,1U,1U,1U,1U,1U,1U,1U,\
                                        1U,1U,1U,1U,1U,1U,1U,1U,\
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
                                        EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,\
                                        EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,\
                                        EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,EffectEmpty,\
                                        EffectEmpty\
                                    }
#define GAME_LASTING_EFFECTS        {\
                                        HordeAttack\
                                    }
#define BEING_TYPES                 {\
                                        {\
                                            32,\
                                            PLAYER_VELOCITY * 2.5F,\
                                            1000,\
                                            {0.0F, 1.0F, 1.0F, 1.5F},\
                                            {20.0F, 0.0F, 0.0F, 0.5F},\
                                            UpdateBeing0\
                                        },\
                                        {\
                                            48,\
                                            PLAYER_VELOCITY * 1.5F,\
                                            2000,\
                                            {10.0F, 0.75F, 0.25F, 0.5F},\
                                            {20.0F, 0.5F, 10.0F, 0.25F},\
                                            UpdateBeing1\
                                        },\
                                        {\
                                            32,\
                                            PLAYER_VELOCITY * 2.5F,\
                                            1000,\
                                            {0.0F, 1.0F, 1.0F, 1.5F},\
                                            {128.0F, 0.125F, 16.0F, 0.5F},\
                                            UpdateAlly0\
                                        }\
                                    }
#define PC_BLADE_IMPACT             {1200.0F, 1.0F, 0.0F, 3.0F}//dmg, penetr, magic, stun
#define PC_RANGE_IMPACT             {10.0F, 0.5F, 350.0F, 1.0F}
#define PC_BLADE_PENETRATIONS       {0.0F, 0.0F, 0.5F}
#define PC_ARMOUR                   {125.0F, 0.875F, 1.0F, 0.25F}
#define PC_MAX_ARMOUR               {500.0F, 0.5F, 0.5F, 0.25F}
#define PROJECTILES_UPDATE_FUNC     {\
                                        UpdatePCProjectile,\
                                        UpdateHostileProjectile,\
                                        UpdateSpecialProjectile\
                                    }
#define BEING_WEAPON_BASE_PLCMNT    {{BLADE_BASE_X, BLADE_BASE_Y}, BLADE_BASE_DIRECTION_BEING}
#define BEING_WEAPON_PREPARE_PLCMNT {{20.0F, -16.0F}, 0.5F}
#define BEING_WEAPON_ATTACK_PLCMNT  {{0.0F, 24.0F}, 0.0F}
#define PC_BLADE_BASE_PLCMNT        {{BLADE_BASE_X, BLADE_BASE_Y}, BLADE_BASE_DIRECTION_PC}
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
                                    1, 1, 1, 1, 1, 1, 1,\
                                    1, 1, 1, 1, 1, 1, 1,\
                                    0, 1, 1, 1, 1, 1, 0,\
                                    0, 0, 1, 1, 1, 0, 0,\
                                }
#define ZERO_POINT_F            ((SDL_FPoint){0.0F, 0.0F})

#endif