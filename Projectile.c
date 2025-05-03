#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Being.h>
#include <World.h>
#include <function.h>

Projectile* createProjectile(const SDL_FPoint* position, const float direction, const float velocity) {
	Projectile* pr = (Projectile*)SDL_malloc(sizeof(Projectile));
	if (pr == NULL) return NULL;
	pr->position = *position;
	pr->shift_per_tick.x = SDL_sinf(direction) * velocity;
	pr->shift_per_tick.y = -SDL_cosf(direction) * velocity;
	pr->time_left = 0x00000200U;
	pr->damage = 100;
	return pr;
}

inline void destroyProjectile(Projectile* pr) {
	SDL_free(pr);
}

void destroyProjectiles(Projectiles_array* prs) {
	for (unsigned int i = 0U; i < prs->num; ++i) {
		Projectile* pr = *(prs->array + i);
		destroyProjectile(pr);
	}
	prs->num = 0U;
}

inline bool projectileHitsBeing(Projectile* pr, Being* b) {
	if (SDL_fabsf(pr->position.x - b->position.x) < PLAYER_SIZE * 0.5F) {
		if (SDL_fabsf(pr->position.y - b->position.y) < PLAYER_SIZE * 0.5F) {
			return true;
		}
	}
	return false;
}

extern inline void addProjectileToArray(Projectiles_array* prs, Projectile* pr) {
	*(prs->array + prs->num) = pr;
	++prs->num;
}

inline void destroyProjectileInArray(Projectiles_array* prs, const unsigned int indx) {
	destroyProjectile(*(prs->array + indx));
	if (indx != prs->num - 1) {
		*(prs->array + indx) = *(prs->array + prs->num - 1);
	}
	--prs->num;
}

inline void moveProjectile(Projectile* pr) {
	pr->position.x += pr->shift_per_tick.x;
	pr->position.y += pr->shift_per_tick.y;
}

void updateProjectiles(World* w, Projectiles_array* prs) {
	for (unsigned int i = 0U; i < prs->num; ++i) {
		Projectile* pr = *(prs->array + i);
		if (pr->time_left <= 1U) {
			destroyProjectileInArray(prs, i);
			--i;
			continue;
		}
		else {
			--pr->time_left;
		}
		moveProjectile(pr);
		if (!inBounds(&pr->position)) {
			continue;
		}
		Segment* s = getSegment(w, pr->position.x, pr->position.y);
		for (unsigned int j = 0U; j < s->beings.num; ++j) {
			Being* b = *(s->beings.array + j);
			if (projectileHitsBeing(pr, b)) {
				damageBeing(b, pr->damage);
				destroyProjectileInArray(prs, i);
				--i;
				break;
			}
		}
	}
}
