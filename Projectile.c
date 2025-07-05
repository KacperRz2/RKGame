#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Projectile.h>
#include <Being.h>
#include <World.h>
#include <function.h>

Projectile* CreateProjectile(const SDL_FPoint* position, const float direction, const float velocity) {
	Projectile* pr = (Projectile*)SDL_malloc(sizeof(Projectile));
	if (pr == NULL) return NULL;
	pr->position = *position;
	// pr->shift_per_tick.x = SDL_sinf(direction) * velocity;
	// pr->shift_per_tick.y = -SDL_cosf(direction) * velocity;
	pr->shift_per_tick.x = SineSafe(direction) * velocity;
	pr->shift_per_tick.y = -CosiSafe(direction) * velocity;
	// pr->time_left = 0x00000200U;
	pr->damage = 10;
	return pr;
}

inline void DestroyProjectile(Projectile* pr) {
	SDL_free(pr);
}

void DestroyProjectiles(Projectiles_array* prs) {
	for (unsigned int i = 0U; i < prs->num; ++i) {
		Projectile* pr = *(prs->array + i);
		DestroyProjectile(pr);
	}
	prs->num = 0U;
}

inline bool ProjectileHitsBeing(Projectile* pr, Being* b) {
	if (SDL_fabsf(pr->position.x - b->position.x) < PLAYER_SIZE * 0.5F) {
		if (SDL_fabsf(pr->position.y - b->position.y) < PLAYER_SIZE * 0.5F) {
			return true;
		}
	}
	return false;
}

extern inline void AddProjectileToArray(Projectiles_array* prs, Projectile* pr) {
	*(prs->array + prs->num) = pr;
	++prs->num;
}

inline void DestroyProjectileInArray(Projectiles_array* prs, const unsigned int indx) {
	DestroyProjectile(*(prs->array + indx));
	if (indx != prs->num - 1) {
		*(prs->array + indx) = *(prs->array + prs->num - 1);
	}
	--prs->num;
}

inline void MoveProjectile(Projectile* pr) {
	pr->position.x += pr->shift_per_tick.x;
	pr->position.y += pr->shift_per_tick.y;
}

void UpdateProjectiles(Projectiles_array* prs) {
	for (unsigned int i = 0U; i < prs->num; ++i) {
		Projectile* pr = *(prs->array + i);
		// if (pr->time_left <= 1U) {
		// 	DestroyProjectileInArray(prs, i);
		// 	--i;
		// 	continue;
		// }
		// else {
		// 	--pr->time_left;
		// }
		MoveProjectile(pr);
		// if (!InBounds(&pr->position)) {
		// 	DestroyProjectileInArray(prs, i);
		// 	--i;
		// 	continue;
		// }
		Segment* s = GetSegment(pr->position.x, pr->position.y);
		if(!s->available){
			DestroyProjectileInArray(prs, i);
			--i;
			continue;
		}
		for (unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c) {
			for (unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r) {
				Segment* neighbour = GetSegmentByIndx(c, r);
				for (unsigned int j = 0U; j < neighbour->beings.num; ++j) {
					Being* b = *(neighbour->beings.array + j);
					if (ProjectileHitsBeing(pr, b)) {
						DamageBeing(b, pr->damage);
						DestroyProjectileInArray(prs, i);
						--i;
						goto outside;
					}
				}
			}
		}
		outside:;
	}
}
