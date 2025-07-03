#ifndef RENDER_H_
#define RENDER_H_

struct Graphics_initiation_data {
	SDL_Window** window;
	SDL_Renderer** renderer;
	SDL_Texture** textures[TEXTURES_NUM];
};
int GraphicsInitiation(struct Graphics_initiation_data*);
void RenderGunSightCross(SDL_Renderer*);
void RenderGunSightElements(SDL_Renderer*, const float, const float);
void RenderPlayer(SDL_Renderer*, SDL_Texture*);
void RenderGunSight(SDL_Renderer*, const float, SDL_Texture*);
void RenderProjectiles(SDL_Renderer*, Projectiles_array*, SDL_Texture*, World*, Player*);
void RenderBeings(SDL_Renderer*, Beings_array*, SDL_Texture*, World*, Player*);

#endif