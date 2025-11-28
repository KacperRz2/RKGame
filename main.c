#include <header.h>

int main(int argc, char* argv[]){
	SDL_Event event;
	Render_data rend_data;
	for(unsigned int i = 0U; i < ANGLE_PARTS; ++i){
		*(si + i) = SDL_sinf(MIN_ANGLE * i);
		*(co + i) = SDL_cosf(MIN_ANGLE * i);
	}
	SetRenderData(&rend_data);
	if(GraphicsInitiation(&rend_data)) return 1;
	while(1){
		const int option = MainMenuLoop(&event, &rend_data);
		if(option == menu_quit) break;
		if(option == menu_start){
			GameLoop(&event, &rend_data);
			SDL_SetWindowRelativeMouseMode(rend_data.window, false);
		}
	}
	ClearRenderData(&rend_data);
	return 0;
}
