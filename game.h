#ifndef GAME_H_
#define GAME_H_

int MainMenuLoop(SDL_Event* const, SDL_Renderer* const, SDL_Window* const);
void GameLoop(SDL_Event* const, SDL_Renderer* const, SDL_Window* const, SDL_Texture** const);
void SetGameData(Game_data* const);
void ClearGameData(Game_data* const);

#endif