#ifndef RENDER_H_
#define RENDER_H_

extern World world;

struct Graphics_initiation_data {
	SDL_Window** window;
	SDL_Renderer** renderer;
	SDL_Texture** textures[TEXTURES_NUM];
};
int GraphicsInitiation(struct Graphics_initiation_data* const);
void RenderGunSightCross(SDL_Renderer* const);
void RenderGunSightElements(SDL_Renderer* const, const float, const float);
void RenderPlayer(SDL_Renderer* const, SDL_Texture** const, Blade* const);
void RenderGunSight(SDL_Renderer* const, const float, SDL_Texture* const);
void RenderProjectiles(SDL_Renderer* const, Projectiles_array* const, SDL_Texture* const, Player* const);
void RenderHProjectiles(SDL_Renderer* const, Projectiles_h_array* const, SDL_Texture* const, Player* const);
void RenderBeings(SDL_Renderer* const, Beings_array* const, SDL_Texture** const, Player* const);
void RenderMap(SDL_Renderer* const, Player* const);

#endif