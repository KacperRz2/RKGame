#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Projectile.h>
#include <Player.h>
#include <function.h>
#include <World.h>
#include <Being.h>
#include <enum.h>

void CreatePlayer(Player* const p, const float x, const float y){
	p->blade.position.x = BLADE_BASE_X;
 	p->blade.position.y = BLADE_BASE_Y;
	p->blade.direction = BLADE_BASE_DIRECTION_PC;
	p->blade.damage = BLADE_DAMAGE;
	p->blade.penetration = BLADE_PENETRATION;
	p->blade.hits = 0;
	SetPlayerPosition(p, x, y);
	p->control_flags = 0x00000000U;
	p->direction = 0.0F;
	p->velocity = 0.0F;
	p->max_velocity = PLAYER_VELOCITY;
	p->hit_points = PC_HP;
	p->fatigue_points = (int)(PC_FATIGUE * 0.9F);
	p->magic_points = PC_MAGIC;
	p->max_fatigue = PC_FATIGUE;
	p->max_h_p = PC_HP;
	p->fatigue_block_time = 0;
	p->armour = PC_ARMOUR;
	p->coins = PC_START_COINS;
}

extern inline void SetPlayerPosition(Player* const p, const float x, const float y){
	p->position.x = x;
	p->position.y = y;
}

extern inline void MovePlayer(Game_data* const g_d, const float x, const float y){
	float new_x = g_d->pc.position.x + x;
	float new_y = g_d->pc.position.y + y;

	if(GetSegment(&g_d->world, new_x, new_y) != NULL){
		SetPlayerPosition(&g_d->pc, new_x, new_y);
	}else if(GetSegment(&g_d->world, g_d->pc.position.x, new_y) != NULL){
		g_d->pc.position.y = new_y;
	}else if(GetSegment(&g_d->world, new_x, g_d->pc.position.y) != NULL){
		g_d->pc.position.x = new_x;
	}
}

static void UpdatePlayerMove(Game_data* const g_d){
	static const Uint32 tmp = forward | back | right | left;
	static float move_direction = 0.0F;
	if(g_d->pc.velocity > g_d->pc.max_velocity){
		g_d->pc.velocity *= DECELERATION;
	}else if((g_d->pc.control_flags & tmp) == forward || (g_d->pc.control_flags & tmp) == (tmp & ~(back))){
		g_d->pc.velocity += ACCELERATION;
		move_direction = g_d->pc.direction;
	}else if((g_d->pc.control_flags & tmp) == back){
		g_d->pc.velocity = (g_d->pc.velocity + ACCELERATION) * MOVING_BACK_VELO_MODI;
		move_direction = g_d->pc.direction + SDL_PI_F;
	}else if((g_d->pc.control_flags & tmp) == right){
		g_d->pc.velocity += ACCELERATION;
		move_direction = g_d->pc.direction + SDL_PI_F * 0.5F;
	}else if((g_d->pc.control_flags & tmp) == left){
		g_d->pc.velocity += ACCELERATION;
		move_direction = g_d->pc.direction + SDL_PI_F * 1.5F;
	}else if((g_d->pc.control_flags & tmp) == (forward | right)){
		g_d->pc.velocity += ACCELERATION;
		move_direction = g_d->pc.direction + SDL_PI_F * 0.25F;
	}else if((g_d->pc.control_flags & tmp) == (forward | left)){
		g_d->pc.velocity += ACCELERATION;
		move_direction = g_d->pc.direction + SDL_PI_F * 1.75F;
	}else if((g_d->pc.control_flags & tmp) == (back | right)){
		g_d->pc.velocity = (g_d->pc.velocity + ACCELERATION) * MOVING_BACK_VELO_MODI;
		move_direction = g_d->pc.direction + SDL_PI_F * 0.75F;
	}else if((g_d->pc.control_flags & tmp) == (back | left)){
		g_d->pc.velocity = (g_d->pc.velocity + ACCELERATION) * MOVING_BACK_VELO_MODI;
		move_direction = g_d->pc.direction + SDL_PI_F * 1.25F;
	}else{
		g_d->pc.velocity *= DECELERATION;
		if(g_d->pc.velocity < 0.05F) g_d->pc.velocity = 0.0F;
	}
	if(!(g_d->pc.control_flags & block)){
		if(g_d->pc.control_flags & dodge && !(g_d->pc.control_flags & forward)){
			if(g_d->pc.fatigue_points >= PC_DODGE_FATIG && g_d->pc.fatigue_block_time < 1){
				g_d->pc.fatigue_points -= PC_DODGE_FATIG;
				g_d->pc.fatigue_block_time = PC_DODGE_FATIG_BLOCK_TIME;
				if(!(g_d->pc.control_flags & tmp)){
					move_direction = g_d->pc.direction + SDL_PI_F;
				}
				g_d->pc.velocity = PC_DODGE_VELOCITY;
			}else{
				g_d->pc.fatigue_block_time += PC_FAILURE_FATIG_BLOCK_TIME;
				if(!(g_d->pc.control_flags & tmp)){
					move_direction = g_d->pc.direction + SDL_PI_F;
				}
				g_d->pc.velocity = PC_FAILURE_VELOCITY;
			}
			g_d->pc.control_flags &= ~(dodge);
		}
		if((g_d->pc.control_flags & (run | forward | back)) == (run | forward)){
			if(g_d->pc.fatigue_points <= 1){
				g_d->pc.max_velocity = PLAYER_VELOCITY;
				g_d->pc.control_flags &= ~(run);
			}else{
				g_d->pc.max_velocity = RUN_VELOCITY;
				--(g_d->pc.fatigue_points);
			}
		}else{
			g_d->pc.max_velocity = PLAYER_VELOCITY;
		}
	}else{
		g_d->pc.max_velocity = half(PLAYER_VELOCITY);
	}
	if(g_d->pc.velocity > 0.0F){
		MovePlayer(g_d, SDL_sinf(move_direction) * g_d->pc.velocity, -SDL_cosf(move_direction) * g_d->pc.velocity);
	}
}

static void UpdatePlayerDirection(Player* const p){
	if(p->direction > 2.0F * SDL_PI_F){
		p->direction -= 2.0F * SDL_PI_F;
	}else if(p->direction < 0.0F){
		p->direction += 2.0F * SDL_PI_F;
	}
}

static void UpdatePlayerPoints(Player* const p){
	static unsigned int count = PC_FATIGUE_GAIN_INTERVAL;
	if(count <= 1U){
		if(p->fatigue_block_time > 0){
			--(p->fatigue_block_time);
		}else if(p->fatigue_points < p->max_fatigue){
			++(p->fatigue_points);
		}
		if(p->control_flags & block){
			count = PC_FATIGUE_GAIN_INTERVAL * 2;
		}else{
			count = PC_FATIGUE_GAIN_INTERVAL;
		}
	}
	--count;
}

void UpdatePlayer(Game_data* const g_d){
	UpdatePlayerDirection(&g_d->pc);
	UpdatePlayerMove(g_d);
	if(g_d->pc.control_flags & range_mode){
		UpdatePlayerFire(g_d);
	}else{
		UpdatePlayerBlade(g_d);
	}
	UpdatePlayerPoints(&g_d->pc);
}

static inline void ShiftBlade(Blade* const bl, Status_frame* const shift){
	bl->position.x += shift->position.x;
	bl->position.y += shift->position.y;
	bl->direction += shift->direction;
}

static inline void SetBladePosition(Blade* const bl, const Status_frame* const position){
	bl->position.x = position->position.x;
	bl->position.y = position->position.y;
	bl->direction = position->direction;
}

static inline void SetShiftToBase(Blade* const bl, Status_frame* const step_shift, const unsigned int steps){
	step_shift->position.x = (BLADE_BASE_X - bl->position.x) / steps;
	step_shift->position.y = (BLADE_BASE_Y - bl->position.y) / steps;
	step_shift->direction = (BLADE_BASE_DIRECTION_PC - bl->direction) / steps;
}

static inline void SetShiftToPosition(Blade* const bl, Status_frame* const step_shift, const Status_frame* const frame, const unsigned int steps){
	step_shift->position.x = (frame->position.x - bl->position.x) / steps;
	step_shift->position.y = (frame->position.y - bl->position.y) / steps;
	step_shift->direction = (frame->direction - bl->direction) / steps;
}

static inline Status_frame GetBladeLocation(Player* const p, float* const sine, float* const cosine){
	float direct = p->direction + p->blade.direction;
	*sine = SineSafe(direct);
	*cosine = CosiSafe(direct);
	Status_frame ret = {{p->position.x + *sine * p->blade.position.x, p->position.y - *cosine * p->blade.position.y}, direct};
	return ret;
}

static inline bool BladeHitsBeing(Blade* const bl, Status_frame* const location, Being* const b, SDL_FPoint* const dangerous_points){
	for(unsigned int i = 0U; i < PC_BLADE_CHECKPOINTS; ++i){
		if(SDL_fabsf((dangerous_points + i)->x - b->position.x) < half(b->type->size) && SDL_fabsf((dangerous_points + i)->y - b->position.y) < half(b->type->size)){
			for(unsigned int j = bl->hits; j > 0U; --j){
				if(*(bl->hit_targets + (j - 1U)) == b->id){
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

static bool UnleashDestruction(Game_data* const g_d){
	static const float length_part = BLADE_SIZE * 0.85F / (float)PC_BLADE_CHECKPOINTS;
	float sine_blade_direction;
	float cosine_blade_direction;
	Status_frame blade_true_location = GetBladeLocation(&g_d->pc, &sine_blade_direction, &cosine_blade_direction);
	Segment* s = GetSegment(&g_d->world, blade_true_location.position.x, blade_true_location.position.y);
	if(s == NULL){
		return true;
	}
	float shift_x = sine_blade_direction * length_part;
	float shift_y = -cosine_blade_direction * length_part;
	SDL_FPoint dangerous_points[PC_BLADE_CHECKPOINTS] = {
		{blade_true_location.position.x + 2.0F * shift_x, blade_true_location.position.y + 2.0F * shift_y},
		{blade_true_location.position.x + shift_x, blade_true_location.position.y + shift_y}
	};
	Blade* const bl = &g_d->pc.blade;
	for(unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c){
	for(unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r){
		Segment* neighbour = GetSegmentByIndx(&g_d->world, c, r);
		if(neighbour == NULL) continue;
		for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
			Being* b = *(neighbour->beings.array + i);
			if(!BladeHitsBeing(bl, &blade_true_location, b, dangerous_points)) continue;
			if(DamageBeing(b, bl->damage)){
				if(bl->hits < --bl->penetration){
					--i;
					continue;
				}
			}else if(bl->hits < bl->penetration){
				*(bl->hit_targets + bl->hits++) = b->id;
				continue;
			}
			return true;
		}
	}}
	return false;
}

static void UpdatePlayerBlade(Game_data* const g_d){
	static const Status_frame blade_key_frames_0[] = {
		{{16.0F, -8.0F}, SDL_PI_F * 0.55F},
		{{6.0F, 8.0F}, SDL_PI_F * 0.25F},
		{{-4.0F, 10.0F}, 0.0F},
		{{-4.0F, 10.0F}, SDL_PI_F * -0.416F},
		{{-14.0F, 0.0F}, SDL_PI_F * -0.55F}
	};
	static const Status_frame blade_key_frames_1[] = {
		{{-14.0F, 0.0F}, SDL_PI_F * -0.65F},
		{{-4.0F, 10.0F}, SDL_PI_F * -0.38F},
		{{6.0F, 8.0F}, SDL_PI_F * -0.138F},
		{{16.0F, -8.0F}, SDL_PI_F * 0.45F},
		{{16.0F, -8.0F}, SDL_PI_F * 0.55F}
	};
	static const Status_frame blade_key_frames_2[] = {
		{{16.0F, -8.0F}, SDL_PI_F * 0.45F},
		{{16.0F, 0.0}, 0.0F},
		{{-1.0F, 24.0F}, 0.0F},
		{{0.0F, 25.0F}, 0.0F}
	};
	static const Status_frame* blade_moves[] = {blade_key_frames_0, blade_key_frames_1, blade_key_frames_2};
	static const unsigned int sizes[] = {SDL_arraysize(blade_key_frames_0), SDL_arraysize(blade_key_frames_1), SDL_arraysize(blade_key_frames_2)};

	static Status_frame step_shift = {{0.0F, 0.0F}, 0.0};
	static int key = 0;
	static int steps = 0;
	static int step = 0;
	static unsigned int chain = 0U;
	static unsigned int chain_next = 0U;
	static bool abide = false;
	static bool freehand = false;
	static unsigned int idle_ticks = 0U;
	static float charge = PC_BLADE_CHARGE_BASE;
	if(g_d->pc.control_flags & attack){
		charge *= PC_BLADE_CHARGE_MODIFIER;
	}
	if(!abide){
		if(charge != PC_BLADE_CHARGE_BASE){
			if(!(g_d->pc.control_flags & attack)){
				chain = chain_next;
				key = 0;
				step = 0;
				abide = true;
				idle_ticks = 0U;
				g_d->pc.blade.hits = 0U;
				g_d->pc.blade.damage = (int)(PC_BLADE_DAMAGE_BASE * (1.0F - charge));
				g_d->pc.blade.penetration = (unsigned int)((float)BLADE_PENETRATION * (1.0F - charge));
				charge = PC_BLADE_CHARGE_BASE;
				SetShiftToPosition(&g_d->pc.blade, &step_shift, *(blade_moves + chain), steps = PC_BLADE_FIRST_MOVE_STEPS);
				chain_next = (chain_next + 1U) % SDL_arraysize(sizes);
				freehand = false;
				return;
			}
		}else if(++idle_ticks > PC_BLADE_MAX_IDLE_TICKS){
			chain_next = 0U;
			SetShiftToBase(&g_d->pc.blade, &step_shift, steps = PC_BLADE_RETURN_STEPS);
			step = 0;
			freehand = true;
		}
		if(!freehand) return;
	}
	if(step == steps){
		if(freehand){
			freehand = false;
		}else if(key > *(sizes + chain) - 2){
			abide = false;
			freehand = true;
			SetShiftToPosition(&g_d->pc.blade, &step_shift, *(blade_moves + chain_next), steps = PC_BLADE_TO_NEXT_STEPS);
		}else{
			SetShiftToPosition(&g_d->pc.blade, &step_shift, *(blade_moves + chain) + ++key, steps = PC_BLADE_STRIKE_STEPS);
		}
		step = 0;
	}else{
		ShiftBlade(&g_d->pc.blade, &step_shift);
		if(!freehand && key > 1 && (freehand = UnleashDestruction(g_d))){
			abide = false;
			g_d->pc.blade.hits = 0U;
			g_d->pc.blade.direction = chain == 1 ? g_d->pc.blade.direction - PC_BLADE_BOUNCE_ANGLE : g_d->pc.blade.direction + PC_BLADE_BOUNCE_ANGLE;
			SetShiftToPosition(&g_d->pc.blade, &step_shift, *(blade_moves + chain_next), steps = PC_BLADE_AFTER_BLOCK_STEPS);
			step = 0;
		}
	}
	++step;
}

extern inline bool PointInPlayer(const float x, const float y, Player* const pl){
	if(pow2(x - pl->position.x) + pow2(y - pl->position.y) < (float)pow2(half(PLAYER_SIZE))){
		return true;
	}
	return false;
}

extern inline void DamagePlayer(Player* const p, const int damage){
	p->hit_points -= damage;
}

static void UpdatePlayerFire(Game_data* const g_d){
	static int shoot_reload = 0;
	if(g_d->pc.control_flags & attack && shoot_reload <= 0 && g_d->projectiles.num < MAX_PROJECTILES_NUM){
		AddProjectileToArray(&g_d->projectiles, &g_d->pc.position, g_d->pc.direction + 0.25F * (SDL_randf() - 0.5F), PROJECTILE_VELOCITY, TEST_DAMAGE, TEST_PENETRATION);
		shoot_reload = PC_SHOOT_RELOAD;
	}
	--shoot_reload;
}

extern inline void HaltPlayer(Player* const p){
	p->velocity = 0.0F;
}

extern inline void HitBarrier(Player* const p, const int damage){
	p->fatigue_points /= 2;
}