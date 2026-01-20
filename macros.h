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
#define BLADE_SIZE                  64.0F
#define DECELERATION		        0.93F
#define TICK_TIME			        0x200000ULL
#define TICK_TIME_MS		        (TICK_TIME / 1000000ULL)
#define ACCELERATION		        6.25e-2F
#define PLAYER_VELOCITY		        0x0.Ap+0F
#define RUN_MULTIPL		            2.5F
#define BLOCK_VELOCITY_MULTIP		0.5F
#define DODGE_VELOCITY_MULTIP		(PC_DODGE_VELOCITY / PLAYER_VELOCITY)
#define ROTATION_SPEED		        0xA.0p-11F//0x8.0p-11F
#define FRAME_TIME			        0x400000ULL
#define FRAME_TIME_MS		        (FRAME_TIME / 1000000ULL)
#define MAX_PROJECTILES_NUM	        0x1000U
#define MAX_SEGM_BEINGS		        0x10U
#define MAX_BEINGS_NUM		        0x3000U
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
#define PC_ARMOUR_REGEN_BLOCK       0x2000
#define PC_ARMOUR_GAIN_INTERVAL     0x100
#define TEST_DAMAGE                 100
#define TEST_PENETRATION            1U
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
#define PC_SHOOT_RELOAD             192
#define MAX_START_BEINGS_NUM        (MAX_BEINGS_NUM / 0x3U)
#define DOOR_SIZE                   (SEGMENT_SIZE * 0.5F)
#define SHOP_SIZE                   128.0F
#define SCROLL_SIZE                 32.0F
#define BARRIER_SIZE                ((float)PLAYER_SIZE * 2.0F)
#define BOX_SIZE                    32.0F
#define PC_START_COINS              3
#define KEYS_NUM                    7U
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
#define HORDE_ATTACK_POINTS         8
#define HUGE_SEGMENTS_X             7
#define COMMANDER_EFFECT_TICKS      1024
#define HP_REGEN_TICKS              1024
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
#define BEING_CHARGE_VELOCITY_MULT  0x1.8p+0F

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
#define KEY_TMP                     SDL_SCANCODE_PERIOD
#define KEY_TMP1                    SDL_SCANCODE_H
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
#define ZERO_POINT_F            ((SDL_FPoint){0.0F, 0.0F})
#define SPIRAL_X_SHIFTS         {0,0,1,1,1,0,-1,-1,-1,-1,0,1,2,2,2,2,2,1,0,-1,-2,-2,-2,-2,-2,-2,-1,0,1,2,3,3,3,3,3,3,3,2,1,0,-1,-2,-3,-3,-3,-3,-3,-3,-3,-3,-2,-1,0,1,2,3,4,4,4,4,4,4,4,4,4,3,2,1,0,-1,-2,-3,-4,-4,-4,-4,-4,-4,-4,-4,-4}
#define SPIRAL_Y_SHIFTS         {0,-1,-1,0,1,1,1,0,-1,-2,-2,-2,-2,-1,0,1,2,2,2,2,2,1,0,-1,-2,-3,-3,-3,-3,-3,-3,-2,-1,0,1,2,3,3,3,3,3,3,3,2,1,0,-1,-2,-3,-4,-4,-4,-4,-4,-4,-4,-4,-3,-2,-1,0,1,2,3,4,4,4,4,4,4,4,4,4,3,2,1,0,-1,-2,-3,-4}

#endif