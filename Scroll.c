#include <SDL3/SDL.h>
#include <macros.h>
#include <enum.h>
#include <types.h>
#include <Scroll.h>
#include <function.h>
#include <Being.h>
#include <Player.h>
#include <World.h>
#include <render.h>
#include <Projectile.h>

extern inline int ScrollCost(const unsigned int scroll_id){
    const int costs[] = SCR_COSTS;
    return *(costs + scroll_id);
}

extern inline bool UseScroll(Game_data* const gd){
    const bool (*effect[])(Game_data* const) = SCR_EFFECTS;
    return (*(effect + human(gd)->selected_scroll))(gd);
}

Uint8 GetRandomScroll(){
    return SDL_rand(scroll_empty);
}

bool push(Game_data* const gd){
    const SDL_FPoint target_xy = GetMouseWorldPosition(gd);
    const Segment* const target_seg = GetSegmentSafe(&gd->world, target_xy.x, target_xy.y);
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
            const float distance_squared = GetDistanceSquared(&bg->position, &target_xy);
            const float range_squared = pow2(SEGMENT_SIZE * range);
            if(distance_squared <= range_squared){
                const float power = SCROLL_PUSH_POWER * bg->armour.unstability * (1.0F - distance_squared / range_squared);
                const float angle = GetDirectionToPush(&target_xy, &bg->position);
                const float vel = BASE_FLY_VELOCITY * power;
                CatapultBeing(bg, SineSafe(angle) * vel, -CosiSafe(angle) * vel, BASE_FLY_TICKS * power);
                any = true;
                AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &bg->position, SPELL0_RGB);
            }
        }
    }
    if(!any){
        return false;
    }
    AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &target_xy, SPELL0_RGB);
    return true;
}

bool HPRegen(Game_data* const gd){
    AddOrUpdatePlayerEffect(human(gd), (Lasting_effect){pc_effect_hpregen, HP_REGEN_TICKS});
    AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &human(gd)->position, SPELL0_RGB);
    return true;
}

bool AddAlly(Game_data* const gd){
    const SDL_FPoint target_xy = GetMouseWorldPosition(gd);
    Segment* const target_seg = GetSegmentSafe(&gd->world, target_xy.x, target_xy.y);
    if(target_seg && gd->beings.num < MAX_BEINGS_NUM && target_seg->ally_beings.num < MAX_SEGM_BEINGS){
        Being* const bg = AddIdleAllyToArray(&gd->beings, ally_ordinary, target_xy.x, target_xy.y, target_seg, human(gd));
        AddBeingEffect(bg, (Lasting_effect){being_effect_ally_lifetime, ALLY_LIFETIME});
        AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &target_xy, SPELL0_RGB);
        return true;
    }
    return false;
}

bool RenewArmour(Game_data* const gd){
    human(gd)->armour.absorption = human(gd)->max_armour.absorption;
    human(gd)->armour.multipl = human(gd)->max_armour.multipl;
    human(gd)->armour.magic_multipl = human(gd)->max_armour.magic_multipl;
    AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &human(gd)->position, SPELL0_RGB);
    return true;
}

bool slow(Game_data* const gd){
    const SDL_FPoint target_xy = GetMouseWorldPosition(gd);
    const Segment* const target_seg = GetSegmentSafe(&gd->world, target_xy.x, target_xy.y);
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
            if(effect_indx == -1){
                AddBeingEffect(bg, (Lasting_effect){being_effect_slow, SLOW_EFFECT_TICKS});
            }else{
                (bg->effects + effect_indx)->ticks_left = SLOW_EFFECT_TICKS;
            }
            any = true;
            AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &bg->position, SPELL0_RGB);
        }
    }
    if(!any){
        return false;
    }
    AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &target_xy, SPELL0_RGB);
    return true;
}

bool fire(Game_data* const gd){
    const SDL_FPoint target_xy = GetMouseWorldPosition(gd);
    float sh_x, sh_y;
    GetShift(&human(gd)->position, &target_xy, FIRE_PROJECTILE_VELOCITY, &sh_x, &sh_y);
    const unsigned int ticks = sh_x != 0.0F ? (target_xy.x - human(gd)->position.x) / sh_x : (target_xy.y - human(gd)->position.y) / sh_y;
    AddSpecialProjectileToArray(&gd->projectiles, &human(gd)->position, sh_x, sh_y, projectile_fire, ticks);
    AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &human(gd)->position, SPELL0_RGB);
    return true;
}

bool thunderbolt(Game_data* const gd){
    const SDL_FPoint target_xy = GetMouseWorldPosition(gd);
    Segment* const target_seg = GetSegmentSafe(&gd->world, target_xy.x, target_xy.y);
    Segment* seg = target_seg;
    if(!seg || !(seg->flags & segment_in_sight)){
        return false;
    }
    Being* bg = NULL;
    if(seg->beings.num > 0U){
        const int rand = SDL_rand(seg->beings.num);
        int indx = rand;
        do{
            if(BeingHasEffect(gd->beings.array + *(seg->beings.beings_ind + indx), being_effect_thunderbolt) == -1){
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
            if(BeingHasEffect(gd->beings.array + *(seg->beings.beings_ind + indx), being_effect_thunderbolt) == -1){
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
    AddBoltVisualEffect(&gd->rend_data_ptr->visual_effects, &bg->position, (position16){(_Float16)human(gd)->position.x, (_Float16)human(gd)->position.y});
    AddBeingEffect(bg, (Lasting_effect){being_effect_thunderbolt, BOLT_CHAIN_TICKS});
    AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &human(gd)->position, SPELL0_RGB);
    return true;
}

bool convert(Game_data* const gd){
    const SDL_FPoint target_xy = GetMouseWorldPosition(gd);
    const Segment* const target_seg = GetSegmentSafe(&gd->world, target_xy.x, target_xy.y);
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
            const float distance_squared = GetDistanceSquared(&bg->position, &target_xy);
            const float range_squared = pow2(SEGMENT_SIZE * range);
            if(distance_squared <= range_squared){
                EndAllBeingEffects(gd, bg);
                RemoveBeingFromSegment(bg, &neighbour->beings, gd->beings.array);
                bg->type_id = ally_ordinary;
                HaltBeing(bg, 1);
                AddBeingToSegment(neighbour, bg, &neighbour->ally_beings);
                bg->target.player = human(gd);
                bg->target_last_seen_at = (position16){bg->target.player->position.x, bg->target.player->position.y};
                AddBeingEffect(bg, (Lasting_effect){being_effect_ally_lifetime, ALLY_LIFETIME + SDL_rand(256)});
                --i;
                AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &bg->position, SPELL0_RGB);
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
    AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &target_xy, SPELL0_RGB);
    return true;
}

bool FatigueRegen(Game_data* const gd){
    AddOrUpdatePlayerEffect(human(gd), (Lasting_effect){pc_effect_fpregen, FP_REGEN_TICKS});
    AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &human(gd)->position, SPELL0_RGB);
    return true;
}

bool EffectEmpty(Game_data* const gd){return false;}