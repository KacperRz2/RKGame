#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Projectile.h>
#include <Being.h>
#include <World.h>
#include <function.h>

void DestroyProjectiles(Projectiles_array* const prs) {
    SDL_free(prs->array);
	prs->num = 0U;
}

inline bool ProjectileHitsBeing(Projectile* const pr, Being* const b) {
	if (SDL_fabsf(pr->position.x - b->position.x) < PLAYER_SIZE * 0.5F && SDL_fabsf(pr->position.y - b->position.y) < PLAYER_SIZE * 0.5F) {
		for(unsigned int i = pr->hits; i > 0U; --i){
			if (*(pr->hit_targets + (i - 1U)) == b) {
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
		// while(s == NULL || SDL_abs(player_seg->indx.x - s->indx.x) > PROJECTILE_RAN_SEG || SDL_abs(player_seg->indx.y - s->indx.y) > PROJECTILE_RAN_SEG){
		// 	*pr = *(prs->array + prs->num-- - 1U);
		// 	MoveProjectile(pr);
		// 	s = GetSegment(pr->position.x, pr->position.y);
		// }
		for (unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c) {
		for (unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r) {
			Segment* neighbour = GetSegmentByIndx(c, r);
			if(neighbour == NULL) continue;
			for (unsigned int j = 0U; j < neighbour->beings.num; ++j) {
				Being* b = *(neighbour->beings.array + j);
				if (ProjectileHitsBeing(pr, b)) {
					DamageBeing(b, pr->damage);
					if(pr->hits < pr->penetration){
						*(pr->hit_targets + pr->hits++) = b;
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
// void UpdateProjectiles(Projectiles_array* prs, Segment* player_seg) {
// 	for (unsigned int i = 0U; i < prs->num; ++i) {
// 		Projectile* pr = (prs->array + i);
// 		MoveProjectile(pr);
// 		Segment* s = GetSegment(pr->position.x, pr->position.y);
// 		if(s == NULL || SDL_abs(player_seg->indx.x - s->indx.x) > PROJECTILE_RAN_SEG || SDL_abs(player_seg->indx.y - s->indx.y) > PROJECTILE_RAN_SEG){
// 			DestroyProjectileInArray(prs, i--);
// 			continue;
// 		}
// 		for (unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c) {
// 		for (unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r) {
// 			Segment* neighbour = GetSegmentByIndx(c, r);
// 			if(neighbour == NULL) continue;
// 			for (unsigned int j = 0U; j < neighbour->beings.num; ++j) {
// 				Being* b = *(neighbour->beings.array + j);
// 				if (ProjectileHitsBeing(pr, b)) {
// 					DamageBeing(b, pr->damage);
// 					if(pr->hits < pr->penetration){
// 						*(pr->hit_targets + pr->hits++) = b;
// 					}else{
// 						DestroyProjectileInArray(prs, i--);
// 						goto outside;
// 					}
// 				}
// 			}
// 		}}
// 		outside:;
// 	}
// }