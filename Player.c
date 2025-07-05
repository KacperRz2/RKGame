#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Projectile.h>
#include <Player.h>
#include <function.h>
#include <World.h>

Player* CreatePlayer(const float x, const float y) {
	Player* p = (Player*)SDL_malloc(sizeof(Player));
	if (p == NULL) return NULL;
	p->blade.position.x = -(PLAYER_SIZE * 0.25F);
 	p->blade.position.y = -(PLAYER_SIZE * 1.0F);
	p->blade.direction = SDL_PI_F * 0.45F;
	p->blade.damage = BLADE_DAMAGE;
	p->blade.penetration = BLADE_PENETRATION;
	p->blade.hit_targets = (void**)SDL_malloc(sizeof(void*) * BLADE_PENETRATION);
	for(unsigned int i = 0U; i < BLADE_PENETRATION; ++i){
		*(p->blade.hit_targets + i) = NULL;
	}
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
	return p;
}

inline void SetPlayerPosition(Player* p, const float x, const float y) {
	p->position.x = x;
	p->position.y = y;
}

inline void MovePlayer(Player* p, const float x, const float y) {
	float new_x = p->position.x + x;
	float new_y = p->position.y + y;
	if (GetSegment(new_x, new_y)->available) {
		SetPlayerPosition(p, new_x, new_y);
	} else if (GetSegment(p->position.x, new_y)->available) {
		p->position.y = new_y;
	} else if (GetSegment(new_x, p->position.y)->available) {
		p->position.x = new_x;
	}

	// p->position.x += x;
	// p->position.y += y;
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

void UpdatePlayerMove(Player* p) {
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
			AddProjectileToArray(prs, CreateProjectile(&p->position, p->direction + 0.25F * (SDL_randf() - 0.5F), 3.0F, 10, 20U));
		}
	}
	
	static const Status_frame blade_key_frames[] = {
		{{-8.0F, -32.0F}, SDL_PI_F * 0.45F},
		{{-8.0F, -32.0F}, 150.0F * (SDL_PI_F / 180.0F)},
		{{-18.0F, -48.0F}, 45.0F * (SDL_PI_F / 180.0F)},
		{{-28.0F, -50.0F}, 0.0F},
		{{-28.0F, -50.0F}, -75.0F * (SDL_PI_F / 180.0F)},
		{{-38.0F, -40.0F}, -150.F * (SDL_PI_F / 180.0F)},
		{{-28.0F, -50.0F}, -70.0F * (SDL_PI_F / 180.0F)},
		{{-18.0F, -48.0F}, -25.0F * (SDL_PI_F / 180.0F)},
		{{-8.0F, -32.0F}, SDL_PI_F * 0.45F},
		{{-8.0F, -32.0F}, 150.0F * (SDL_PI_F / 180.0F)},
		{{-8.0F, -32.0F}, SDL_PI_F * 0.45F},
		{{-8.0F, -40.0F}, 0.0F},
		{{-25.0F, -64.0F}, 0.0F},
		{{-18.0F, -50.0F}, 0.0F},
		{{-8.0F, -32.0F}, SDL_PI_F * 0.45F}
		// {{0.0F, 0.0F}, 0.0F}
	};
	static Status_frame step_shift = {{0.0F, 0.0F}, 0.0};
	static int indx = 0;
	static int indx_plus = 1;
	static const int steps = 64;
	static int step = steps;
	if(step == steps){
		p->blade.position.x = (blade_key_frames + indx)->position.x;
		p->blade.position.y = (blade_key_frames + indx)->position.y;
		p->blade.direction = (blade_key_frames + indx)->direction;
		step_shift.position.x = ((blade_key_frames + indx + 1)->position.x - (blade_key_frames + indx)->position.x) / steps;
		step_shift.position.y = ((blade_key_frames + indx + 1)->position.y - (blade_key_frames + indx)->position.y) / steps;
		step_shift.direction = ((blade_key_frames + indx + 1)->direction - (blade_key_frames + indx)->direction) / steps;
		indx = (indx + 1) % (sizeof(blade_key_frames) / sizeof(Status_frame) - 1);
		step = 0;
	}else{
		p->blade.position.x += step_shift.position.x;
		p->blade.position.y += step_shift.position.y;
		p->blade.direction += step_shift.direction;
	}
	++step;

	UpdatePlayerDirection(p);
	UpdatePlayerMove(p);
	UpdatePlayerPoints(p);
}
