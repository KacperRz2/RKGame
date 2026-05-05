#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <common.h>

int main(int argc, char* argv[]){
	SDL_SetAppMetadata("KacApp", "1.0", NULL);
	SDL_Event event;
	Render_data rend_data = {
		.counter = 0U,
		.window = NULL,
		.renderer = NULL
	};
	SetSineCosineArrays();
	GraphicsInitiation(&rend_data);
	while(1){
		const int option = MainMenuLoop(&event, &rend_data);
		if(option == menu_quit) break;
		if(option == menu_start){
			Game_data game_data;
			game_data.rend_data_ptr = &rend_data;
			game_data.ev_ptr = &event;
			do{
				game_data.seed = (((Uint64)SDL_rand_bits() << 32ULL) + (Uint64)SDL_rand_bits());
			}while(!game_data.seed);
			SetGameData(&game_data);
			StartNewLevel(&game_data);
			DrawMap(&rend_data, &game_data.world);
			SaveGame(&game_data);
			GameLoop(&game_data);
			ClearGameData(&game_data);
			SDL_SetWindowRelativeMouseMode(rend_data.window, false);
		}else if(option == menu_load){
			Game_data game_data;
			game_data.rend_data_ptr = &rend_data;
			game_data.ev_ptr = &event;
			LoadGame(&game_data);
			GameLoop(&game_data);
			ClearGameData(&game_data);
			SDL_SetWindowRelativeMouseMode(rend_data.window, false);
		}else if(option == menu_settings){
			ToggleFullscreen(&rend_data);
		}
	}
	ClearRenderData(&rend_data);
	return 0;
}
