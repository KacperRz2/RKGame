#include <common.h>
#include <function.h>
#include <sound.h>

static bool push(Game_data* const, const SDL_FPoint *const, const unsigned int);
static bool HPRegen(Game_data* const, const SDL_FPoint *const, const unsigned int);
static bool AddAlly(Game_data* const, const SDL_FPoint *const, const unsigned int);
static bool RenewArmour(Game_data* const, const SDL_FPoint *const, const unsigned int);
static bool slow(Game_data* const, const SDL_FPoint *const, const unsigned int);
static bool fire(Game_data* const, const SDL_FPoint *const, const unsigned int);
static bool thunderbolt(Game_data* const, const SDL_FPoint *const, const unsigned int);
static bool convert(Game_data* const, const SDL_FPoint *const, const unsigned int);
static bool FatigueRegen(Game_data* const, const SDL_FPoint *const, const unsigned int);
static bool EffectEmpty(Game_data* const, const SDL_FPoint *const, const unsigned int);

extern inline int ScrollCost(const Uint8 scroll_id){
    const int costs[] = SCR_COSTS;
    return *(costs + scroll_id);
}

extern inline bool UseScroll(Game_data* const gd, const SDL_FPoint *const target_xy, const unsigned int indx){
    bool (*const effect[])(Game_data* const, const SDL_FPoint *const, const unsigned int) = SCR_EFFECTS;
    return (*(effect + (gd->champions.array + indx)->selected_scroll))(gd, target_xy, indx);
}

Uint8 GetRandomScroll(){
    return SDL_rand(scroll_empty);
}

static bool push(Game_data* const gd, const SDL_FPoint *const target_xy, const unsigned int indx){
    const Segment* const target_seg = GetSegmentSafe(&gd->world, target_xy->x, target_xy->y);
    if(!target_seg){
        return false;
    }
    const int range = 3;
    const unsigned int array_size = pow2(1 + range * 2);
    Segment* neighbour_segs[array_size];
    GetNeighbourSegmentsFar(neighbour_segs, &gd->world, target_seg, range);
    bool any = false;
    for(unsigned int k = 0U; k < array_size; ++k){
        Segment* neighbour = *(neighbour_segs + k);
        if(neighbour == NULL) continue;
        for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
            Being* bg = (gd->beings.array + *(neighbour->beings.beings_ind + i));
            const float distance_squared = GetDistanceSquared(&bg->position, target_xy);
            const float range_squared = pow2(SEGMENT_SIZE * range);
            if(distance_squared <= range_squared){
                const float power = SCROLL_PUSH_POWER * bg->armour.unstability * (1.0F - distance_squared / range_squared);
                const float angle = GetDirectionToPush(target_xy, &bg->position);
                const float vel = BASE_FLY_VELOCITY * power;
                CatapultBeing(bg, SineSafe(angle) * vel, -CosiSafe(angle) * vel, BASE_FLY_TICKS * power);
                any = true;
                AddSpellVisualEffect(gd, &bg->position, SPELL0_RGB);
            }
        }
    }
    if(!any){
        return false;
    }
    PlayGameSound(gd->snd_data_ptr, snd_push_last);
    AddSpellVisualEffect(gd, target_xy, SPELL0_RGB);
    return true;
}

static bool HPRegen(Game_data* const gd, const SDL_FPoint *const target_xy, const unsigned int indx){
    PlayGameSound(gd->snd_data_ptr, snd_bless_last);
    Player *const pc = gd->champions.array + indx;
    AddOrUpdatePlayerEffect(pc, (Lasting_effect){pc_effect_hpregen, HP_REGEN_TICKS});
    AddSpellVisualEffect(gd, &pc->position, SPELL0_RGB);
    return true;
}

static bool AddAlly(Game_data* const gd, const SDL_FPoint *const target_xy, const unsigned int indx){
    Player *const pc = gd->champions.array + indx;
    Segment* const target_seg = GetSegmentSafe(&gd->world, target_xy->x, target_xy->y);
    if(target_seg && gd->beings.num < MAX_BEINGS_NUM && target_seg->ally_beings.num < MAX_SEGM_BEINGS){
        Being* const bg = AddIdleAllyToArray(&gd->beings, ally_ordinary, target_xy->x, target_xy->y, target_seg, pc);
        AddBeingEffect(bg, (Lasting_effect){being_effect_ally_lifetime, ALLY_LIFETIME});
        AddSpellVisualEffect(gd, target_xy, SPELL0_RGB);
        return true;
    }
    return false;
}

static bool RenewArmour(Game_data* const gd, const SDL_FPoint *const target_xy, const unsigned int indx){
    Player *const pc = gd->champions.array + indx;
    pc->armour.absorption = pc->max_armour.absorption;
    pc->armour.multipl = pc->max_armour.multipl;
    pc->armour.magic_multipl = pc->max_armour.magic_multipl;
    AddSpellVisualEffect(gd, &pc->position, SPELL0_RGB);
    return true;
}

static bool slow(Game_data* const gd, const SDL_FPoint *const target_xy, const unsigned int indx){
    const Segment* const target_seg = GetSegmentSafe(&gd->world, target_xy->x, target_xy->y);
    if(!target_seg){
        return false;
    }
    const int range = 2;
    const unsigned int array_size = pow2(1 + range * 2);
    Segment* neighbour_segs[array_size];
    GetNeighbourSegmentsFar(neighbour_segs, &gd->world, target_seg, range);
    bool any = false;
    for(unsigned int k = 0U; k < array_size; ++k){
        Segment* neighbour = *(neighbour_segs + k);
        if(neighbour == NULL) continue;
        for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
            Being* bg = (gd->beings.array + *(neighbour->beings.beings_ind + i));
            bg->velocity *= 0.25F;
            const int effect_indx = BeingHasEffect(bg, being_effect_slow);
            if(effect_indx == NOT_FOUND){
                AddBeingEffect(bg, (Lasting_effect){being_effect_slow, SLOW_EFFECT_TICKS});
            }else{
                (bg->effects + effect_indx)->ticks_left = SLOW_EFFECT_TICKS;
            }
            any = true;
            AddSpellVisualEffect(gd, &bg->position, SPELL0_RGB);
        }
    }
    if(!any){
        return false;
    }
    AddSpellVisualEffect(gd, target_xy, SPELL0_RGB);
    return true;
}

static bool fire(Game_data* const gd, const SDL_FPoint *const target_xy, const unsigned int indx){
    Player *const pc = gd->champions.array + indx;
    float sh_x, sh_y;
    GetShift(&pc->position, target_xy, FIRE_PROJECTILE_VELOCITY, &sh_x, &sh_y);
    const unsigned int ticks = sh_x != 0.0F ? (target_xy->x - pc->position.x) / sh_x : (target_xy->y - pc->position.y) / sh_y;
    AddSpecialProjectileToArray(&gd->projectiles, &pc->position, sh_x, sh_y, projectile_fire, ticks);
    AddSpellVisualEffect(gd, &pc->position, SPELL0_RGB);
    return true;
}

static bool thunderbolt(Game_data* const gd, const SDL_FPoint *const target_xy, const unsigned int indx){
    Player *const pc = gd->champions.array + indx;
    Segment* const target_seg = GetSegmentSafe(&gd->world, target_xy->x, target_xy->y);
    Segment* seg = target_seg;
    if(!seg){
        return false;
    }
    Being* bg = NULL;
    if(seg->beings.num > 0U){
        const int rand = SDL_rand(seg->beings.num);
        int indx = rand;
        do{
            if(BeingHasEffect(gd->beings.array + *(seg->beings.beings_ind + indx), being_effect_thunderbolt) == NOT_FOUND){
                bg = gd->beings.array + *(seg->beings.beings_ind + indx);
                goto search_end;
            }
            indx = (indx + 1) % seg->beings.num;
        }while(indx != rand);
    }
    const unsigned int range = 2U;
    const unsigned int max_i = SPIRAL_STEPS(range);
    for(unsigned int i = 1U; i < max_i; ++i){
        seg = GetDistantSegmentBySpiral(&gd->world, target_seg, i);
        if(seg == NULL || seg->beings.num == 0U) continue;
        const int rand = SDL_rand(seg->beings.num);
        int indx = rand;
        do{
            if(BeingHasEffect(gd->beings.array + *(seg->beings.beings_ind + indx), being_effect_thunderbolt) == NOT_FOUND){
                bg = gd->beings.array + *(seg->beings.beings_ind + indx);
                goto search_end;
            }
            indx = (indx + 1) % seg->beings.num;
        }while(indx != rand);
    }
    search_end:
    if(!bg){
        return false;
    }
    AddBoltVisualEffect(gd, &bg->position, (position16){(_Float16)pc->position.x, (_Float16)pc->position.y});
    AddBeingEffect(bg, (Lasting_effect){being_effect_thunderbolt, BOLT_CHAIN_TICKS});
    AddSpellVisualEffect(gd, &pc->position, SPELL0_RGB);
    return true;
}

static bool convert(Game_data* const gd, const SDL_FPoint *const target_xy, const unsigned int indx){
    Player *const pc = gd->champions.array + indx;
    const Segment* const target_seg = GetSegmentSafe(&gd->world, target_xy->x, target_xy->y);
    if(!target_seg){
        return false;
    }
    const int range = 1;
    const unsigned int array_size = pow2(1 + range * 2);
    Segment* neighbour_segs[array_size];
    GetNeighbourSegments(neighbour_segs, &gd->world, target_seg);
    bool any = false;
    for(unsigned int k = 0U; k < array_size; ++k){
        Segment* neighbour = *(neighbour_segs + k);
        if(neighbour == NULL || neighbour->ally_beings.num >= MAX_SEGM_BEINGS) continue;
        for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
            Being* bg = (gd->beings.array + *(neighbour->beings.beings_ind + i));
            const float distance_squared = GetDistanceSquared(&bg->position, target_xy);
            const float range_squared = pow2(SEGMENT_SIZE * range);
            if(distance_squared <= range_squared){
                EndAllBeingEffects(gd, bg);
                RemoveBeingFromSegment(bg, &neighbour->beings, gd->beings.array);
                bg->type_id = ally_ordinary;
                HaltBeing(bg, 1);
                AddBeingToSegment(neighbour, bg, &neighbour->ally_beings);
                bg->target.player = pc;
                bg->target_last_seen_at = (position16){bg->target.player->position.x, bg->target.player->position.y};
                AddBeingEffect(bg, (Lasting_effect){being_effect_ally_lifetime, ALLY_LIFETIME + SDL_rand(ALLY_LIFETIME_MAX_SHIFT)});
                --i;
                AddSpellVisualEffect(gd, &bg->position, SPELL0_RGB);
                any = true;
                if(neighbour->ally_beings.num >= MAX_SEGM_BEINGS){
                    break;
                }
            }
        }
    }
    if(!any){
        return false;
    }
    AddSpellVisualEffect(gd, target_xy, SPELL0_RGB);
    return true;
}

static bool FatigueRegen(Game_data* const gd, const SDL_FPoint *const target_xy, const unsigned int indx){
    Player *const pc = gd->champions.array + indx;
    AddOrUpdatePlayerEffect(pc, (Lasting_effect){pc_effect_fpregen, FP_REGEN_TICKS});
    AddSpellVisualEffect(gd, &pc->position, SPELL0_RGB);
    return true;
}

static bool EffectEmpty(Game_data* const gd, const SDL_FPoint *const target_xy, const unsigned int indx){return false;}
