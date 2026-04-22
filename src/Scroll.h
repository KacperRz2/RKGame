#ifndef SCROLL_H_
#define SCROLL_H_

int ScrollCost(const unsigned int);
bool UseScroll(Game_data* const);
Uint8 GetRandomScroll();
static bool push(Game_data* const);
static bool HPRegen(Game_data* const);
static bool AddAlly(Game_data* const);
static bool RenewArmour(Game_data* const);
static bool slow(Game_data* const);
static bool fire(Game_data* const);
static bool thunderbolt(Game_data* const);
static bool convert(Game_data* const);
static bool FatigueRegen(Game_data* const);
static bool EffectEmpty(Game_data* const);

#endif