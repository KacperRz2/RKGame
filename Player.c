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
#include <game.h>

static void InitScrolls(Player* const p){
	*p->scrolls = 99U;
	*(p->scrolls + 1) = 99U;
	*(p->scrolls + 2) = 99U;
	*(p->scrolls + 3) = 99U;
	for(unsigned int i = 4U; i < (unsigned int)scroll_empty; ++i){
		*(p->scrolls + i) = 0U;
	}
	*(p->scrolls + scroll_empty) = 0U;
	*(p->scrolls_quick_access) = scroll_empty;
	*(p->scrolls_quick_access + 1) = scroll_0;
	*(p->scrolls_quick_access + 2) = scroll_1;
	*(p->scrolls_quick_access + 3) = scroll_2;
	*(p->scrolls_quick_access + 4) = scroll_3;
	for(unsigned int i = 5U; i < QUICK_SCROLLS; ++i){
		*(p->scrolls_quick_access + i) = scroll_empty;
	}
}

void CreatePlayer(Player* const p, const float x, const float y){
	p->blade.placement = (Placement)PC_BLADE_BASE_PLCMNT;
	p->blade.impact = (Impact){};
	p->blade.penetration = BLADE_PENETRATION;
	p->blade.hits = 0;
	p->blade.step_shift = (Placement){{0.0F, 0.0F}, 0.0};
	p->blade.key = 0;
	p->blade.steps = 0;
	p->blade.step = 0;
	p->blade.chain = 0U;
	p->blade.chain_next = 0U;
	p->blade.abide = false;
	p->blade.freehand = false;
	p->blade.idle_ticks = 0U;
	p->blade.charge = PC_BLADE_CHARGE_BASE;
	p->blade_attack = (Impact)PC_BLADE_IMPACT;
	p->range_attack = (Impact)PC_RANGE_IMPACT;
	SetPlayerPosition(p, x, y);
	p->flags = 0x00000000U;
	p->direction = 0.0F;
	p->move_direction = 0.0F;
	p->velocity = 0.0F;
	p->max_velocity = PLAYER_VELOCITY;
	p->hit_points = PC_HP;
	p->fatigue_points = (int)(PC_FATIGUE * 0.9F);
	p->magic_points = PC_MAGIC;
	p->max_fatigue = PC_FATIGUE;
	p->max_h_p = PC_HP;
	p->block_times = (Block_times){0, 0, 0, 0, 0, 0};
	p->armour = (Armour)PC_ARMOUR;
	p->max_armour = (Armour)PC_MAX_ARMOUR;
	p->coins = PC_START_COINS;
	p->selected_scroll = scroll_empty;
	InitScrolls(p);
    p->effects_num = 0U;
}

extern inline void SetPlayerPosition(Player* const p, const float x, const float y){
	p->position = (SDL_FPoint){x, y};
}

extern inline void MovePlayer(Game_data* const g_d, Player* const p, const float x, const float y){
	float new_x = p->position.x + x;
	float new_y = p->position.y + y;
	const Segment* s = GetSegment(&g_d->world, new_x, new_y);
	if(s != NULL){
		SetPlayerPosition(p, new_x, new_y);
	}else if((s = GetSegment(&g_d->world, p->position.x, new_y)) != NULL){
		p->position.y = new_y;
	}else if((s = GetSegment(&g_d->world, new_x, p->position.y)) != NULL){
		p->position.x = new_x;
	}
	if(s != p->segment){
		p->last_seen_in = p->segment;
		p->segment = GetSegment(&g_d->world, p->position.x, p->position.y);
	}
}

static void UpdatePlayerMove(Game_data* const g_d, const unsigned int indx){
	Player* const p = g_d->champions.array + indx;
	static const Uint32 tmp = forward | back | right | left;
	const bool runs = ((p->flags & (run | forward | back)) == (run | forward));
	if(p->velocity > p->max_velocity * RUN_MULTIPL || (!runs && p->velocity > p->max_velocity) || ((p->flags & block) && p->velocity > p->max_velocity * BLOCK_VELOCITY_MULTIP)){
		p->velocity *= DECELERATION;
	}else if((p->flags & tmp) == forward || (p->flags & tmp) == (tmp & ~(back))){
		p->velocity += ACCELERATION;
		p->move_direction = p->direction;
	}else if((p->flags & tmp) == back){
		p->velocity = (p->velocity + ACCELERATION) * MOVING_BACK_VELO_MODI;
		p->move_direction = p->direction + SDL_PI_F;
	}else if((p->flags & tmp) == right){
		p->velocity += ACCELERATION;
		p->move_direction = p->direction + SDL_PI_F * 0.5F;
	}else if((p->flags & tmp) == left){
		p->velocity += ACCELERATION;
		p->move_direction = p->direction + SDL_PI_F * 1.5F;
	}else if((p->flags & tmp) == (forward | right)){
		p->velocity += ACCELERATION;
		p->move_direction = p->direction + SDL_PI_F * 0.25F;
	}else if((p->flags & tmp) == (forward | left)){
		p->velocity += ACCELERATION;
		p->move_direction = p->direction + SDL_PI_F * 1.75F;
	}else if((p->flags & tmp) == (back | right)){
		p->velocity = (p->velocity + ACCELERATION) * MOVING_BACK_VELO_MODI;
		p->move_direction = p->direction + SDL_PI_F * 0.75F;
	}else if((p->flags & tmp) == (back | left)){
		p->velocity = (p->velocity + ACCELERATION) * MOVING_BACK_VELO_MODI;
		p->move_direction = p->direction + SDL_PI_F * 1.25F;
	}else{
		p->velocity *= DECELERATION;
		if(p->velocity < 0.05F) p->velocity = 0.0F;
	}
	if(!(p->flags & block)){
		if(p->flags & dodge && !(p->flags & forward)){
			if(p->fatigue_points >= PC_DODGE_FATIG && p->block_times.dodge < 1){
				p->fatigue_points -= PC_DODGE_FATIG;
				BlockPlayerFatigue(p, PC_DODGE_FATIG_BLOCK_TIME);
				p->block_times.dodge = PC_DODGE_RELOAD;
				if(!(p->flags & tmp)){
					p->move_direction = p->direction + SDL_PI_F;
				}
				p->velocity = PC_DODGE_VELOCITY;
			}else{
				BlockPlayerFatigue(p, PC_FAILURE_FATIG_BLOCK_TIME);
				if(!(p->flags & tmp)){
					p->move_direction = p->direction + SDL_PI_F;
				}
				p->velocity = PC_FAILURE_VELOCITY;
			}
			p->flags &= ~(dodge);
		}
		if(runs){
			if(p->fatigue_points <= 1){
				p->flags &= ~(run);
			}else{
				--(p->fatigue_points);
			}
		}
	}
	if(p->block_times.dodge > 0){
		--p->block_times.dodge;
	}
	if(p->velocity > 0.0F){
		MovePlayer(g_d, p, SDL_sinf(p->move_direction) * p->velocity, -SDL_cosf(p->move_direction) * p->velocity);
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
	if(p->block_times.fatigue < 1){
		if(p->fatigue_points < p->max_fatigue){
			++(p->fatigue_points);
		}
		if(p->flags & block){
			BlockPlayerFatigue(p, PC_FATIGUE_GAIN_INTERVAL * 2);
		}else{
			BlockPlayerFatigue(p, PC_FATIGUE_GAIN_INTERVAL);
		}
	}else{
		--(p->block_times.fatigue);
	}
	if(p->block_times.armour < 1U){
		PlayerGainArmour(p, 0.01F);
		BlockPlayerArmourRegen(p, PC_ARMOUR_GAIN_INTERVAL);
	}else{
		--(p->block_times.armour);
	}
}

static inline void UpdatePlayer(Game_data* const g_d, const unsigned int indx){
	UpdatePlayerDirection(g_d->champions.array + indx);
	UpdatePlayerMove(g_d, indx);
	UpdatePlayerCast(g_d, indx);
	UpdatePlayerPush(g_d, indx);
	UpdatePlayerFire(g_d, indx);
	UpdatePlayerBlade(g_d, indx);
	UpdatePlayerPoints(g_d->champions.array + indx);
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
				if(*(bl->hit_targets + (j - 1U)) == b->main_indx){
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

static bool UnleashDestruction(Game_data* const g_d, const unsigned int indx){
	static const float length_part = BLADE_SIZE * 0.85F / (float)PC_BLADE_CHECKPOINTS;
	float sine_blade_direction;
	float cosine_blade_direction;
	Placement blade_true_location = GetBladeLocation(g_d->champions.array + indx, &sine_blade_direction, &cosine_blade_direction);
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
	Blade* const bl = &(g_d->champions.array + indx)->blade;
	for(unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c){
	for(unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r){
		Segment* neighbour = GetSegmentByIndx(&g_d->world, c, r);
		if(neighbour == NULL) continue;
		for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
			Being* b = (g_d->beings.array + *(neighbour->beings.beings_ind + i));
			if(!BladeHitsBeing(bl, &blade_true_location, b, dangerous_points)) continue;
			if(DamageBeing(b, &bl->impact, g_d->beings.array)){
				if(bl->hits < --bl->penetration){
					--i;
					continue;
				}
			}else{
				const float angle = GetDirectionToPush(&(g_d->champions.array + indx)->position, &b->position);
				const float stun_power = CalculateStunPower(&bl->impact, &b->armour);
				CatapultBeing(b, SineSafe(angle) * stun_power, -CosiSafe(angle) * stun_power, BEING_DEFAULT_LEFT_TICKS * 2);
				if(bl->hits < bl->penetration){
					*(bl->hit_targets + bl->hits++) = b->main_indx;
					continue;
				}
			}
			return true;
		}
	}}
	return false;
}

static void UpdatePlayerBlade(Game_data* const g_d, const unsigned int indx){
	static const Placement blade_key_frames_0[] = PC_BLADE_FRAMES0;
	static const Placement blade_key_frames_1[] = PC_BLADE_FRAMES1;
	static const Placement blade_key_frames_2[] = PC_BLADE_FRAMES2;
	static const Placement* blade_moves[] = {blade_key_frames_0, blade_key_frames_1, blade_key_frames_2};
	static const unsigned int sizes[] = {SDL_arraysize(blade_key_frames_0), SDL_arraysize(blade_key_frames_1), SDL_arraysize(blade_key_frames_2)};
	static const float extra_penetration[] = PC_BLADE_PENETRATIONS;
	Blade* const bl = &(g_d->champions.array + indx)->blade;
	if((g_d->champions.array + indx)->flags & range_mode){
		if(bl->step <= bl->steps){
			SetBladePositionToBase(bl);
			bl->abide = false;
			bl->freehand = false;
			bl->charge = PC_BLADE_CHARGE_BASE;
			bl->idle_ticks = PC_BLADE_MAX_IDLE_TICKS;
			bl->step = bl->steps + 1;
			bl->chain_next = 0U;
		}
		return;
	}
	if((g_d->champions.array + indx)->flags & block){
		bl->abide = false;
		bl->charge = PC_BLADE_CHARGE_BASE;
		bl->idle_ticks = PC_BLADE_MAX_IDLE_TICKS;
	}else if((g_d->champions.array + indx)->flags & attack){
		bl->charge *= PC_BLADE_CHARGE_MODIFIER;
	} 
	if(!bl->abide){
		if(bl->charge != PC_BLADE_CHARGE_BASE){
			if(!((g_d->champions.array + indx)->flags & attack)){
				bl->chain = bl->chain_next;
				bl->key = 0;
				bl->step = 0;
				bl->abide = true;
				bl->idle_ticks = 0U;
				bl->hits = 0U;
				const float multip = 1.0F - bl->charge;
				bl->impact = (Impact){
					(g_d->champions.array + indx)->blade_attack.damage * multip,
					(g_d->champions.array + indx)->blade_attack.armour_reduction * multip + *(extra_penetration + bl->chain),
					(g_d->champions.array + indx)->blade_attack.magic * multip,
					(g_d->champions.array + indx)->blade_attack.stun * multip
				};
				bl->penetration = (unsigned int)((float)BLADE_PENETRATION * multip);
				bl->charge = PC_BLADE_CHARGE_BASE;
				SetShiftToPosition(bl, &bl->step_shift, *(blade_moves + bl->chain), bl->steps = PC_BLADE_FIRST_MOVE_STEPS);
				bl->chain_next = (bl->chain_next + 1U) % SDL_arraysize(sizes);
				bl->freehand = false;
				return;
			}
		}else if(++bl->idle_ticks > PC_BLADE_MAX_IDLE_TICKS){
			bl->chain_next = 0U;
			SetShiftToBase(bl, &bl->step_shift, bl->steps = PC_BLADE_RETURN_STEPS);
			bl->step = 0;
			bl->freehand = true;
		}
		if(!bl->freehand) return;
	}
	if(bl->step == bl->steps){
		if(bl->freehand){
			bl->freehand = false;
		}else if(bl->key > *(sizes + bl->chain) - 2){
			bl->abide = false;
			bl->freehand = true;
			SetShiftToPosition(bl, &bl->step_shift, *(blade_moves + bl->chain_next), bl->steps = PC_BLADE_TO_NEXT_STEPS);
		}else{
			SetShiftToPosition(bl, &bl->step_shift, *(blade_moves + bl->chain) + ++bl->key, bl->steps = PC_BLADE_STRIKE_STEPS);
		}
		bl->step = 0;
	}else{
		ShiftBlade(bl, &bl->step_shift);
		if(!bl->freehand && bl->key > 1 && (bl->freehand = UnleashDestruction(g_d, indx))){
			bl->abide = false;
			bl->hits = 0U;
			bl->placement.direction = bl->chain == 1 ? bl->placement.direction - PC_BLADE_BOUNCE_ANGLE : bl->placement.direction + PC_BLADE_BOUNCE_ANGLE;
			SetShiftToPosition(bl, &bl->step_shift, *(blade_moves + bl->chain_next), bl->steps = PC_BLADE_AFTER_BLOCK_STEPS);
			bl->step = 0;
		}
	}
	++bl->step;
}

extern inline bool PointInPlayer(const float x, const float y, Player* const pl){
	if(pow2(x - pl->position.x) + pow2(y - pl->position.y) < (float)pow2(half(PLAYER_SIZE))){
		return true;
	}
	return false;
}

extern inline bool CircleMeetsPlayer(const float x, const float y, const float diameter, Player* const pl){
	if(pow2(x - pl->position.x) + pow2(y - pl->position.y) < pow2(half((float)PLAYER_SIZE + diameter))){
		return true;
	}
	return false;
}

extern inline void DamagePlayer(Player* const p, const Impact* impact){
	p->hit_points -= CalculateDamage(impact, &p->armour);
	if(impact->damage > 0.0F){
		if(p->armour.absorption > 0.0F){
			p->armour.absorption -= impact->damage;
			if(p->armour.absorption < 0.0F){
				p->armour.absorption = 0.0F;
			}
		}
		if(p->armour.multipl < 1.0F){
			p->armour.multipl += (1.0F - p->max_armour.multipl) * (impact->damage / p->max_armour.absorption);
			if(p->armour.multipl > 1.0F){
				p->armour.multipl = 1.0F;
			}
		}
	}
	if(impact->magic > 0.0F){
		if(p->armour.magic_multipl < 1.0F){
			p->armour.magic_multipl += (1.0F - p->max_armour.magic_multipl) * 0.01F;
			if(p->armour.magic_multipl > 1.0F){
				p->armour.magic_multipl = 1.0F;
			}
		}
	}
	BlockPlayerArmourRegen(p, PC_ARMOUR_REGEN_BLOCK);
}

static void UpdatePlayerFire(Game_data* const g_d, const unsigned int indx){
	if((g_d->champions.array + indx)->block_times.shoot > 0){
		--(g_d->champions.array + indx)->block_times.shoot;
	}else if((g_d->champions.array + indx)->selected_scroll == scroll_empty && ((g_d->champions.array + indx)->flags & (range_mode | attack | block)) == (range_mode | attack) && g_d->projectiles.num < MAX_PROJECTILES_NUM){
		float x, y;
		GetShiftFromAngle((g_d->champions.array + indx)->direction + 0.25F * (SDL_randf() - 0.5F), PROJECTILE_VELOCITY, &x, &y);
		AddPCProjectileToArray(&g_d->projectiles, &(g_d->champions.array + indx)->position, x, y, &(g_d->champions.array + indx)->range_attack, TEST_PENETRATION);
		(g_d->champions.array + indx)->block_times.shoot = PC_SHOOT_RELOAD;
	}
}

static void UpdatePlayerPush(Game_data* const g_d, const unsigned int indx){
	if((g_d->champions.array + indx)->block_times.push > 0){
		--(g_d->champions.array + indx)->block_times.push;
	}else if(((g_d->champions.array + indx)->flags & (attack | block)) == (attack | block) && (g_d->champions.array + indx)->fatigue_points >= PC_PUSH_FATIG){
		(g_d->champions.array + indx)->fatigue_points -= PC_PUSH_FATIG;
		BlockPlayerFatigue(g_d->champions.array + indx, PC_PUSH_FATIG_BLOCK_TIME);
		(g_d->champions.array + indx)->block_times.push = PC_PUSH_RELOAD;
		Segment* s = GetSegment(&g_d->world, (g_d->champions.array + indx)->position.x, (g_d->champions.array + indx)->position.y);
		for(unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c){
		for(unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r){
			Segment* neighbour = GetSegmentByIndx(&g_d->world, c, r);
			if(neighbour == NULL) continue;
			for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
				Being* b = (g_d->beings.array + *(neighbour->beings.beings_ind + i));
				if(pow2((g_d->champions.array + indx)->position.x - b->position.x) + pow2((g_d->champions.array + indx)->position.y - b->position.y) < pow2(PC_PUSH_REACH)){
					float angle = GetDirectionToPush(&(g_d->champions.array + indx)->position, &b->position);
					if(angle < 0.0F){
						angle += FULL_ANGLE;
					}
					float difference = SDL_fabsf((g_d->champions.array + indx)->direction - angle);
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

extern inline void HitBarrier(Player* const p, const Impact* const impact){
	p->fatigue_points -= (int)(impact->stun * BLOCK_COST);
	BlockPlayerFatigue(p, PC_BLOCK_FATIG_BLOCK_TIME);
	if(p->fatigue_points < 0){
		p->flags &= ~(block);
		p->fatigue_points = 0;
	}
}

extern inline float GetDirectionToPush(SDL_FPoint* const pushing, SDL_FPoint* const pushed){
    return arctan2(pushed->y - pushing->y, pushed->x - pushing->x) + SDL_PI_F * 0.5F;
}

static void UpdatePlayerCast(Game_data* const g_d, const unsigned int indx){
	if((g_d->champions.array + indx)->block_times.cast > 0){
		--(g_d->champions.array + indx)->block_times.cast;
	}else if((g_d->champions.array + indx)->flags & cast){
		(g_d->champions.array + indx)->flags &= ~(cast);
		const int cost = ScrollCost((g_d->champions.array + indx)->selected_scroll);
		if((g_d->champions.array + indx)->magic_points >= cost){
			(g_d->champions.array + indx)->magic_points -= cost;
			--(*((g_d->champions.array + indx)->scrolls + (g_d->champions.array + indx)->selected_scroll));
			UseScroll(g_d);
			(g_d->champions.array + indx)->block_times.cast = PC_CAST_RELOAD;
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

static inline void BlockPlayerFatigue(Player* const p, const int time){
	if(p->block_times.fatigue < time){
		p->block_times.fatigue = time;
	}
}

static inline void BlockPlayerArmourRegen(Player* const p, const int time){
	if(p->block_times.armour < time){
		p->block_times.armour = time;
	}
}

static inline void PlayerGainArmour(Player* const p, const float perc){
	if(p->armour.absorption < p->max_armour.absorption){
		p->armour.absorption += p->max_armour.absorption * perc;
		if(p->armour.absorption > p->max_armour.absorption){
			p->armour.absorption = p->max_armour.absorption;
		}
	}
	if(p->armour.multipl > p->max_armour.multipl){
		p->armour.multipl -= (1.0F - p->max_armour.multipl) * perc;
		if(p->armour.multipl < p->max_armour.multipl){
			p->armour.multipl = p->max_armour.multipl;
		}
	}
	if(p->armour.magic_multipl > p->max_armour.magic_multipl){
		p->armour.magic_multipl -= (1.0F - p->max_armour.magic_multipl) * perc;
		if(p->armour.magic_multipl < p->max_armour.magic_multipl){
			p->armour.magic_multipl = p->max_armour.magic_multipl;
		}
	}
}

void UpdatePlayers(Game_data* const g_d){
	for(unsigned int i = 0U; i < g_d->champions.num; ++i){
		if(i != g_d->human_indx){
			UpdateCPUPlayerFlags(g_d, i);
		}
		UpdatePlayer(g_d, i);
    }
}

static void UpdateCPUPlayerFlags(Game_data* const g_d, const unsigned int indx){
	Player* const p = g_d->champions.array + indx;
	unsigned int next_i = (indx + 1U) % g_d->champions.num;
	if(next_i == g_d->human_indx && g_d->champions.num > 2U){
		next_i = (next_i + 1U) % g_d->champions.num;
	}
	if(pow2((g_d->champions.array + next_i)->position.x - p->position.x) + pow2((g_d->champions.array + next_i)->position.y - p->position.y) < pow2(PLAYER_SIZE * 2.0F)){
		if(indx % 2U == 0U){
			p->flags |= left;
			p->flags &= ~(right);
		}else{
			p->flags |= right;
			p->flags &= ~(left);
		}
	}else{
		p->flags &= ~(right | left);
	}
	Being* target;
	float d_x;
	float d_y;
	float d_squared;
	if(target = BeingNear(p->segment, g_d)){
		d_x = target->position.x - p->position.x;
		d_y = target->position.y - p->position.y;
		d_squared = pow2(d_x) + pow2(d_y);
		if(IsClearSightWithKnownDistance(&p->position, target->segment, &g_d->world, d_x, d_y, d_squared)){
			p->direction = arctan2(-d_y, -d_x) - SDL_PI_F * 0.5F;
			if(d_squared <= pow2(BEING_ATTACK_DISTANCE)){
				p->flags &= ~(back);
				if(d_squared <= pow2(BEING_HALT_DISTANCE)){
					p->flags &= ~(forward | run);
				}else{
					p->flags |= forward;
				}
				if((p->flags & (attack | range_mode)) != attack){
					p->flags &= ~(range_mode);
					p->flags |= attack;
				}else if((p->flags & (attack | range_mode)) == attack && SDL_randf() < 0x1.0p-7F){
					p->flags &= ~(attack);
				}
				return;
			}
			d_x = p->position.x - (g_d->champions.array + g_d->human_indx)->position.x;
			d_y = p->position.y - (g_d->champions.array + g_d->human_indx)->position.y;
			d_squared = pow2(d_x) + pow2(d_y);
			if(d_squared > pow2(SEGMENT_SIZE)){
				const float direction_to_main_player = arctan2(d_y, d_x) - SDL_PI_F * 0.5F;
				float direc_diff = p->direction - direction_to_main_player;
				if(direc_diff < 0.0F){
					direc_diff += FULL_ANGLE;
				}
				if(direc_diff < SDL_PI_F * 1.5F && direc_diff > SDL_PI_F * 0.5F){
					p->flags |= back;
					if(direc_diff < SDL_PI_F * 0.75F){
						p->flags |= left;
					}else if(direc_diff > SDL_PI_F * 1.25F){
						p->flags |= right;
					}
				}else{
					p->flags &= ~(back);
				}
			}else{
				p->flags &= ~(back);
			}
			p->flags &= ~(forward | run);
			p->flags |= range_mode | attack;
			return;
		}
	}
	p->flags &= ~(attack | back);
	d_x = (g_d->champions.array + g_d->human_indx)->position.x - p->position.x;
	d_y = (g_d->champions.array + g_d->human_indx)->position.y - p->position.y;
	d_squared = pow2(d_x) + pow2(d_y);
	if(!IsClearSightWithKnownDistance(&p->position, (g_d->champions.array + g_d->human_indx)->segment, &g_d->world, d_x, d_y, d_squared)){
		d_x = SegmentCenterX((g_d->champions.array + g_d->human_indx)->last_seen_in) - p->position.x;
		d_y = SegmentCenterY((g_d->champions.array + g_d->human_indx)->last_seen_in) - p->position.y;
	}
	p->direction = arctan2(-d_y, -d_x) - SDL_PI_F * 0.5F;
	if(d_squared < pow2(SEGMENT_SIZE)){
		p->flags &= ~(forward | run);
		return;
	}
	p->flags |= forward;
	if(d_squared > pow2(SEGMENT_SIZE * 2.0F)){
		p->flags |= run;
	}else{
		p->flags &= ~(run);
	}
}

static inline Being* BeingNear(Segment* s, Game_data* const gd){
	if(s->beings.num > 0U){
		return gd->beings.array + *s->beings.beings_ind;
	}
	int c = s->indx.x;
	int r = s->indx.y;
	unsigned int i = 0U;
	int sign = 1;
	do{
		unsigned int j = 0U;
		unsigned int k = 0U;
		++i;
		while(j < i){
			c += sign;
			s = GetSegmentByIndxSafe(&gd->world, c, r);
			if(s && s->beings.num > 0U){
				return gd->beings.array + *s->beings.beings_ind;
			}
			++j;
		}
		while(k < i){
			r += sign;
			s = GetSegmentByIndxSafe(&gd->world, c, r);
			if(s && s->beings.num > 0U){
				return gd->beings.array + *s->beings.beings_ind;
			}
			++k;
		}
		sign *= -1;
	}while(i < 13);
	return NULL;
}