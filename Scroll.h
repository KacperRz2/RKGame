#ifndef SCROLL_H_
#define SCROLL_H_

int ScrollCost(const unsigned int);
bool UseScroll(Game_data* const);
Uint8 GetRandomScroll();
bool effect0(Game_data* const);
bool effect1(Game_data* const);
bool effect2(Game_data* const);
bool effect3(Game_data* const);
bool slow(Game_data* const);
bool fire(Game_data* const);
bool thunderbolt(Game_data* const);
bool effect7(Game_data* const);
bool EffectEmpty(Game_data* const);

#endif