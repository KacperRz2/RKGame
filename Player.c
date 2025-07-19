#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Projectile.h>
#include <Player.h>
#include <function.h>
#include <World.h>
#include <Being.h>
#include <enum.h>

// bool tmp = false;

void CreatePlayer(Player* const p, const float x, const float y){
	p->blade.position.x = 16.0F;
 	p->blade.position.y = -8.0F;
	p->blade.direction = SDL_PI_F * 0.45F;
	p->blade.damage = BLADE_DAMAGE;
	p->blade.penetration = BLADE_PENETRATION;
	p->blade.hits = 0;
	SetPlayerPosition(p, x, y);
	p->control_flags = 0x00000000U;
	p->direction = 0.0F;
	p->velocity = 0.0F;
	p->max_velocity = PLAYER_VELOCITY;
	p->hit_points = 100;
	p->fatigue_points = 900;
	p->magic_points = 100;
	// p->range = 700.0F;
	p->max_fatigue = 1000;
	p->max_h_p = 100;
	p->max_magic = 100;
	p->fatigue_block_time = 0;
	p->armour = 0.875F;
}

inline void SetPlayerPosition(Player* const p, const float x, const float y){
	p->position.x = x;
	p->position.y = y;
}

extern inline void MovePlayer(Player* const p, const float x, const float y){
	float new_x = p->position.x + x;
	float new_y = p->position.y + y;

	if(GetSegment(new_x, new_y) != NULL){
		SetPlayerPosition(p, new_x, new_y);
	}else if(GetSegment(p->position.x, new_y) != NULL){
		p->position.y = new_y;
	}else if(GetSegment(new_x, p->position.y) != NULL){
		p->position.x = new_x;
	}
}

inline void UpdatePlayerMove(Player* const p){
	static const Uint32 tmp = forward | back | right | left;
	static float move_direction = 0.0F;
	if(p->velocity > p->max_velocity){
		p->velocity *= DECELERATION;
	}else if((p->control_flags & tmp) == forward || (p->control_flags & tmp) == (tmp & ~(back))){
		p->velocity += ACCELERATION;
		move_direction = p->direction;
	}else if((p->control_flags & tmp) == back){
		p->velocity = (p->velocity + ACCELERATION) * 0.93F;
		move_direction = p->direction + SDL_PI_F;
	}else if((p->control_flags & tmp) == right){
		p->velocity += ACCELERATION;
		move_direction = p->direction + SDL_PI_F * 0.5F;
	}else if((p->control_flags & tmp) == left){
		p->velocity += ACCELERATION;
		move_direction = p->direction + SDL_PI_F * 1.5F;
	}else if((p->control_flags & tmp) == (forward | right)){
		p->velocity += ACCELERATION;
		move_direction = p->direction + SDL_PI_F * 0.25F;
	}else if((p->control_flags & tmp) == (forward | left)){
		p->velocity += ACCELERATION;
		move_direction = p->direction + SDL_PI_F * 1.75F;
	}else if((p->control_flags & tmp) == (back | right)){
		p->velocity = (p->velocity + ACCELERATION) * 0.93F;
		move_direction = p->direction + SDL_PI_F * 0.75F;
	}else if((p->control_flags & tmp) == (back | left)){
		p->velocity = (p->velocity + ACCELERATION) * 0.93F;
		move_direction = p->direction + SDL_PI_F * 1.25F;
	}else{
		p->velocity *= DECELERATION;
		if(p->velocity < 0.05F) p->velocity = 0.0F;
	}
	if(!(p->control_flags & forward) && p->control_flags & dodge){
		if(p->fatigue_points > 99){
			p->fatigue_points -= 100;
			p->fatigue_block_time = 50;
			if(!(p->control_flags & tmp)){
				move_direction = p->direction + SDL_PI_F;
			}
			p->velocity = 10.0F;
		}else{
			p->fatigue_block_time += 2;
			if(!(p->control_flags & tmp)){
				move_direction = p->direction + SDL_PI_F;
			}
			p->velocity = 1.0F;
		}
		p->control_flags &= ~(dodge);
	}
	if((p->control_flags & (run | forward | back)) == (run | forward)){
		if(p->fatigue_points <= 1){
			p->max_velocity = PLAYER_VELOCITY;
			p->control_flags &= ~(run);
		}else{
			p->max_velocity = RUN_VELOCITY;
			--(p->fatigue_points);
		}
	}else{
		p->max_velocity = PLAYER_VELOCITY;
	}
	if(p->velocity > 0.0F){
		MovePlayer(p, SDL_sinf(move_direction) * p->velocity, -SDL_cosf(move_direction) * p->velocity);
	}
}

inline void UpdatePlayerDirection(Player* const p){
	if(p->direction > 2.0F * SDL_PI_F){
		p->direction -= 2.0F * SDL_PI_F;
	}else if(p->direction < 0.0F){
		p->direction += 2.0F * SDL_PI_F;
	}
}

inline void UpdatePlayerPoints(Player* const p){
	static unsigned int count = 4U;
	if(count <= 1){
		if(p->fatigue_block_time > 0){
			--(p->fatigue_block_time);
		}else if(p->fatigue_points < p->max_fatigue){
			++(p->fatigue_points);
		}
		count = 4U;
	}
	--count;
}

extern inline void UpdatePlayer(Player* const p, Projectiles_array* const prs){
	if(p->control_flags & fire && prs->num < MAX_PROJECTILES_NUM){
		AddProjectileToArray(prs, &p->position, p->direction + 0.25F * (SDL_randf() - 0.5F), 3.0F, 10, 3U);
	}
	UpdatePlayerBlade(p);
	UpdatePlayerDirection(p);
	UpdatePlayerMove(p);
	UpdatePlayerPoints(p);
}

inline void ShiftBlade(Blade* const bl, Status_frame* const shift){
	bl->position.x += shift->position.x;
	bl->position.y += shift->position.y;
	bl->direction += shift->direction;
}

inline void SetBladePosition(Blade* const bl, const Status_frame* const position){
	bl->position.x = position->position.x;
	bl->position.y = position->position.y;
	bl->direction = position->direction;
}

inline void SetShiftToBase(Blade* const bl, Status_frame* const step_shift, const unsigned int steps){
	step_shift->position.x = (16.0F - bl->position.x) / steps;
	step_shift->position.y = (-8.0F - bl->position.y) / steps;
	step_shift->direction = (SDL_PI_F * 0.45F - bl->direction) / steps;
}

inline void SetShiftToPosition(Blade* const bl, Status_frame* const step_shift, const Status_frame* const frame, const unsigned int steps){
	step_shift->position.x = (frame->position.x - bl->position.x) / steps;
	step_shift->position.y = (frame->position.y - bl->position.y) / steps;
	step_shift->direction = (frame->direction - bl->direction) / steps;
}

inline Status_frame GetBladeLocation(Player* const p, float* const sine, float* const cosine){
	float direct = p->direction + p->blade.direction;
	*sine = SineSafe(direct);
	*cosine = CosiSafe(direct);
	Status_frame ret = {{p->position.x + *sine * p->blade.position.x, p->position.y - *cosine * p->blade.position.y}, direct};
	return ret;
}

inline bool BladeHitsBeing(Blade* const bl, Status_frame* const location, Being* const b, SDL_FPoint* const dangerous_points){
	for(unsigned int i = 0U; i < 2U; ++i){
		if(SDL_fabsf((dangerous_points + i)->x - b->position.x) < PLAYER_SIZE * 0.5F && SDL_fabsf((dangerous_points + i)->y - b->position.y) < PLAYER_SIZE * 0.5F){
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

inline bool UnleashDestruction(Player* const p){
	static const float length_part = BLADE_SIZE * 0.85F * 0.5F;
	float sine_blade_direction;
	float cosine_blade_direction;
	Status_frame blade_true_location = GetBladeLocation(p, &sine_blade_direction, &cosine_blade_direction);
	Segment* s = GetSegment(blade_true_location.position.x, blade_true_location.position.y);
	if(s == NULL){
		return true;
	} //if(tmp){SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "####################");}
	float shift_x = sine_blade_direction * length_part;
	float shift_y = -cosine_blade_direction * length_part;
	SDL_FPoint dangerous_points[] = {
		{blade_true_location.position.x + 2.0F * shift_x, blade_true_location.position.y + 2.0F * shift_y},
		{blade_true_location.position.x + shift_x, blade_true_location.position.y + shift_y}
	};
	Blade* const bl = &p->blade;
	for(unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c){
	for(unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r){
		Segment* neighbour = GetSegmentByIndx(c, r);
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

inline void UpdatePlayerBlade(Player* const p){
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
	static int steps = 128;
	static int step = 0;
	static unsigned int chain = 0U;
	static unsigned int chain_next = 0U;
	static bool abide = false;
	static bool freehand = false;
	static unsigned int idle_ticks = 0U;
	static float charge = 0xF.Fp-4F;
	if(p->control_flags & attack){
		charge *= 0xF.F8p-4F;
	}
	if(!abide){
		if(charge != 0xF.Fp-4F){
			if(!(p->control_flags & attack)){
				chain = chain_next;
				key = 0;
				step = 0;
				abide = true;
				idle_ticks = 0U;
				p->blade.hits = 0U;
				p->blade.damage = (int)(120.0F * (1.0F - charge));
				p->blade.penetration = (unsigned int)((float)BLADE_PENETRATION * (1.0F - charge)); //SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "damage: %u penetration: %u", p->blade.damage, p->blade.penetration);
				charge = 0xF.Fp-4F;
				SetShiftToPosition(&p->blade, &step_shift, *(blade_moves + chain), steps = 128);
				chain_next = (chain_next + 1U) % SDL_arraysize(sizes);
				freehand = false;
				return;
			}
		}else if(++idle_ticks > 192U){
			chain_next = 0U;
			SetShiftToBase(&p->blade, &step_shift, steps = 384);
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
			SetShiftToPosition(&p->blade, &step_shift, *(blade_moves + chain_next), steps = 192);
		}else{
			SetShiftToPosition(&p->blade, &step_shift, *(blade_moves + chain) + ++key, steps = 32);
		}
		step = 0;
	}else{
		ShiftBlade(&p->blade, &step_shift);
		if(!freehand && key > 1 && (freehand = UnleashDestruction(p))){
			abide = false;
			p->blade.hits = 0U;
			p->blade.direction = chain == 1 ? p->blade.direction - 0.375F : p->blade.direction + 0.375F;
			SetShiftToPosition(&p->blade, &step_shift, *(blade_moves + chain_next), steps = 512);
			step = 0;
		}
	}
	++step;
}

extern inline bool PointInPlayer(const float x, const float y, Player* const pl){
	// if(SDL_sqrtf((x - pl->position.x) * (x - pl->position.x) + (y - pl->position.y) * (y - pl->position.y)) < (float)PLAYER_SIZE * 0.5F){
	if(pow2(x - pl->position.x) + pow2(y - pl->position.y) < (float)pow2(PLAYER_SIZE * 0.5F)){
		return true;
	}
	return false;
}

extern inline void DamagePlayer(Player* const p, const int damage){
	p->hit_points -= damage;
}