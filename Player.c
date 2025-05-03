#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Projectile.h>
#include <Player.h>

Player* createPlayer(const float x, const float y) {
	Player* p = (Player*)SDL_malloc(sizeof(Player));
	if (p == NULL) return NULL;
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

inline void movePlayer(Player* p, const float x, const float y) {
	p->position.x += x;
	p->position.y += y;
}

inline void setPlayerInBounds(Player* p) {
	if (p->position.x >= BOUNDS_R) {
		p->position.x = BOUNDS_R - 1.0F;
	}
	else if (p->position.x <= BOUNDS_L) {
		p->position.x = BOUNDS_L + 1.0F;
	}
	if (p->position.y >= BOUNDS_D) {
		p->position.y = BOUNDS_D - 1.0F;
	}
	else if (p->position.y <= BOUNDS_U) {
		p->position.y = BOUNDS_U + 1.0F;
	}
}

void updatePlayerMove(Player* p) {
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
		movePlayer(p, SDL_sinf(move_direction) * p->velocity, -SDL_cosf(move_direction) * p->velocity);
	}
	setPlayerInBounds(p);
}

inline void updatePlayerDirection(Player* p) {
	if (p->direction > 2.0F * SDL_PI_F) {
		p->direction -= 2.0F * SDL_PI_F;
	}
	else if (p->direction < 0.0F) {
		p->direction += 2.0F * SDL_PI_F;
	}
}

extern inline void destroyPlayer(Player* p) {
	SDL_free(p);
}

inline void updatePlayerPoints(Player* p) {
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

extern inline void updatePlayer(Player* p, Projectiles_array* prs) {
	if (p->control_flags & 1 << 7) {
		if (prs->num < MAX_PROJECTILES_NUM) {
			addProjectileToArray(prs, createProjectile(&p->position, p->direction + 0.25F * (SDL_randf() - 0.5F), 3.0F));
		}
	}
	updatePlayerDirection(p);
	updatePlayerMove(p);
	updatePlayerPoints(p);
}
