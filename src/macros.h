#ifndef MACROS_H_
#define MACROS_H_

#define SQRT2DIV2			        (1.4142135623730950488016887242097F * 0.5F)
#define SQRT2			            1.4142135623730950488016887242097F
#define FULL_ANGLE			        (SDL_PI_F * 2.0F)
#define pow2(x)                     ((x) * (x))
#define half(x)                     ((x) * (0.5F))
#define human(game_data_ptr)        (game_data_ptr->champions.array + game_data_ptr->human_indx)
#define BIG_SEGMENTS_X              29U
#define BIG_SEGMENT_SEGMENTS_X      10U
#define SEGMENTS_X			        (BIG_SEGMENT_SEGMENTS_X * BIG_SEGMENTS_X)
#define SEGMENTS_Y			        SEGMENTS_X
#define SEGMENT_SIZE		        128.0F
#define WORLD_W				        (SEGMENT_SIZE * SEGMENTS_X)
#define WORLD_H				        WORLD_W
#define WORLD_SIZE			        WORLD_W
#define PLAYER_SIZE			        32
#define GUN_SIGHT_SIZE		        16
#define BULLET_SIZE			        16
#define BOOM_SIZE			        320
#define BOOM_EFFECTS_NUM		    64U
#define V_EFFECT_MAX_DELAY          16
#define WEAPON_SIZE                 64.0F
#define BLADE_LEN                   (WEAPON_SIZE * BLADE_HANDLER_POSITION)
#define DECELERATION		        0.93F
#define MIN_PC_VELOCITY		        0.05F
#define TICK_TIME			        0x200000ULL
#define TICK_TIME_MS		        (TICK_TIME / 1000000ULL)
#define ACCELERATION		        6.25e-2F
#define PLAYER_VELOCITY		        0x0.Ap+0F
#define RUN_MULTIPL		            2.5F
#define BLOCK_VELOCITY_MULTIP		0.5F
#define DODGE_VELOCITY_MULTIP		(PC_DODGE_VELOCITY / PLAYER_VELOCITY)
#define ROTATION_SPEED		        0xA.0p-11F//0x8.0p-11F
#define DIRECTION_SHIFT_ADDITION	0.125F
#define FRAME_TIME			        0x400000ULL
#define FRAME_TIME_MS		        (FRAME_TIME / 1000000ULL)
#define MAX_PROJECTILES_NUM	        0x1000U
#define MAX_SEGM_BEINGS		        0x10U
#define MAX_BEINGS_NUM		        0x3000U
#define MAX_START_BEINGS_NUM        0x1000U
#define MAX_VISUAL_EFFECTS_NUM		0x800U
#define MAX_PLAYERS_NUM		        0x1U
#define START_PLAYERS_NUM		    0x1U
#define ANGLE_PARTS		            512
#define MIN_ANGLE                   (2.0F * SDL_PI_F / (float)ANGLE_PARTS)
#define BEING_HIT_CIRCLE_DIAMET     24.0F
#define MAX_HITS                    12U
#define BLADE_PENETRATION           MAX_HITS
#define RAD_TO_MINE                 ((float)ANGLE_PARTS * 0.5F / SDL_PI_F)
#define RANGE                       700.0F
#define BEING_ATTACK_STEPS          64
#define BEING_STRIKE_STEPS          (BEING_ATTACK_STEPS * 3)

#define BEING_RELOAD                512
#define BEING_DEFAULT_LEFT_TICKS    32
#define NAP_TICKS                   128
#define SHOCK_TICKS                 0x180
#define BLADE_BASE_X                16.0F
#define BLADE_BASE_Y                (-8.0F)
#define BLADE_BASE_DIRECTION_BEING  0.0F
#define BEING_WALK_TICKS            128
#define BEING_FLEE_WALK_TICKS       BEING_WALK_TICKS
#define FLEE_COLLI_WALK_TICKS       64
#define IDLE_BEING_ACTION_DISTANCE  900.0F
#define BEING_ATTACK_DISTANCE       140.0F
#define BEING_SHOOT_DISTANCE        700.0F
#define BEING_FOCUS_DISTANCE        256.0F
#define BEING_RELOAD_TICKS          128
#define BEING_STUN_AFTER_FLY_DURAT  512
#define PROJECTILE_VELOCITY         4.0F
#define PC_SHOOT_MAX_SPREAD         0.125F
#define FIRE_PROJECTILE_VELOCITY    2.0F
#define BEING_HALT_DISTANCE         (BEING_MIN_DISTANCE + 2.0F)
#define BEING_MIN_DISTANCE          ((float)MAX_BEING_SIZE)
#define CHECK_COLLISION_DISTANCE    768.0F
#define RARE_UPDATE_INTERVAL        1U
#define BLADE_BASE_DIRECTION_PC     (SDL_PI_F * 0.45F)
#define PC_HP                       0x100
#define PC_FATIGUE                  0x200
#define PC_MAGIC                    20
// #define PC_M_MAGIC                  0x10000000
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
#define BLOCKING_PC_FA_GAIN_INTERV  (PC_FATIGUE_GAIN_INTERVAL * 2)
#define PC_ARMOUR_REGEN_BLOCK       0x2000
#define PC_ARMOUR_GAIN_INTERVAL     0x100
#define PC_ARMOUR_GAIN              0.01F
#define TEST_DAMAGE                 100
#define TEST_PENETRATION            1U
#define PC_BLADE_CHECKPOINTS        2U
#define PC_BLADE_CHARGE_BASE        0xF.Fp-4F
#define PC_BLADE_CHARGE_MODIFIER    0xF.F8p-4F
#define PC_BLADE_MAX_IDLE_TICKS     192U
#define PC_BLD_LAST_HARMLESS_PHASE  1
#define PC_BLADE_FIRST_MOVE_STEPS   128
#define PC_BLADE_RETURN_STEPS       384
#define PC_BLADE_TO_NEXT_STEPS      192
#define PC_BLADE_STRIKE_STEPS       32
#define PC_BLADE_AFTER_BLOCK_STEPS  512
#define PC_BLADE_BOUNCE_ANGLE       0.375F
#define BLADE_HANDLER_POSITION      0.85F
#define PC_SHOOT_RELOAD             192
#define DOOR_SIZE                   (SEGMENT_SIZE * 0.5F)
#define SHOP_SIZE                   128.0F
#define SCROLL_SIZE                 32.0F
#define BARRIER_SIZE                ((float)PLAYER_SIZE * 2.0F)
#define BOX_SIZE                    32.0F
#define PC_START_COINS              3
#define MIN_KEYS                    7U
#define ONE_KEY_MAPS_NUM            2U
#define BOX_SCROLL_CHANCE_FACTOR    3
#define BOXES_NUM                   0x180U
#define BOX_SLOTS                   8
#define BLOCK_COST                  32
#define PC_BLOCK_FATIG_BLOCK_TIME   64
#define PC_CAST_FATIG_BLOCK_TIME    64
#define BASE_FLY_VELOCITY           0x3.0p-4F
#define BASE_FLY_TICKS              10
#define QUICK_SCROLLS               9U
#define SCROLLS_NUM                 (scroll_empty + 1U)
#define OPTIONS_NUM                 5U
#define MAX_BEING_EFFECTS           8
#define MAX_PC_EFFECTS              8
#define MAX_GAME_EFFECTS            8
#define WEAPON_ATTACK_Y             24.0F
#define MAX_PC_ARMOUR_ABSORP        200.0F
#define ICONS_IN_VIEWF_ROW          7
#define ICONS_IN_VIEWF_COL          (SCROLLS_NUM / ICONS_IN_VIEWF_ROW)
#define BOX_MAX_COINS               256U
#define BOX_MAX_MP                  16U
#define MAX_KEYS                    16U
#define SMALL_PLAN_SIZE             (BIG_SEGMENTS_X / 4)
#define BIG_SEGMENT_SIZE            (SEGMENT_SIZE * BIG_SEGMENT_SEGMENTS_X)
#define HUGE_SEGMENT_SIZE           ((WORLD_SIZE - SEGMENT_SIZE * 2.0F) / (float)HUGE_SEGMENTS_X)
#define HORDE_ATTACK_START_TICKS    0x800
#define MIN_TICKS_FROM_HORDE        (HORDE_ATTACK_START_TICKS * 2)
#define MAX_TICKS_FROM_HORDE        (HORDE_ATTACK_START_TICKS * 32)
#define HORDE_ATTACK_POINTS         8
#define GET_FROM_VOID_CHANCE_FACTOR 0x10
#define HUGE_SEGMENTS_X             7
#define HUGE_SEGMENT_BIG_SEGMENTS_X 3U
#define COMMANDER_EFFECT_TICKS      1024
#define HP_REGEN_TICKS              1024
#define HP_REGEN_TIMES              128
#define DEFAULT_VIS_EFF_INTERV      64
#define FP_REGEN_TICKS              0x1000
#define WEAK_EFFECT_TICKS           0x1000
#define SLOW_EFFECT_TICKS           0x800
#define BURN_TICKS                  0x1000
#define BOLT_CHAIN_TICKS            0x100U
#define OPENING_PORTAL_TICKS        128
#define ATTENTION_RECT_SIZE         (SEGMENT_SIZE * 32.0F)
#define SPIRAL_STEPS(range)         (pow2(range * 2 + 1))
#define ALLY_ATTENTION_RANGE        3U
#define BEING_ALLY_DETEC_RANGE      1U
#define UNLIMITET_SPEC_PROJE        0U
#define WARLOCK_SPEC_PROJE_TICKS    0x1000U
#define SHOP_SCROLLS_NUM            18U
#define SHOPS_NUM                   7U
#define SHOP_ENTRY_RANGE            64.0F
#define MAX_ITEMS_TO_SELL           9U
#define SHOP_COLS                   20U
#define SHOP_ROWS                   10U
#define SHOP_COLS                   20U
#define SHOP_SIDE_COLS              7U
#define SHOP_SCROLLS_COLS           3U
#define FIRST_SHOP_ROW              2U
#define SHOP_POSITIONS              (SHOP_ROWS * SHOP_COLS)
#define MAX_MORALE                  0x4000
#define MIN_MORALE                  (-0x1000)
#define NEW_SPIRIT_MORALE           0x1000
#define SELL_DIVIDER                10
#define DODGE_EFFECT_TICKS          64
#define MAX_NOT_NULL_SEGS           (11U * 37U * pow2(BIG_SEGMENT_SEGMENTS_X))
#define PC_PUSH_POWER               8.0F
#define SCROLL_PUSH_POWER           12.0F
#define HORDE_BEING_CHANCE_FACTOR   32
#define MAX_ONE_TYPE_ITEMS          255U
#define ALLY_LIFETIME               0x2000
#define ALLY_LIFETIME_MAX_SHIFT     0x100
#define BEING_CHARGE_VELOCITY_MULT  0x1.8p+0F
#define BIG_SEG_PLAN_NULL_SEG       (-1)
#define NOT_FOUND                   (-1)

#define KEY_MOVE_FORWARD            SDL_SCANCODE_W
#define KEY_MOVE_BACK               SDL_SCANCODE_S
#define KEY_MOVE_RIGHT              SDL_SCANCODE_D
#define KEY_MOVE_LEFT               SDL_SCANCODE_A
#define KEY_DODGE                   SDL_SCANCODE_SPACE
#define KEY_RUN                     SDL_SCANCODE_LALT
#define KEY_ACTION                  SDL_SCANCODE_E
#define KEY_SWITCH_RANGE            SDL_SCANCODE_Q
#define KEY_MANAGE_SCROLLS          SDL_SCANCODE_I
#define KEY_SHOW_MAP                SDL_SCANCODE_M
#define KEY_SELECT                  SDL_SCANCODE_E
#define BUTTON_ATTACK               SDL_BUTTON_LEFT
#define BUTTON_BLOCK                SDL_BUTTON_RIGHT

#define UNCOVERED_SEG_RGB           255U, 255U, 255U
#define POPULATED_SEG_RGB           127U, 127U, 127U
#define SPELL0_RGB                  127U, 127U, 63U
#define SPELL1_RGB                  127U, 15U, 127U
#define SCR_COSTS                   {\
                                        5U,\
                                        5U,\
                                        7U,\
                                        5U,\
                                        7U,\
                                        12U,\
                                        10U,\
                                        15U,\
                                        5U,\
                                        0U\
                                    }
#define ITEMS_PRICES                {\
                                        100,\
                                        100,\
                                        140,\
                                        100,\
                                        140,\
                                        240,\
                                        200,\
                                        300,\
                                        100,\
                                        0,\
                                        20,\
                                        200,\
                                        9999,\
                                        500,\
                                        1000,\
                                        7000\
                                    }
#define SCR_EFFECTS                 {\
                                        push,\
                                        HPRegen,\
                                        AddAlly,\
                                        RenewArmour,\
                                        slow,\
                                        fire,\
                                        thunderbolt,\
                                        convert,\
                                        FatigueRegen,\
                                        EffectEmpty\
                                    }
#define GAME_LASTING_EFFECTS        {\
                                        HordeAttack\
                                    }
#define BEING_LASTING_EFFECTS       {\
                                        SlowBeing,\
                                        CommanderAura,\
                                        CommanderIsNear,\
                                        OpeningPortal,\
                                        Burn,\
                                        ThunderboltChain,\
                                        AllyLifetime\
                                    }
#define BEING_LASTING_EFFECTS_ENDS  {\
                                        SlowBeingEnd,\
                                        BeingLastingEffectVoidEnd,\
                                        CommanderIsNearEnd,\
                                        BeingLastingEffectVoidEnd,\
                                        BeingLastingEffectVoidEnd,\
                                        BeingLastingEffectVoidEnd,\
                                        BeingLastingEffectVoidEnd\
                                    }
#define PLAYER_LASTING_EFFECTS      {\
                                        SlowPlayer,\
                                        PlayerHPRegeneration,\
                                        PlayerFatigueRegeneration,\
                                        PlayerWeakness,\
                                        PlayerDodge\
                                    }
#define MAX_BEING_SIZE              48
#define BEING_TYPES                 {\
                                        {\
                                            32,\
                                            PLAYER_VELOCITY * 2.5F,\
                                            100,\
                                            {0.0F, 1.0F, 1.0F, 1.125F},\
                                            {10.0F, 0.0F, 0.0F, 0.25F},\
                                            UpdateBeingOrdinary\
                                        },\
                                        {\
                                            34,\
                                            PLAYER_VELOCITY * 2.0F,\
                                            2000,\
                                            {15.0F, 0.875F, 1.0F, 1.0F},\
                                            {20.0F, 0.125F, 0.0F, 0.5F},\
                                            UpdateBeingOrdinary\
                                        },\
                                        {\
                                            36,\
                                            PLAYER_VELOCITY * 1.5F,\
                                            1000,\
                                            {10.0F, 0.875F, 1.0F, 1.0F},\
                                            {20.0F, 0.125F, 1.0F, 0.25F},\
                                            UpdateBeingRanger\
                                        },\
                                        {\
                                            40,\
                                            PLAYER_VELOCITY * 1.875F,\
                                            2000,\
                                            {20.0F, 0.25F, 0.875F, 0.875F},\
                                            {30.0F, 0.5F, 5.0F, 1.0F},\
                                            UpdateBeingOrdinary\
                                        },\
                                        {\
                                            MAX_BEING_SIZE,\
                                            PLAYER_VELOCITY * 1.375F,\
                                            2000,\
                                            {15.0F, 0.5F, 0.5F, 0.875F},\
                                            {20.0F, 0.5F, 10.0F, 0.25F},\
                                            UpdateBeingRanger\
                                        },\
                                        {\
                                            32,\
                                            PLAYER_VELOCITY * 2.0F,\
                                            4000,\
                                            {25.0F, 0.125F, 0.875F, 0.75F},\
                                            {1.0F, 0.875F, 0.0F, 0.75F},\
                                            UpdateBeingCommander\
                                        },\
                                        {\
                                            40,\
                                            PLAYER_VELOCITY * 1.5F,\
                                            2000,\
                                            {10.0F, 0.875F, 0.125F, 0.875F},\
                                            {5.0F, 0.875F, 20.0F, 0.25F},\
                                            UpdateBeingWarlock\
                                        },\
                                        {\
                                            34,\
                                            PLAYER_VELOCITY * 2.25F,\
                                            1000,\
                                            {0.0F, 1.0F, 1.0F, 1.125F},\
                                            {128.0F, 0.125F, 16.0F, 0.5F},\
                                            UpdateAlly0\
                                        }\
                                    }
#define PC_BLADE_DMG                3200.0F
#define PC_BLADE_MAGIC              0.0F
#define PC_BLADE_PENETR             1.0F
#define PC_RANGE_DMG                10.0F
#define PC_RANGE_MAGIC              350.0F
#define PC_RANGE_PENETR             0.5F
#define PC_BLADE_IMPACT             {PC_BLADE_DMG, PC_BLADE_PENETR, PC_BLADE_MAGIC, PC_PUSH_POWER}//dmg, penetr, magic, stun
#define PC_RANGE_IMPACT             {PC_RANGE_DMG, PC_RANGE_PENETR, PC_RANGE_MAGIC, 1.0F}
#define PC_FIRE_PROJECTILE_IMPACT   {16.0F, PC_RANGE_PENETR, 700.0F, 1.0F}
#define PC_BLADE_PENETRATIONS       {0.0F, 0.0F, 0.5F}
#define PC_ARMOUR                   {500.0F, 0.5F, 0.5F, 0.5F}//absorption, multipl, magic_multipl, unstability
#define PC_MAX_ARMOUR               PC_ARMOUR
#define PC_MAX_ARMOUR_II_ABS        750.0F
#define PC_MAX_ARMOUR_II            {PC_MAX_ARMOUR_II_ABS, 0.25F, 0.25F, 0.25F}
#define BURN_EFFECT_INTERVAL        4
#define BURN_EFFECT_IMPACT          (Impact){1.0F, 1.0F, 2.0F, 1.0F}
#define PROJECTILES_UPDATE_FUNC     {\
                                        UpdatePCProjectile,\
                                        UpdateHostileProjectile,\
                                        UpdateSpecialProjectile\
                                    }
#define SPEC_PROJECTILES_FUNC       {\
                                        WarlockProjectile,\
                                        FireProjectile\
                                    }
#define BEING_WEAPON_BASE_PLCMNT    {{BLADE_BASE_X, BLADE_BASE_Y}, BLADE_BASE_DIRECTION_BEING}
#define BEING_WEAPON_PREPARE_PLCMNT {{20.0F, -16.0F}, 0.5F}
#define BEING_WEAPON_ATTACK_PLCMNT  {{0.0F, 24.0F}, 0.0F}
#define PC_BLADE_BASE_PLCMNT        {{BLADE_BASE_X, BLADE_BASE_Y}, BLADE_BASE_DIRECTION_PC}
#define PC_BLADE_FRAMES0	        {\
                                        {{16.0F, -8.0F}, SDL_PI_F * 0.55F},\
                                        {{20.0F, 12.0F}, SDL_PI_F * 0.25F},\
                                        {{-4.0F, 24.0F}, 0.0F},\
                                        {{-16.0F, 20.0F}, SDL_PI_F * -0.416F},\
                                        {{-14.0F, 0.0F}, SDL_PI_F * -0.55F}\
                                    }
#define PC_BLADE_FRAMES1	        {\
                                        {{-14.0F, 0.0F}, SDL_PI_F * -0.65F},\
                                        {{-16.0F, 26.0F}, SDL_PI_F * -0.38F},\
                                        {{12.0F, 24.0F}, SDL_PI_F * -0.138F},\
                                        {{20.0F, -8.0F}, SDL_PI_F * 0.45F},\
                                        {{16.0F, -8.0F}, SDL_PI_F * 0.55F}\
                                    }
#define PC_BLADE_FRAMES2	        {\
                                        {{16.0F, -16.0F}, SDL_PI_F * 0x1.0p-3F},\
                                        {{16.0F, 0.0}, 0.0F},\
                                        {{-1.0F, 28.0F}, 0.0F},\
                                        {{0.0F, 32.0F}, 0.0F}\
                                    }
#define BEINGS_INCIDENCES           {\
                                        400,\
                                        400+80,\
                                        400+80+50,\
                                        400+80+50+40,\
                                        400+80+50+40+20,\
                                        400+80+50+40+20+3,\
                                        400+80+50+40+20+3+4\
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
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\
                                }
#define WORLD_SMALL_BASE        {\
                                    0, 0, 1, 1, 1, 0, 0,\
                                    0, 1, 1, 1, 1, 1, 0,\
                                    1, 1, 1, 1, 1, 1, 1,\
                                    1, 1, 1, 1, 1, 1, 1,\
                                    1, 1, 1, 1, 1, 1, 1,\
                                    0, 1, 1, 1, 1, 1, 0,\
                                    0, 0, 1, 1, 1, 0, 0\
                                }
#define NOT_NULL_HUGE_SEGS_NUM  37
#define SHOPS_HUGE_SEGS        {\
                                    2, 1,\
                                    4, 1,\
                                    1, 3,\
                                    3, 3,\
                                    5, 3,\
                                    2, 5,\
                                    4, 5\
                                }
#define HOSTILES_MORALE_COSTS   {\
                                    0x20,\
                                    0x400,\
                                    0x400,\
                                    0x800,\
                                    0x800,\
                                    0x2000,\
                                    0x1000,\
                                    0x0\
                                }
#define ZERO_POINT              ((SDL_Point){0, 0})
#define ZERO_POINT_F            ((SDL_FPoint){0.0F, 0.0F})
#define SPIRAL_X_SHIFTS         {0,0,1,1,1,0,-1,-1,-1,-1,0,1,2,2,2,2,2,1,0,-1,-2,-2,-2,-2,-2,-2,-1,0,1,2,3,3,3,3,3,3,3,2,1,0,-1,-2,-3,-3,-3,-3,-3,-3,-3,-3,-2,-1,0,1,2,3,4,4,4,4,4,4,4,4,4,3,2,1,0,-1,-2,-3,-4,-4,-4,-4,-4,-4,-4,-4,-4}
#define SPIRAL_Y_SHIFTS         {0,-1,-1,0,1,1,1,0,-1,-2,-2,-2,-2,-1,0,1,2,2,2,2,2,1,0,-1,-2,-3,-3,-3,-3,-3,-3,-2,-1,0,1,2,3,3,3,3,3,3,3,2,1,0,-1,-2,-3,-4,-4,-4,-4,-4,-4,-4,-4,-3,-2,-1,0,1,2,3,4,4,4,4,4,4,4,4,4,3,2,1,0,-1,-2,-3,-4}

#define SAVE_LEN(nums)          sizeof(struct nums)\
		                        + sizeof(unsigned int)\
		                        + sizeof(Player) * nums.champions\
		                        + sizeof(Being) * nums.beings\
		                        + sizeof(Projectile) * nums.projectiles\
		                        + sizeof(Uint64) * (BIG_SEGMENTS_X + 1U)\
		                        + sizeof(Key_location) * nums.needed_keys\
		                        + sizeof(Shop) * SHOPS_NUM\
		                        + sizeof(Box) * nums.boxes\
		                        + sizeof(Uint8) * (nums.needed_keys + 3U)\
		                        + sizeof(int)\
		                        + sizeof(Lasting_effect) * nums.effects\
		                        + sizeof(union horde_data)\
		                        + sizeof(struct coordinates) * nums.known_segs

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
