#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <common.h>
#include <sound.h>
#include <function.h>

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
	Sound_data sound_data;
	SoundInitiation(&sound_data);
	Game_data game_data = {
		.rend_data_ptr = &rend_data,
		.ev_ptr = &event,
		.snd_data_ptr = &sound_data
	};
	while(1){
		const int option = MainMenuLoop(&game_data);
		if(option == menu_quit) break;
		if(option == menu_start || menu_multipl_host == option){
			do{
				game_data.seed = (((Uint64)SDL_rand_bits() << 32ULL) + (Uint64)SDL_rand_bits());
			}while(!game_data.seed);
			if(option == menu_start){
				SetGameData(&game_data);
				StartNewLevel(&game_data);
				DrawMap(&rend_data, &game_data.world);
				SaveGame(&game_data, SAVE_PATH0);
				GameLoop(&game_data);
				ClearGameData(&game_data);
			}else{
				HostGameLoop(&game_data);
			}
			ToMenuMouseMode(&rend_data);
		}else if(menu_multipl == option){
			ClientGameLoop(&game_data);
			ToMenuMouseMode(&rend_data);
		}else if(option == menu_load){
			LoadGame(&game_data, SAVE_PATH0);
			GameLoop(&game_data);
			ClearGameData(&game_data);
			ToMenuMouseMode(&rend_data);
		}else if(option == menu_credits){
			CreditsLoop(&event, &rend_data);
		}
	}
	SoundDataDestruction(&sound_data);
	ClearRenderData(&rend_data);
	return 0;
}
