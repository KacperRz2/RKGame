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
	const char* texture_files[TEXTURE_FILES_NUM] = {
		"img0.bmp",
		"img1.bmp",
		"img2.bmp",
		"being.bmp",
		"img3.bmp",
		"img4.bmp",
		"img5.bmp",
		"img6.bmp",
		"img7.bmp",
		"img8.bmp"
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

	*data->textures = SDL_CreateTexture(data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, GUN_SIGHT_SIZE, GUN_SIGHT_SIZE);

	if(*data->textures == NULL){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError()); return 1;
	}
	
	SDL_SetTextureScaleMode(*(data->textures + 5), SDL_SCALEMODE_NEAREST);

	SDL_SetWindowRelativeMouseMode(data->window, true);
	//SDL_HideCursor();
	return 0;
}

void SetRenderData(Render_data* const rend_data, const float window_w, const float window_h){
	rend_data->window = NULL;
	rend_data->renderer = NULL;
	rend_data->window_w = window_w;
	rend_data->window_h = window_h;
	rend_data->viewfinder = VIEWFINDER_SIZE;
	rend_data->viewfinder_rect.x = (int)((WINDOW_W - VIEWFINDER) * 0.5F);
	rend_data->viewfinder_rect.y = (int)((WINDOW_H - VIEWFINDER) * 0.5F);
	rend_data->viewfinder_rect.w = (int)(VIEWFINDER);
	rend_data->viewfinder_rect.h = (int)(VIEWFINDER);
	rend_data->visible_rect.x = 0.0F;
	rend_data->visible_rect.y = 0.0F;
	rend_data->visible_rect.w = VIEWFINDER;
	rend_data->visible_rect.h = VIEWFINDER;
}

// void RenderGunSightCross(SDL_Renderer* const rend){
// 	static const SDL_FRect rect0 = { 
// 		GUN_SIGHT_SIZE * 0.5F - 1.0F,
// 		1.0F,
// 		3.0F,
// 		(float)GUN_SIGHT_SIZE - 2.0F
// 	};
// 	static const SDL_FRect rect1 = {
// 		1.0F,
// 		GUN_SIGHT_SIZE * 0.5F - 1.0F,
// 		(float)GUN_SIGHT_SIZE - 2.0F,
// 		3.0F
// 	};
// 	SDL_RenderFillRect(rend, &rect0);
// 	SDL_RenderFillRect(rend, &rect1);
// }

// void RenderGunSightElements(SDL_Renderer* const rend, const float distance, const float range){
// 	SDL_SetRenderDrawColor(rend, 64, 64, 64, 0);
// 	SDL_RenderClear(rend);
// 	if(distance > range){
// 		SDL_SetRenderDrawColor(rend, 255, 0, 0, 64);
// 		RenderGunSightCross(rend);
// 	}else{
// 		SDL_FRect rect;
// 		rect.w = GUN_SIGHT_SIZE * distance / range * 0.9F;
// 		rect.h = rect.w;
// 		rect.x = GUN_SIGHT_SIZE * 0.5F - rect.w * 0.5F;
// 		rect.y = rect.x;
// 		SDL_SetRenderDrawColor(rend, 0, 255, 0, 64);
// 		RenderGunSightCross(rend);
// 		SDL_SetRenderDrawColor(rend, 0, 255, 0, 0);
// 		SDL_RenderFillRect(rend, &rect);
// 	}
// }

void RenderPlayer(Render_data* const rend_data, Blade* const blade){
	const SDL_FRect rect = {
		VIEWFINDER_CENTER - PLAYER_SIZE * 0.5F,
		VIEWFINDER_CENTER - PLAYER_SIZE * 0.5F + PLAYER_REND_Y_SHIFT,
		(float)PLAYER_SIZE,
		(float)PLAYER_SIZE
	};
	SDL_FRect rect_blade = {
		VIEWFINDER_CENTER - BLADE_SIZE * 0.5F - (PLAYER_SIZE * 0.25F),
		VIEWFINDER_CENTER - BLADE_SIZE * 0.5F - PLAYER_SIZE + PLAYER_REND_Y_SHIFT,
		BLADE_SIZE,
		BLADE_SIZE
	};
	static const SDL_FPoint blade_rotation_point = {
		BLADE_SIZE * 0.5F,
		BLADE_SIZE * BLADE_HANDLER_POSITION
	};
	rect_blade.x = (VIEWFINDER_CENTER - BLADE_SIZE * 0.5F) + blade->position.x;
	rect_blade.y = (VIEWFINDER_CENTER - BLADE_SIZE * BLADE_HANDLER_POSITION + PLAYER_REND_Y_SHIFT) - blade->position.y;
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_pc), NULL, &rect);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_pc_blade), NULL, &rect_blade, (double)RadToDeg(blade->direction), &blade_rotation_point, SDL_FLIP_NONE);
}

// void RenderGunSight(SDL_Renderer* const rend, const float cursor_point_y, SDL_Texture* const tx){
// 	SDL_FRect rect = {
// 		VIEWFINDER_CENTER - GUN_SIGHT_SIZE * 0.5F,
// 		0.0F,
// 		(float)GUN_SIGHT_SIZE,
// 		(float)GUN_SIGHT_SIZE
// 	};
// 	rect.y = cursor_point_y - GUN_SIGHT_SIZE * 0.5F;
// 	SDL_RenderTexture(rend, tx, NULL, &rect);
// }

void RenderProjectiles(Render_data* const rend_data, Projectiles_array* const prs, Player* const p){
	static SDL_FRect rect = { 
		0.0F,
		0.0F,
		BULLET_SIZE,
		BULLET_SIZE
	};
	for(Projectile* pr = prs->array; pr != (prs->array + prs->num); ++pr){
		SDL_FPoint point;
		if(GetRenderPointFromTrue(rend_data, pr->position.x, pr->position.y, p, &point)){
			rect.x = point.x - BULLET_SIZE * 0.5F;
			rect.y = point.y - BULLET_SIZE * 0.5F;
			SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_projectile), NULL, &rect);
		}
	}
}

void RenderHProjectiles(Render_data* const rend_data, Projectiles_h_array* const prs, Player* const p){
	static SDL_FRect rect = { 
		0.0F,
		0.0F,
		BULLET_SIZE,
		BULLET_SIZE
	};
	for(Projectile_hostile* pr = prs->array; pr != (prs->array + prs->num); ++pr){
		SDL_FPoint point;
		if(GetRenderPointFromTrue(rend_data, pr->position.x, pr->position.y, p, &point)){
			rect.x = point.x - BULLET_SIZE * 0.5F;
			rect.y = point.y - BULLET_SIZE * 0.5F;
			SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_h_projectile), NULL, &rect);
		}
	}
}

void RenderBeings(Render_data* const rend_data, Game_data* const g_d){
	static SDL_FRect rect = {
		0.0F,
		0.0F,
		PLAYER_SIZE,
		PLAYER_SIZE
	};
	SDL_FRect rect_blade = {
		VIEWFINDER_CENTER - BLADE_SIZE * 0.5F - (PLAYER_SIZE * 0.25F),
		VIEWFINDER_CENTER - BLADE_SIZE * 0.5F - PLAYER_SIZE + PLAYER_REND_Y_SHIFT,
		BLADE_SIZE,
		BLADE_SIZE
	};
	static const SDL_FPoint blade_rotation_point = {
		BLADE_SIZE * 0.5F,
		BLADE_SIZE * BLADE_HANDLER_POSITION
	};
    for(Being* b = g_d->beings.array; b != (g_d->beings.array + g_d->beings.num); ++b){
		SDL_FPoint point;
		if(GetRenderPointFromTrue(rend_data, b->position.x, b->position.y, &g_d->pc, &point)){
			rect.x = point.x - PLAYER_SIZE * 0.5F;
			rect.y = point.y - PLAYER_SIZE * 0.5F;

			float being_direction = b->direction - g_d->pc.direction;

			float sine = SineSafe(being_direction);
			float cosine = CosiSafe(being_direction);
			rect_blade.x = point.x + (b->blade.position.x * cosine + b->blade.position.y * sine) - BLADE_SIZE * 0.5F;
			rect_blade.y = point.y + (b->blade.position.x * sine - b->blade.position.y * cosine) - BLADE_SIZE * BLADE_HANDLER_POSITION;
			SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_being), NULL, &rect, (double)RadToDeg(being_direction), NULL, SDL_FLIP_NONE);
			SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_being_blade), NULL, &rect_blade, (double)RadToDeg(b->blade.direction + being_direction), &blade_rotation_point, SDL_FLIP_NONE);
		}
	}

	// SDL_FPoint corners[] = {
	// 	{VIEWFINDER_CENTER + ((WORLD_W / SEGMENTS_X - p->position.x) * world->cos_player_direction + (WORLD_H / SEGMENTS_Y - p->position.y) * world->sin_player_direction), VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - ((WORLD_W / SEGMENTS_X - p->position.x) * world->sin_player_direction - (WORLD_H / SEGMENTS_Y - p->position.y) * world->cos_player_direction)},
	// 	{VIEWFINDER_CENTER + ((WORLD_W - WORLD_W / SEGMENTS_X - p->position.x) * world->cos_player_direction + (WORLD_H / SEGMENTS_Y - p->position.y) * world->sin_player_direction), VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - ((WORLD_W - WORLD_W / SEGMENTS_X - p->position.x) * world->sin_player_direction - (WORLD_H / SEGMENTS_Y - p->position.y) * world->cos_player_direction)},
	// 	{VIEWFINDER_CENTER + ((WORLD_W - WORLD_W / SEGMENTS_X - p->position.x) * world->cos_player_direction + (WORLD_H - WORLD_H / SEGMENTS_Y - p->position.y) * world->sin_player_direction), VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - ((WORLD_W - WORLD_W / SEGMENTS_X - p->position.x) * world->sin_player_direction - (WORLD_H - WORLD_H / SEGMENTS_Y - p->position.y) * world->cos_player_direction)},
	// 	{VIEWFINDER_CENTER + ((WORLD_W / SEGMENTS_X - p->position.x) * world->cos_player_direction + (WORLD_H - WORLD_H / SEGMENTS_Y - p->position.y) * world->sin_player_direction), VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - ((WORLD_W / SEGMENTS_X - p->position.x) * world->sin_player_direction - (WORLD_H - WORLD_H / SEGMENTS_Y - p->position.y) * world->cos_player_direction)},
	// 	{VIEWFINDER_CENTER + ((WORLD_W / SEGMENTS_X - p->position.x) * world->cos_player_direction + (WORLD_H / SEGMENTS_Y - p->position.y) * world->sin_player_direction), VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - ((WORLD_W / SEGMENTS_X - p->position.x) * world->sin_player_direction - (WORLD_H / SEGMENTS_Y - p->position.y) * world->cos_player_direction)}
	// };
	// SDL_SetRenderDrawColor(rend, 255, 255, 255, 0);
	// SDL_RenderLines(rend, corners, 5);
}

void RenderMap(Render_data* const rend_data, Player* const p){
	static const int map_size = 300;
	const SDL_Rect rect = {
		(int)(((WINDOW_W - VIEWFINDER) * 0.5F) + VIEWFINDER) + 10,
		50,
		map_size,
		map_size
	};
	const SDL_FRect destination_rect0a = {
		WINDOW_W - 100.0F,
		WINDOW_H * 0.4F,
		50.0F,
		50.0F
	};
	const SDL_FRect destination_rect0b = {
		WINDOW_W - 90.0F,
		WINDOW_H * 0.4F + 10.0F,
		30.0F,
		30.0F
	};
	const SDL_FRect destination_rect1 = {
		10.0F,
		40.0F,
		50.0F,
		50.0F
	};
	double rotation = RadToDeg(p->direction);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_compass), NULL, &destination_rect1, -rotation, NULL, SDL_FLIP_NONE);//compass
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_nesw), NULL, &destination_rect0a);
	SDL_RenderTextureRotated(rend_data->renderer, *(rend_data->textures + tx_arrow), NULL, &destination_rect0b, rotation, NULL, SDL_FLIP_NONE);
	SDL_SetRenderDrawColor(rend_data->renderer, 255, 255, 255, 0);
	SDL_SetRenderViewport(rend_data->renderer, &rect);
	SDL_RenderRect(rend_data->renderer, NULL);
	SDL_SetRenderDrawColor(rend_data->renderer, 255, 127, 127, 0);
	SDL_RenderPoint(rend_data->renderer, p->position.x * (map_size / WORLD_W), p->position.y * (map_size / WORLD_H));
	SDL_SetRenderViewport(rend_data->renderer, NULL);
}

inline bool GetRenderPointFromTrue(Render_data* const rend_data, const float true_point_x, const float true_point_y, const Player* const p, SDL_FPoint* const rend_point){
	float dx = true_point_x - p->position.x;
	if(SDL_fabsf(dx) > VIEWFINDER) return false;
	float dy = true_point_y - p->position.y;
	if(SDL_fabsf(dy) > VIEWFINDER) return false;
	rend_point->x =	VIEWFINDER_CENTER + (dx * rend_data->cos_player_direction + dy * rend_data->sin_player_direction);
	rend_point->y =	VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - (dx * rend_data->sin_player_direction - dy * rend_data->cos_player_direction);
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
	// SDL_RenderDebugTextFormat(rrer, 10, 190, "seg coord: %.0f %.0f", (*(*(world->segments + 0) + 0)).coordinates.x, (*(*(world->segments + 0) + 0)).coordinates.y);
	// SDL_RenderDebugTextFormat(rrer, 10, 250, "sizeof: %d MB", sizeof(world->segments) / 100000);
	SDL_RenderDebugTextFormat(rend, 10, 260, "player: x: %d y: %d", player_seg->indx.x, player_seg->indx.y);
	//for(unsigned int i = 0U; i < projectiles.num; ++i){
	//    Projectile* pr = *(projectiles.array + i);
	//    s = GetSegment(world, pr->position.x, pr->position.y);
	//    SDL_RenderDebugTextFormat(renderer, 10, 270 + i * 10, "%u     : %.0f %.0f", i, s->coordinates.x, s->coordinates.y);
	//}
	//for(unsigned int i = 0U; i < beings.num; ++i){
	//    Being* b = *(beings.array + i);
	//    Segment* s = b->segment;
	//    SDL_RenderDebugTextFormat(renderer, 10, 200 + i * 10, "%u: %.0f %.0f", i, s->coordinates.x, s->coordinates.y);
	//}

	//SDL_RenderDebugTextFormat(renderer, 10, 200, "min_delay: %llu", MINIMAL_DELAY);
}

void RenderPlayerStatus(Render_data* const rend_data, Player* const p){
	const SDL_FRect rect0a = {
		10.0F,
		WINDOW_H * 0.5F,
		(WINDOW_W - VIEWFINDER) * 0.49F,
		30.0F
	};
	const SDL_FRect rect1a = {
		10.0F,
		WINDOW_H * 0.5F + 40.0F,
		(WINDOW_W - VIEWFINDER) * 0.49F,
		30.0F
	};
	const SDL_FRect rect2a = {
		10.0F,
		WINDOW_H * 0.5F + 80.0F,
		(WINDOW_W - VIEWFINDER) * 0.49F,
		30.0F
	};
	SDL_FRect rect0b = {
		11.0F,
		WINDOW_H * 0.5F + 1.0F,
		0.0F,
		28.0F
	};
	SDL_FRect rect1b = {
		11.0F,
		WINDOW_H * 0.5F + 41.0F,
		0.0F,
		28.0F
	};
	SDL_FRect rect2b = {
		11.0F,
		WINDOW_H * 0.5F + 81.0F,
		0.0F,
		28.0F
	};
	rect0b.w = (float)p->hit_points / (float)p->max_h_p * (rect0a.w - 2.0F);
	rect1b.w = (float)p->magic_points / (float)p->max_magic * (rect1a.w - 2.0F);
	rect2b.w = (float)p->fatigue_points / (float)p->max_fatigue * (rect1a.w - 2.0F);
	SDL_SetRenderDrawColor(rend_data->renderer, 255U, 255U, 255U, 0U);
	SDL_RenderRect(rend_data->renderer, &rect0a);
	SDL_RenderRect(rend_data->renderer, &rect1a);
	SDL_RenderRect(rend_data->renderer, &rect2a);
	SDL_SetRenderDrawColor(rend_data->renderer, 255U, 0U, 0U, 0U);
	SDL_RenderFillRect(rend_data->renderer, &rect0b);
	SDL_SetRenderDrawColor(rend_data->renderer, 0U, 127U, 255U, 0U);
	SDL_RenderFillRect(rend_data->renderer, &rect1b);
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
	SDL_RenderDebugText(rend_data->renderer, WINDOW_W * 0.25F * 0.25F, WINDOW_H * 0.5F * 0.25F, "Press SPACE");
	SDL_SetRenderScale(rend_data->renderer, 1.0F, 1.0F);
	SDL_RenderPresent(rend_data->renderer);
}

extern inline void RenderGame(Render_data* const rend_data, Game_data* const g_d){
	SetSineCosine(rend_data, &g_d->pc);
	// SDL_SetRenderTarget(renderer, *textures);//Gun Sight
	// RenderGunSightElements(renderer, cursor_distance, RANGE);
	// SDL_SetRenderTarget(renderer, NULL);
	SDL_SetRenderDrawColor(rend_data->renderer, 0, 0, 0, 0);
	SDL_RenderClear(rend_data->renderer);
	SDL_SetRenderViewport(rend_data->renderer, &rend_data->viewfinder_rect);
	SDL_SetRenderDrawColor(rend_data->renderer, 50, 50, 50, 0);
	SDL_RenderFillRect(rend_data->renderer, NULL);
	if(!(g_d->pc.control_flags & tmp0)){
		RenderBeings(rend_data, g_d);
	}
	RenderProjectiles(rend_data, &g_d->projectiles, &g_d->pc);
	RenderHProjectiles(rend_data, &g_d->h_projectiles, &g_d->pc);
	SDL_RenderTexture(rend_data->renderer, *(rend_data->textures + tx_viewfinder), NULL, NULL);//viewfinder
	RenderPlayer(rend_data, &g_d->pc.blade);
	// RenderGunSight(renderer, cursor_y, *textures); SDL_SetRenderScale(renderer, 1.0F, 1.0F);
	SDL_SetRenderViewport(rend_data->renderer, NULL);
	RenderMap(rend_data, &g_d->pc);
	RenderPlayerStatus(rend_data, &g_d->pc);
}

extern inline void SetSineCosine(Render_data* const rend_data, Player* const p){
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