#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Scroll.h>
#include <function.h>
#include <Being.h>
#include <Player.h>
#include <World.h>
#include <enum.h>


extern inline int ScrollCost(const unsigned int scroll_id){
    const int costs[] = SCR_COSTS;
    return *(costs + scroll_id);
}

extern inline void UseScroll(Game_data* const g_d){
    const void (*effect[])(Game_data* const) = SCR_EFFECTS;
    (*(effect + (g_d->champions.array + g_d->human_indx)->selected_scroll))(g_d);
}

Uint8 GetRandomScroll(){
    return SDL_rand(scroll_empty);
}

void effect0(Game_data* const g_d){
    Segment* s = GetSegment(&g_d->world, (g_d->champions.array + g_d->human_indx)->position.x, (g_d->champions.array + g_d->human_indx)->position.y);
    for(unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c){
    for(unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r){
        Segment* neighbour = GetSegmentByIndx(&g_d->world, c, r);
        if(neighbour == NULL) continue;
        for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
            Being* b = (g_d->beings.array + *(neighbour->beings.beings_ind + i));
            const float angle = GetDirectionToPush(&(g_d->champions.array + g_d->human_indx)->position, &b->position);
            const float stun = DEFAULT_FLY_VELOCITY * b->armour.unstability;
            CatapultBeing(b, SineSafe(angle) * stun, -CosiSafe(angle) * stun, BEING_DEFAULT_LEFT_TICKS * 8);
        }
    }}
}

void effect1(Game_data* const g_d){
    HealPlayer(g_d->champions.array + g_d->human_indx, PC_HP / 10);
}

void effect2(Game_data* const g_d){
    if(g_d->beings.num < MAX_BEINGS_NUM && (g_d->champions.array + g_d->human_indx)->segment->ally_beings.num < MAX_SEGM_BEINGS){
        AddBeingToArray(&g_d->beings, ally_0, (g_d->champions.array + g_d->human_indx)->position.x, (g_d->champions.array + g_d->human_indx)->position.y, (g_d->champions.array + g_d->human_indx)->segment, g_d->champions.array + g_d->human_indx);
    }
}

void effect3(Game_data* const g_d){
    (g_d->champions.array + g_d->human_indx)->armour.absorption = (g_d->champions.array + g_d->human_indx)->max_armour.absorption;
    (g_d->champions.array + g_d->human_indx)->armour.multipl = (g_d->champions.array + g_d->human_indx)->max_armour.multipl;
    (g_d->champions.array + g_d->human_indx)->armour.magic_multipl = (g_d->champions.array + g_d->human_indx)->max_armour.magic_multipl;
}

void effect4(Game_data* const g_d){
    
}

void effect5(Game_data* const g_d){
    
}

void effect6(Game_data* const g_d){
    
}

void EffectEmpty(Game_data* const g_d){}