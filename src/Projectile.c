#include <common.h>
#include <function.h>

static bool ProjectileHitsBeing(Projectile* const, Being* const);
static void DestroyProjectileInArray(Projectiles_array* const, const unsigned int);
static void MoveProjectile(Projectile* const);
static bool ProjectileHitsPlayer(Projectile* const, Game_data* const, const unsigned int);

static bool UpdatePCProjectile(Projectile* const, Game_data* const);
static bool UpdateHostileProjectile(Projectile* const, Game_data* const);
static bool UpdateSpecialProjectile(Projectile* const, Game_data* const);

static bool WarlockProjectile(Projectile* const, Game_data* const);
static bool FireProjectile(Projectile* const, Game_data* const);

void DestroyProjectiles(Projectiles_array* const prs){
    SDL_free(prs->array);
	prs->num = 0U;
}

static inline bool ProjectileHitsBeing(Projectile* const pr, Being* const b){
	if(SDL_fabsf(pr->position.x - b->position.x) < half(BeingSize(b)) && SDL_fabsf(pr->position.y - b->position.y) < half(BeingSize(b))){
		for(unsigned int i = pr->penetrating.hits; i > 0U; --i){
			if(*(pr->penetrating.hit_targets + (i - 1U)) == b->main_indx){
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
	pr->penetrating.impact = *impact;
	pr->penetrating.penetration = penetration;
	pr->penetrating.hits = 0U;
}

extern inline void AddHProjectileToArray(Projectiles_array* const prs, const SDL_FPoint* const position, const float shift_x, const float shift_y, const Impact* const impact){
	Projectile* pr = (prs->array + prs->num++);
	pr->type_id = projectile_hostile;
	pr->position = *position;
	pr->shift_per_tick = (SDL_FPoint){shift_x, shift_y};
	pr->basic.impact = *impact;
}

extern inline void AddSpecialProjectileToArray(Projectiles_array* const prs, const SDL_FPoint* const position, const float shift_x, const float shift_y, const Uint8 effect, const unsigned int ticks){
	Projectile* pr = (prs->array + prs->num++);
	pr->type_id = projectile_special;
	pr->position = *position;
	pr->shift_per_tick = (SDL_FPoint){shift_x, shift_y};
	pr->special.effect_id = effect;
	pr->special.ticks = ticks;
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

void UpdateProjectiles(Game_data* const gd){
	const bool (*update[])(Projectile* const, Game_data* const) = PROJECTILES_UPDATE_FUNC;
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
			if(DamageBeing(bg, &pr->penetrating.impact, gd->beings.array)){
				if(pr->penetrating.hits < pr->penetrating.penetration--){
					--j;
					continue;
				}
			}else{
				if(bg->status != being_fly){
					const float power = CalculateStunPower(&pr->penetrating.impact, &bg->armour);
					if(power >= 1.0F){
						if(bg->status == being_stunned){
							const float angle = GetDirectionToPush(&pr->position, &bg->position);
							const float vel = BASE_FLY_VELOCITY * power;
							CatapultBeing(bg, SineSafe(angle) * vel, -CosiSafe(angle) * vel, BASE_FLY_TICKS * power);
						}else{
							StunBeing(bg, (int)(BEING_DEFAULT_LEFT_TICKS * power));
						}
					}
				}
				if(pr->penetrating.hits < pr->penetrating.penetration){
					*(pr->penetrating.hit_targets + pr->penetrating.hits++) = bg->main_indx;
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
			if(!DamageAlly(bg, &pr->basic.impact, gd->beings.array)){
				if(bg->status != being_fly){
					const float power = CalculateStunPower(&pr->basic.impact, &bg->armour);
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

static bool UpdateSpecialProjectile(Projectile* const pr, Game_data* const gd){
	const bool (*update[])(Projectile* const, Game_data* const) = SPEC_PROJECTILES_FUNC;
	return (*(update + pr->special.effect_id))(pr, gd);
}

static bool WarlockProjectile(Projectile* const pr, Game_data* const gd){
	if(pr->special.ticks-- < 1U){
		*pr = *(gd->projectiles.array + gd->projectiles.num-- - 1U);
		return false;
	}
	Segment* seg = GetSegmentUnsafe(&gd->world, pr->position.x + pr->shift_per_tick.x, pr->position.y + pr->shift_per_tick.y);
	if(!seg){
		if(!GetSegmentUnsafe(&gd->world, pr->position.x + pr->shift_per_tick.x, pr->position.y)){
			pr->shift_per_tick.x = -pr->shift_per_tick.x;
		}
		if(!GetSegmentUnsafe(&gd->world, pr->position.x, pr->position.y + pr->shift_per_tick.y)){
			pr->shift_per_tick.y = -pr->shift_per_tick.y;
		}
	}
	MoveProjectile(pr);
	if(!GetSegmentUnsafe(&gd->world, pr->position.x, pr->position.y)){
		*pr = *(gd->projectiles.array + gd->projectiles.num-- - 1U);
		return false;
	}
	for(unsigned int i = 0U; i < gd->champions.num; ++i){
		Player* const pc = gd->champions.array + i;
		if(!(pc->flags & dodge_time) && pow2(pr->position.x - pc->position.x) + pow2(pr->position.y - pc->position.y) < pow2(half(PLAYER_SIZE))){
			if(pc->flags & block && (SineUnsafe(pc->direction) * pr->shift_per_tick.x) + (-CosiUnsafe(pc->direction) * pr->shift_per_tick.y) <= 0){
				pr->position.x -= pr->shift_per_tick.x;
				pr->position.y -= pr->shift_per_tick.y;
				GetShiftFromAngle(pc->direction, PROJECTILE_VELOCITY, &pr->shift_per_tick.x, &pr->shift_per_tick.y);
				break;
			}else{
				AddOrUpdatePlayerEffect(pc, (Lasting_effect){pc_effect_weak, WEAK_EFFECT_TICKS});
				pc->blade_attack.damage *= 0.5F;
				pc->blade_attack.magic *= 0.5F;
				pc->blade_attack.armour_reduction = 0.0F;
				pc->range_attack.damage *= 0.5F;
				pc->range_attack.magic *= 0.5F;
				pc->range_attack.armour_reduction = 0.0F;
				*pr = *(gd->projectiles.array + gd->projectiles.num-- - 1U);
    			AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &pc->position, SPELL1_RGB);
				return false;
			}
		}
	}
	if(!(pr->special.ticks % 3U)){
		AddProjectileVisualEffect(&gd->rend_data_ptr->visual_effects, &pr->position);
	}
	return true;
}

static bool FireProjectile(Projectile* const pr, Game_data* const gd){
	Segment* seg = GetSegmentUnsafe(&gd->world, pr->position.x + pr->shift_per_tick.x, pr->position.y + pr->shift_per_tick.y);
	if(pr->special.ticks-- < 1U || seg == NULL){
		if(seg == NULL){
			seg = GetSegmentUnsafe(&gd->world, pr->position.x, pr->position.y);
		}else{
			MoveProjectile(pr);
		}
		AddBigBurnVisualEffect(&gd->rend_data_ptr->visual_effects, &pr->position);
		AddBoomVisualEffect(&gd->rend_data_ptr->visual_effects, &pr->position);
		AddBoomVisualEffectTimer(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
			pr->position.x + (SDL_randf() - 0.5F) * half(BOOM_SIZE),
			pr->position.y + (SDL_randf() - 0.5F) * half(BOOM_SIZE)
		}, SDL_rand(V_EFFECT_MAX_DELAY) + 1U);
		for(unsigned int i = 0U; i < BOOM_EFFECTS_NUM; ++i){
			float angle = FULL_ANGLE * SDL_randf();
			float rand_float = SDL_randf();
			float distance = half(BOOM_SIZE) * rand_float;
			AddBigBurnVisualEffectTimer(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
				pr->position.x + SineUnsafe(angle) * distance,
				pr->position.y - CosiUnsafe(angle) * distance
			}, V_EFFECT_MAX_DELAY * rand_float + 1U);
			angle = FULL_ANGLE * SDL_randf();
			rand_float = SDL_randf();
			distance = half(BOOM_SIZE) * rand_float;
			AddSmallBurnVisualEffectTimer(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
				pr->position.x + SineUnsafe(angle) * distance,
				pr->position.y - CosiUnsafe(angle) * distance
			}, V_EFFECT_MAX_DELAY * SDL_randf() + 1U);
		}
		const int range = 2;
		const unsigned int array_size = pow2(1 + range * 2);
		Segment* neighbour_segs[array_size];
		GetNeighbourSegmentsFar(neighbour_segs, &gd->world, seg, range);
		for(unsigned int k = 0U; k < array_size; ++k){
			Segment* neighbour = *(neighbour_segs + k);
			if(neighbour == NULL) continue;
			for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
				Being* bg = (gd->beings.array + *(neighbour->beings.beings_ind + i));
				const float distance_squared = GetDistanceSquared(&bg->position, &pr->position);
				const float range_squared = pow2(SEGMENT_SIZE * range);
				if(distance_squared <= range_squared){
					AddDamageVisualEffect(&gd->rend_data_ptr->visual_effects, &bg->position);
					const Impact impact = PC_FIRE_PROJECTILE_IMPACT;
					const float impact_level = (1.0F - distance_squared / range_squared);
					if(DamageBeing(bg, &(Impact){
							impact.damage * impact_level,
							impact.armour_reduction,
							impact.magic * impact_level,
							impact.stun
						}, gd->beings.array)){
						--i;
					}else{
						AddOrUpdateBeingEffect(bg, (Lasting_effect){being_effect_burn, BURN_TICKS});
						const float power = SCROLL_PUSH_POWER * bg->armour.unstability * impact_level;
						const float angle = GetDirectionToPush(&pr->position, &bg->position);
						const float vel = BASE_FLY_VELOCITY * power;
						CatapultBeing(bg, SineSafe(angle) * vel, -CosiSafe(angle) * vel, BASE_FLY_TICKS * power);
					}
				}
			}
		}
		*pr = *(gd->projectiles.array + gd->projectiles.num-- - 1U);
		return false;
	}
	MoveProjectile(pr);
	if(!(pr->special.ticks % 4U)){
		if(!(pr->special.ticks % 16U)){
			AddBigBurnVisualEffect(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
				pr->position.x + (SDL_randf() - 0.5F) * BULLET_SIZE,
				pr->position.y + (SDL_randf() - 0.5F) * BULLET_SIZE
			});
		}else{
			AddSmallBurnVisualEffect(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
				pr->position.x + (SDL_randf() - 0.5F) * BULLET_SIZE,
				pr->position.y + (SDL_randf() - 0.5F) * BULLET_SIZE
			});
		}
	}
	return true;
}

static inline bool ProjectileHitsPlayer(Projectile* const pr, Game_data* const gd, const unsigned int pc_indx){
	Player* const pc = gd->champions.array + pc_indx;
	if(!(pc->flags & dodge_time) && pow2(pr->position.x - pc->position.x) + pow2(pr->position.y - pc->position.y) < pow2(half(PLAYER_SIZE))){
		if(pc->flags & block && (SineUnsafe(pc->direction) * pr->shift_per_tick.x) + (-CosiUnsafe(pc->direction) * pr->shift_per_tick.y) <= 0){
			HitBarrier(pc, &pr->basic.impact);
		}else{
			DamagePlayer(pc, &pr->basic.impact);
			AddDamageVisualEffect(&gd->rend_data_ptr->visual_effects, &pr->position);
		}
		return true;
	}
	return false;
}
