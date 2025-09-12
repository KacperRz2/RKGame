#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Projectile.h>
#include <Being.h>
#include <World.h>
#include <Player.h>
#include <function.h>
#include <enum.h>

void DestroyProjectiles(Projectiles_array* const prs){
    SDL_free(prs->array);
	prs->num = 0U;
}

static inline bool ProjectileHitsBeing(Projectile* const pr, Being* const b){
	if(SDL_fabsf(pr->position.x - b->position.x) < half(b->type->size) && SDL_fabsf(pr->position.y - b->position.y) < half(b->type->size)){
		for(unsigned int i = pr->hits; i > 0U; --i){
			if(*(pr->hit_targets + (i - 1U)) == b->id){
				return false;
			}
		}
		return true;
	}
	return false;
}

extern inline void AddProjectileToArray(Projectiles_array* const prs, const SDL_FPoint* const position, const float direction, const float velocity, const int damage, const unsigned int penetration){
	Projectile* pr = (prs->array + prs->num++);
	pr->position = *position;
	pr->shift_per_tick.x = SineSafe(direction) * velocity;
	pr->shift_per_tick.y = -CosiSafe(direction) * velocity;
	pr->damage = damage;
	pr->penetration = penetration;
	pr->hits = 0;
}

static inline void DestroyProjectileInArray(Projectiles_array* const prs, const unsigned int indx){
	if(indx != prs->num - 1){
		*(prs->array + indx) = *(prs->array + prs->num - 1);
	}
	--prs->num;
}

static inline void MoveProjectile(Projectile* const pr){
	pr->position.x += pr->shift_per_tick.x;
	pr->position.y += pr->shift_per_tick.y;
}

void UpdateProjectiles(Game_data* const g_d, Segment* const player_seg){
	for(Projectile* pr = g_d->projectiles.array; pr != (g_d->projectiles.array + g_d->projectiles.num); ++pr){
		MoveProjectile(pr);
		Segment* s = GetSegment(&g_d->world, pr->position.x, pr->position.y);
		if(s == NULL || SDL_abs(player_seg->indx.x - s->indx.x) > PROJECTILE_RAN_SEG || SDL_abs(player_seg->indx.y - s->indx.y) > PROJECTILE_RAN_SEG){
			*pr = *(g_d->projectiles.array + g_d->projectiles.num-- - 1U);
			--pr;
			continue;
		}
		for(unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c){
		for(unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r){
			Segment* neighbour = GetSegmentByIndx(&g_d->world, c, r);
			if(neighbour == NULL) continue;
			for(unsigned int j = 0U; j < neighbour->beings.num; ++j){
				Being* b = *(neighbour->beings.array + j);
				if(!ProjectileHitsBeing(pr, b)) continue;
				if(DamageBeing(b, pr->damage)){
					if(pr->hits < --pr->penetration){
						--j;
						continue;
					}
				}else if(pr->hits < pr->penetration){
					*(pr->hit_targets + pr->hits++) = b->id;
					continue;
				}
				*pr = *(g_d->projectiles.array + g_d->projectiles.num-- - 1U);
				--pr;
				goto outside;	//to next projectile
			}
		}}
		outside:;
	}
}

//

void DestroyHProjectiles(Projectiles_h_array* const prs){
    SDL_free(prs->array);
	prs->num = 0U;
}

static inline bool ProjectileHitsPlayerOrLost(Projectile_hostile* const pr, Player* const p){
	float distance_squated = pow2(pr->position.x - p->position.x) + pow2(pr->position.y - p->position.y);
	if(distance_squated < pow2(half(PLAYER_SIZE))){
		if(p->control_flags & block && (sine(p->direction) * pr->shift_per_tick.x) + (-cosi(p->direction) * pr->shift_per_tick.y) <= 0){
			HitBarrier(p, pr->damage);
		}else{
			DamagePlayer(p, pr->damage);
		}
		return true;
	}
	if(distance_squated > pow2(SEGMENT_SIZE * PROJECTILE_RAN_SEG)){
		return true;
	}
	return false;
}

extern inline void AddHProjectileToArray(Projectiles_h_array* const prs, const SDL_FPoint* const position, const float direction, const float velocity, const int damage){
	Projectile_hostile* pr = (prs->array + prs->num++);
	pr->position = *position;
	pr->shift_per_tick.x = SineSafe(direction) * velocity;
	pr->shift_per_tick.y = -CosiSafe(direction) * velocity;
	pr->damage = damage;
}

static inline void DestroyHProjectileInArray(Projectiles_h_array* const prs, const unsigned int indx){
	if(indx != prs->num - 1){
		*(prs->array + indx) = *(prs->array + prs->num - 1);
	}
	--prs->num;
}

static inline void MoveHProjectile(Projectile_hostile* const pr){
	pr->position.x += pr->shift_per_tick.x;
	pr->position.y += pr->shift_per_tick.y;
}

void UpdateHProjectiles(Game_data* const g_d){
	for(Projectile_hostile* pr = g_d->h_projectiles.array; pr != (g_d->h_projectiles.array + g_d->h_projectiles.num); ++pr){
		MoveHProjectile(pr);
		if(ProjectileHitsPlayerOrLost(pr, &g_d->pc)){
			*pr = *(g_d->h_projectiles.array + g_d->h_projectiles.num-- - 1U);
			--pr;
			continue;
		}
		if(GetSegment(&g_d->world, pr->position.x, pr->position.y) == NULL){
			*pr = *(g_d->h_projectiles.array + g_d->h_projectiles.num-- - 1U);
			--pr;
		}
	}
}