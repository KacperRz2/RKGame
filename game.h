#ifndef GAME_H_
#define GAME_H_

int MainMenuLoop(SDL_Event* const, Render_data* const);
void GameLoop(SDL_Event* const, Render_data* const);
void SetGameData(Game_data* const);
void ClearGameData(Game_data* const);

#endif