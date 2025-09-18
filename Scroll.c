#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Scroll.h>
#include <function.h>
#include <Being.h>
#include <Player.h>
#include <World.h>

const void (*effect[])(Game_data* const) = SCR_EFFECTS;
const int costs[] = SCR_COSTS;

extern inline int ScrollCost(const unsigned int scroll_id){
    return *(costs + scroll_id);
}

extern inline void UseScroll(Game_data* const g_d){
    (*(effect + g_d->pc.selected_scroll))(g_d);
}

void effect0(Game_data* const g_d){
    Segment* s = GetSegment(&g_d->world, g_d->pc.position.x, g_d->pc.position.y);
    for(unsigned int c = s->indx.x - 1; c < s->indx.x + 2; ++c){
    for(unsigned int r = s->indx.y - 1; r < s->indx.y + 2; ++r){
        Segment* neighbour = GetSegmentByIndx(&g_d->world, c, r);
        if(neighbour == NULL) continue;
        for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
            Being* b = *(neighbour->beings.array + i);
            float angle = GetDirectionToPush(&g_d->pc.position, &b->position);
            CatapultBeing(b, SineSafe(angle) * DEFAULT_FLY_VELOCITY, -CosiSafe(angle) * DEFAULT_FLY_VELOCITY, BEING_DEFAULT_LEFT_TICKS * 8);
        }
    }}
}

void effect1(Game_data* const g_d){
    HealPlayer(&g_d->pc, PC_HP / 10);
}

void effect2(Game_data* const g_d){
    
}

void effect3(Game_data* const g_d){
    
}

void effect4(Game_data* const g_d){
    
}

void effect5(Game_data* const g_d){
    
}

void effect6(Game_data* const g_d){
    
}

void EffectEmpty(Game_data* const g_d){}