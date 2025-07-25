#include <header.h>

int main(int argc, char* argv[]){
	SDL_Event event;
	Render_data rend_data;
	for(unsigned int i = 0U; i < ANGLE_PARTS; ++i){
		*(si + i) = SDL_sinf(MIN_ANGLE * i);
		*(co + i) = SDL_cosf(MIN_ANGLE * i);
	}
	SetRenderData(&rend_data, WINDOW_START_W, WINDOW_START_H);
	if(GraphicsInitiation(&rend_data)) return 1;
	while(1){
		if(MainMenuLoop(&event, &rend_data) == menu_quit) break;
		GameLoop(&event, &rend_data);
	}
	ClearRenderData(&rend_data);
	return 0;
}
