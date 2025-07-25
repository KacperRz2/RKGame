#ifndef MACROS_H_
#define MACROS_H_

#define SQRT2DIV2			        (1.4142135623730950488016887242097F * 0.5F)
#define WINDOW_W			        (rend_data->window_w)
#define WINDOW_H			        (rend_data->window_h)
#define VIEWFINDER			        (rend_data->viewfinder)
#define VIEWFINDER_CENTER	        (VIEWFINDER * 0.5F)
#define WINDOW_CENTER_Y		        (WINDOW_H / 2)
#define WORLD_W				        0x2p+16F
#define WORLD_H				        WORLD_W
#define PLAYER_REND_Y_SHIFT	        (VIEWFINDER * 0.5F * 0.875F)
#define PLAYER_SIZE			        32//48
#define GUN_SIGHT_SIZE		        67
#define BULLET_SIZE			        16
#define DECELERATION		        0.93F//0.995F
#define TICK_TIME			        0x200000ULL
#define TICK_TIME_MS		        (TICK_TIME / 1000000ULL)
#define ACCELERATION		        6.25e-2F
#define PLAYER_VELOCITY		        0x0.Ap+0F
#define RUN_VELOCITY		        (PLAYER_VELOCITY * 2.0F)
#define ROTATION_SPEED		        3.90625e-3F
#define TEXTURE_FILES_NUM	        10
#define TEXTURE_TARGET_NUM	        1
#define TEXTURES_NUM		        (TEXTURE_FILES_NUM + TEXTURE_TARGET_NUM)
#define SIGHT_SQUARED   	        (VIEWFINDER_CENTER * VIEWFINDER_CENTER + (VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT) * (VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT))
#define SIGHT_BACK_SQUARED	        (VIEWFINDER_CENTER * VIEWFINDER_CENTER + (VIEWFINDER_CENTER - PLAYER_REND_Y_SHIFT) * (VIEWFINDER_CENTER - PLAYER_REND_Y_SHIFT))
#define FRAME_TIME			        0x400000ULL
#define FRAME_TIME_MS		        	(FRAME_TIME / 1000000ULL)
#define SEGMENTS_X			        0x400U
#define SEGMENTS_Y			        SEGMENTS_X
#define WORLD_SIZE			        WORLD_W
#define SEGMENTS_SIZE		        (WORLD_SIZE / (float)SEGMENTS_X)
#define MAX_PROJECTILES_NUM	        0x800U
#define MAX_SEGM_BEINGS		        16
#define MAX_BEINGS_NUM		        0x8000U//0x10000U//(SEGMENTS_X * SEGMENTS_Y * MAX_SEGM_BEINGS / 32)
#define ANGLE_PARTS		            512
#define MIN_ANGLE                   (2.0F * SDL_PI_F / (float)ANGLE_PARTS)
#define BLADE_DAMAGE                50
#define BLADE_PENETRATION           25U
#define BLADE_SIZE                  64.0F
#define RANGE_SEGMENTS              (SEGMENTS_X / 4U)
#define PROJECTILE_RAN_SEG          (SEGMENTS_X / 64U)
#define MAX_HITS                    25U
#define RAD_TO_MINE                 ((float)ANGLE_PARTS * 0.5F / SDL_PI_F)
#define RANGE                       700.0F
#define BEING_ATTACK_STEPS          64
#define pow2(x)                     ((x) * (x))
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
#define PROJECTILE_VELOCITY         3.0F
#define BEING_HALT_DISTANCE         70.0F
#define CHECK_COLLISION_DISTANCE    768.0F
#define UPDATE_BEINGS_INTERVAL      1U
#define WINDOW_START_W              1600
#define WINDOW_START_H              900
#define BLADE_BASE_DIRECTION_PC     (SDL_PI_F * 0.45F)
#define PC_HP                       100
#define PC_FATIGUE                  1000
#define PC_MAGIC                    100
#define PC_ARMOUR                   0.875F
#define MOVING_BACK_VELO_MODI       0.93F
#define PC_DODGE_FATIG              100
#define PC_DODGE_FATIG_BLOCK_TIME   50
#define PC_DODGE_VELOCITY           10.0F
#define PC_FAILURE_FATIG_BLOCK_TIME 2
#define PC_FAILURE_VELOCITY         0.5F
#define PC_FATIGUE_GAIN_INTERVAL    4U
#define TEST_DAMAGE                 10
#define TEST_PENETRATION            3U
#define PC_BLADE_CHECKPOINTS        2U
#define PC_BLADE_CHARGE_BASE        0xF.Fp-4F
#define PC_BLADE_CHARGE_MODIFIER    0xF.F8p-4F
#define PC_BLADE_DAMAGE_BASE        120.0F
#define PC_BLADE_MAX_IDLE_TICKS     192U
#define PC_BLADE_FIRST_MOVE_STEPS   128
#define PC_BLADE_RETURN_STEPS       384
#define PC_BLADE_TO_NEXT_STEPS      192
#define PC_BLADE_STRIKE_STEPS       32
#define PC_BLADE_AFTER_BLOCK_STEPS  512
#define PC_BLADE_BOUNCE_ANGLE       0.375F
#define VIEWFINDER_SIZE             (WINDOW_H * 0.984375F)
#define BLADE_HANDLER_POSITION      0.85F

// #define SQRT2				1.4142135623730950488016887242097F
// #define WINDOW_CENTER_X		(WINDOW_W / 2)
// #define WORLD_TEXTURE_SCALE
// #define SIGHT_ANGLE
// #define SIGHT_BACK_ANGLE
// #define BODY_SIZE
#endif