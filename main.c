#include <header.h>

int main(int argc, char* argv[]){
	SDL_Event event;
	for(unsigned int i = 0U; i < ANGLE_PARTS; ++i){
		*(si + i) = SDL_sinf(MIN_ANGLE * i);
		*(co + i) = SDL_cosf(MIN_ANGLE * i);
	}
	SetRenderData(1600, 900);
	while(1){
		if(MainMenuLoop(&event, rend_data.renderer, rend_data.window) == menu_quit) break;
		GameLoop(&event, rend_data.renderer, rend_data.window, rend_data.textures);
	}
	ClearRenderData();
	return 0;
}
