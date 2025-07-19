#include <header.h>

int main(int argc, char* argv[]){
	SetRenderData(1600, 900);
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* textures[TEXTURES_NUM];
	SDL_Event event;
	for(unsigned int i = 0U; i < ANGLE_PARTS; ++i){
		*(si + i) = SDL_sinf(MIN_ANGLE * i);
		*(co + i) = SDL_cosf(MIN_ANGLE * i);
	}
	struct Graphics_initiation_data* graphics_initiation_data = (struct Graphics_initiation_data*)SDL_malloc(sizeof(struct Graphics_initiation_data));
	graphics_initiation_data->window = &window;
	graphics_initiation_data->renderer = &renderer;
	for(int i = 0; i < TEXTURES_NUM; ++i){
		*(graphics_initiation_data->textures + i) = textures + i;
	}
	if(GraphicsInitiation(graphics_initiation_data)) return 1;
	SDL_free(graphics_initiation_data);

	while(1){
		if(MainMenuLoop(&event, renderer, window) == menu_quit) break;
		GameLoop(&event, renderer, window, textures);
	}

	for(int i = 0; i < TEXTURES_NUM; ++i){
		SDL_DestroyTexture(*(textures + i));
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
