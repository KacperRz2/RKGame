#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Projectile.h>
#include <Player.h>
#include <function.h>
#include <World.h>
#include <Being.h>

// bool tmp = false;

void CreatePlayer(Player* p, const float x, const float y) {
	// Player p = (Player*)SDL_malloc(sizeof(Player));
	// if (p == NULL) return NULL;
	p->blade.position.x = 16.0F;
 	p->blade.position.y = -8.0F;
	p->blade.direction = SDL_PI_F * 0.45F;
	p->blade.damage = BLADE_DAMAGE;
	p->blade.penetration = BLADE_PENETRATION;
	// p->blade.hit_targets = (void**)SDL_malloc(sizeof(void*) * BLADE_PENETRATION);
	// for(unsigned int i = 0U; i < BLADE_PENETRATION; ++i){
	// 	*(p->blade.hit_targets + i) = NULL;
	// }
	p->blade.hits = 0;
	p->position.x = x;
	p->position.y = y;
	p->control_flags = 0x00000000U;
	p->direction = 0.0F;
	p->velocity = 0.0F;
	p->max_velocity = PLAYER_VELOCITY;
	p->hit_points = 100;
	p->fatigue_points = 900;
	p->magic_points = 100;
	p->range = 700.0F;
	p->max_fatigue = 1000;
	p->max_h_p = 100;
	p->max_magic = 100;
	p->fatigue_block_time = 0;
	p->armour = 0.875F;
	// return p;
}

inline void SetPlayerPosition(Player* p, const float x, const float y) {
	p->position.x = x;
	p->position.y = y;
}

extern inline void MovePlayer(Player* p, const float x, const float y) {
	float new_x = p->position.x + x;
	float new_y = p->position.y + y;

	if(GetSegment(new_x, new_y)->available){
		SetPlayerPosition(p, new_x, new_y);
	}else if(GetSegment(p->position.x, new_y)->available){
		p->position.y = new_y;
	}else if(GetSegment(new_x, p->position.y)->available){
		p->position.x = new_x;
	}
	// Segment* s = GetSegment(new_x, new_y);
	// if(!s->available) {
	// 	s = GetSegment(p->position.x, new_y);
	// 	if(!s->available){
	// 		s = GetSegment(new_x, p->position.y);
	// 		if(!s->available){
	// 			return;
	// 		}else{
	// 			new_y = p->position.y;
	// 		}
	// 	}else{
	// 		new_x = p->position.x;
	// 	}
	// }
	// for (unsigned int i = 0U; i < s->beings.num; ++i) {
    //     Being* b = *(s->beings.array + i);
	// 	if(CollideWithBeing(b, new_x, new_y)){
	// 		return;
	// 	}
	// }
	// SetPlayerPosition(p, new_x, new_y);

	// float new_x = p->position.x + x;
	// float new_y = p->position.y + y;
	// int old = 0b00;
	// Segment* s = GetSegment(new_x, new_y);
	// if(!s->available) {
	// 	s = GetSegment(p->position.x, new_y);
	// 	if(!s->available){
	// 		s = GetSegment(new_x, p->position.y);
	// 		if(!s->available){
	// 			return;
	// 		}else{
	// 			new_y = p->position.y;
	// 			old = 0b10;
	// 		}
	// 	}else{
	// 		new_x = p->position.x;
	// 		old = 0b01;
	// 	}
	// }
	// for (unsigned int c = s->indx.x - 1U; c < s->indx.x + 2U; ++c) {
	// 	for (unsigned int r = s->indx.y - 1U; r < s->indx.y + 2U; ++r) {
	// 		Segment* neighbour = GetSegmentByIndx(c, r);
	// 		for (unsigned int i = 0U; i < neighbour->beings.num; ++i) {
	// 			Being* b = *(neighbour->beings.array + i);
	// 			if(CollideWithBeing(b, new_x, new_y)){
	// 				if(old == 0b00){
	// 					if(CollideWithBeing(b, p->position.x, new_y)){
	// 						if(CollideWithBeing(b, new_x, p->position.y)){
	// 							// new_y = p->position.y - 0.1F * (new_y - p->position.y);
	// 							// new_x = p->position.x - 0.1F * (new_x - p->position.x);
	// 							old = 0b11;
	// 							// SetPlayerPosition(p, new_x, new_y);
	// 							return;
	// 						}else{
	// 							// new_y = p->position.y - 0.1F * (new_y - p->position.y);
	// 							old |= 0b10;
	// 						}
	// 					}else{
	// 						// new_x = p->position.x - 0.1F * (new_x - p->position.x);
	// 						old |= 0b01;
	// 					}
	// 				}
	// 				if(old == 0b11){
	// 					// float distance_x = p->position.x - b->position.x;
	// 					// float distance_y = p->position.y - b->position.y;
	// 					// float distance = SDL_sqrtf(distance_x * distance_x + distance_y * distance_y);
	// 					// float velocity_xy = distance / PLAYER_VELOCITY;
	// 					// // float x_shift = distance_x / velocity_xy;
	// 					// // float y_shift = distance_y / velocity_xy;
	// 					// float new_x = p->position.x + distance_x / velocity_xy;
	// 					// float new_y = p->position.y + distance_y / velocity_xy;
	// 					// SetPlayerPosition(p, new_x, new_y);
	// 					return;
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	// for (unsigned int i = 0U; i < s->beings.num; ++i) {
    //     Being* b = *(s->beings.array + i);
		
	// }
	// if(old == 0b00){
	// 	SetPlayerPosition(p, new_x, new_y);
	// }else if(old == 0b01){
	// 	p->position.y = new_y;
	// }else{
	// 	p->position.x = new_x;
	// }
}

// inline void SetPlayerInBounds(Player* p) {
// 	if (p->position.x >= BOUNDS_R) {
// 		p->position.x = BOUNDS_R - 1.0F;
// 	}
// 	else if (p->position.x <= BOUNDS_L) {
// 		p->position.x = BOUNDS_L + 1.0F;
// 	}
// 	if (p->position.y >= BOUNDS_D) {
// 		p->position.y = BOUNDS_D - 1.0F;
// 	}
// 	else if (p->position.y <= BOUNDS_U) {
// 		p->position.y = BOUNDS_U + 1.0F;
// 	}
// }

inline void UpdatePlayerMove(Player* p) {
	static float move_direction = 0.0F;
	if (p->velocity > p->max_velocity) {
		p->velocity *= DECELERATION;
	}
	else if ((p->control_flags & 0b00001111U) == 0b00000001U || (p->control_flags & 0b00001111U) == 0b00001101U) {
		p->velocity += ACCELERATION;
		move_direction = p->direction;
	}
	else if ((p->control_flags & 0b00001111U) == 0b00000010U) {
		p->velocity = (p->velocity + ACCELERATION) * 0.93F;
		move_direction = p->direction + SDL_PI_F;
	}
	else if ((p->control_flags & 0b00001111U) == 0b00000100U) {
		p->velocity += ACCELERATION;
		move_direction = p->direction + SDL_PI_F * 0.5F;
	}
	else if ((p->control_flags & 0b00001111U) == 0b00001000U) {
		p->velocity += ACCELERATION;
		move_direction = p->direction + SDL_PI_F * 1.5F;
	}
	else if ((p->control_flags & 0b00001111U) == 0b00000101U) {
		p->velocity += ACCELERATION;
		move_direction = p->direction + SDL_PI_F * 0.25F;
	}
	else if ((p->control_flags & 0b00001111U) == 0b00001001U) {
		p->velocity += ACCELERATION;
		move_direction = p->direction + SDL_PI_F * 1.75F;
	}
	else if ((p->control_flags & 0b00001111U) == 0b00000110U) {
		p->velocity = (p->velocity + ACCELERATION) * 0.93F;
		move_direction = p->direction + SDL_PI_F * 0.75F;
	}
	else if ((p->control_flags & 0b00001111U) == 0b00001010U) {
		p->velocity = (p->velocity + ACCELERATION) * 0.93F;
		move_direction = p->direction + SDL_PI_F * 1.25F;
	}
	else {
		p->velocity *= DECELERATION;
		if (p->velocity < 0.05F) p->velocity = 0.0F;
	}
	if (!(p->control_flags & 1 << 0) && p->control_flags & 1 << 4) {
		if (p->fatigue_points > 99) {
			p->fatigue_points -= 100;
			p->fatigue_block_time = 50;
			if (!(p->control_flags & 0b00001111U)) {
				move_direction = p->direction + SDL_PI_F;
			}
			p->velocity = 10.0F;
		}
		else {
			p->fatigue_block_time += 2;
			if (!(p->control_flags & 0b00001111U)) {
				move_direction = p->direction + SDL_PI_F;
			}
			p->velocity = 1.0F;
		}
		p->control_flags &= ~(1 << 4);
	}
	if ((p->control_flags & 0b00100011U) == 0b00100001U) {
		if (p->fatigue_points <= 1) {
			p->max_velocity = PLAYER_VELOCITY;
			p->control_flags &= ~(1 << 5);
		}
		else {
			p->max_velocity = RUN_VELOCITY;
			--(p->fatigue_points);
		}
	}
	else {
		p->max_velocity = PLAYER_VELOCITY;
	}
	if (p->velocity > 0.0F) {
		MovePlayer(p, SDL_sinf(move_direction) * p->velocity, -SDL_cosf(move_direction) * p->velocity);
	}
	// SetPlayerInBounds(p);
}

inline void UpdatePlayerDirection(Player* p) {
	if (p->direction > 2.0F * SDL_PI_F) {
		p->direction -= 2.0F * SDL_PI_F;
	}
	else if (p->direction < 0.0F) {
		p->direction += 2.0F * SDL_PI_F;
	}
}

extern inline void DestroyPlayer(Player* p) {
	SDL_free(p->blade.hit_targets);
	SDL_free(p);
}

inline void UpdatePlayerPoints(Player* p) {
	static unsigned int count = 4U;
	if (count <= 1) {
		if (p->fatigue_block_time > 0) {
			--(p->fatigue_block_time);
		}else if (p->fatigue_points < p->max_fatigue) {
			++(p->fatigue_points);
		}
		count = 4U;
	}
	--count;
}

extern inline void UpdatePlayer(Player* p, Projectiles_array* prs) {
	if (p->control_flags & 1 << 8) {
		if (prs->num < MAX_PROJECTILES_NUM) {
			AddProjectileToArray(prs, &p->position, p->direction + 0.25F * (SDL_randf() - 0.5F), 3.0F, 10, 3U);
		}
	}
	UpdatePlayerBlade(p);
	UpdatePlayerDirection(p);
	UpdatePlayerMove(p);
	UpdatePlayerPoints(p);
}

inline void ShiftBlade(Blade* bl, Status_frame* shift){
		bl->position.x += shift->position.x;
		bl->position.y += shift->position.y;
		bl->direction += shift->direction;
}

inline void SetBladePosition(Blade* bl, const Status_frame* position){
		bl->position.x = position->position.x;
		bl->position.y = position->position.y;
		bl->direction = position->direction;
}

// inline void SetShift(Status_frame* step_shift, const Status_frame* frame, const unsigned int steps){
// 	step_shift->position.x = ((frame + 1)->position.x - frame->position.x) / steps;
// 	step_shift->position.y = ((frame + 1)->position.y - frame->position.y) / steps;
// 	step_shift->direction = ((frame + 1)->direction - frame->direction) / steps;
// }

// inline void SetShiftToBase(Status_frame* step_shift, const Status_frame* frame, const unsigned int steps){
// 	step_shift->position.x = (16.0F - frame->position.x) / steps;
// 	step_shift->position.y = (8.0F - frame->position.y) / steps;
// 	step_shift->direction = (SDL_PI_F * 0.45F - frame->direction) / steps;
// }

inline void SetShiftToBase(Blade* bl, Status_frame* step_shift, const unsigned int steps){
	step_shift->position.x = (16.0F - bl->position.x) / steps;
	step_shift->position.y = (-8.0F - bl->position.y) / steps;
	step_shift->direction = (SDL_PI_F * 0.45F - bl->direction) / steps;
}

inline void SetShiftToPosition(Blade* bl, Status_frame* step_shift, const Status_frame* frame, const unsigned int steps){
	step_shift->position.x = (frame->position.x - bl->position.x) / steps;
	step_shift->position.y = (frame->position.y - bl->position.y) / steps;
	step_shift->direction = (frame->direction - bl->direction) / steps;
}

inline Status_frame GetBladeLocation(Player* p, float* sine, float* cosine){
	float direct = p->direction + p->blade.direction;
	*sine = SineSafe(direct);
	*cosine = CosiSafe(direct);
	Status_frame ret = {{p->position.x + *sine * p->blade.position.x, p->position.y - *cosine * p->blade.position.y}, direct};
	return ret;
}

inline bool BladeHitsBeing(Blade* bl, Status_frame* location, Being* b, SDL_FPoint* dangerous_points) {
	// static const float length_part = BLADE_SIZE * 0.85F * 0.5F;
	// float shift_x = SineSafe(location->direction) * length_part;
	// float shift_y = -CosiSafe(location->direction) * length_part;
	// float shift_x = SDL_sinf(location->direction) * length_part;
	// float shift_y = -SDL_cosf(location->direction) * length_part;
	for (unsigned int i = 0U; i < 2U; ++i) {
		// SDL_FPoint dangerous_point = {location->position.x + i * shift_x, location->position.y + i * shift_y};// if(tmp) SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "%f, %f", dangerous_point.x, dangerous_point.y);
		if (SDL_fabsf((dangerous_points + i)->x - b->position.x) < PLAYER_SIZE * 0.5F) {
			if (SDL_fabsf((dangerous_points + i)->y - b->position.y) < PLAYER_SIZE * 0.5F) {// int tmp = 0;
				for(unsigned int j = bl->hits; j > 0U; --j){
					if (*(bl->hit_targets + (j - 1U)) == b) {// SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "same! %d!", tmp);
						return false;
					}// ++tmp;
				}
				return true;
			}
		}
	} //SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "x!");
	return false;
}

inline bool UnleashDestruction(Player* p){
	static const float length_part = BLADE_SIZE * 0.85F * 0.5F;
	float sine_blade_direction;
	float cosine_blade_direction;
	Status_frame blade_true_location = GetBladeLocation(p, &sine_blade_direction, &cosine_blade_direction); //if(tmp) SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "%f, %f, %f", blade_true_location.position.x, blade_true_location.position.y, blade_true_location.direction);
	Segment* s = GetSegment(blade_true_location.position.x, blade_true_location.position.y);
	if(!s->available){
		return true;
	} //if(tmp) {SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "####################");}
	float shift_x = sine_blade_direction * length_part;
	float shift_y = -cosine_blade_direction * length_part;
	SDL_FPoint dangerous_points[] = {
		{blade_true_location.position.x + 2.0F * shift_x, blade_true_location.position.y + 2.0F * shift_y},
		{blade_true_location.position.x + shift_x, blade_true_location.position.y + shift_y}
	};
	for (unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c) {
		for (unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r) {
			Segment* neighbour = GetSegmentByIndx(c, r);// SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "/bla: %d, %d", neighbour->indx.x, neighbour->indx.y);
			if(!neighbour->available) continue;
			for (unsigned int i = 0U; i < neighbour->beings->num; ++i) {
				Being* b = *(neighbour->beings->array + i); //if(tmp) SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "@bei: %f, %f", b->position.x, b->position.y);
				if (BladeHitsBeing(&p->blade, &blade_true_location, b, dangerous_points)) {
					DamageBeing(b, p->blade.damage);
					if(p->blade.hits < p->blade.penetration){
						*(p->blade.hit_targets + p->blade.hits++) = b;
					}else{
						return true;// goto outside;
					}
				}
			}
		}
	}
	return false;// outside:;
}

// inline void ClearBlade(Blade* bl){
// 	bl->
// }

inline void UpdatePlayerBlade(Player* p){
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
	static unsigned int press_count = 0U;
	static bool start = false;
	static bool abide = false;
	static bool freehand = false;
	static unsigned int idle_ticks = 0U;
	// static bool blocked = false;

	if(p->control_flags & 1 << 7) {
		start = true;
	}
	if(!abide){
		if(start){
			if(!(p->control_flags & 1 << 7)) {
				chain = chain_next;
				key = 0;
				step = 0;
				abide = true;
				idle_ticks = 0U;
				start = false;
				p->blade.hits = 0U;
				// SetBladePosition(&p->blade, *(blade_moves + chain) + key);
				SetShiftToPosition(&p->blade, &step_shift, *(blade_moves + chain), steps = 128);
				// SetShift(&step_shift, *(blade_moves + chain) + key++, steps = 64);
				chain_next = (chain_next + 1) % SDL_arraysize(sizes);
				freehand = false;
				return;
			}
		}
		else if(++idle_ticks > 192U){
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
			// SetShiftToBase(&p->blade, &step_shift, steps = 384);
			SetShiftToPosition(&p->blade, &step_shift, *(blade_moves + chain_next), steps = 192);
		}else{
			SetShiftToPosition(&p->blade, &step_shift, *(blade_moves + chain) + ++key, steps = 32);
			// SetBladePosition(&p->blade, *(blade_moves + chain) + key);
			// SetShift(&step_shift, *(blade_moves + chain) + key++, steps);
		}
		step = 0;
	}else{
		ShiftBlade(&p->blade, &step_shift); //if(chain == 2 && key == 2) tmp = true; else tmp = false;
		if(!freehand) {
			if(key > 1) {
				if(freehand = UnleashDestruction(p)){
					abide = false;
					p->blade.hits = 0U;
					p->blade.direction = chain == 1 ? p->blade.direction - 0.375F : p->blade.direction + 0.375F;
					// SetShiftToBase(&p->blade, &step_shift, steps = 512);
					SetShiftToPosition(&p->blade, &step_shift, *(blade_moves + chain_next), steps = 512);
					step = 0;
				}
			}
		}
	}
	++step;
}