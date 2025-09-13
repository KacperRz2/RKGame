#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <render.h>
#include <function.h>
#include <enum.h>
#include <World.h>

int GraphicsInitiation(Render_data* const data){
	SDL_Surface* surface = NULL;
	char* bmp_path = NULL;
	const char* const texture_files[TEXTURE_FILES_NUM] = {
		"img0.bmp",
		"img1.bmp",
		"img2.bmp",
		"being.bmp",
		"img3.bmp",
		"img4.bmp",
		"img5.bmp",
		"img6.bmp",
		"img7.bmp",
		"img8.bmp",
		"img9.bmp",
		"imgA.bmp",
		"imgB.bmp",
		"imgC.bmp",
		"imgD.bmp",
		"imgE.bmp",
		"imgF.bmp",
		"img10.bmp"
	};
	SDL_SetAppMetadata("KacApp", "1.0", NULL);

	if(!SDL_Init(SDL_INIT_VIDEO)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	if(!SDL_CreateWindowAndRenderer("KacWindow", data->window_w, data->window_h, SDL_WINDOW_BORDERLESS, &data->window, &data->renderer)){
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
		*(data->textures + i + TEXTURE_TARGET_NUM) = SDL_CreateTextureFromSurface(data->renderer, surface);
		if(!*(data->textures + i + TEXTURE_TARGET_NUM)){
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
			return 3;
		}
	}

	SDL_free(bmp_path);
	SDL_DestroySurface(surface);
	
	SDL_SetTextureScaleMode(*(data->textures + tx_viewfinder), SDL_SCALEMODE_NEAREST);

	SDL_SetWindowRelativeMouseMode(data->window, true);
	SDL_WarpMouseInWindow(data->window, half(data->window_w), half(data->window_h));
	const SDL_Rect mouse_barrier = {
		data->viewfinder_rect.x,
		data->viewfinder_rect.y,
		data->viewfinder_rect.w,
		(int)(data->viewfinder * VIEWFINDER_BEFORE_PC_PART - GUN_SIGHT_MIN_DISTANCE)
	};
	SDL_SetWindowMouseRect(data->window, &mouse_barrier);
	return 0;
}

void SetRenderData(Render_data* const rend_data, const float window_w, const float window_h){
	rend_data->window = NULL;
	rend_data->renderer = NULL;
	rend_data->window_w = window_w;
	rend_data->window_h = window_h;
	rend_data->viewfinder = VIEWFINDER_SIZE;
	rend_data->viewfinder_rect.x = (int)(half(rend_data->window_w - rend_data->viewfinder));
	rend_data->viewfinder_rect.y = (int)(half(rend_data->window_h - rend_data->viewfinder));
	rend_data->viewfinder_rect.w = (int)(rend_data->viewfinder);
	rend_data->viewfinder_rect.h = (int)(rend_data->viewfinder);
	rend_data->visible_rect.x = 0.0F;
	rend_data->visible_rect.y = 0.0F;
	rend_data->visible_rect.w = rend_data->viewfinder;
	rend_data->visible_rect.h = rend_data->viewfinder;
}

static void RenderBlade(Render_data* const rend_data, Blade* const blade){
	static SDL_FRect rect_blade = {
		0.0F,
		0.0F,
		BLADE_SIZE,
		BLADE_SIZE
	};
	static const SDL_FPoint blade_rotation_point = {
		half(BLADE_SIZE),
		BLADE_SIZE * BLADE_HANDLER_POSITION
	};
	rect_blade.x = (VIEWFINDER_CENTER - half(BLADE_SIZE)) + blade->position.x;
	rect_blade.y = (VIEWFINDER_CENTER - BLADE_SIZE * BLADE_HANDLER_POSITION + PLAYER_REND_Y_SHIFT) - blade->position.y;
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_pc_blade), NULL, &rect_blade, (double)RadToDeg(blade->direction), &blade_rotation_point, SDL_FLIP_NONE);
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
		if(GetRenderPointFromTrue(rend_data, pr->position.x, pr->position.y, &g_d->pc, &point)){
			rect.x = point.x - half(BULLET_SIZE);
			rect.y = point.y - half(BULLET_SIZE);
			SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_projectile), NULL, &rect);
		}
	}
}

static void RenderHProjectiles(Render_data* const rend_data, Game_data* const g_d){
	static SDL_FRect rect = { 
		0.0F,
		0.0F,
		BULLET_SIZE,
		BULLET_SIZE
	};
	for(Projectile_hostile* pr = g_d->h_projectiles.array; pr != (g_d->h_projectiles.array + g_d->h_projectiles.num); ++pr){
		SDL_FPoint point;
		if(GetRenderPointFromTrue(rend_data, pr->position.x, pr->position.y, &g_d->pc, &point)){
			rect.x = point.x - half(BULLET_SIZE);
			rect.y = point.y - half(BULLET_SIZE);
			SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_h_projectile), NULL, &rect);
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
	static const SDL_FPoint blade_rotation_point = {
		half(BLADE_SIZE),
		BLADE_SIZE * BLADE_HANDLER_POSITION
	};
    for(Being* b = g_d->beings.array; b != (g_d->beings.array + g_d->beings.num); ++b){
		SDL_FPoint point;
		if(GetRenderPointFromTrue(rend_data, b->position.x, b->position.y, &g_d->pc, &point)){
			SDL_FRect rect = {
				point.x - half(b->type->size),
				point.y - half(b->type->size),
				b->type->size,
				b->type->size
			};
			float being_direction = b->direction - g_d->pc.direction;
			if(b->status == being_follow || b->status == being_strike){
				float sine = SineSafe(being_direction);
				float cosine = CosiSafe(being_direction);
				rect_blade.x = point.x + (b->blade.position.x * cosine + b->blade.position.y * sine) - half(BLADE_SIZE);
				rect_blade.y = point.y + (b->blade.position.x * sine - b->blade.position.y * cosine) - BLADE_SIZE * BLADE_HANDLER_POSITION;
				SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_being_blade), NULL, &rect_blade, (double)RadToDeg(b->blade.direction + being_direction), &blade_rotation_point, SDL_FLIP_NONE);
			}
			SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_being), NULL, &rect, (double)RadToDeg(being_direction), NULL, SDL_FLIP_NONE);
			if(b->status == being_stunned){
				SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_stun), NULL, &rect, -(double)b->status_ticks_left, NULL, SDL_FLIP_NONE);
			}
		}
	}
}

static void RenderMap(Render_data* const rend_data, Player* const p){
	static const int map_size = 300;
	const SDL_Rect rect = {
		(int)((half(rend_data->window_w - rend_data->viewfinder)) + rend_data->viewfinder) + 10,
		50,
		map_size,
		map_size
	};
	const SDL_FRect destination_rect0a = {
		rend_data->window_w - 100.0F,
		rend_data->window_h * 0.4F,
		50.0F,
		50.0F
	};
	const SDL_FRect destination_rect0b = {
		rend_data->window_w - 90.0F,
		rend_data->window_h * 0.4F + 10.0F,
		30.0F,
		30.0F
	};
	const SDL_FRect destination_rect1 = {
		10.0F,
		40.0F,
		50.0F,
		50.0F
	};
	SDL_FRect static pc_rect = {
		0.0F,
		0.0F,
		MINIMAP_PC_SIZE,
		MINIMAP_PC_SIZE
	};
	double rotation = RadToDeg(p->direction);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_compass), NULL, &destination_rect1, -rotation, NULL, SDL_FLIP_NONE);//compass
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_nesw), NULL, &destination_rect0a);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_arrow), NULL, &destination_rect0b, rotation, NULL, SDL_FLIP_NONE);
	SDL_SetRenderViewport(rend_data->renderer, &rect);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_map), NULL, NULL);
	SDL_SetRenderDrawColor(rend_data->renderer, 192, 0, 0, 0);
	pc_rect.x = p->position.x * (map_size / WORLD_W) - half(MINIMAP_PC_SIZE);
	pc_rect.y = p->position.y * (map_size / WORLD_H) - half(MINIMAP_PC_SIZE);
	SDL_RenderFillRect(rend_data->renderer, &pc_rect);
	SDL_SetRenderViewport(rend_data->renderer, NULL);
}

static inline bool GetRenderPointFromTrue(Render_data* const rend_data, const float true_point_x, const float true_point_y, const Player* const p, SDL_FPoint* const rend_point){
	float dx = true_point_x - p->position.x;
	if(SDL_fabsf(dx) > rend_data->viewfinder) return false;
	float dy = true_point_y - p->position.y;
	if(SDL_fabsf(dy) > rend_data->viewfinder) return false;
	rend_point->x =	VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction);
	rend_point->y =	PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction);
	if(SDL_PointInRectFloat(rend_point, &rend_data->visible_rect)) return true;
	return false;
}

void RenderTextInfo(SDL_Renderer* const rend, const Uint64 tps, Game_data* const g_d, Segment* const player_seg){
	SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
	SDL_RenderDebugTextFormat(rend, 10, 10, "Fatigue: %d", g_d->pc.fatigue_points);
	SDL_RenderDebugTextFormat(rend, 10, 20, "Position: %.2f %.2f", g_d->pc.position.x, g_d->pc.position.y);
	SDL_RenderDebugTextFormat(rend, 10, 30, "Direction: %.2f", g_d->pc.direction);

	SDL_RenderDebugTextFormat(rend, 10, 140, "Ticks per sec.: %d", tps);
	SDL_RenderDebugTextFormat(rend, 10, 150, "max FPS: ~%u", (1000000000ULL / FRAME_TIME));

	SDL_RenderDebugTextFormat(rend, 10, 160, "beings: %d", g_d->beings.num);
	SDL_RenderDebugTextFormat(rend, 10, 170, "projectiles: %d", g_d->projectiles.num);
	SDL_RenderDebugTextFormat(rend, 10, 180, "projectiles: %d", g_d->h_projectiles.num);
	SDL_RenderDebugTextFormat(rend, 10, 190, "hp: %d", g_d->pc.hit_points);
	SDL_RenderDebugTextFormat(rend, 10, 200, "coins: %d", g_d->pc.coins);
	SDL_RenderDebugTextFormat(rend, 10, 210, "boxes: %d", g_d->boxes.num);
	SDL_RenderDebugTextFormat(rend, 10, 220, "keys: %d", g_d->keys);
	SDL_RenderDebugTextFormat(rend, 10, 230, "needed: %d", g_d->needed_keys);
	SDL_RenderDebugTextFormat(rend, 10, 260, "player: x: %d y: %d", player_seg->indx.x, player_seg->indx.y);
}

static void RenderPlayerStatus(Render_data* const rend_data, Player* const p){
	const SDL_FRect rect0a = {
		10.0F,
		half(rend_data->window_h),
		(rend_data->window_w - rend_data->viewfinder) * 0.49F,
		30.0F
	};
	const SDL_FRect rect1a = {
		half(rend_data->window_w - rend_data->viewfinder) + rend_data->viewfinder + 10.0F,
		half(rend_data->window_h) + 40.0F,
		(rend_data->window_w - rend_data->viewfinder) * 0.48F,
		30.0F
	};
	const SDL_FRect rect2a = {
		10.0F,
		half(rend_data->window_h) + 40.0F,
		(rend_data->window_w - rend_data->viewfinder) * 0.49F,
		30.0F
	};
	SDL_FRect rect0b = {
		11.0F,
		half(rend_data->window_h) + 1.0F,
		0.0F,
		28.0F
	};
	SDL_FRect rect2b = {
		11.0F,
		half(rend_data->window_h) + 41.0F,
		0.0F,
		28.0F
	};
	rect0b.w = (float)p->hit_points / (float)p->max_h_p * (rect0a.w - 2.0F);
	rect2b.w = (float)p->fatigue_points / (float)p->max_fatigue * (rect2a.w - 2.0F);
	SDL_SetRenderDrawColor(rend_data->renderer, 255U, 255U, 255U, 0U);
	SDL_RenderRect(rend_data->renderer, &rect0a);
	SDL_RenderRect(rend_data->renderer, &rect1a);
	SDL_RenderRect(rend_data->renderer, &rect2a);
	SDL_SetRenderDrawColor(rend_data->renderer, 255U, 0U, 0U, 0U);
	SDL_RenderFillRect(rend_data->renderer, &rect0b);
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 127U, 255U, 255U);
	SDL_SetRenderScale(rend_data->renderer, 2.0F, 4.0F);
	SDL_RenderDebugTextFormat(rend_data->renderer, (half(rend_data->window_w - rend_data->viewfinder) + rend_data->viewfinder + 11.0F) * 0.5F, (half(rend_data->window_h) + 41.0F) * 0.25F, "%d", p->magic_points);
	SDL_SetRenderScale(rend_data->renderer, 1.0F, 1.0F);
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 255U, 0U, 0U);
	SDL_RenderFillRect(rend_data->renderer, &rect2b);
}

void RenderMainMenu(Render_data* const rend_data){
	static float angle = 0.0F;
	angle += 0.02F;
	if(angle >= SDL_PI_F * 2.0F){
		angle = 0.0F;
	}
	Uint8 colour = (Uint8)((sine(angle) + 1.0F) * 127.5F);
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 0U, 0U, 0U);
	SDL_RenderClear(rend_data->renderer);
	SDL_SetRenderDrawColor(rend_data->renderer, colour, colour, colour, 255U);
	SDL_SetRenderScale(rend_data->renderer, 4.0F, 4.0F);
	SDL_RenderDebugText(rend_data->renderer, rend_data->window_w * 0.25F * 0.25F, half(rend_data->window_h) * 0.25F, "Press SPACE");
	SDL_SetRenderScale(rend_data->renderer, 1.0F, 1.0F);
	SDL_RenderPresent(rend_data->renderer);
}

void RenderGame(Render_data* const rend_data, Game_data* const g_d){
	SetSineCosine(rend_data, &g_d->pc);
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 0);
	SDL_RenderClear(rend_data->renderer);
	SDL_SetRenderViewport(rend_data->renderer, &rend_data->viewfinder_rect);
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 0);
	SDL_RenderFillRect(rend_data->renderer, NULL);
	RenderTerrain(rend_data, g_d);
	RenderStaticThings(rend_data, g_d);
	RenderProjectiles(rend_data, g_d);
	if(!(g_d->pc.control_flags & range_mode)){
		RenderBlade(rend_data, &g_d->pc.blade);
	}
	RenderBeings(rend_data, g_d);
	RenderHProjectiles(rend_data, g_d);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_viewfinder), NULL, NULL);//viewfinder
	RenderPlayer(rend_data);
	if(g_d->pc.control_flags & block){
		RenderBarrier(rend_data);
	}
	RenderGunSight(rend_data);
	SDL_SetRenderViewport(rend_data->renderer, NULL);
	RenderMap(rend_data, &g_d->pc);
	RenderPlayerStatus(rend_data, &g_d->pc);
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
	const SDL_FPoint corner_first = {g_d->pc.position.x - (rend_data->viewfinder + SEGMENT_SIZE), g_d->pc.position.y - (rend_data->viewfinder + SEGMENT_SIZE)};
	const SDL_FPoint corner_last = {g_d->pc.position.x + (rend_data->viewfinder + SEGMENT_SIZE), g_d->pc.position.y + (rend_data->viewfinder + SEGMENT_SIZE)};
	SDL_FPoint point = corner_first;
	float shift_y = SDL_fmodf(point.y, SEGMENT_SIZE) + half(SEGMENT_SIZE);
	point.x -= SDL_fmodf(point.x, SEGMENT_SIZE) + half(SEGMENT_SIZE);
	point.y -= shift_y;
	while(point.x < corner_last.x){
		while(point.y < corner_last.y){
			SDL_FPoint rend_point;
			if(GetExtendedRenderPointFromTrue(rend_data, point.x, point.y, SEGMENT_SIZE * SQRT2DIV2, &g_d->pc, &rend_point)){
				rect.x = rend_point.x - half(SEGMENT_SIZE);
				rect.y = rend_point.y - half(SEGMENT_SIZE);
				if(point.x > 0.0F && point.y > 0.0F && point.x < WORLD_SIZE && point.y < WORLD_SIZE && GetSegment(&g_d->world, point.x, point.y) != NULL){
					SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_terrain), NULL, &rect, -RadToDeg(g_d->pc.direction), NULL, SDL_FLIP_NONE);
				}else{
					SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_wall), NULL, &rect, -RadToDeg(g_d->pc.direction), NULL, SDL_FLIP_NONE);
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
	float dx = true_point_x - p->position.x;
	if(SDL_fabsf(dx) > rend_data->viewfinder + extension) return false;
	float dy = true_point_y - p->position.y;
	if(SDL_fabsf(dy) > rend_data->viewfinder + extension) return false;
	rend_point->x =	VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction);
	rend_point->y =	PLAYER_REND_Y - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction);
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
	RenderStaticThing(rend_data, g_d->world.portalA.x, g_d->world.portalA.y, &g_d->pc, DOOR_SIZE, tx_portal);
	RenderStaticThing(rend_data, g_d->world.portalB.x, g_d->world.portalB.y, &g_d->pc, DOOR_SIZE, tx_portal);
	RenderStaticThing(rend_data, g_d->world.door.x, g_d->world.door.y, &g_d->pc, DOOR_SIZE, tx_door);
	for(unsigned int i = 0U; i < g_d->boxes.num; ++i){
		RenderStaticThing(rend_data, (g_d->boxes.array + i)->location.x, (g_d->boxes.array + i)->location.y, &g_d->pc, BOX_SIZE, tx_box);
	}
}

static void RenderStaticThing(Render_data* const rend_data, const float pos_x, const float pos_y, Player* const p, const float size, const int tx_num){
	SDL_FPoint point;
	if(GetRenderPointFromTrue(rend_data, pos_x, pos_y, p, &point)){
		SDL_FRect rect = {
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

static void RenderPlayer(Render_data* const rend_data){
	const SDL_FRect rect = {
		VIEWFINDER_CENTER - half(PLAYER_SIZE),
		VIEWFINDER_CENTER - half(PLAYER_SIZE) + PLAYER_REND_Y_SHIFT,
		(float)PLAYER_SIZE,
		(float)PLAYER_SIZE
	};
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_pc), NULL, &rect);
}

static void RenderBarrier(Render_data* const rend_data){
	const SDL_FRect rect = {
		VIEWFINDER_CENTER - half(PLAYER_SIZE * 2.0F),
		VIEWFINDER_CENTER - half(PLAYER_SIZE * 2.0F) + PLAYER_REND_Y_SHIFT,
		(float)PLAYER_SIZE * 2.0F,
		(float)PLAYER_SIZE * 2.0F
	};
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_barrier), NULL, &rect);
}