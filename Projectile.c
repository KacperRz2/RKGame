#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Projectile.h>
#include <Being.h>
#include <World.h>
#include <Player.h>
#include <function.h>
#include <enum.h>

void DestroyProjectiles(Projectiles_array* const prs) {
    SDL_free(prs->array);
	prs->num = 0U;
}

inline bool ProjectileHitsBeing(Projectile* const pr, Being* const b) {
	if (SDL_fabsf(pr->position.x - b->position.x) < PLAYER_SIZE * 0.5F && SDL_fabsf(pr->position.y - b->position.y) < PLAYER_SIZE * 0.5F) {
		for(unsigned int i = pr->hits; i > 0U; --i){
			if (*(pr->hit_targets + (i - 1U)) == b->id) {
				return false;
			}
		}
		return true;
	}
	return false;
}

extern inline void AddProjectileToArray(Projectiles_array* const prs, const SDL_FPoint* const position, const float direction, const float velocity, const int damage, const unsigned int penetration) {
	Projectile* pr = (prs->array + prs->num++);
	pr->position = *position;
	pr->shift_per_tick.x = SineSafe(direction) * velocity;
	pr->shift_per_tick.y = -CosiSafe(direction) * velocity;
	pr->damage = damage;
	pr->penetration = penetration;
	pr->hits = 0;
	// ++prs->num;
}

inline void DestroyProjectileInArray(Projectiles_array* const prs, const unsigned int indx) {
	if (indx != prs->num - 1) {
		*(prs->array + indx) = *(prs->array + prs->num - 1);
	}
	--prs->num;
}

inline void MoveProjectile(Projectile* const pr) {
	pr->position.x += pr->shift_per_tick.x;
	pr->position.y += pr->shift_per_tick.y;
}

void UpdateProjectiles(Projectiles_array* const prs, Segment* const player_seg) {
	for (Projectile* pr = prs->array; pr != (prs->array + prs->num); ++pr) {
		MoveProjectile(pr);
		Segment* s = GetSegment(pr->position.x, pr->position.y);
		if(s == NULL || SDL_abs(player_seg->indx.x - s->indx.x) > PROJECTILE_RAN_SEG || SDL_abs(player_seg->indx.y - s->indx.y) > PROJECTILE_RAN_SEG){
			*pr = *(prs->array + prs->num-- - 1U);
			--pr;
			continue;
		}
		for (unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c) {
		for (unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r) {
			Segment* neighbour = GetSegmentByIndx(c, r);
			if(neighbour == NULL) continue;
			static unsigned int mark = 1U;
			for (unsigned int j = 0U; j < neighbour->beings.num; ++j) {
				Being* b = *(neighbour->beings.array + j);
				if (ProjectileHitsBeing(pr, b)) {
					if(DamageBeing(b, pr->damage)){
						--j;
					}
					if(pr->hits < pr->penetration){
						*(pr->hit_targets + pr->hits++) = b->id;
					}else{
						*pr = *(prs->array + prs->num-- - 1U);
						--pr;
						goto outside;
					}
				}
			}
		}}
		outside:;
	}
}

//

void DestroyHProjectiles(Projectiles_h_array* const prs) {
    SDL_free(prs->array);
	prs->num = 0U;
}

inline bool ProjectileHitsPlayerOrLost(Projectile_hostile* const pr, Player* const p) {
	// if (SDL_fabsf(pr->position.x - p->position.x) < PLAYER_SIZE * 0.5F && SDL_fabsf(pr->position.y - p->position.y) < PLAYER_SIZE * 0.5F) {
	float distance_squated = pow2(pr->position.x - p->position.x) + pow2(pr->position.y - p->position.y);
	if (distance_squated < pow2(PLAYER_SIZE * 0.5F)) {
		DamagePlayer(p, pr->damage);
		return true;
	}
	if (distance_squated > pow2(SEGMENTS_SIZE * PROJECTILE_RAN_SEG)) {
		return true;
	}
	return false;
}

extern inline void AddHProjectileToArray(Projectiles_h_array* const prs, const SDL_FPoint* const position, const float direction, const float velocity, const int damage) {
	Projectile_hostile* pr = (prs->array + prs->num++);
	pr->position = *position;
	pr->shift_per_tick.x = SineSafe(direction) * velocity;
	pr->shift_per_tick.y = -CosiSafe(direction) * velocity;
	pr->damage = damage;
}

inline void DestroyHProjectileInArray(Projectiles_h_array* const prs, const unsigned int indx) {
	if (indx != prs->num - 1) {
		*(prs->array + indx) = *(prs->array + prs->num - 1);
	}
	--prs->num;
}

inline void MoveHProjectile(Projectile_hostile* const pr) {
	pr->position.x += pr->shift_per_tick.x;
	pr->position.y += pr->shift_per_tick.y;
}

void UpdateHProjectiles(Projectiles_h_array* const prs, Player* const p) {
	for (Projectile_hostile* pr = prs->array; pr != (prs->array + prs->num); ++pr) {
		MoveHProjectile(pr);
		if (ProjectileHitsPlayerOrLost(pr, p)) {
			*pr = *(prs->array + prs->num-- - 1U);
			--pr;
			continue;
		}
		if(GetSegment(pr->position.x, pr->position.y) == NULL){
			*pr = *(prs->array + prs->num-- - 1U);
			--pr;
		}
	}
}