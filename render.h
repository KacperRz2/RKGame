#ifndef RENDER_H_
#define RENDER_H_

int GraphicsInitiation(Render_data* const);

void AddVisalEffect(Visual_effects* const, const Visual_effect* const);
static void RemoveVisalEffect(Visual_effects* const, const Uint16);
static void RenderVisualEffects(Render_data* const, Game_data* const);
void AddDamageVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddBonusVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddCurseVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddDeadVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddPortalVisualEffect(Visual_effects* const, const SDL_FPoint* const);
void AddProjectileVisualEffect(Visual_effects* const, const SDL_FPoint* const);

static void DrawBeings(Render_data* const, SDL_Surface*, char*);
static void DrawColouredThings(Render_data* const, SDL_Surface*, char*);
static void DrawBeing(Render_data* const, SDL_Texture* const, SDL_Texture* const, SDL_Texture* const, const Uint8* const, const Uint8* const);
static void DrawColouredThing(Render_data* const, SDL_Texture* const, SDL_Texture* const, const Uint8* const);
void SetRenderData(Render_data* const);
void ResetRenderData(Render_data* const);
static void RenderHumanPlayerBlade(Render_data* const, Blade* const);
static void RenderProjectiles(Render_data* const, Game_data* const);
static void RenderBeing(Render_data* const, Game_data* const, Being* const, const unsigned int, const unsigned int);
static void RenderBeings(Render_data* const, Game_data* const, Segment** const, const unsigned int);
static void RenderMap(Render_data* const, Player* const);
static bool GetRenderPointFromTrue(Render_data* const, const float, const float, const Player* const, SDL_FPoint* const, World* const);
static bool GetRenderPointFromTrueWithKnownSegmentVisibility(Render_data* const, const float, const float, const Player* const, SDL_FPoint* const);
void RenderTextInfo(Render_data* const, const Uint64, Game_data* const);//temporary func.
static void RenderPlayerStatus(Render_data* const, Player* const, const Game_data* const);
void RenderMainMenu(Render_data* const, const unsigned int);
void RenderGame(Render_data* const, Game_data* const, const int);
static void SetSineCosine(Render_data* const, Player* const);
void ClearRenderData(Render_data* const);
static void RenderTerrain(Render_data* const, Game_data* const, Segment** const, unsigned int* const);
static bool GetExtendedRenderPointFromTrue(Render_data* const, const float, const float, const float, const Player* const, SDL_FPoint* const);
static void RenderGunSight(Render_data* const);
static void RenderStaticThings(Render_data* const, Game_data* const);
static void RenderStaticThing(Render_data* const, const float, const float, Player* const, const float, const int, World* const);
static void RenderStaticThingRotating(Render_data* const, const float, const float, Player* const, const float, const int, World* const, const float);
void DrawMap(Render_data* const, const World* const);
void UpdateMap(Render_data* const, const Uint8, const Uint8, const Uint8, const unsigned int, const unsigned int);
static void RenderHumanPlayer(Render_data* const);
static void RenderHumanPlayerBarrier(Render_data* const, const Player* const);
static void RenderHumanPlayerScroll(Render_data* const, const bool);
static void	RenderPlayersBladesAndScrolls(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static void	RenderPlayers(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static void	RenderBarriers(Render_data* const, const float, Player**, SDL_FPoint*, const unsigned int);
static Placement GetBeingWeaponPlacement(const Being* const);
static Placement GetWeaponPlacement(Placement* const, Placement* const, const int, const int);
static void RenderBackground(Render_data* const);
static void RenderFrame(Render_data* const, SDL_Texture* const, const SDL_FRect* const, const float);
static void RenderDirectionArrow(Render_data* const, const double);
static void RenderQuickScrolls(Render_data* const, const Player* const);
static void RenderScrollsManagement(Render_data* const, const Player* const);
static void RenderMenu(Render_data* const, const Player* const);
static void DrawBackgroud(Render_data* const, SDL_Surface*, char*);
static void RenderPortrait(Render_data* const, SDL_Texture* const);
static SDL_FPoint GetCharacterXPositionAndWidth(const int);
static unsigned int GetCharacterRow(const int);
static void RenderText(Render_data* const, float, const float, const float, const Uint8*);
static void RenderInt(Render_data* const, const float, const float, const float, int);
void ResetTextTextureAlpha(Render_data* const);
void SetSelectetScrollMouseSelection(const Render_data* const, Player* const);
void SetPointedScrollMouseSelection(const Render_data* const, Player* const);
void SetPointedOptionMouseSelection(const Render_data* const, Uint8* const);
int GetMouseScrollManagPositionNum(const Render_data* const);
static int GetMouseMenuPositionNum(const Render_data* const);
void RenderDefeatedScreen(Render_data* const);
void RenderVictoryScreen(Render_data* const);

#endif