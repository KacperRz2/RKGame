#ifndef GAME_H_
#define GAME_H_

static void PopulateBigSeg(Game_data* const, const unsigned int, const unsigned int);
static void PlayerInUncoveredBigSeg(Game_data* const);
int MainMenuLoop(SDL_Event* const, Render_data* const);
void GameLoop(Game_data* const);
static void EndLoop(SDL_Event* const, Game_data* const, const int);
void SetGameData(Game_data* const);
void ClearGameData(Game_data* const);
static int RareEventsService(Game_data* const);
void AddBoxToArray(Boxes* const, const float, const float);
void AddToBox(Box* const, const unsigned int, const int, const unsigned int);
static void FreeBoxPlaceInArray(Boxes* const, const unsigned int);
static void DestroyBoxes(Boxes* const);
int GetNearbyBoxIndx(const Boxes* const, const SDL_FPoint* const, const float);
static void LootBox(Game_data* const, const unsigned int);
static void DestroyBoxInArray(Boxes* const, unsigned int);
static int GetNearbyShopIndx(Game_data* const);
static void EnterShop(Game_data* const, Player* const, const unsigned int);
int CalculateDamage(const Impact* const, const Armour* const);
float CalculateStunPower(const Impact* const, const Armour* const);
static int UpdateGame(Game_data* const);
int ActivateMenuOption(const unsigned int, Game_data* const);
int ItemPrice(const unsigned int);
static SDL_FPoint GetBeingCreationPoint(Game_data* const, const float);

static void UpdateEffects(Game_data* const);
int HasEffect(Lasting_effect* const, const unsigned int, const unsigned int);
void AddLastingEffect(Lasting_effect* const, const Lasting_effect, const int);
void RemoveLastingEffect(Lasting_effect* const, const int, const int);
static void AddGameEffect(Game_data* const, const Lasting_effect);
static void RemoveGameEffect(Game_data* const, const int);
static void UpdateGameEffects(Game_data* const);
static void UpdateGameEffect(Game_data* const, const int);
void HordeAttack(Game_data* const, const int);

void SaveGame(const Game_data* const);
void LoadGame(Game_data* const);

#endif