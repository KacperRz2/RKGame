#ifndef RENDER_H_
#define RENDER_H_

int GraphicsInitiation(Render_data* const);
void SetRenderData(Render_data* const, const float, const float);
// void RenderGunSightCross(SDL_Renderer* const);
// void RenderGunSightElements(SDL_Renderer* const, const float, const float);
void RenderPlayer(Render_data* const, Blade* const);
// void RenderGunSight(SDL_Renderer* const, const float, SDL_Texture* const);
void RenderProjectiles(Render_data* const, Projectiles_array* const, Player* const);
void RenderHProjectiles(Render_data* const, Projectiles_h_array* const, Player* const);
void RenderBeings(Render_data* const, Game_data* const);
void RenderMap(Render_data* const, Player* const);
bool GetRenderPointFromTrue(Render_data* const, const float, const float, const Player* const, SDL_FPoint* const);
void RenderTextInfo(SDL_Renderer* const, const Uint64, Game_data* const, Segment* const);//temporary func.
void RenderPlayerStatus(Render_data* const, Player* const);
void RenderMainMenu(Render_data* const);
void RenderGame(Render_data* const, Game_data* const);
void SetSineCosine(Render_data* const, Player* const);
void ClearRenderData(Render_data* const);

#endif