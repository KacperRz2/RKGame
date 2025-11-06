#ifndef RENDER_H_
#define RENDER_H_

int GraphicsInitiation(Render_data* const);
void SetRenderData(Render_data* const);
void ResetRenderData(Render_data* const);
static void RenderHumanPlayerBlade(Render_data* const, Blade* const);
static void RenderProjectiles(Render_data* const, Game_data* const);
static void RenderBeings(Render_data* const, Game_data* const);
static void RenderMap(Render_data* const, Player* const);
static bool GetRenderPointFromTrue(Render_data* const, const float, const float, const Player* const, SDL_FPoint* const, World* const);
void RenderTextInfo(Render_data* const, const Uint64, Game_data* const);//temporary func.
static void RenderPlayerStatus(Render_data* const, Player* const, const Game_data* const);
void RenderMainMenu(Render_data* const, const float);
void RenderGame(Render_data* const, Game_data* const, const int);
static void SetSineCosine(Render_data* const, Player* const);
void ClearRenderData(Render_data* const);
static void RenderTerrain(Render_data* const, Game_data* const);
static bool GetExtendedRenderPointFromTrue(Render_data* const, const float, const float, const float, const Player* const, SDL_FPoint* const);
static void RenderGunSight(Render_data* const);
static void RenderStaticThings(Render_data* const, Game_data* const);
static void RenderStaticThing(Render_data* const, const float, const float, Player* const, const float, const int, World* const);
void DrawMap(Render_data* const, World* const);
static void RenderHumanPlayer(Render_data* const);
static void RenderHumanPlayerBarrier(Render_data* const, const Player* const);
static void RenderHumanPlayerScroll(Render_data* const, const bool);
static void	RenderPlayersBladesAndScrolls(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static void	RenderPlayers(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static void	RenderBarriers(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static Placement GetBeingWeaponPlacement(const Being* const);
static Placement GetWeaponPlacement(Placement* const, Placement* const, const int, const int);
static void RenderBackground(Render_data* const);
static void RenderFrame(Render_data* const, const SDL_FRect* const, const float);
static void RenderDirectionArrow(Render_data* const, const double);
static void RenderQuickScrolls(Render_data* const, const Player* const);
static void RenderScrollsManagement(Render_data* const, const Player* const);
static void DrawBackgroud(Render_data* const);
static void RenderPortrait(Render_data* const);
static SDL_FPoint GetCharacterXPositionAndWidth(const int);
static unsigned int GetCharacterRow(const int);
static void RenderText(Render_data* const, float, const float, const float, const Uint8*);
static void RenderInt(Render_data* const, const float, const float, const float, int);
void ResetTextTextureAlpha(Render_data* const);
void SetSelectetScrollMouseSelection(const Render_data* const, Player* const);
void SetPointedScrollMouseSelection(const Render_data* const, Player* const);
int GetMousePointedPositionNum(const Render_data* const);

#endif