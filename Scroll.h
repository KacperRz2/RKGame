#ifndef SCROLL_H_
#define SCROLL_H_

int ScrollCost(const unsigned int);
bool UseScroll(Game_data* const);
Uint8 GetRandomScroll();
bool push(Game_data* const);
bool HPRegen(Game_data* const);
bool AddAlly(Game_data* const);
bool RenewArmour(Game_data* const);
bool slow(Game_data* const);
bool fire(Game_data* const);
bool thunderbolt(Game_data* const);
bool convert(Game_data* const);
bool FatigueRegen(Game_data* const);
bool EffectEmpty(Game_data* const);

#endif