#ifndef RENDER_H_
#define RENDER_H_

int GraphicsInitiation(Render_data* const);
void SetRenderData(Render_data* const, const float, const float);
static void RenderHumanPlayerBlade(Render_data* const, Blade* const);
static void RenderProjectiles(Render_data* const, Game_data* const);
static void RenderBeings(Render_data* const, Game_data* const);
static void RenderMap(Render_data* const, Player* const);
static bool GetRenderPointFromTrue(Render_data* const, const float, const float, const Player* const, SDL_FPoint* const);
void RenderTextInfo(SDL_Renderer* const, const Uint64, Game_data* const);//temporary func.
static void RenderPlayerStatus(Render_data* const, Player* const);
void RenderMainMenu(Render_data* const);
void RenderGame(Render_data* const, Game_data* const);
static void SetSineCosine(Render_data* const, Player* const);
void ClearRenderData(Render_data* const);
static void RenderTerrain(Render_data* const, Game_data* const);
static bool GetExtendedRenderPointFromTrue(Render_data* const, const float, const float, const float, const Player* const, SDL_FPoint* const);
static void RenderGunSight(Render_data* const);
static void RenderStaticThings(Render_data* const, Game_data* const);
static void RenderStaticThing(Render_data* const, const float, const float, Player* const, const float, const int);
void DrawMap(Render_data* const, World* const);
static void RenderHumanPlayer(Render_data* const);
static void RenderHumanPlayerBarrier(Render_data* const, const Player* const);
static void RenderHumanPlayerScroll(Render_data* const, const bool);
static void	RenderPlayersBladesAndScrolls(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static void	RenderPlayers(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static void	RenderBarriers(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static Placement GetBeingWeaponPlacement(const Being* const);
static Placement GetWeaponPlacement(Placement* const, Placement* const, const int, const int);

#endif