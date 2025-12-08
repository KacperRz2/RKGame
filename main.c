#include <header.h>

int main(int argc, char* argv[]){
	SDL_Event event;
	Render_data rend_data;
	SetSineCosineArrays();
	SetRenderData(&rend_data);
	if(GraphicsInitiation(&rend_data)) return 1;
	while(1){
		const int option = MainMenuLoop(&event, &rend_data);
		if(option == menu_quit) break;
		if(option == menu_start){
			GameLoop(&event, &rend_data);
			SDL_SetWindowRelativeMouseMode(rend_data.window, false);
		}else if(option == menu_settings){
			ToggleFullscreen(&rend_data);
		}
	}
	ClearRenderData(&rend_data);
	return 0;
}
