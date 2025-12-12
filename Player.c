#include <SDL3/SDL.h>
#include <macros.h>
#include <enum.h>
#include <types.h>
#include <Projectile.h>
#include <Player.h>
#include <function.h>
#include <World.h>
#include <Being.h>
#include <Scroll.h>
#include <game.h>
#include <render.h>

static void InitScrolls(Player* const p){
	*(p->scrolls + scroll_0) = 99U;
	*(p->scrolls + scroll_1) = 99U;
	*(p->scrolls + scroll_2) = 99U;
	*(p->scrolls + scroll_3) = 99U;
	*(p->scrolls + scroll_slow) = 99U;
	for(unsigned int i = 5U; i < SCROLLS_NUM / 2U; ++i){
		*(p->scrolls + i) = 16U;
	}
	for(unsigned int i = SCROLLS_NUM / 2U; i < scroll_empty; ++i){
		*(p->scrolls + i) = 0U;
	}
	*(p->scrolls + scroll_empty) = 1U;
	*(p->scrolls_quick_access) = scroll_empty;
	for(unsigned int i = 1U; i < 9U; ++i){
		*(p->scrolls_quick_access + i) = i - 1U;
	}
}

void CreatePlayer(Player* const pc, const float x, const float y){
	pc->blade.placement = (Placement)PC_BLADE_BASE_PLCMNT;
	pc->blade.impact = (Impact){};
	pc->blade.penetration = BLADE_PENETRATION;
	pc->blade.hits = 0;
	pc->blade.step_shift = (Placement){{0.0F, 0.0F}, 0.0};
	pc->blade.key = 0;
	pc->blade.steps = 0;
	pc->blade.step = 0;
	pc->blade.chain = 0U;
	pc->blade.chain_next = 0U;
	pc->blade.abide = false;
	pc->blade.freehand = false;
	pc->blade.idle_ticks = 0U;
	pc->blade.charge = PC_BLADE_CHARGE_BASE;
	pc->blade_attack = (Impact)PC_BLADE_IMPACT;
	pc->range_attack = (Impact)PC_RANGE_IMPACT;
	SetPlayerPosition(pc, x, y);
	pc->flags = 0x00000000U;
	pc->direction = 0.0F;
	pc->move_direction = 0.0F;
	pc->velocity = 0.0F;
	pc->max_velocity = PLAYER_VELOCITY;
	pc->dodge_velocity_multipl = DODGE_VELOCITY_MULTIP;
	pc->hit_points = PC_HP;
	pc->fatigue_points = (int)(PC_FATIGUE * 0.9F);
	pc->magic_points = PC_MAGIC;
	pc->max_fatigue = PC_FATIGUE;
	pc->max_h_p = PC_HP;
	pc->block_times = (Block_times){0, 0, 0, 0, 0, 0};
	pc->armour = (Armour)PC_ARMOUR;
	pc->max_armour = (Armour)PC_MAX_ARMOUR;
	pc->coins = PC_START_COINS;
	pc->selected_scroll = scroll_empty;
	InitScrolls(pc);
    pc->effects_num = 0U;
	pc->help_data.menu_position = 0U;
	pc->attention_rect = (SDL_FRect) {
		pc->position.x - half(ATTENTION_RECT_SIZE),
		pc->position.y - half(ATTENTION_RECT_SIZE),
		ATTENTION_RECT_SIZE,
		ATTENTION_RECT_SIZE
	};
}

extern inline void SetPlayerPosition(Player* const p, const float x, const float y){
	p->position = (SDL_FPoint){x, y};
}

extern inline void MovePlayer(Game_data* const gd, Player* const pc, const float x, const float y){
	float new_x = pc->position.x + x;
	float new_y = pc->position.y + y;
	const Segment* seg = GetSegmentUnsafe(&gd->world, new_x, new_y);
	if(seg){
		SetPlayerPosition(pc, new_x, new_y);
	}else if((seg = GetSegmentUnsafe(&gd->world, pc->position.x, new_y))){
		pc->position.y = new_y;
	}else if((seg = GetSegmentUnsafe(&gd->world, new_x, pc->position.y))){
		pc->position.x = new_x;
	}
	if(seg != pc->segment){
		UpdatePlayerNewSegment(&gd->world, pc);
	}
}

extern inline void UpdatePlayerNewSegment(World* const wld, Player* const pc){
	pc->last_seen_in = pc->segment;
	pc->segment = GetSegmentUnsafe(wld, pc->position.x, pc->position.y);
}

static void UpdatePlayerMove(Game_data* const gd, const unsigned int indx){
	Player* const pc = gd->champions.array + indx;
	static const Uint32 tmp = forward | back | right | left;
	const bool runs = ((pc->flags & (run | forward | back)) == (run | forward));
	if(pc->velocity > pc->max_velocity * RUN_MULTIPL || (!runs && pc->velocity > pc->max_velocity) || ((pc->flags & block) && pc->velocity > pc->max_velocity * BLOCK_VELOCITY_MULTIP)){
		pc->velocity *= DECELERATION;
	}else if((pc->flags & tmp) == forward || (pc->flags & tmp) == (tmp & ~(back))){
		pc->velocity += ACCELERATION;
		pc->move_direction = pc->direction;
	}else if((pc->flags & tmp) == back){
		pc->velocity = (pc->velocity + ACCELERATION) * MOVING_BACK_VELO_MODI;
		pc->move_direction = pc->direction + SDL_PI_F;
	}else if((pc->flags & tmp) == right){
		pc->velocity += ACCELERATION;
		pc->move_direction = pc->direction + SDL_PI_F * 0.5F;
	}else if((pc->flags & tmp) == left){
		pc->velocity += ACCELERATION;
		pc->move_direction = pc->direction + SDL_PI_F * 1.5F;
	}else if((pc->flags & tmp) == (forward | right)){
		pc->velocity += ACCELERATION;
		pc->move_direction = pc->direction + SDL_PI_F * 0.25F;
	}else if((pc->flags & tmp) == (forward | left)){
		pc->velocity += ACCELERATION;
		pc->move_direction = pc->direction + SDL_PI_F * 1.75F;
	}else if((pc->flags & tmp) == (back | right)){
		pc->velocity = (pc->velocity + ACCELERATION) * MOVING_BACK_VELO_MODI;
		pc->move_direction = pc->direction + SDL_PI_F * 0.75F;
	}else if((pc->flags & tmp) == (back | left)){
		pc->velocity = (pc->velocity + ACCELERATION) * MOVING_BACK_VELO_MODI;
		pc->move_direction = pc->direction + SDL_PI_F * 1.25F;
	}else{
		pc->velocity *= DECELERATION;
		if(pc->velocity < 0.05F) pc->velocity = 0.0F;
	}
	if(!(pc->flags & block)){
		if(pc->flags & dodge && !(pc->flags & forward)){
			if(pc->fatigue_points >= PC_DODGE_FATIG && pc->block_times.dodge < 1){
				pc->fatigue_points -= PC_DODGE_FATIG;
				BlockPlayerFatigue(pc, PC_DODGE_FATIG_BLOCK_TIME);
				pc->block_times.dodge = PC_DODGE_RELOAD;
				if(!(pc->flags & tmp)){
					pc->move_direction = pc->direction + SDL_PI_F;
				}
				pc->velocity = pc->max_velocity * pc->dodge_velocity_multipl;
			}else{
				BlockPlayerFatigue(pc, PC_FAILURE_FATIG_BLOCK_TIME);
				if(!(pc->flags & tmp)){
					pc->move_direction = pc->direction + SDL_PI_F;
				}
				pc->velocity = PC_FAILURE_VELOCITY;
			}
			pc->flags &= ~(dodge);
		}
		if(runs){
			if(pc->fatigue_points <= 1){
				pc->flags &= ~(run);
			}else{
				--(pc->fatigue_points);
			}
		}
	}
	if(pc->velocity > 0.0F){
		MovePlayer(gd, pc, SDL_sinf(pc->move_direction) * pc->velocity, -SDL_cosf(pc->move_direction) * pc->velocity);
	}
}

static void UpdatePlayerDirection(Player* const p){
	if(p->direction > 2.0F * SDL_PI_F){
		p->direction -= 2.0F * SDL_PI_F;
	}else if(p->direction < 0.0F){
		p->direction += 2.0F * SDL_PI_F;
	}
}

static void UpdatePlayerPoints(Player* const pc){
	if(pc->block_times.fatigue < 1){
		if(pc->fatigue_points < pc->max_fatigue){
			++(pc->fatigue_points);
		}
		if(pc->flags & block){
			BlockPlayerFatigue(pc, PC_FATIGUE_GAIN_INTERVAL * 2);
		}else{
			BlockPlayerFatigue(pc, PC_FATIGUE_GAIN_INTERVAL);
		}
	}else{
		--(pc->block_times.fatigue);
	}
	if(pc->block_times.armour < 1U){
		PlayerGainArmour(pc, 0.01F);
		BlockPlayerArmourRegen(pc, PC_ARMOUR_GAIN_INTERVAL);
	}else{
		--(pc->block_times.armour);
	}
	if(pc->block_times.dodge > 0){
		--pc->block_times.dodge;
	}
}

static inline void UpdatePlayer(Game_data* const gd, const unsigned int indx){
	UpdatePlayerDirection(gd->champions.array + indx);
	UpdatePlayerMove(gd, indx);
	UpdatePlayerCast(gd, indx);
	UpdatePlayerPush(gd, indx);
	UpdatePlayerFire(gd, indx);
	UpdatePlayerBlade(gd, indx);
	UpdatePlayerPoints(gd->champions.array + indx);
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

static inline Placement GetBladeLocation(Player* const pc){
	const float sine = SineUnsafe(pc->direction);
	const float cosine = CosiUnsafe(pc->direction);
	return (Placement){
		{
			pc->position.x + cosine * pc->blade.placement.position.x + sine * pc->blade.placement.position.y,
			pc->position.y + sine * pc->blade.placement.position.x - cosine * pc->blade.placement.position.y
		},
		pc->direction + pc->blade.placement.direction
	};
}

static inline bool BladeHitsBeing(Blade* const bl, Being* const bg, SDL_FPoint* const dangerous_points){
	for(unsigned int i = 0U; i < PC_BLADE_CHECKPOINTS; ++i){
		if(SDL_fabsf((dangerous_points + i)->x - bg->position.x) < half(BeingSize(bg)) && SDL_fabsf((dangerous_points + i)->y - bg->position.y) < half(BeingSize(bg))){
			for(unsigned int j = bl->hits; j > 0U; --j){
				if(*(bl->hit_targets + (j - 1U)) == bg->main_indx){
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

static bool UnleashDestruction(Game_data* const gd, const unsigned int indx){
	static const float length_part = BLADE_SIZE * 0.85F / (float)PC_BLADE_CHECKPOINTS;
	const Placement blade_true_location = GetBladeLocation(gd->champions.array + indx);
	const Segment* seg = GetSegmentUnsafe(&gd->world, blade_true_location.position.x, blade_true_location.position.y);
	if(seg == NULL){
		return true;
	}
	const float shift_x = SineSafe(blade_true_location.direction) * length_part;
	const float shift_y = -CosiSafe(blade_true_location.direction) * length_part;
	SDL_FPoint dangerous_points[PC_BLADE_CHECKPOINTS] = {
		{blade_true_location.position.x + 2.0F * shift_x, blade_true_location.position.y + 2.0F * shift_y},
		{blade_true_location.position.x + shift_x, blade_true_location.position.y + shift_y}
	};
	Blade* const bl = &(gd->champions.array + indx)->blade;
	Segment* neighbour_segs[4];
	Get4NearestSegments(neighbour_segs, &gd->world, seg, blade_true_location.position.x, blade_true_location.position.y);
	for(unsigned int k = 0U; k < 4; ++k){
		Segment* neighbour = *(neighbour_segs + k);
		if(neighbour == NULL) continue;
		for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
			Being* bg = (gd->beings.array + *(neighbour->beings.beings_ind + i));
			if(!BladeHitsBeing(bl, bg, dangerous_points)) continue;
			AddDamageVisualEffect(&gd->rend_data_ptr->visual_effects, dangerous_points);
			if(DamageBeing(bg, &bl->impact, gd->beings.array)){
				if(bl->hits < --bl->penetration){
					--i;
					continue;
				}
			}else{
				const float stun_power = CalculateStunPower(&bl->impact, &bg->armour);
				if(stun_power >= 1.0F){
					const float angle = GetDirectionToPush(&(gd->champions.array + indx)->position, &bg->position);
					CatapultBeing(bg, SineSafe(angle) * stun_power, -CosiSafe(angle) * stun_power, BEING_DEFAULT_LEFT_TICKS * 2);
				}
				if(bl->hits < bl->penetration){
					*(bl->hit_targets + bl->hits++) = bg->main_indx;
					continue;
				}
			}
			return true;
		}
	}
	return false;
}

static void UpdatePlayerBlade(Game_data* const gd, const unsigned int indx){
	static const Placement blade_key_frames_0[] = PC_BLADE_FRAMES0;
	static const Placement blade_key_frames_1[] = PC_BLADE_FRAMES1;
	static const Placement blade_key_frames_2[] = PC_BLADE_FRAMES2;
	static const Placement* blade_moves[] = {blade_key_frames_0, blade_key_frames_1, blade_key_frames_2};
	static const unsigned int sizes[] = {SDL_arraysize(blade_key_frames_0), SDL_arraysize(blade_key_frames_1), SDL_arraysize(blade_key_frames_2)};
	static const float extra_penetration[] = PC_BLADE_PENETRATIONS;
	Blade* const bl = &(gd->champions.array + indx)->blade;
	if((gd->champions.array + indx)->flags & range_mode){
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
	if((gd->champions.array + indx)->flags & block){
		bl->abide = false;
		bl->charge = PC_BLADE_CHARGE_BASE;
		bl->idle_ticks = PC_BLADE_MAX_IDLE_TICKS;
	}else if((gd->champions.array + indx)->flags & attack){
		bl->charge *= PC_BLADE_CHARGE_MODIFIER;
	} 
	if(!bl->abide){
		if(bl->charge != PC_BLADE_CHARGE_BASE){
			if(!((gd->champions.array + indx)->flags & attack)){
				bl->chain = bl->chain_next;
				bl->key = 0;
				bl->step = 0;
				bl->abide = true;
				bl->idle_ticks = 0U;
				bl->hits = 0U;
				const float multip = 1.0F - bl->charge;
				bl->impact = (Impact){
					(gd->champions.array + indx)->blade_attack.damage * multip,
					(gd->champions.array + indx)->blade_attack.armour_reduction * multip + *(extra_penetration + bl->chain),
					(gd->champions.array + indx)->blade_attack.magic * multip,
					(gd->champions.array + indx)->blade_attack.stun * multip
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
		if(!bl->freehand && bl->key > 1 && (bl->freehand = UnleashDestruction(gd, indx))){
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

extern inline void DamagePlayer(Player* const pc, const Impact* impact){
	pc->hit_points -= CalculateDamage(impact, &pc->armour);
	if(impact->damage > 0.0F){
		if(pc->armour.absorption > 0.0F){
			pc->armour.absorption -= impact->damage;
			if(pc->armour.absorption < 0.0F){
				pc->armour.absorption = 0.0F;
			}
		}
		if(pc->armour.multipl < 1.0F){
			pc->armour.multipl += (1.0F - pc->max_armour.multipl) * (impact->damage / pc->max_armour.absorption);
			if(pc->armour.multipl > 1.0F){
				pc->armour.multipl = 1.0F;
			}
		}
	}
	if(impact->magic > 0.0F){
		if(pc->armour.magic_multipl < 1.0F){
			pc->armour.magic_multipl += (1.0F - pc->max_armour.magic_multipl) * 0.01F;
			if(pc->armour.magic_multipl > 1.0F){
				pc->armour.magic_multipl = 1.0F;
			}
		}
	}
	BlockPlayerArmourRegen(pc, PC_ARMOUR_REGEN_BLOCK);
	StunPlayer(pc);
}

static void UpdatePlayerFire(Game_data* const gd, const unsigned int indx){
	if((gd->champions.array + indx)->block_times.shoot > 0){
		--(gd->champions.array + indx)->block_times.shoot;
	}else if((gd->champions.array + indx)->selected_scroll == scroll_empty && ((gd->champions.array + indx)->flags & (range_mode | attack | block)) == (range_mode | attack) && gd->projectiles.num < MAX_PROJECTILES_NUM){
		float x, y;
		GetShiftFromAngle((gd->champions.array + indx)->direction + 0.125F * (SDL_randf() - 0.5F), PROJECTILE_VELOCITY, &x, &y);
		AddPCProjectileToArray(&gd->projectiles, &(gd->champions.array + indx)->position, x, y, &(gd->champions.array + indx)->range_attack, TEST_PENETRATION);
		(gd->champions.array + indx)->block_times.shoot = PC_SHOOT_RELOAD;
	}
}

static void UpdatePlayerPush(Game_data* const gd, const unsigned int indx){
	if((gd->champions.array + indx)->block_times.push > 0){
		--(gd->champions.array + indx)->block_times.push;
	}else if(((gd->champions.array + indx)->flags & (attack | block)) == (attack | block) && (gd->champions.array + indx)->fatigue_points >= PC_PUSH_FATIG){
		(gd->champions.array + indx)->fatigue_points -= PC_PUSH_FATIG;
		BlockPlayerFatigue(gd->champions.array + indx, PC_PUSH_FATIG_BLOCK_TIME);
		(gd->champions.array + indx)->block_times.push = PC_PUSH_RELOAD;
		Segment* seg = GetSegmentUnsafe(&gd->world, (gd->champions.array + indx)->position.x, (gd->champions.array + indx)->position.y);
		Segment* neighbour_segs[4];
		Get4NearestSegments(neighbour_segs, &gd->world, seg, (gd->champions.array + indx)->position.x, (gd->champions.array + indx)->position.y);
		for(unsigned int k = 0U; k < 4; ++k){
			Segment* neighbour = *(neighbour_segs + k);
			if(neighbour == NULL) continue;
			for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
				Being* b = (gd->beings.array + *(neighbour->beings.beings_ind + i));
				if(pow2((gd->champions.array + indx)->position.x - b->position.x) + pow2((gd->champions.array + indx)->position.y - b->position.y) < pow2(PC_PUSH_REACH)){
					float angle = GetDirectionToPush(&(gd->champions.array + indx)->position, &b->position);
					if(angle < 0.0F){
						angle += FULL_ANGLE;
					}
					const float difference = SDL_fabsf((gd->champions.array + indx)->direction - angle);
					if(difference <= SDL_PI_F * 0.5F || difference >= SDL_PI_F * 1.5F){
						CatapultBeing(b, SineUnsafe(angle) * DEFAULT_FLY_VELOCITY, -CosiUnsafe(angle) * DEFAULT_FLY_VELOCITY, BEING_DEFAULT_LEFT_TICKS * 4);
					}
				}
			}
		}
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

static void UpdatePlayerCast(Game_data* const gd, const unsigned int indx){
	if((gd->champions.array + indx)->block_times.cast > 0){
		--(gd->champions.array + indx)->block_times.cast;
	}else if((gd->champions.array + indx)->flags & cast){
		(gd->champions.array + indx)->flags &= ~(cast);
		const int cost = ScrollCost((gd->champions.array + indx)->selected_scroll);
		if(*((gd->champions.array + indx)->scrolls + (gd->champions.array + indx)->selected_scroll) > 0U && (gd->champions.array + indx)->magic_points >= cost){
			(gd->champions.array + indx)->magic_points -= cost;
			--(*((gd->champions.array + indx)->scrolls + (gd->champions.array + indx)->selected_scroll));
			UseScroll(gd);
			(gd->champions.array + indx)->block_times.cast = PC_CAST_RELOAD;
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

static inline void PlayerGainArmour(Player* const pc, const float perc){
	if(pc->armour.absorption < pc->max_armour.absorption){
		pc->armour.absorption += pc->max_armour.absorption * perc;
		if(pc->armour.absorption > pc->max_armour.absorption){
			pc->armour.absorption = pc->max_armour.absorption;
		}
	}
	if(pc->armour.multipl > pc->max_armour.multipl){
		pc->armour.multipl -= (1.0F - pc->max_armour.multipl) * perc;
		if(pc->armour.multipl < pc->max_armour.multipl){
			pc->armour.multipl = pc->max_armour.multipl;
		}
	}
	if(pc->armour.magic_multipl > pc->max_armour.magic_multipl){
		pc->armour.magic_multipl -= (1.0F - pc->max_armour.magic_multipl) * perc;
		if(pc->armour.magic_multipl < pc->max_armour.magic_multipl){
			pc->armour.magic_multipl = pc->max_armour.magic_multipl;
		}
	}
}

void UpdatePlayers(Game_data* const gd){
	for(unsigned int i = 0U; i < gd->champions.num; ++i){
		if(i != gd->human_indx){
			UpdateCPUPlayerFlags(gd, i);
		}
		UpdatePlayer(gd, i);
    }
}

void UpdatePlayersEffects(Game_data* const gd){
	for(unsigned int i = 0U; i < gd->champions.num; ++i){
		UpdatePlayerEffects(gd, i);
    }
}

static void UpdateCPUPlayerFlags(Game_data* const gd, const unsigned int indx){
	Player* const p = gd->champions.array + indx;
	unsigned int next_i = (indx + 1U) % gd->champions.num;
	if(next_i == gd->human_indx && gd->champions.num > 2U){
		next_i = (next_i + 1U) % gd->champions.num;
	}
	if(pow2((gd->champions.array + next_i)->position.x - p->position.x) + pow2((gd->champions.array + next_i)->position.y - p->position.y) < pow2(PLAYER_SIZE * 2.0F)){
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
	if(target = BeingNear(p->segment, gd)){
		d_x = target->position.x - p->position.x;
		d_y = target->position.y - p->position.y;
		d_squared = pow2(d_x) + pow2(d_y);
		if(IsClearSightWithKnownDistance(&p->position, target->segment, &gd->world, d_x, d_y, d_squared)){
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
				}else if((p->flags & (attack | range_mode)) == attack && SDL_randf() < 0x1.0p-5F){
					p->flags &= ~(attack);
				}
				return;
			}
			d_x = p->position.x - human(gd)->position.x;
			d_y = p->position.y - human(gd)->position.y;
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
	d_x = human(gd)->position.x - p->position.x;
	d_y = human(gd)->position.y - p->position.y;
	d_squared = pow2(d_x) + pow2(d_y);
	if(!IsClearSightWithKnownDistance(&p->position, human(gd)->segment, &gd->world, d_x, d_y, d_squared)){
		d_x = SegmentCenterX(human(gd)->last_seen_in) - p->position.x;
		d_y = SegmentCenterY(human(gd)->last_seen_in) - p->position.y;
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

extern inline void SetQuickScroll(Player* const pc, int num){
	for(unsigned int i = 0U; i < 9U; ++i){
		if(*(pc->scrolls_quick_access + i) == pc->help_data.menu_position){
			if(i != num){
				*(pc->scrolls_quick_access + i) = *(pc->scrolls_quick_access + num);
				break;
			}else{
				return;
			}
		}
	}
	*(pc->scrolls_quick_access + num) = pc->help_data.menu_position;
}

static inline void StunPlayer(Player* const pc){
	const int effect_indx = PlayerHasEffect(pc, pc_effect_slow);
	if(effect_indx == -1){
		AddPlayerEffect(pc, (Lasting_effect){pc_effect_slow, PC_DODGE_RELOAD});
		pc->flags |= stunned;
		pc->max_velocity *= 0.25F;
	}else{
		(pc->effects + effect_indx)->ticks_left = PC_DODGE_RELOAD;
	}
	pc->flags &= ~(dodge | run | block | attack | cast | action);
	pc->block_times.dodge = PC_DODGE_RELOAD;
	pc->blade.charge = PC_BLADE_CHARGE_BASE;
	pc->blade.idle_ticks = PC_BLADE_MAX_IDLE_TICKS;
}

extern inline void AddPlayerEffect(Player* const pc, const Lasting_effect effect){
	if(pc->effects_num < MAX_PC_EFFECTS){
		AddLastingEffect(pc->effects, effect, pc->effects_num++);
	}
}

extern inline void AddOrUpdatePlayerEffect(Player* const pc, const Lasting_effect effect){
	const int effect_indx = PlayerHasEffect(pc, effect.id);
	if(effect_indx == -1){
		AddPlayerEffect(pc, effect);
	}else{
		(pc->effects + effect_indx)->ticks_left = effect.ticks_left;
	}
}

static inline void RemovePlayerEffect(Player* const pc, const int indx){
	RemoveLastingEffect(pc->effects, indx, pc->effects_num--);
}

static inline void UpdatePlayerEffects(Game_data* const gd, const unsigned int indx){
	Player* const pc = gd->champions.array + indx;
	for(unsigned int i = 0U; i < pc->effects_num; ++i){
		UpdatePlayerEffect(gd, pc, i);
	}
}

static inline void UpdatePlayerEffect(Game_data* const gd, Player* const pc, const int effect_indx){
    const void (*effect[])(Game_data* const, Player* const, const int) = PLAYER_LASTING_EFFECTS;
    (*(effect + (pc->effects + effect_indx)->id))(gd, pc, (pc->effects + effect_indx)->ticks_left--);
	if((pc->effects + effect_indx)->ticks_left < 1){
		RemovePlayerEffect(pc, effect_indx);
	}
}

extern inline int PlayerHasEffect(Player* const pc, const int effect_id){
    for(unsigned int i = 0U; i < pc->effects_num; ++i){
        if((pc->effects + i)->id == effect_id){
            return i;
        }
    }
    return -1;
}

void SlowPlayer(Game_data* const gd, Player* const pc, const int ticks_left){
    if(ticks_left < 2){
        pc->max_velocity = PLAYER_VELOCITY;
		pc->flags &= ~(stunned);
    }
}

void PlayerHPRegeneration(Game_data* const gd, Player* const pc, const int ticks_left){
    HealPlayer(pc, 1);
}

void PlayerWeakness(Game_data* const gd, Player* const pc, const int ticks_left){
	if(ticks_left < 2){
		if(pc->blade_attack.damage < PC_BLADE_DMG){
        	pc->blade_attack.damage = PC_BLADE_DMG;
		}
		if(pc->blade_attack.magic < PC_BLADE_MAGIC){
        	pc->blade_attack.magic = PC_BLADE_MAGIC;
		}
		if(pc->blade_attack.armour_reduction < PC_BLADE_PENETR){
        	pc->blade_attack.armour_reduction = PC_BLADE_PENETR;
		}
		if(pc->range_attack.damage < PC_RANGE_DMG){
        	pc->range_attack.damage = PC_RANGE_DMG;
		}
		if(pc->range_attack.magic < PC_RANGE_MAGIC){
        	pc->range_attack.magic = PC_RANGE_MAGIC;
		}
		if(pc->range_attack.armour_reduction < PC_RANGE_PENETR){
        	pc->range_attack.armour_reduction = PC_RANGE_PENETR;
		}
    }else if(ticks_left % 64 == 0){
        AddCurseVisualEffect(&gd->rend_data_ptr->visual_effects, &pc->position);
    }
}