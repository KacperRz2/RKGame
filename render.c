#include <SDL3/SDL.h>
#include <macros.h>
#include <render_macros.h>
#include <types.h>
#include <render.h>
#include <function.h>
#include <enum.h>
#include <World.h>
#include <Scroll.h>
#include <Being.h>

const unsigned int beings_textures[] = BEINGS_TEXTURES;
const unsigned int beings_weapon_textures[] = BEINGS_WEAPON_TEXTURES;
const unsigned int projectile_textures[] = PROJECTILE_TEXTURES;

int GraphicsInitiation(Render_data* const rend_data){
	SDL_Surface* surface = NULL;
	char* bmp_path = NULL;
	const char* const texture_files[TEXTURE_FILES_NUM] = TEXTURE_FILES_NAMES;
	SDL_SetAppMetadata("KacApp", "1.0", NULL);

	if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	if(!SDL_CreateWindowAndRenderer("KacWindow", rend_data->window_w, rend_data->window_h, SDL_WINDOW_BORDERLESS, &rend_data->window, &rend_data->renderer)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		return 3;
	}

	for(int i = 0; i < TEXTURE_FILES_NUM; ++i){
		SDL_asprintf(&bmp_path, "%sdata/%s", SDL_GetBasePath(), *(texture_files + i));
		surface = SDL_LoadBMP(bmp_path);
		if(!surface){
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
			return 3;
		}
		*(rend_data->textures + i) = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
		if(!*(rend_data->textures + i)){
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
			return 3;
		}
	}
	SDL_free(bmp_path);
	SDL_DestroySurface(surface);

	SDL_SetWindowRelativeMouseMode(rend_data->window, true);
	SDL_WarpMouseInWindow(rend_data->window, half(rend_data->window_w), half(rend_data->window_h));
	SDL_SetWindowMouseRect(rend_data->window, &(SDL_Rect)MOUSE_RECT);
	DrawBackgroud(rend_data);
	return 0;
}

void SetRenderData(Render_data* const rend_data){
	rend_data->window = NULL;
	rend_data->renderer = NULL;
	rend_data->window_w = WINDOW_START_W;
	rend_data->window_h = WINDOW_START_H;
	rend_data->viewfinder = VIEWFINDER_SIZE;
	rend_data->viewfinder_rect = (SDL_Rect)VIEWFINDER_RECT;
	rend_data->visible_rect = (SDL_FRect)VISIBLE_RECT;
}

void ResetRenderData(Render_data* const rend_data){
	int w, h;
	SDL_GetWindowSizeInPixels(rend_data->window, &w, &h);
	rend_data->window_w = w;
	rend_data->window_h = h;
	rend_data->viewfinder = VIEWFINDER_SIZE;
	rend_data->viewfinder_rect = (SDL_Rect)VIEWFINDER_RECT;
	rend_data->visible_rect = (SDL_FRect)VISIBLE_RECT;
	SDL_WarpMouseInWindow(rend_data->window, half(rend_data->window_w), half(rend_data->window_h));
	SDL_SetWindowMouseRect(rend_data->window, &(SDL_Rect)MOUSE_RECT);
	DrawBackgroud(rend_data);
}

static void RenderHumanPlayerBlade(Render_data* const rend_data, Blade* const blade){
	static SDL_FRect rect_blade = {
		0.0F,
		0.0F,
		BLADE_SIZE,
		BLADE_SIZE
	};
	rect_blade.x = (VIEWFINDER_CENTER - half(BLADE_SIZE)) + blade->placement.position.x;
	rect_blade.y = (VIEWFINDER_CENTER - BLADE_SIZE * BLADE_HANDLER_POSITION + PLAYER_REND_Y_SHIFT) - blade->placement.position.y;
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_pc_blade), NULL, &rect_blade, (double)RadToDeg(blade->placement.direction), &(SDL_FPoint)WEAPON_ROTATION_POINT, SDL_FLIP_NONE);
}

static void RenderProjectiles(Render_data* const rend_data, Game_data* const g_d){
	static SDL_FRect rect = { 
		0.0F,
		0.0F,
		BULLET_SIZE,
		BULLET_SIZE
	};
	for(Projectile* pr = g_d->projectiles.array; pr != (g_d->projectiles.array + g_d->projectiles.num); ++pr){
		SDL_FPoint point;
		if(GetRenderPointFromTrue(rend_data, pr->position.x, pr->position.y, g_d->champions.array + g_d->human_indx, &point, &g_d->world)){
			rect.x = point.x - half(BULLET_SIZE);
			rect.y = point.y - half(BULLET_SIZE);
			SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + *(projectile_textures + pr->type_id)), NULL, &rect);
		}
	}
}

static void RenderBeings(Render_data* const rend_data, Game_data* const g_d){
	static SDL_FRect rect_blade = {
		0.0F,
		0.0F,
		BLADE_SIZE,
		BLADE_SIZE
	};
    for(unsigned int i = 0U; i < g_d->beings.num; ++i){
		Being* b = (g_d->beings.array + *(g_d->beings.indices + i));
		SDL_FPoint point;
		if((b->segment->flags & segment_in_sight) && GetRenderPointFromTrue(rend_data, b->position.x, b->position.y, g_d->champions.array + g_d->human_indx, &point, &g_d->world)){
			const SDL_FRect rect = {
				point.x - half(BeingSize(b)),
				point.y - half(BeingSize(b)),
				BeingSize(b),
				BeingSize(b)
			};
			if(b->status == being_attack_being){
				b->direction = arctan2(b->position.y - b->target.being->position.y, b->position.x - b->target.being->position.x) - SDL_PI_F * 0.5F;
			}else if(b->status == being_fly){
				b->direction = b->rend_fly_help_data.start_angle + FULL_ANGLE * (1.0F - (float)b->status_ticks_left / (float)b->rend_fly_help_data.ticks);
			}else if(b->status <= being_strike){
				b->direction = arctan2(b->position.y - b->target.player->position.y, b->position.x - b->target.player->position.x) - SDL_PI_F * 0.5F;
			}else if(b->status <= being_strike_being){
				b->direction = arctan2(b->position.y - b->target.being->position.y, b->position.x - b->target.being->position.x) - SDL_PI_F * 0.5F;
			}else if(b->status == being_search){
				b->direction = arctan2(-b->special_move_shift.y, -b->special_move_shift.x) - SDL_PI_F * 0.5F;
			}
			const float being_direction = b->direction - (g_d->champions.array + g_d->human_indx)->direction;
			const float sine = SineSafe(being_direction);
			const float cosine = CosiSafe(being_direction);
			const Placement weapon = GetBeingWeaponPlacement(b);
			rect_blade.x = point.x + (weapon.position.x * cosine + weapon.position.y * sine) - half(BLADE_SIZE);
			rect_blade.y = point.y + (weapon.position.x * sine - weapon.position.y * cosine) - BLADE_SIZE * BLADE_HANDLER_POSITION;
			SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + *(beings_weapon_textures + b->type_id)), NULL, &rect_blade, (double)RadToDeg(weapon.direction + being_direction), &(SDL_FPoint)WEAPON_ROTATION_POINT, SDL_FLIP_NONE);
			SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + *(beings_textures + b->type_id)), NULL, &rect, (double)RadToDeg(being_direction), NULL, SDL_FLIP_NONE);
			if(b->status == being_stunned){
				SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_stun), NULL, &rect, -(double)b->status_ticks_left, NULL, SDL_FLIP_NONE);
			}
		}
	}
}

static void RenderMap(Render_data* const rend_data, Player* const p){
	const int map_size = half(rend_data->window_w - rend_data->viewfinder_rect.w) - FRAME_W * 6;
	const SDL_Rect rect = {
		RIGHT_AREA_X + FRAME_W * 2,
		FRAME_W * 3,
		map_size,
		map_size
	};
	const SDL_FRect destination_rect0a = {
		rect.w - 51.0F,
		rect.h - 51.0F,
		50.0F,
		50.0F
	};
	const SDL_FRect destination_rect0b = {
		rect.w - 41.0F,
		rect.h - 41.0F,
		30.0F,
		30.0F
	};
	SDL_FRect static pc_rect = {
		0.0F,
		0.0F,
		MINIMAP_PC_SIZE,
		MINIMAP_PC_SIZE
	};
	RenderFrame(rend_data, &(SDL_FRect){
		rect.x - FRAME_W,
		rect.y - FRAME_W,
		rect.w + FRAME_W * 2.0F,
		rect.h + FRAME_W * 2.0F
	}, FRAME_W);
	double rotation = RadToDeg(p->direction);
	SDL_SetRenderViewport(rend_data->renderer, &rect);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_map), NULL, NULL);
	SDL_SetRenderDrawColor(rend_data->renderer, 192, 0, 0, 255);
	pc_rect.x = p->position.x * (map_size / WORLD_W) - half(MINIMAP_PC_SIZE);
	pc_rect.y = p->position.y * (map_size / WORLD_H) - half(MINIMAP_PC_SIZE);
	SDL_RenderFillRect(rend_data->renderer, &pc_rect);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_arrow), NULL, &destination_rect0b, rotation, NULL, SDL_FLIP_NONE);
	SDL_SetRenderViewport(rend_data->renderer, NULL);
}

static inline bool GetRenderPointFromTrue(Render_data* const rend_data, const float true_point_x, const float true_point_y, const Player* const human_player, SDL_FPoint* const rend_point, World* const w){
	float dx = true_point_x - human_player->position.x;
	if(SDL_fabsf(dx) > rend_data->viewfinder) return false;
	float dy = true_point_y - human_player->position.y;
	if(SDL_fabsf(dy) > rend_data->viewfinder) return false;
	if(!(GetSegment(w, true_point_x, true_point_y)->flags & segment_in_sight)) return false;
	*rend_point = (SDL_FPoint){
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	if(SDL_PointInRectFloat(rend_point, &rend_data->visible_rect)) return true;
	return false;
}

void RenderTextInfo(Render_data* const rend_data, const Uint64 tps, Game_data* const g_d){
	SDL_Renderer* const rend = rend_data->renderer;
	const float x = rend_data->viewfinder_rect.x + 2.0F;
	SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
	SDL_RenderDebugTextFormat(rend, x, 10, "position: %.2f %.2f", (g_d->champions.array + g_d->human_indx)->position.x, (g_d->champions.array + g_d->human_indx)->position.y);
	SDL_RenderDebugTextFormat(rend, x, 20, "direction: %.2f", (g_d->champions.array + g_d->human_indx)->direction);

	SDL_RenderDebugTextFormat(rend, x, 30, "ticks per sec.: %d", tps);
	SDL_RenderDebugTextFormat(rend, x, 40, "max FPS: ~%u", (1000000000ULL / FRAME_TIME));

	SDL_RenderDebugTextFormat(rend, x, 50, "beings: %u", g_d->beings.num - 1U);
	SDL_RenderDebugTextFormat(rend, x, 60, "projectiles: %d", g_d->projectiles.num);
	SDL_RenderDebugTextFormat(rend, x, 70, "blade charge: %f", 1.0F - (g_d->champions.array + g_d->human_indx)->blade.charge);
	SDL_RenderDebugTextFormat(rend, x, 80, "fatigue: %d", (g_d->champions.array + g_d->human_indx)->fatigue_points);
	SDL_RenderDebugTextFormat(rend, x, 90, "hp: %d", (g_d->champions.array + g_d->human_indx)->hit_points);
	SDL_RenderDebugTextFormat(rend, x, 110, "boxes: %d", g_d->boxes.num);
	SDL_RenderDebugTextFormat(rend, x, 130, "keys needed: %d", g_d->needed_keys);
	SDL_RenderDebugTextFormat(rend, x, 140, "player segment: x: %d y: %d", (g_d->champions.array + g_d->human_indx)->segment->indx.x, (g_d->champions.array + g_d->human_indx)->segment->indx.y);
	SDL_RenderDebugTextFormat(rend, x, 150, "selected scroll: %d", (g_d->champions.array + g_d->human_indx)->selected_scroll);
	SDL_RenderDebugTextFormat(rend, x, 160, "selected scroll num: %d", *((g_d->champions.array + g_d->human_indx)->scrolls + (g_d->champions.array + g_d->human_indx)->selected_scroll));
	SDL_RenderDebugTextFormat(rend, x, 170, "selected scroll cost: %d", ScrollCost((g_d->champions.array + g_d->human_indx)->selected_scroll));
	RenderMap(rend_data, g_d->champions.array + g_d->human_indx);
}

static void RenderPlayerStatus(Render_data* const rend_data, Player* const p, const Game_data* const gd){
	const float BAR_H = BAR_H_CALC;
	const SDL_FRect rect_armo = {
		BAR_X,
		AREAA_Y + BAR_H + FRAME_W * 2.0F,
		((1.0F - p->armour.multipl) / (1.0F - p->max_armour.multipl)) * BAR_W,
		SMALL_BAR_H
	};
	const SDL_FRect rect_marmo = {
		BAR_X,
		rect_armo.y + SMALL_BAR_H,
		((1.0F - p->armour.magic_multipl) / (1.0F - p->max_armour.magic_multipl)) * BAR_W,
		SMALL_BAR_H
	};
	const SDL_FRect rectHP = {
		BAR_X,
		rect_armo.y + BAR_H * 2.0F + FRAME_W * 5.0F,
		(float)p->hit_points / (float)p->max_h_p * BAR_W,
		BAR_H
	};
	const SDL_FPoint MP_position = {
		RIGHT_AREA_X + FRAME_W * 2.0F,
		AREA_MP_Y + BAR_H + FRAME_W * 2.0F
	};
	const SDL_FRect rectFP = {
		BAR_X,
		rectHP.y + BAR_H * 2.0F + FRAME_W * 5.0F,
		(float)p->fatigue_points / (float)p->max_fatigue * BAR_W,
		BAR_H
	};
	SDL_SetTextureColorMod(*(rend_data->textures + tx_bar), 128U, 128U, 128U);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_bar), NULL, &rect_armo);
	SDL_SetTextureColorMod(*(rend_data->textures + tx_bar), 128U, 128U, 255U);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_bar), NULL, &rect_marmo);
	SDL_SetTextureColorMod(*(rend_data->textures + tx_bar), 255U, 0U, 0U);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_bar), NULL, &rectHP);
	SDL_SetTextureColorMod(*(rend_data->textures + tx_bar), 0U, 255U, 0U);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_bar), NULL, &rectFP);
	SDL_SetTextureColorMod(*(rend_data->textures + tx_chars), 0U, 208U, 255U);
	RenderInt(rend_data, MP_position.x + 4.0F, MP_position.y, MP_BAR_H, p->magic_points);
	SDL_SetTextureColorMod(*(rend_data->textures + tx_chars), 255U, 0U, 127U);
	RenderInt(rend_data, MP_position.x + 4.0F, MP_position.y + MP_BAR_H, MP_BAR_H, ScrollCost(p->selected_scroll));
	SDL_SetTextureColorMod(*(rend_data->textures + tx_chars), 255U, 255U, 255U);
	RenderInt(rend_data, KEYS_NUM_X, KEYS_NUM_Y, BAR_H, gd->keys);
	RenderInt(rend_data, KEYS_NUM_X, COINS_NUM_Y, half(BAR_H), p->coins);
	if(p->blade.charge != PC_BLADE_CHARGE_BASE){
		const SDL_FRect rect_cha = {
			half(rend_data->window_w) - half(PLAYER_SIZE),
			PLAYER_REND_Y + PLAYER_SIZE,
			PLAYER_SIZE,
			10.0F
		};
		const SDL_FRect rect_chb = {
			half(rend_data->window_w) - (half(PLAYER_SIZE) - 1.0F),
			PLAYER_REND_Y + PLAYER_SIZE + 1.0F,
			(1.0F - p->blade.charge) * (rect_cha.w - 2.0F),
			8.0F
		};
		const Uint8 red = (Uint8)((1.0F - p->blade.charge) * 255.0F);
		const Uint8 green = (Uint8)(p->blade.charge * 255.0F);
		SDL_SetRenderDrawColor(rend_data->renderer, red / 4, green / 4, 0U, 255U);
		SDL_RenderRect(rend_data->renderer, &rect_cha);
		SDL_SetTextureColorMod(*(rend_data->textures + tx_bar), red, green, 0U);
		SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_bar), NULL, &rect_chb);
	}
}

void RenderMainMenu(Render_data* const rend_data, const float angle){
	Uint8 alpha = (Uint8)((sine(angle) + 1.0F) * 127.5F);
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 0U, 0U, 255U);
	SDL_RenderClear(rend_data->renderer);
	SDL_SetTextureAlphaMod(*(rend_data->textures + tx_chars), alpha);
	RenderText(rend_data, rend_data->window_w * 0.25F, half(rend_data->window_h) - 36.0F, 72.0F, (Uint8[])MENU_TEXT);
	SDL_RenderPresent(rend_data->renderer);
}

void RenderGame(Render_data* const rend_data, Game_data* const g_d, const int event_code){
	Player* const p = g_d->champions.array + g_d->human_indx;
	SetSineCosine(rend_data, p);
	RenderBackground(rend_data);
	SDL_SetRenderViewport(rend_data->renderer, &rend_data->viewfinder_rect);
	if(event_code == 0){
		RenderTerrain(rend_data, g_d);
		RenderStaticThings(rend_data, g_d);
		if(!(p->flags & range_mode)){
			RenderHumanPlayerBlade(rend_data, &p->blade);
		}else{
			RenderHumanPlayerScroll(rend_data, (bool)(p->flags & attack));
		}
		Player* players_to_rend[g_d->champions.num - 1U];
		SDL_FPoint players_rend_points[g_d->champions.num - 1U];
		unsigned int indx = 0U;
		for(unsigned int i = 0U; i < g_d->champions.num; ++i){
			if(i == g_d->human_indx){
				continue;
			}
			SDL_FPoint point;
			if(GetRenderPointFromTrue(rend_data, (g_d->champions.array + i)->position.x, (g_d->champions.array + i)->position.y, p, &point, &g_d->world)){
				*(players_to_rend + indx) = g_d->champions.array + i;
				*(players_rend_points + indx++) = point;
			}
		}
		RenderPlayersBladesAndScrolls(rend_data, p->direction, players_to_rend, players_rend_points, indx);
		RenderBeings(rend_data, g_d);
		RenderProjectiles(rend_data, g_d);
		RenderHumanPlayer(rend_data);
		RenderPlayers(rend_data, p->direction, players_to_rend, players_rend_points, indx);
		if(p->flags & block){
			RenderHumanPlayerBarrier(rend_data, p);
		}
		RenderBarriers(rend_data, p->direction, players_to_rend, players_rend_points, indx);
		SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_viewfinder), NULL, NULL);//viewfinder
		RenderGunSight(rend_data);
	}else if(event_code == event_manage_scrolls){
		RenderScrollsManagement(rend_data, p);
	}
	SDL_SetRenderViewport(rend_data->renderer, NULL);
	RenderDirectionArrow(rend_data, RadToDeg(p->direction));
	RenderPlayerStatus(rend_data, p, g_d);
	RenderQuickScrolls(rend_data, p);
}

static inline void SetSineCosine(Render_data* const rend_data, Player* const p){
	rend_data->sin_player_direction = sine(p->direction);
	rend_data->cos_player_direction = cosi(p->direction);
}

void ClearRenderData(Render_data* const rend_data){
	for(int i = 0; i < TEXTURES_NUM; ++i){
		SDL_DestroyTexture(*(rend_data->textures + i));
	}
	SDL_DestroyRenderer(rend_data->renderer);
	SDL_DestroyWindow(rend_data->window);
	SDL_Quit();
}

static void RenderTerrain(Render_data* const rend_data, Game_data* const g_d){
	static SDL_FRect rect = {
		0.0F,
		0.0F,
		SEGMENT_SIZE,
		SEGMENT_SIZE
	};
	const SDL_FPoint corner_first = {(g_d->champions.array + g_d->human_indx)->position.x - (rend_data->viewfinder + SEGMENT_SIZE), (g_d->champions.array + g_d->human_indx)->position.y - (rend_data->viewfinder + SEGMENT_SIZE)};
	const SDL_FPoint corner_last = {(g_d->champions.array + g_d->human_indx)->position.x + (rend_data->viewfinder + SEGMENT_SIZE), (g_d->champions.array + g_d->human_indx)->position.y + (rend_data->viewfinder + SEGMENT_SIZE)};
	SDL_FPoint point = corner_first;
	float shift_y = SDL_fmodf(point.y, SEGMENT_SIZE) + half(SEGMENT_SIZE);
	point.x -= SDL_fmodf(point.x, SEGMENT_SIZE) + half(SEGMENT_SIZE);
	point.y -= shift_y;
	while(point.x < corner_last.x){
		while(point.y < corner_last.y){
			SDL_FPoint rend_point;
			if(GetExtendedRenderPointFromTrue(rend_data, point.x, point.y, SEGMENT_SIZE * SQRT2DIV2, g_d->champions.array + g_d->human_indx, &rend_point)){
				rect.x = rend_point.x - half(SEGMENT_SIZE);
				rect.y = rend_point.y - half(SEGMENT_SIZE);
				if(point.x > 0.0F && point.y > 0.0F && point.x < WORLD_SIZE && point.y < WORLD_SIZE){
					Segment* s = GetSegment(&g_d->world, point.x, point.y);
					if(SegmentInSight(&(g_d->champions.array + g_d->human_indx)->position, &point, s, &g_d->world)){
						if(s != NULL){
							s->flags |= segment_in_sight | segment_known;
							SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_terrain), NULL, &rect, -RadToDeg((g_d->champions.array + g_d->human_indx)->direction), NULL, SDL_FLIP_NONE);
						}else{
							SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_wall), NULL, &rect, -RadToDeg((g_d->champions.array + g_d->human_indx)->direction), NULL, SDL_FLIP_NONE);
						}
					}else{
						if(s){
							if(s->flags & segment_known){
								SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_terrain1), NULL, &rect, -RadToDeg((g_d->champions.array + g_d->human_indx)->direction), NULL, SDL_FLIP_NONE);
							}
							s->flags &= ~(segment_in_sight);
						}
					}
				}
			}
			point.y += SEGMENT_SIZE;
		}
		point.x += SEGMENT_SIZE;
		point.y = corner_first.y - shift_y;
	}
}

static inline bool GetExtendedRenderPointFromTrue(Render_data* const rend_data, const float true_point_x, const float true_point_y, const float extension, const Player* const p, SDL_FPoint* const rend_point){
	const SDL_FRect extended_view = {
		rend_data->visible_rect.x - extension,
		rend_data->visible_rect.y - extension,
		rend_data->visible_rect.w + extension * 2.0F,
		rend_data->visible_rect.h + extension * 2.0F
	};
	const float dx = true_point_x - p->position.x;
	if(SDL_fabsf(dx) > rend_data->viewfinder + extension) return false;
	const float dy = true_point_y - p->position.y;
	if(SDL_fabsf(dy) > rend_data->viewfinder + extension) return false;
	*rend_point = (SDL_FPoint){
		VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction),
		PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction)
	};
	if(SDL_PointInRectFloat(rend_point, &extended_view)) return true;
	return false;
}

static void RenderGunSight(Render_data* const rend_data){
    float cursor_y;
	SDL_GetMouseState(NULL, &cursor_y);
	const float spread = (PLAYER_REND_Y - cursor_y) / (float)rend_data->viewfinder * GUN_SIGHT_SPREAD_RANGE + GUN_SIGHT_SPREAD_MIN;
	SDL_FRect rect = {
		VIEWFINDER_CENTER - half(GUN_SIGHT_SIZE),
		cursor_y - half(GUN_SIGHT_SIZE) - spread,
		(float)GUN_SIGHT_SIZE,
		(float)GUN_SIGHT_SIZE
	};
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_gunsightpart), NULL, &rect);
	rect.y += spread * 2.0F;
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_gunsightpart), NULL, &rect);
	rect.x -= spread;
	rect.y = cursor_y - half(GUN_SIGHT_SIZE);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_gunsightpart), NULL, &rect, 90.0, NULL, SDL_FLIP_NONE);
	rect.x += spread * 2.0F;
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_gunsightpart), NULL, &rect, 90.0, NULL, SDL_FLIP_NONE);
}

static void RenderStaticThings(Render_data* const rend_data, Game_data* const g_d){
	RenderStaticThing(rend_data, g_d->world.portalA.x, g_d->world.portalA.y, g_d->champions.array + g_d->human_indx, DOOR_SIZE, tx_portal, &g_d->world);
	RenderStaticThing(rend_data, g_d->world.portalB.x, g_d->world.portalB.y, g_d->champions.array + g_d->human_indx, DOOR_SIZE, tx_portal, &g_d->world);
	RenderStaticThing(rend_data, g_d->world.door.x, g_d->world.door.y, g_d->champions.array + g_d->human_indx, DOOR_SIZE, tx_door, &g_d->world);
	for(unsigned int i = 0U; i < g_d->boxes.num; ++i){
		RenderStaticThing(rend_data, (g_d->boxes.array + i)->location.x, (g_d->boxes.array + i)->location.y,g_d->champions.array + g_d->human_indx, BOX_SIZE, tx_box, &g_d->world);
	}
}

static void RenderStaticThing(Render_data* const rend_data, const float pos_x, const float pos_y, Player* const p, const float size, const int tx_num, World* const w){
	SDL_FPoint point;
	if(GetRenderPointFromTrue(rend_data, pos_x, pos_y, p, &point, w)){
		const SDL_FRect rect = {
			point.x - half(size),
			point.y - half(size),
			size,
			size
		};
		SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_num), NULL, &rect, (double)(-RadToDeg(p->direction)), NULL, SDL_FLIP_NONE);
	}
}

void DrawMap(Render_data* const rend_data, World* const w){
	if(*(rend_data->textures + tx_map) != NULL){
		SDL_DestroyTexture(*(rend_data->textures + tx_map));
	}
	SDL_Surface* surface = SDL_CreateSurface(BIG_SEGMENTS_X, BIG_SEGMENTS_X, SDL_PIXELFORMAT_RGBA8888);
	for(unsigned int c = BIG_SEGMENT_SEGMENTS_X / 2; c < SEGMENTS_X - 1U; c += BIG_SEGMENT_SEGMENTS_X){
		for(unsigned int r = BIG_SEGMENT_SEGMENTS_X / 2; r < SEGMENTS_Y - 1U; r += BIG_SEGMENT_SEGMENTS_X){
			if(*(*(w->segments + c) + r) == NULL){
				SDL_WriteSurfacePixel(surface, (c - 1) / BIG_SEGMENT_SEGMENTS_X, (r - 1) / BIG_SEGMENT_SEGMENTS_X, 0, 0, 0, 255);
			}else{
				SDL_WriteSurfacePixel(surface, (c - 1) / BIG_SEGMENT_SEGMENTS_X, (r - 1) / BIG_SEGMENT_SEGMENTS_X, 255, 255, 255, 255);
			}
		}
	}
	*(rend_data->textures + tx_map) = SDL_CreateTextureFromSurface(rend_data->renderer, surface);
	SDL_SetTextureScaleMode(*(rend_data->textures + tx_map), SDL_SCALEMODE_NEAREST);
	SDL_DestroySurface(surface);
}

static inline void RenderHumanPlayer(Render_data* const rend_data){
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_pc), NULL, &(SDL_FRect)PC_RECT);
		
}

static inline void RenderHumanPlayerBarrier(Render_data* const rend_data, const Player* const p){
	if(p->block_times.push < 1){
		SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_barrier), NULL, &(SDL_FRect)PC_SHIELD_RECT);
	}else{
		const float size = BARRIER_SIZE + (float)p->block_times.push * BARRIER_MAGNIFICATION;
		SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_barrier), NULL,
			&(SDL_FRect){
				VIEWFINDER_CENTER - half(size),
				VIEWFINDER_CENTER - half(size) + PLAYER_REND_Y_SHIFT,
				size,
				size
			}
		);
	}
}

static inline void RenderHumanPlayerScroll(Render_data* const rend_data, const bool unrolled){
	SDL_FRect src_rect = SRC_SCROLL_RECT;
	if(unrolled){
		src_rect.x = SCROLL_TX_SIZE;
	}
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_scroll), &src_rect, &(SDL_FRect)PC_SCROLL_RECT);
}

static void	RenderPlayersBladesAndScrolls(Render_data* const rend_data, const float human_player_direction, Player** plys, SDL_FPoint* points, const unsigned int num){
	SDL_FRect rect_blade = {
		0.0F,
		0.0F,
		BLADE_SIZE,
		BLADE_SIZE
	};
	SDL_FRect rect_scroll = {
		0.0F,
		0.0F,
		SCROLL_SIZE,
		SCROLL_SIZE
	};
	for(unsigned int i = 0U; i < num; ++i){
		const float player_direction = (*(plys + i))->direction - human_player_direction;
		const float sine = SineSafe(player_direction);
		const float cosine = CosiSafe(player_direction);
		if(!((*(plys + i))->flags & range_mode)){
			rect_blade.x = (points + i)->x + ((*(plys + i))->blade.placement.position.x * cosine + (*(plys + i))->blade.placement.position.y * sine) - half(BLADE_SIZE);
			rect_blade.y = (points + i)->y + ((*(plys + i))->blade.placement.position.x * sine - (*(plys + i))->blade.placement.position.y * cosine) - BLADE_SIZE * BLADE_HANDLER_POSITION;
			SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_pc_blade), NULL, &rect_blade, (double)RadToDeg(player_direction + (*(plys + i))->blade.placement.direction), &(SDL_FPoint)WEAPON_ROTATION_POINT, SDL_FLIP_NONE);
		}else{
			SDL_FRect src_rect = SRC_SCROLL_RECT;
			if((*(plys + i))->flags & attack){
				src_rect.x = SCROLL_TX_SIZE;
			}
			rect_scroll.x = (points + i)->x + (half(SCROLL_SIZE) * cosine + half(SCROLL_SIZE) * sine) - half(SCROLL_SIZE);
			rect_scroll.y = (points + i)->y + (half(SCROLL_SIZE) * sine - half(SCROLL_SIZE) * cosine) - half(SCROLL_SIZE);
			SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_scroll), &src_rect, &rect_scroll, (double)RadToDeg(player_direction), NULL, SDL_FLIP_NONE);
		}
	}
}

static void	RenderPlayers(Render_data* const rend_data, const float human_player_direction, Player** plys, SDL_FPoint* points, const unsigned int num){
	SDL_FRect rect_player = {
		0.0F,
		0.0F,
		(float)PLAYER_SIZE,
		(float)PLAYER_SIZE
	};
	for(unsigned int i = 0U; i < num; ++i){
		rect_player.x = (points + i)->x - half(PLAYER_SIZE);
		rect_player.y = (points + i)->y - half(PLAYER_SIZE);
		SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_pc), NULL, &rect_player, (double)RadToDeg((*(plys + i))->direction - human_player_direction), NULL, SDL_FLIP_NONE);
	}
}

static void	RenderBarriers(Render_data* const rend_data, const float human_player_direction, Player** plys, SDL_FPoint* points, const unsigned int num){
	SDL_FRect rect_barrier = {
		0.0F,
		0.0F,
		BARRIER_SIZE,
		BARRIER_SIZE
	};
	for(unsigned int i = 0U; i < num; ++i){
		if(!((*(plys + i))->flags & block)){
			continue;
		}
		rect_barrier.x = (points + i)->x - half(BARRIER_SIZE);
		rect_barrier.y = (points + i)->y - half(BARRIER_SIZE);
		if((*(plys + i))->block_times.push < 1){
			SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_barrier), NULL, &rect_barrier, (double)RadToDeg((*(plys + i))->direction - human_player_direction), NULL, SDL_FLIP_NONE);
		}else{
			const float size = BARRIER_SIZE + (float)(*(plys + i))->block_times.push * BARRIER_MAGNIFICATION;
			SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_barrier), NULL,
				&(SDL_FRect){
					(points + i)->x - half(size),
					(points + i)->y - half(size),
					size,
					size
				}, (double)RadToDeg((*(plys + i))->direction - human_player_direction), NULL, SDL_FLIP_NONE
			);
		}
	}
}

static inline Placement GetBeingWeaponPlacement(const Being* const b){
    if(b->status == being_strike || b->status == being_strike_being){
        if(b->status_ticks_left <= BEING_ATTACK_STEPS){
            const int step = BEING_ATTACK_STEPS - b->status_ticks_left;
			return GetWeaponPlacement(&(Placement)BEING_WEAPON_ATTACK_PLCMNT, &(Placement)BEING_WEAPON_BASE_PLCMNT, step, BEING_ATTACK_STEPS);
        }
        if(b->status_ticks_left <= BEING_ATTACK_STEPS * 2){
            const int step = BEING_ATTACK_STEPS * 2 - b->status_ticks_left;
            return GetWeaponPlacement(&(Placement)BEING_WEAPON_PREPARE_PLCMNT, &(Placement)BEING_WEAPON_ATTACK_PLCMNT, step, BEING_ATTACK_STEPS);
        }
        const int step = BEING_ATTACK_STEPS * 3 - b->status_ticks_left;
		return GetWeaponPlacement(&(Placement)BEING_WEAPON_BASE_PLCMNT, &(Placement)BEING_WEAPON_PREPARE_PLCMNT, step, BEING_ATTACK_STEPS);
    }else if(b->status == being_shoot || b->status == being_shoot_being){
		if(b->status_ticks_left < BEING_ATTACK_STEPS / 2){
			const int step = BEING_ATTACK_STEPS / 2 - b->status_ticks_left;
			return GetWeaponPlacement(&(Placement)BEING_WEAPON_ATTACK_PLCMNT, &(Placement)BEING_WEAPON_BASE_PLCMNT, step, BEING_ATTACK_STEPS / 2);
		}
		if(b->status_ticks_left < BEING_ATTACK_STEPS * 3){
        	return (Placement)BEING_WEAPON_ATTACK_PLCMNT;
		}
		if(b->status_ticks_left < (int)(BEING_ATTACK_STEPS * 4)){
			const int step = (int)(BEING_ATTACK_STEPS * 4) - b->status_ticks_left;
			return GetWeaponPlacement(&(Placement)BEING_WEAPON_BASE_PLCMNT, &(Placement)BEING_WEAPON_ATTACK_PLCMNT, step, BEING_ATTACK_STEPS);
		}
        return (Placement)BEING_WEAPON_BASE_PLCMNT;
    }else{
        return (Placement)BEING_WEAPON_BASE_PLCMNT;
    }
}

static inline Placement GetWeaponPlacement(Placement* const start, Placement* const end, const int step, const int steps){
	return (Placement){
		{
			start->position.x + (end->position.x - start->position.x) / (float)steps * step,
			start->position.y + (end->position.y - start->position.y) / (float)steps * step
		},
		start->direction + (end->direction - start->direction) / (float)steps * step
	};
}

static inline void RenderBackground(Render_data* const rend_data){
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_background), NULL, NULL);
}

static void RenderFrame(Render_data* const rend_data, const SDL_FRect* const frame, const float width){
	const SDL_FRect rects[8] = {
		{
			frame->x,
			frame->y,
			width,
			width
		}, {
			frame->x + frame->w - width,
			frame->y,
			width,
			width
		}, {
			frame->x,
			frame->y + frame->h - width,
			width,
			width
		}, {
			frame->x + frame->w - width,
			frame->y + frame->h - width,
			width,
			width
		}, {//
			frame->x + width,
			frame->y,
			frame->w - width * 2.0F,
			width
		}, {
			frame->x + width,
			frame->y + frame->h - width,
			frame->w - width * 2.0F,
			width
		}, {
			frame->x,
			frame->y + width,
			width,
			frame->h - width * 2.0F
		}, {
			frame->x + frame->w - width,
			frame->y + width,
			width,
			frame->h - width * 2.0F
		}
	};
	const SDL_FRect src_rects[3] = {
		{0.0F, 0.0F, 8.0F, 8.0F},
		{8.0F, 0.0F, 8.0F, 8.0F},
		{0.0F, 8.0F, 8.0F, 8.0F}
	};
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects, rects);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects, rects + 1, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects, rects + 2, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects, rects + 3, 180.0, NULL, SDL_FLIP_NONE);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 1, rects + 4);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 1, rects + 5, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 2, rects + 6);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 2, rects + 7, 0.0, NULL, SDL_FLIP_HORIZONTAL);
}

static void RenderDirectionArrow(Render_data* const rend_data, const double rotation){
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_arrow), NULL, &(SDL_FRect){
		RIGHT_AREA_X + FRAME_W + 10.0F,
		FRAME_W * 2.0F + 10.0F,
		30.0F,
		30.0F
	}, rotation, NULL, SDL_FLIP_NONE);
}

static void RenderQuickScrolls(Render_data* const rend_data, const Player* const p){
	const float BAR_H = BAR_H_CALC;
	const float Q_SCROLL_SIZE = Q_SCROLL_SIZE_CALC;
	const float shift_scroll = Q_SCROLL_SIZE + FRAME_W * 2.5F;
	SDL_FRect scroll_rect = {
		RIGHT_AREA_X + FRAME_W * 1.5F,
		AREA_Q_SCROLLS_Y,
		Q_SCROLL_SIZE,
		Q_SCROLL_SIZE
	};
	for(unsigned int i = 0U; i < 3U; ++i){
		scroll_rect.x = RIGHT_AREA_X + FRAME_W * 1.5F;
		for(unsigned int j = 0U; j < 3U; ++j){
			SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_icons), &(SDL_FRect){
				(*(p->scrolls_quick_access + i * 3 + j) % TX_ICONS_IN_ROW) * ICON_TX_SIZE,
				ICON_SCRL_0_Y + (*(p->scrolls_quick_access + i * 3 + j) / TX_ICONS_IN_ROW) * ICON_TX_SIZE,
				ICON_TX_SIZE,
				ICON_TX_SIZE
			}, &scroll_rect);
			SDL_SetTextureColorMod(*(rend_data->textures + tx_chars), 255U, 0U, 0U);
			RenderInt(rend_data, scroll_rect.x + 1.0F, scroll_rect.y - 1.0F, 16.0F, (i * 3 + j + 2) % 10);
			SDL_SetTextureColorMod(*(rend_data->textures + tx_chars), 255U, 255U, 255U);
			RenderInt(rend_data, scroll_rect.x + 1.0F, scroll_rect.y + Q_SCROLL_SIZE - 15.0F, 16.0F, *(p->scrolls + *(p->scrolls_quick_access + i * 3 + j)));
			scroll_rect.x += shift_scroll;
		}
		scroll_rect.y += shift_scroll;
	}
	for(unsigned int i = 0U; i < 9U; ++i){
		if(p->selected_scroll == *(p->scrolls_quick_access + i)){
			SDL_SetRenderDrawColor(rend_data->renderer, 255, 0, 0, 255);
			SDL_RenderRect(rend_data->renderer, &(SDL_FRect){
				RIGHT_AREA_X + FRAME_W * 1.5F + shift_scroll * (i % 3),
				AREA_Q_SCROLLS_Y + shift_scroll * (i / 3),
				Q_SCROLL_SIZE,
				Q_SCROLL_SIZE
			});
			break;
		}
	}
}

static void RenderScrollsManagement(Render_data* const rend_data, const Player* const pc){
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float scrolls_num_text_height = icon_size * 0.25F;
	SDL_FRect dst_rect = {
		FRAME_W,
		icon_size + FRAME_W * 3.0F,
		icon_size,
		icon_size
	};
	SDL_FPoint dst_point_scrolls_num = {
		FRAME_W + 4.0F,
		icon_size * 1.75F + FRAME_W * 3.0F 
	};
	const float shift = icon_size + FRAME_W;
	const SDL_FRect menu_ptr_rect = {
		FRAME_W + shift * (pc->help_data.menu_position % ICONS_IN_VIEWF_ROW),
		icon_size + FRAME_W * 3.0F + shift * (pc->help_data.menu_position / ICONS_IN_VIEWF_ROW),
		icon_size,
		icon_size
	};
	SDL_FRect quick_scroll_rect = {
		FRAME_W + shift * (*pc->scrolls_quick_access % ICONS_IN_VIEWF_ROW),
		icon_size + FRAME_W * 3.0F + shift * (*pc->scrolls_quick_access / ICONS_IN_VIEWF_ROW),
		icon_size,
		icon_size
	};
	const SDL_FRect selection_rect = {
		FRAME_W + shift * (pc->selected_scroll % ICONS_IN_VIEWF_ROW),
		icon_size + FRAME_W * 3.0F + shift * (pc->selected_scroll / ICONS_IN_VIEWF_ROW),
		icon_size,
		icon_size
	};
	for(unsigned int i = 0U; i < ICONS_IN_VIEWF_ROW - 2U; ++i){
		dst_rect.x = FRAME_W;
		dst_point_scrolls_num.x = FRAME_W + 4.0;
		for(unsigned int j = 0U; j < ICONS_IN_VIEWF_ROW; ++j){
			SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_icons), &(SDL_FRect){
				((i * ICONS_IN_VIEWF_ROW + j) % TX_ICONS_IN_ROW) * ICON_TX_SIZE,
				ICON_SCRL_0_Y + ((i * ICONS_IN_VIEWF_ROW + j) / TX_ICONS_IN_ROW) * ICON_TX_SIZE,
				ICON_TX_SIZE,
				ICON_TX_SIZE
			}, &dst_rect);
			RenderInt(rend_data, dst_point_scrolls_num.x, dst_point_scrolls_num.y, scrolls_num_text_height, *(pc->scrolls + i * ICONS_IN_VIEWF_ROW + j));
			dst_rect.x += shift;
			dst_point_scrolls_num.x += shift;
		}
		dst_rect.y += shift;
		dst_point_scrolls_num.y += shift;
	}
	SDL_SetTextureColorMod(*(rend_data->textures + tx_menu_ptr), 31U, 127U, 255U);
	SDL_SetTextureColorMod(*(rend_data->textures + tx_chars), 255U, 0U, 0U);
	for(unsigned int i = 0U; i < 9U; ++i){
		quick_scroll_rect.x = FRAME_W + shift * (*(pc->scrolls_quick_access + i) % ICONS_IN_VIEWF_ROW);
		quick_scroll_rect.y = icon_size + FRAME_W * 3.0F + shift * (*(pc->scrolls_quick_access + i) / ICONS_IN_VIEWF_ROW);
		SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_menu_ptr), NULL, &quick_scroll_rect);
		RenderInt(rend_data, quick_scroll_rect.x + 4.0F, quick_scroll_rect.y, scrolls_num_text_height, (i + 2) % 10);
	}
	SDL_SetTextureColorMod(*(rend_data->textures + tx_menu_ptr), 255U, 255U, 255U);
	SDL_SetTextureColorMod(*(rend_data->textures + tx_chars), 255U, 255U, 255U);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_menu_ptr), NULL, &menu_ptr_rect);
	SDL_SetRenderDrawColor(rend_data->renderer, 255, 0, 0, 255);
	SDL_RenderRect(rend_data->renderer, &selection_rect);
	Uint8 text[] = TEST_TEXT;
	RenderText(rend_data, FRAME_W, rend_data->viewfinder - 80.0F, 32.0F, text);
}

static void DrawBackgroud(Render_data* const rend_data){
	if(*(rend_data->textures + tx_background) != NULL){
		SDL_DestroyTexture(*(rend_data->textures + tx_background));
	}
	*(rend_data->textures + tx_background) = SDL_CreateTexture(rend_data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rend_data->window_w, rend_data->window_h);
	if(!(*(rend_data->textures + tx_background))){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError());
		exit(-1);
	}
	SDL_SetRenderTarget(rend_data->renderer, *(rend_data->textures + tx_background));
	const SDL_FRect nesw_rect = {
		RIGHT_AREA_X + FRAME_W,
		FRAME_W * 2.0F,
		50.0F,
		50.0F
	};
	const SDL_FRect rects[16] = {
		{
			FRAME_W,
			FRAME_W,
			SIDE_AREA_WIDTH,
			rend_data->window_h - FRAME_W * 2.0
		}, {
			RIGHT_AREA_X,
			FRAME_W,
			SIDE_AREA_WIDTH,
			rend_data->window_h - FRAME_W * 2.0
		}, {//
			SIDE_AREA_WIDTH,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			RIGHT_AREA_X,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			SIDE_AREA_WIDTH,
			rend_data->viewfinder_rect.y + rend_data->viewfinder_rect.h,
			FRAME_W,
			FRAME_W
		}, {
			RIGHT_AREA_X,
			rend_data->viewfinder_rect.y + rend_data->viewfinder_rect.h,
			FRAME_W,
			FRAME_W
		}, {//
			0.0F,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			0.0F + rend_data->window_w - FRAME_W,
			0.0F,
			FRAME_W,
			FRAME_W
		}, {
			0.0F,
			0.0F + rend_data->window_h - FRAME_W,
			FRAME_W,
			FRAME_W
		}, {
			0.0F + rend_data->window_w - FRAME_W,
			0.0F + rend_data->window_h - FRAME_W,
			FRAME_W,
			FRAME_W
		}, {//
			FRAME_W,
			0.0F,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {
			RIGHT_AREA_X + FRAME_W,
			0.0F,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {
			FRAME_W,
			rend_data->window_h - FRAME_W,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {
			RIGHT_AREA_X + FRAME_W,
			rend_data->window_h - FRAME_W,
			rend_data->viewfinder_rect.x - FRAME_W * 3.0F,
			FRAME_W
		}, {//
			0.0F,
			FRAME_W,
			FRAME_W,
			rend_data->window_h - FRAME_W * 2.0F
		}, {
			rend_data->window_w - FRAME_W,
			FRAME_W,
			FRAME_W,
			rend_data->window_h - FRAME_W * 2.0F
		}
	};
	const SDL_FRect src_rects[6] = {
		{0.0F, 0.0F, 512.0F, 1024.0F},
		{512.0F, 0.0F, 512.0F, 1024.0F},
		{0.0F, 0.0F, 8.0F, 8.0F},
		{8.0F, 0.0F, 8.0F, 8.0F},
		{0.0F, 8.0F, 8.0F, 8.0F},
		{8.0F, 8.0F, 8.0F, 8.0F}
	};
	const float BAR_H = BAR_H_CALC;
	const float Q_SCROLL_SIZE = Q_SCROLL_SIZE_CALC;
	SDL_FRect frames[] = {
		{
			ICON_X - FRAME_W,
			AREAA_Y - FRAME_W,
			BAR_H + FRAME_W * 2.0F,
			BAR_H + FRAME_W * 2.0F
		}, {
			BAR_X - FRAME_W,
			AREAA_Y + BAR_H + FRAME_W,
			BAR_W + FRAME_W * 2.0F,
			BAR_H + FRAME_W * 2.0F
		}, {
			RIGHT_AREA_X + FRAME_W,
			AREA_MP_Y - FRAME_W,
			BAR_H + FRAME_W * 2.0F,
			BAR_H + FRAME_W * 2.0F
		}, {
			RIGHT_AREA_X + FRAME_W,
			AREA_MP_Y + BAR_H + FRAME_W,
			BAR_W + FRAME_W * 2.0F,
			MP_BAR_H * 2.0F + FRAME_W * 2.0F
		}, {
			RIGHT_AREA_X + FRAME_W * 0.5F,
			AREA_Q_SCROLLS_Y - FRAME_W,
			Q_SCROLL_SIZE + FRAME_W * 2.0F,
			Q_SCROLL_SIZE + FRAME_W * 2.0F
		}, {
			nesw_rect.x - FRAME_W,
			nesw_rect.y - FRAME_W,
			nesw_rect.w + FRAME_W * 2.0F,
			nesw_rect.h + FRAME_W * 2.0F
		}, {
			ICON_X + BAR_H + FRAME_W,
			KEY_ICON_FRAME_Y,
			BAR_W - BAR_H,
			BAR_H + FRAME_W * 2.0F
		}
	};
	SDL_FRect empty_areas[] = {
		{
			ICON_X,
			AREAA_Y,
			BAR_H,
			BAR_H
		}, {
			RIGHT_AREA_X + FRAME_W * 2.0F,
			AREA_MP_Y,
			BAR_H,
			BAR_H
		}, {
			RIGHT_AREA_X + FRAME_W * 2.0F,
			AREA_MP_Y + BAR_H + FRAME_W * 2.0F,
			BAR_W,
			MP_BAR_H
		}, {
			RIGHT_AREA_X + FRAME_W * 1.5F,
			AREA_Q_SCROLLS_Y,
			Q_SCROLL_SIZE,
			Q_SCROLL_SIZE
		}
	};
	const SDL_FRect empty_bar_rects[] = {
		{
			BAR_X,
			AREAA_Y + BAR_H + FRAME_W * 2.0F,
			BAR_W,
			SMALL_BAR_H
		}, {
			BAR_X,
			AREAA_Y + BAR_H + FRAME_W * 2.0F + SMALL_BAR_H,
			BAR_W,
			SMALL_BAR_H
		}, {
			BAR_X,
			AREAA_Y + BAR_H * 3.0F + FRAME_W * 7.0F,
			BAR_W,
			BAR_H
		}, {
			BAR_X,
			AREAA_Y + BAR_H * 5.0F + FRAME_W * 12.0F,
			BAR_W,
			BAR_H
		}, {
			BAR_X,
			KEY_ICON_FRAME_Y + FRAME_W,
			BAR_W ,
			BAR_H
		}, {
			BAR_X,
			KEY_ICON_FRAME_Y + FRAME_W * 4.0F + BAR_H,
			BAR_W,
			BAR_H
		}
	};
	SDL_FRect icon_scr_rect = FIRST_ICON_SRC_RECT;
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_backgr0), src_rects, rects);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_backgr0), src_rects + 1, rects + 1);

	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 2, rects + 2, 180.0, NULL, SDL_FLIP_NONE);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 2, rects + 3, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 2, rects + 4, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 2, rects + 5);

	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 5, rects + 6);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 5, rects + 7, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 5, rects + 8, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 5, rects + 9, 180.0, NULL, SDL_FLIP_NONE);

	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 3, rects + 10, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 3, rects + 11, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 3, rects + 12);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 3, rects + 13);

	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 4, rects + 14, 0.0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_backgr1), src_rects + 4, rects + 15);

	RenderFrame(rend_data, &(SDL_FRect)VIEWFINDER_FRAME, FRAME_W);
	RenderPortrait(rend_data);
	const float shift_bar = BAR_H * 2.0F + FRAME_W * 5.0F;
	const float shift_scroll = Q_SCROLL_SIZE + FRAME_W * 2.5F;
	icon_scr_rect.x = ICON_TX_SIZE * ic_arm;
	for(unsigned int i = 0U; i < 3U; ++i){
		RenderFrame(rend_data, frames, FRAME_W);
		RenderFrame(rend_data, frames + 1, FRAME_W);
		SDL_SetTextureColorMod(*(rend_data->textures + tx_backgr1), 127U, 127U, 127U);
		SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), &FRAME_PART_0, empty_areas, 0.0F, NULL, SDL_FLIP_VERTICAL);
		SDL_SetTextureColorMod(*(rend_data->textures + tx_backgr1), 255U, 255U, 255U);
		SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_icons), &icon_scr_rect, empty_areas);
		icon_scr_rect.x += ICON_TX_SIZE;
		frames->y += shift_bar;
		(frames + 1)->y += shift_bar;
		empty_areas->y += shift_bar;
	}
	SDL_SetTextureColorMod(*(rend_data->textures + tx_backgr1), 127U, 127U, 127U);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), &FRAME_PART_0, empty_bar_rects + 4, 0.0F, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), &FRAME_PART_0, empty_bar_rects + 5, 0.0F, NULL, SDL_FLIP_VERTICAL);
	empty_areas->y = KEY_ICON_FRAME_Y + FRAME_W;
	icon_scr_rect.x = ICON_TX_SIZE * ic_key;
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_icons), &icon_scr_rect, empty_areas);
	empty_areas->y += BAR_H + FRAME_W * 3.0F;
	icon_scr_rect.x = ICON_TX_SIZE * ic_coin;
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_icons), &icon_scr_rect, empty_areas);
	SDL_SetTextureColorMod(*(rend_data->textures + tx_backgr1), 255U, 255U, 255U);
	frames->y = KEY_ICON_FRAME_Y;
	RenderFrame(rend_data, frames, FRAME_W);
	RenderFrame(rend_data, frames + 6, FRAME_W);
	(frames + 6)->y += BAR_H + FRAME_W * 3.0F;
	RenderFrame(rend_data, frames + 6, FRAME_W);
	frames->y +=  BAR_H + FRAME_W * 3.0F;
	RenderFrame(rend_data, frames, FRAME_W);
	RenderFrame(rend_data, frames + 2, FRAME_W);
	RenderFrame(rend_data, frames + 3, FRAME_W);
	RenderFrame(rend_data, frames + 5, FRAME_W);
	SDL_SetTextureColorMod(*(rend_data->textures + tx_backgr1), 127U, 127U, 127U);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), &FRAME_PART_0, empty_areas + 1, 0.0F, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), &FRAME_PART_0, empty_areas + 2, 0.0F, NULL, SDL_FLIP_VERTICAL);
	(empty_areas + 2)->y += MP_BAR_H;
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), &FRAME_PART_0, empty_areas + 2, 0.0F, NULL, SDL_FLIP_VERTICAL);
	SDL_SetTextureColorMod(*(rend_data->textures + tx_backgr1), 255U, 255U, 255U);
	icon_scr_rect.x = ICON_TX_SIZE * ic_mp;
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_icons), &icon_scr_rect, empty_areas + 1);
	for(unsigned int i = 0U; i < 3U; ++i){
		(frames + 4)->x = RIGHT_AREA_X + FRAME_W * 0.5F;
		(empty_areas + 3)->x = RIGHT_AREA_X + FRAME_W * 1.5F;
		for(unsigned int j = 0U; j < 3U; ++j){
			RenderFrame(rend_data, frames + 4, FRAME_W);
			SDL_SetTextureColorMod(*(rend_data->textures + tx_backgr1), 127U, 127U, 127U);
			SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), &FRAME_PART_0, empty_areas + 3, 0.0F, NULL, SDL_FLIP_VERTICAL);
			SDL_SetTextureColorMod(*(rend_data->textures + tx_backgr1), 255U, 255U, 255U);
			(frames + 4)->x += shift_scroll;
			(empty_areas + 3)->x += shift_scroll;
		}
		(frames + 4)->y += shift_scroll;
		(empty_areas + 3)->y += shift_scroll;
	}
	for(unsigned int i = 0U; i < 4U; ++i){
		SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_backgr1), &FRAME_PART_0, empty_bar_rects + i, 0.0F, NULL, SDL_FLIP_VERTICAL);
	}
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(rend_data->renderer, &(SDL_FRect){
		rend_data->viewfinder_rect.x,
		rend_data->viewfinder_rect.y,
		rend_data->viewfinder_rect.w,
		rend_data->viewfinder_rect.h
	});
	SDL_RenderFillRect(rend_data->renderer, &nesw_rect);
	icon_scr_rect.x = ICON_TX_SIZE * ic_directions;
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_icons), &icon_scr_rect, &nesw_rect);
	SDL_SetRenderTarget(rend_data->renderer, NULL);
}

static void RenderPortrait(Render_data* const rend_data){
	const SDL_FRect portrait_frame_rect = {
		SIDE_AREA_WIDTH < PORTRAIT_H ? PORTRAIT_Y - FRAME_W : FRAME_W + half(SIDE_AREA_WIDTH) - half(PORTRAIT_H),
		PORTRAIT_Y - FRAME_W,
		SIDE_AREA_WIDTH < PORTRAIT_H ? SIDE_AREA_WIDTH - FRAME_W * 2.0F : PORTRAIT_H,
		SIDE_AREA_WIDTH < PORTRAIT_H ? SIDE_AREA_WIDTH - FRAME_W * 2.0F : PORTRAIT_H
	};
	const SDL_FRect portrait_rect = {
		portrait_frame_rect.x + FRAME_W,
		portrait_frame_rect.y + FRAME_W,
		portrait_frame_rect.w - FRAME_W * 2.0F,
		portrait_frame_rect.h - FRAME_W * 2.0F,
	};
	RenderFrame(rend_data, &portrait_frame_rect, FRAME_W);
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(rend_data->renderer, &portrait_rect);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_icon), NULL, &portrait_rect);
}

static SDL_FPoint GetCharacterXPositionAndWidth(const int character_num){
	const SDL_FPoint x_positions[] = CHARACTERS_X_AND_W;
	return *(x_positions + character_num);
}

static inline unsigned int GetCharacterRow(const int character_num){
	if(character_num >= ROW_5_CHARS){
		return 6U;
	}
	if(character_num >= ROW_4_CHARS){
		return 5U;
	}
	if(character_num >= ROW_3_CHARS){
		return 4U;
	}
	if(character_num >= ROW_2_CHARS){
		return 3U;
	}
	if(character_num >= ROW_1_CHARS){
		return 2U;
	}
	if(character_num >= ROW_0_CHARS){
		return 1U;
	}
	return 0U;
}

static void RenderText(Render_data* const rend_data, float x, const float y, const float size, const Uint8* char_nums){
	do{
		const SDL_FPoint x_and_width = GetCharacterXPositionAndWidth(*char_nums);
		const float size_factor = size / CHARS_ROW_HEIGHT;
		const float char_width = x_and_width.y * size_factor;
		SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_chars), &(SDL_FRect){
			x_and_width.x,
			GetCharacterRow(*char_nums) * CHARS_ROW_HEIGHT,
			x_and_width.y,
			CHARS_ROW_HEIGHT
		}, &(SDL_FRect){
			x,
			y,
			char_width,
			CHARS_ROW_HEIGHT * size_factor
		});
		x += char_width + 1.0F;
		++char_nums;
	}while(*char_nums != char_end);
}

static void RenderInt(Render_data* const rend_data, const float x, const float y, const float size, int num){
	const Uint8 cipher_nums[] = CIPHER_NUMS;
	unsigned int text_len = 1U;
	int tmp = 10;
	while(num >= tmp){
		++text_len;
		tmp *= 10;
	}
	Uint8 char_nums[text_len + 1U];
	for(unsigned int i = 0U; i < text_len; ++i){
		tmp /= 10;
		unsigned int cipher = num / tmp;
		*(char_nums + i) = *(cipher_nums + cipher);
		num %= tmp;
	}
	*(char_nums + text_len) = char_end;
	RenderText(rend_data, x, y, size, char_nums);
}

extern inline void ResetTextTextureAlpha(Render_data* const rend_data){
	SDL_SetTextureAlphaMod(*(rend_data->textures + tx_chars), 255U);
}

void SetSelectetScrollMouseSelection(const Render_data* const rend_data, Player* const p){
	const int num = GetMousePointedPositionNum(rend_data);
	if(num >= 0){
		p->selected_scroll = num;
	}
}

void SetPointedScrollMouseSelection(const Render_data* const rend_data, Player* const p){
	const int num = GetMousePointedPositionNum(rend_data);
	if(num >= 0){
		p->help_data.menu_position = num;
	}
}

int GetMousePointedPositionNum(const Render_data* const rend_data){
	const float icon_size = SCROLLS_MANAG_ICON_SIZE;
	const float shift = icon_size + FRAME_W;
	const SDL_FRect manage_scrolls_rect = MANAGE_SCROLLS_RECT;
	SDL_FPoint mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);
	if(SDL_PointInRectFloat(&mouse, &manage_scrolls_rect)){
		const unsigned int num = (unsigned int)((mouse.x - manage_scrolls_rect.x) / shift) + (unsigned int)((mouse.y - manage_scrolls_rect.y) / shift) * ICONS_IN_VIEWF_ROW;
		if(num < SCROLLS_NUM){
			return num;
		}
		return SCROLLS_NUM - 1U;
	}
	return -1;
}