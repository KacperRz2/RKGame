#ifndef RENDER_H_
#define RENDER_H_

extern World world;
extern Render_data rend_data;

struct Graphics_initiation_data{
	SDL_Window** window;
	SDL_Renderer** renderer;
	SDL_Texture** textures[TEXTURES_NUM];
};
int GraphicsInitiation(struct Graphics_initiation_data* const);
void SetRenderData(const float, const float);
void RenderGunSightCross(SDL_Renderer* const);
void RenderGunSightElements(SDL_Renderer* const, const float, const float);
void RenderPlayer(SDL_Renderer* const, SDL_Texture** const, Blade* const);
void RenderGunSight(SDL_Renderer* const, const float, SDL_Texture* const);
void RenderProjectiles(SDL_Renderer* const, Projectiles_array* const, SDL_Texture* const, Player* const);
void RenderHProjectiles(SDL_Renderer* const, Projectiles_h_array* const, SDL_Texture* const, Player* const);
void RenderBeings(SDL_Renderer* const, Beings_array* const, SDL_Texture** const, Player* const);
void RenderMap(SDL_Renderer* const, SDL_Texture** const, Player* const);
bool GetRenderPointFromTrue(const float, const float, const Player* const, SDL_FPoint* const);
void RenderTextInfo(SDL_Renderer* const, Player* const , const Uint64, Beings_array* const, Projectiles_array* const, Projectiles_h_array* const, Segment* const);
void RenderPlayerStatus(SDL_Renderer* const, Player* const);
void RenderMainMenu(SDL_Renderer* const);
void RenderGame(SDL_Renderer* const, SDL_Texture** const, Player* const, Beings_array* const, Projectiles_array* const, Projectiles_h_array* const);

#endif