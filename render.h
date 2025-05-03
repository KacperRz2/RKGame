#ifndef RENDER_H_
#define RENDER_H_

struct Graphics_initiation_data {
	SDL_Window** window;
	SDL_Renderer** renderer;
	SDL_Texture** textures[TEXTURES_NUM];
};
int graphicsInitiation(struct Graphics_initiation_data*);
void renderGunSightCross(SDL_Renderer*);
void renderGunSightElements(SDL_Renderer*, const float, const float);
void renderPlayer(SDL_Renderer*, SDL_Texture*);
void renderGunSight(SDL_Renderer*, const float, SDL_Texture*);
void drawStaticWorld(SDL_Renderer*, SDL_Texture*);
void renderProjectiles(SDL_Renderer*, Projectiles_array*, SDL_Texture*);
void renderBeings(SDL_Renderer*, Beings_array*, SDL_Texture*, SDL_FRect);

#endif