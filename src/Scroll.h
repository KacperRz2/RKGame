#ifndef SCROLL_H_
#define SCROLL_H_

enum scroll_type{
    scroll_push,
    scroll_heal,
    scroll_add_ally,
    scroll_renew_armour,
    scroll_slow,
    scroll_fire,
    scroll_thunderbolt,
    scroll_covert,
    scroll_rest,
    scroll_empty
};

int ScrollCost(const unsigned int);
bool UseScroll(Game_data* const);
Uint8 GetRandomScroll();

#endif
