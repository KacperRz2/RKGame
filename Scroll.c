#include <SDL3/SDL.h>
#include <macros.h>
#include <enum.h>
#include <types.h>
#include <Scroll.h>
#include <function.h>
#include <Being.h>
#include <Player.h>
#include <World.h>

extern inline int ScrollCost(const unsigned int scroll_id){
    const int costs[] = SCR_COSTS;
    return *(costs + scroll_id);
}

extern inline void UseScroll(Game_data* const gd){
    const void (*effect[])(Game_data* const) = SCR_EFFECTS;
    (*(effect + human(gd)->selected_scroll))(gd);
}

Uint8 GetRandomScroll(){
    return SDL_rand(scroll_empty);
}

void effect0(Game_data* const gd){
    const unsigned int range = 1U;
    const unsigned int array_size = pow2(1U + range * 2);
    Segment* neighbour_segs[array_size];
    GetNeighbourSegments(neighbour_segs, &gd->world, GetSegmentUnsafe(&gd->world, human(gd)->position.x, human(gd)->position.y));
    for(unsigned int k = 0U; k < array_size; ++k){
        Segment* neighbour = *(neighbour_segs + k);
        if(neighbour == NULL) continue;
        for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
            Being* b = (gd->beings.array + *(neighbour->beings.beings_ind + i));
            const float angle = GetDirectionToPush(&human(gd)->position, &b->position);
            const float stun = DEFAULT_FLY_VELOCITY * b->armour.unstability;
            CatapultBeing(b, SineSafe(angle) * stun, -CosiSafe(angle) * stun, BEING_DEFAULT_LEFT_TICKS * 8);
        }
    }
}

void effect1(Game_data* const gd){
    AddPlayerEffect(human(gd), (Lasting_effect){pc_effect_hpregen, HP_REGEN_TICKS});
}

void effect2(Game_data* const gd){
    if(gd->beings.num < MAX_BEINGS_NUM && human(gd)->segment->ally_beings.num < MAX_SEGM_BEINGS){
        AddIdleBeingToArray(&gd->beings, ally_ordinary, human(gd)->position.x, human(gd)->position.y, human(gd)->segment, human(gd));
    }
}

void effect3(Game_data* const gd){
    human(gd)->armour.absorption = human(gd)->max_armour.absorption;
    human(gd)->armour.multipl = human(gd)->max_armour.multipl;
    human(gd)->armour.magic_multipl = human(gd)->max_armour.magic_multipl;
}

void slow(Game_data* const gd){
    const int range = 2;
    const unsigned int array_size = pow2(1 + range * 2);
    Segment* neighbour_segs[array_size];
    GetNeighbourSegmentsFar(neighbour_segs, &gd->world, human(gd)->segment, range);
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
        }
    }
}

void effect5(Game_data* const gd){
    
}

void effect6(Game_data* const gd){
    
}

void EffectEmpty(Game_data* const gd){}