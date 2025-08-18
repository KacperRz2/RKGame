#ifndef GAME_H_
#define GAME_H_

int MainMenuLoop(SDL_Event* const, Render_data* const);
void GameLoop(SDL_Event* const, Render_data* const);
static void SetGameData(Game_data* const);
static void ClearGameData(Game_data* const);
static void RareEventsService(Game_data* const, Segment* const);
void AddBoxToArray(Boxes* const, const float, const float);
static void AddToBox(Box* const, const unsigned int, const int, void* const);
static void FreeBoxPlaceInArray(Boxes* const, const unsigned int);
static void DestroyBoxes(Boxes* const);

#endif