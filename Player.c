#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Projectile.h>
#include <Player.h>
#include <function.h>
#include <World.h>
#include <Being.h>
#include <enum.h>
#include <Scroll.h>

static void InitScrolls(Player* const p){
	*p->scrolls = 99U;
	*(p->scrolls + 1) = 99U;
	for(unsigned int i = 2U; i < (unsigned int)scroll_empty; ++i){
		*(p->scrolls + i) = 0U;
	}
	*(p->scrolls + scroll_empty) = 0U;
	*(p->scrolls_quick_access) = scroll_empty;
	*(p->scrolls_quick_access + 1) = scroll_0;
	*(p->scrolls_quick_access + 2) = scroll_1;
	for(unsigned int i = 3U; i < QUICK_SCROLLS; ++i){
		*(p->scrolls_quick_access + i) = scroll_empty;
	}
}

void CreatePlayer(Player* const p, const float x, const float y){
	p->blade.placement = (Placement)PC_BLADE_BASE_PLCMNT;
	p->blade.impact = (Impact)PC_BLADE_IMPACT;
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
	p->armour = (Armour)PC_ARMOUR;
	p->coins = PC_START_COINS;
	p->selected_scroll = scroll_empty;
	InitScrolls(p);
    p->effects_num = 0U;
}

extern inline void SetPlayerPosition(Player* const p, const float x, const float y){
	p->position = (SDL_FPoint){x, y};
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
	UpdatePlayerCast(g_d);
	UpdatePlayerPush(g_d);
	UpdatePlayerFire(g_d);
	UpdatePlayerBlade(g_d);
	UpdatePlayerPoints(&g_d->pc);
}

static inline void ShiftBlade(Blade* const bl, Placement* const shift){
	bl->placement.position.x += shift->position.x;
	bl->placement.position.y += shift->position.y;
	bl->placement.direction += shift->direction;
}

static inline void SetBladePosition(Blade* const bl, const Placement* const position){
	bl->placement = *position;
}

static inline void SetShiftToBase(Blade* const bl, Placement* const step_shift, const unsigned int steps){
	*step_shift = (Placement){
		{(BLADE_BASE_X - bl->placement.position.x) / steps, (BLADE_BASE_Y - bl->placement.position.y) / steps},
		(BLADE_BASE_DIRECTION_PC - bl->placement.direction) / steps
	};
}

static inline void SetBladePositionToBase(Blade* const bl){
	bl->placement = (Placement)PC_BLADE_BASE_PLCMNT;
}

static inline void SetShiftToPosition(Blade* const bl, Placement* const step_shift, const Placement* const frame, const unsigned int steps){
	*step_shift = (Placement){
		{(frame->position.x - bl->placement.position.x) / steps, (frame->position.y - bl->placement.position.y) / steps},
		(frame->direction - bl->placement.direction) / steps
	};
}

static inline Placement GetBladeLocation(Player* const p, float* const sine, float* const cosine){
	float direct = p->direction + p->blade.placement.direction;
	*sine = SineSafe(direct);
	*cosine = CosiSafe(direct);
	return (Placement){
		{p->position.x + *sine * p->blade.placement.position.x, p->position.y - *cosine * p->blade.placement.position.y},
		direct
	};
}

static inline bool BladeHitsBeing(Blade* const bl, Placement* const location, Being* const b, SDL_FPoint* const dangerous_points){
	for(unsigned int i = 0U; i < PC_BLADE_CHECKPOINTS; ++i){
		if(SDL_fabsf((dangerous_points + i)->x - b->position.x) < half(BeingSize(b)) && SDL_fabsf((dangerous_points + i)->y - b->position.y) < half(BeingSize(b))){
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
	Placement blade_true_location = GetBladeLocation(&g_d->pc, &sine_blade_direction, &cosine_blade_direction);
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
			if(DamageBeing(b, bl->impact.damage)){
				if(bl->hits < --bl->penetration){
					--i;
					continue;
				}
			}else{
				float angle = GetDirectionToPush(&g_d->pc.position, &b->position);
				CatapultBeing(b, SineSafe(angle) * bl->impact.damage / 256, -CosiSafe(angle) * bl->impact.damage / 256, BEING_DEFAULT_LEFT_TICKS * 2);
				if(bl->hits < bl->penetration){
					*(bl->hit_targets + bl->hits++) = b->id;
					continue;
				}
			}
			return true;
		}
	}}
	return false;
}

static void UpdatePlayerBlade(Game_data* const g_d){
	static const Placement blade_key_frames_0[] = PC_BLADE_FRAMES0;
	static const Placement blade_key_frames_1[] = PC_BLADE_FRAMES1;
	static const Placement blade_key_frames_2[] = PC_BLADE_FRAMES2;
	static const Placement* blade_moves[] = {blade_key_frames_0, blade_key_frames_1, blade_key_frames_2};
	static const unsigned int sizes[] = {SDL_arraysize(blade_key_frames_0), SDL_arraysize(blade_key_frames_1), SDL_arraysize(blade_key_frames_2)};

	static Placement step_shift = {{0.0F, 0.0F}, 0.0};
	static int key = 0;
	static int steps = 0;
	static int step = 0;
	static unsigned int chain = 0U;
	static unsigned int chain_next = 0U;
	static bool abide = false;
	static bool freehand = false;
	static unsigned int idle_ticks = 0U;
	static float charge = PC_BLADE_CHARGE_BASE;
	if(g_d->pc.control_flags & range_mode){
		if(step <= steps){
			SetBladePositionToBase(&g_d->pc.blade);
			abide = false;
			freehand = false;
			charge = PC_BLADE_CHARGE_BASE;
			idle_ticks = PC_BLADE_MAX_IDLE_TICKS;
			step = steps + 1;
			chain_next = 0U;
		}
		return;
	}
	if(g_d->pc.control_flags & block){
		abide = false;
		charge = PC_BLADE_CHARGE_BASE;
		idle_ticks = PC_BLADE_MAX_IDLE_TICKS;
	}else if(g_d->pc.control_flags & attack){
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
				g_d->pc.blade.impact.damage = (int)(PC_BLADE_DAMAGE_BASE * (1.0F - charge));
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
			g_d->pc.blade.placement.direction = chain == 1 ? g_d->pc.blade.placement.direction - PC_BLADE_BOUNCE_ANGLE : g_d->pc.blade.placement.direction + PC_BLADE_BOUNCE_ANGLE;
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
	if(shoot_reload > 0){
		--shoot_reload;
	}else if(g_d->pc.selected_scroll == scroll_empty && (g_d->pc.control_flags & (range_mode | attack | block)) == (range_mode | attack) && g_d->projectiles.num < MAX_PROJECTILES_NUM){
		AddPCProjectileToArray(&g_d->projectiles, &g_d->pc.position, g_d->pc.direction + 0.25F * (SDL_randf() - 0.5F), PROJECTILE_VELOCITY, TEST_DAMAGE, TEST_PENETRATION);
		shoot_reload = PC_SHOOT_RELOAD;
	}
}

static void UpdatePlayerPush(Game_data* const g_d){
	static int push_reload = 0;
	if(push_reload > 0){
		--push_reload;
	}else if((g_d->pc.control_flags & (attack | block)) == (attack | block) && g_d->pc.fatigue_points >= PC_PUSH_FATIG){
		g_d->pc.fatigue_points -= PC_PUSH_FATIG;
		g_d->pc.fatigue_block_time += PC_PUSH_FATIG_BLOCK_TIME;
		push_reload = PC_PUSH_RELOAD;
		Segment* s = GetSegment(&g_d->world, g_d->pc.position.x, g_d->pc.position.y);
		for(unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c){
		for(unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r){
			Segment* neighbour = GetSegmentByIndx(&g_d->world, c, r);
			if(neighbour == NULL) continue;
			for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
				Being* b = *(neighbour->beings.array + i);
				if(pow2(g_d->pc.position.x - b->position.x) + pow2(g_d->pc.position.y - b->position.y) < pow2(PC_PUSH_REACH)){
					float angle = GetDirectionToPush(&g_d->pc.position, &b->position);
					if(angle < 0.0F){
						angle += FULL_ANGLE;
					}
					float difference = SDL_fabsf(g_d->pc.direction - angle);
					if(difference <= SDL_PI_F * 0.5F || difference >= SDL_PI_F * 1.5F){
						CatapultBeing(b, sine(angle) * DEFAULT_FLY_VELOCITY, -cosi(angle) * DEFAULT_FLY_VELOCITY, BEING_DEFAULT_LEFT_TICKS * 4);
					}
				}
			}
		}}
	}
}

extern inline void HaltPlayer(Player* const p){
	p->velocity = 0.0F;
}

extern inline void HitBarrier(Player* const p, const int damage){
	p->fatigue_points -= damage * BLOCK_COST;
	p->fatigue_block_time += PC_BLOCK_FATIG_BLOCK_TIME;
	if(p->fatigue_points < 0){
		p->control_flags &= ~(block);
		p->fatigue_points = 0;
	}
}

extern inline float GetDirectionToPush(SDL_FPoint* const pushing, SDL_FPoint* const pushed){
    return SDL_atan2f(pushed->y - pushing->y, pushed->x - pushing->x) + SDL_PI_F * 0.5F;
}

static void UpdatePlayerCast(Game_data* const g_d){
	static int cast_reload = 0;
	if(cast_reload > 0){
		--cast_reload;
	}else if(g_d->pc.control_flags & cast){
		g_d->pc.control_flags &= ~(cast);
		const int cost = ScrollCost(g_d->pc.selected_scroll);
		if(g_d->pc.magic_points >= cost){
			g_d->pc.magic_points -= cost;
			--(*(g_d->pc.scrolls + g_d->pc.selected_scroll));
			UseScroll(g_d);
			cast_reload = PC_CAST_RELOAD;
		}
	}
}

extern inline void HealPlayer(Player* const p, const int points){
	p->hit_points += points;
	UpdatePlayerHitPoints(p);
}

static inline void UpdatePlayerHitPoints(Player* const p){
	if(p->hit_points > p->max_h_p){
		p->hit_points = p->max_h_p;
	}
}