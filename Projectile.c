#include <SDL3/SDL.h>
#include <macros.h>
#include <enum.h>
#include <types.h>
#include <Projectile.h>
#include <Being.h>
#include <World.h>
#include <Player.h>
#include <function.h>
#include <game.h>
#include <render.h>

const bool (*update[])(Projectile* const, Game_data* const) = PROJECTILES_UPDATE_FUNC;

static inline void UpdateProjectile(Projectile* const pr, Game_data* const gd){
	(*(update + pr->type_id))(pr, gd);
}

void DestroyProjectiles(Projectiles_array* const prs){
    SDL_free(prs->array);
	prs->num = 0U;
}

static inline bool ProjectileHitsBeing(Projectile* const pr, Being* const b){
	if(SDL_fabsf(pr->position.x - b->position.x) < half(BeingSize(b)) && SDL_fabsf(pr->position.y - b->position.y) < half(BeingSize(b))){
		for(unsigned int i = pr->data.penetrating.hits; i > 0U; --i){
			if(*(pr->data.penetrating.hit_targets + (i - 1U)) == b->main_indx){
				return false;
			}
		}
		return true;
	}
	return false;
}

extern inline void AddPCProjectileToArray(Projectiles_array* const prs, const SDL_FPoint* const position, const float shift_x, const float shift_y, const Impact* const impact, const Uint8 penetration){
	Projectile* pr = (prs->array + prs->num++);
	pr->type_id = projectile_penetrat;
	pr->position = *position;
	pr->shift_per_tick = (SDL_FPoint){shift_x, shift_y};
	pr->data.penetrating.impact = *impact;
	pr->data.penetrating.penetration = penetration;
	pr->data.penetrating.hits = 0;
}

extern inline void AddHProjectileToArray(Projectiles_array* const prs, const SDL_FPoint* const position, const float shift_x, const float shift_y, const Impact* const impact){
	Projectile* pr = (prs->array + prs->num++);
	pr->type_id = projectile_hostile;
	pr->position = *position;
	pr->shift_per_tick = (SDL_FPoint){shift_x, shift_y};
	pr->data.basic.impact = *impact;
}

static inline void DestroyProjectileInArray(Projectiles_array* const prs, const Uint16 indx){
	if(indx != prs->num - 1){
		*(prs->array + indx) = *(prs->array + prs->num - 1);
	}
	--prs->num;
}

static inline void MoveProjectile(Projectile* const pr){
	pr->position.x += pr->shift_per_tick.x;
	pr->position.y += pr->shift_per_tick.y;
}

void UpdateProjectiles(Game_data* const gd){
	for(Projectile* pr = gd->projectiles.array; pr != (gd->projectiles.array + gd->projectiles.num); ++pr){
		if(!((*(update + pr->type_id))(pr, gd))){
			--pr;
		}
	}
}

static bool UpdatePCProjectile(Projectile* const pr, Game_data* const gd){
		MoveProjectile(pr);
		Segment* seg = GetSegmentUnsafe(&gd->world, pr->position.x, pr->position.y);
		if(seg == NULL){
			*pr = *(gd->projectiles.array + gd->projectiles.num-- - 1U);
			return false;
		}
		Segment* neighbour_segs[4];
		Get4NearestSegments(neighbour_segs, &gd->world, seg, pr->position.x, pr->position.y);
		for(unsigned int k = 0U; k < 4; ++k){
			Segment* neighbour = *(neighbour_segs + k);
			if(neighbour == NULL) continue;
			for(unsigned int j = 0U; j < neighbour->beings.num; ++j){
				Being* bg = (gd->beings.array + *(neighbour->beings.beings_ind + j));
				if(!ProjectileHitsBeing(pr, bg)) continue;
				AddDamageVisualEffect(&gd->rend_data_ptr->visual_effects, &pr->position);
				if(DamageBeing(bg, &pr->data.penetrating.impact, gd->beings.array)){
					if(pr->data.penetrating.hits < --pr->data.penetrating.penetration){
						--j;
						continue;
					}
				}else{
					if(bg->status != being_fly){
						const float power = CalculateStunPower(&pr->data.penetrating.impact, &bg->armour);
						if(power > 1.0F){
							StunBeing(bg, (int)(BEING_DEFAULT_LEFT_TICKS * power));
						}
					}
					if(pr->data.penetrating.hits < pr->data.penetrating.penetration){
						*(pr->data.penetrating.hit_targets + pr->data.penetrating.hits++) = bg->main_indx;
						continue;
					}
				}
				*pr = *(gd->projectiles.array + gd->projectiles.num-- - 1U);
				return false;
			}
		}
		return true;
}

static bool UpdateHostileProjectile(Projectile* const pr, Game_data* const gd){
	MoveProjectile(pr);
	Segment* seg = GetSegmentUnsafe(&gd->world, pr->position.x, pr->position.y);
	if(seg == NULL){
		*pr = *(gd->projectiles.array + gd->projectiles.num-- - 1U);
		return false;
	}
	Segment* neighbour_segs[4];
	Get4NearestSegments(neighbour_segs, &gd->world, seg, pr->position.x, pr->position.y);
	for(unsigned int k = 0U; k < 4; ++k){
		Segment* neighbour = *(neighbour_segs + k);
		if(neighbour == NULL) continue;
		for(unsigned int j = 0U; j < neighbour->ally_beings.num; ++j){
			Being* bg = (gd->beings.array + *(neighbour->ally_beings.beings_ind + j));
			if(!ProjectileHitsBeing(pr, bg)) continue;
			AddDamageVisualEffect(&gd->rend_data_ptr->visual_effects, &pr->position);
			if(!DamageAlly(bg, &pr->data.basic.impact, gd->beings.array)){
				if(bg->status != being_fly){
					const float power = CalculateStunPower(&pr->data.basic.impact, &bg->armour);
					if(power > 1.0F){
						StunBeing(bg, (int)(BEING_DEFAULT_LEFT_TICKS * power));
					}
				}
			}
			*pr = *(gd->projectiles.array + gd->projectiles.num-- - 1U);
			return false;
		}
	}
	for(unsigned int i = 0U; i < gd->champions.num; ++i){
		if(ProjectileHitsPlayer(pr, gd, i)){
			*pr = *(gd->projectiles.array + gd->projectiles.num-- - 1U);
			return false;
		}
	}
	return true;
}
static bool UpdateSpecialProjectile(Projectile* const pr, Game_data* const gd){}

static inline bool ProjectileHitsPlayer(Projectile* const pr, Game_data* const gd, const unsigned int pc_indx){
	Player* const pc = gd->champions.array + pc_indx;
	if(pow2(pr->position.x - pc->position.x) + pow2(pr->position.y - pc->position.y) < pow2(half(PLAYER_SIZE))){
		if(pc->flags & block && (SineUnsafe(pc->direction) * pr->shift_per_tick.x) + (-CosiUnsafe(pc->direction) * pr->shift_per_tick.y) <= 0){
			HitBarrier(pc, &pr->data.basic.impact);
		}else{
			DamagePlayer(pc, &pr->data.basic.impact);
			AddDamageVisualEffect(&gd->rend_data_ptr->visual_effects, &pr->position);
		}
		return true;
	}
	return false;
}