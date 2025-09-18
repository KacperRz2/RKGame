#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Projectile.h>
#include <Being.h>
#include <World.h>
#include <Player.h>
#include <function.h>
#include <enum.h>

const bool (*update[])(Projectile* const, Game_data* const) = PROJECTILES_UPDATE_FUNC;

static inline void UpdateProjectile(Projectile* const pr, Game_data* const g_d){
	(*(update + pr->type_id))(pr, g_d);
}

void DestroyProjectiles(Projectiles_array* const prs){
    SDL_free(prs->array);
	prs->num = 0U;
}

static inline bool ProjectileHitsBeing(Projectile* const pr, Being* const b){
	if(SDL_fabsf(pr->position.x - b->position.x) < half(BeingSize(b)) && SDL_fabsf(pr->position.y - b->position.y) < half(BeingSize(b))){
		for(unsigned int i = pr->data.penetrating.hits; i > 0U; --i){
			if(*(pr->data.penetrating.hit_targets + (i - 1U)) == b->id){
				return false;
			}
		}
		return true;
	}
	return false;
}

extern inline void AddPCProjectileToArray(Projectiles_array* const prs, const SDL_FPoint* const position, const float direction, const float velocity, const int damage, const unsigned int penetration){
	Projectile* pr = (prs->array + prs->num++);
	pr->type_id = projectile_penetrat;
	pr->position = *position;
	pr->shift_per_tick = (SDL_FPoint){SineSafe(direction) * velocity, -CosiSafe(direction) * velocity};
	pr->data.penetrating.impact = (Impact){damage, 1.0F, 0, 1.0F};
	pr->data.penetrating.penetration = penetration;
	pr->data.penetrating.hits = 0;
}

extern inline void AddHProjectileToArray(Projectiles_array* const prs, const SDL_FPoint* const position, const float direction, const float velocity, const Impact* const impact){
	Projectile* pr = (prs->array + prs->num++);
	pr->type_id = projectile_hostile;
	pr->position = *position;
	pr->shift_per_tick = (SDL_FPoint){SineSafe(direction) * velocity, -CosiSafe(direction) * velocity};
	pr->data.basic.impact = *impact;
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

void UpdateProjectiles(Game_data* const g_d){
	for(Projectile* pr = g_d->projectiles.array; pr != (g_d->projectiles.array + g_d->projectiles.num); ++pr){
		if(!((*(update + pr->type_id))(pr, g_d))){
			--pr;
		}
	}
}

static bool UpdatePCProjectile(Projectile* const pr, Game_data* const g_d){
		MoveProjectile(pr);
		Segment* s = GetSegment(&g_d->world, pr->position.x, pr->position.y);
		if(s == NULL || SDL_abs(g_d->pc.segment->indx.x - s->indx.x) > PROJECTILE_RAN_SEG || SDL_abs(g_d->pc.segment->indx.y - s->indx.y) > PROJECTILE_RAN_SEG){
			*pr = *(g_d->projectiles.array + g_d->projectiles.num-- - 1U);
			return false;
		}
		for(unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c){
		for(unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r){
			Segment* neighbour = GetSegmentByIndx(&g_d->world, c, r);
			if(neighbour == NULL) continue;
			for(unsigned int j = 0U; j < neighbour->beings.num; ++j){
				Being* b = *(neighbour->beings.array + j);
				if(!ProjectileHitsBeing(pr, b)) continue;
				if(DamageBeing(b, pr->data.penetrating.impact.damage)){
					if(pr->data.penetrating.hits < --pr->data.penetrating.penetration){
						--j;
						continue;
					}
				}else{
					if(b->status == being_strike){
						ResetBeingBlade(b);
					}
					if(b->status != being_fly){
						StunBeing(b, BEING_DEFAULT_LEFT_TICKS);
					}
					if(pr->data.penetrating.hits < pr->data.penetrating.penetration){
						*(pr->data.penetrating.hit_targets + pr->data.penetrating.hits++) = b->id;
						continue;
					}
				}
				*pr = *(g_d->projectiles.array + g_d->projectiles.num-- - 1U);
				return false;
			}
		}}
		return true;
}

static bool UpdateHostileProjectile(Projectile* const pr, Game_data* const g_d){
	MoveProjectile(pr);
	if(ProjectileHitsPlayerOrLost(pr, &g_d->pc) || GetSegment(&g_d->world, pr->position.x, pr->position.y) == NULL){
		*pr = *(g_d->projectiles.array + g_d->projectiles.num-- - 1U);
		return false;
	}
	return true;
}
static bool UpdateSpecialProjectile(Projectile* const pr, Game_data* const g_d){}

static inline bool ProjectileHitsPlayerOrLost(Projectile* const pr, Player* const p){
	float distance_squated = pow2(pr->position.x - p->position.x) + pow2(pr->position.y - p->position.y);
	if(distance_squated < pow2(half(PLAYER_SIZE))){
		if(p->control_flags & block && (sine(p->direction) * pr->shift_per_tick.x) + (-cosi(p->direction) * pr->shift_per_tick.y) <= 0){
			HitBarrier(p, pr->data.basic.impact.damage);
		}else{
			DamagePlayer(p, pr->data.basic.impact.damage);
		}
		return true;
	}
	if(distance_squated > pow2(SEGMENT_SIZE * PROJECTILE_RAN_SEG)){
		return true;
	}
	return false;
}