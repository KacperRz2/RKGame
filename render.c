#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <render.h>
#include <function.h>
#include <enum.h>

int GraphicsInitiation(struct Graphics_initiation_data* const data) {
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

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	if (!SDL_CreateWindowAndRenderer("KacWindow", WINDOW_W, WINDOW_H, SDL_WINDOW_BORDERLESS, data->window, data->renderer)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		return 3;
	}

	for (int i = 0; i < TEXTURE_FILES_NUM; ++i) {
		SDL_asprintf(&bmp_path, "%s%s", SDL_GetBasePath(), *(texture_files + i));
		surface = SDL_LoadBMP(bmp_path);
		if (!surface) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create surface from image: %s", SDL_GetError());
			return 3;
		}
		**(data->textures + i + TEXTURE_TARGET_NUM) = SDL_CreateTextureFromSurface(*data->renderer, surface);
		if (!**(data->textures + i + TEXTURE_TARGET_NUM)) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
			return 3;
		}
	}

	SDL_free(bmp_path);
	SDL_DestroySurface(surface);

	**data->textures = SDL_CreateTexture(*data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, GUN_SIGHT_SIZE, GUN_SIGHT_SIZE);

	if (**data->textures == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError()); return 1;
	}
	
	SDL_SetTextureScaleMode(**(data->textures + 5), SDL_SCALEMODE_NEAREST);

	SDL_SetWindowRelativeMouseMode(*data->window, true);
	//SDL_HideCursor();
	return 0;
}

void SetRenderData(const float window_w, const float window_h){
	rend_data.window_w = window_w;
	rend_data.window_h = window_h;
	rend_data.viewfinder = (WINDOW_H * 0.984375F);
	rend_data.viewfinder_rect.x = (int)((WINDOW_W - VIEWFINDER) * 0.5F);
	rend_data.viewfinder_rect.y = (int)((WINDOW_H - VIEWFINDER) * 0.5F);
	rend_data.viewfinder_rect.w = (int)(VIEWFINDER);
	rend_data.viewfinder_rect.h = (int)(VIEWFINDER);
	rend_data.visible_rect.x = 0.0F;
	rend_data.visible_rect.y = 0.0F;
	rend_data.visible_rect.w = VIEWFINDER;
	rend_data.visible_rect.h = VIEWFINDER;
}

void RenderGunSightCross(SDL_Renderer* const rend) {
	static const SDL_FRect rect0 = { 
		GUN_SIGHT_SIZE * 0.5F - 1.0F,
		1.0F,
		3.0F,
		(float)GUN_SIGHT_SIZE - 2.0F
	};
	static const SDL_FRect rect1 = {
		1.0F,
		GUN_SIGHT_SIZE * 0.5F - 1.0F,
		(float)GUN_SIGHT_SIZE - 2.0F,
		3.0F
	};
	SDL_RenderFillRect(rend, &rect0);
	SDL_RenderFillRect(rend, &rect1);
}

void RenderGunSightElements(SDL_Renderer* const rend, const float distance, const float range) {
	SDL_SetRenderDrawColor(rend, 64, 64, 64, 0);
	SDL_RenderClear(rend);
	if (distance > range) {
		SDL_SetRenderDrawColor(rend, 255, 0, 0, 64);
		RenderGunSightCross(rend);
	}
	else {
		SDL_FRect rect;
		rect.w = GUN_SIGHT_SIZE * distance / range * 0.9F;
		rect.h = rect.w;
		rect.x = GUN_SIGHT_SIZE * 0.5F - rect.w * 0.5F;
		rect.y = rect.x;
		SDL_SetRenderDrawColor(rend, 0, 255, 0, 64);
		RenderGunSightCross(rend);
		SDL_SetRenderDrawColor(rend, 0, 255, 0, 0);
		SDL_RenderFillRect(rend, &rect);
	}
}

void RenderPlayer(SDL_Renderer* const rend, SDL_Texture** const tx, Blade* const blade) {
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
		BLADE_SIZE * 0.85F
	};
	rect_blade.x = (VIEWFINDER_CENTER - BLADE_SIZE * 0.5F) + blade->position.x;
	rect_blade.y = (VIEWFINDER_CENTER - BLADE_SIZE * 0.85F + PLAYER_REND_Y_SHIFT) - blade->position.y;
	SDL_RenderTexture(rend, *(tx + tx_pc), NULL, &rect);
	SDL_RenderTextureRotated(rend, *(tx + tx_pc_blade), NULL, &rect_blade, (double)RadToDeg(blade->direction), &blade_rotation_point, SDL_FLIP_NONE);
}

void RenderGunSight(SDL_Renderer* const rend, const float cursor_point_y, SDL_Texture* const tx) {
	SDL_FRect rect = {
		VIEWFINDER_CENTER - GUN_SIGHT_SIZE * 0.5F,
		0.0F,
		(float)GUN_SIGHT_SIZE,
		(float)GUN_SIGHT_SIZE
	};
	rect.y = cursor_point_y - GUN_SIGHT_SIZE * 0.5F;
	SDL_RenderTexture(rend, tx, NULL, &rect);
}

void RenderProjectiles(SDL_Renderer* const rend, Projectiles_array* const prs, SDL_Texture* const tx, Player* const p) {
	static SDL_FRect rect = { 
		0.0F,
		0.0F,
		BULLET_SIZE,
		BULLET_SIZE
	};
	for (Projectile* pr = prs->array; pr != (prs->array + prs->num); ++pr) {
		SDL_FPoint point;
		if (GetRenderPointFromTrue(pr->position.x, pr->position.y, p, &point)) {
			rect.x = point.x - BULLET_SIZE * 0.5F;
			rect.y = point.y - BULLET_SIZE * 0.5F;
			SDL_RenderTexture(rend, tx, NULL, &rect);
		}
	}
}

void RenderHProjectiles(SDL_Renderer* const rend, Projectiles_h_array* const prs, SDL_Texture* const tx, Player* const p) {
	static SDL_FRect rect = { 
		0.0F,
		0.0F,
		BULLET_SIZE,
		BULLET_SIZE
	};
	for (Projectile_hostile* pr = prs->array; pr != (prs->array + prs->num); ++pr) {
		SDL_FPoint point;
		if (GetRenderPointFromTrue(pr->position.x, pr->position.y, p, &point)) {
			rect.x = point.x - BULLET_SIZE * 0.5F;
			rect.y = point.y - BULLET_SIZE * 0.5F;
			SDL_RenderTexture(rend, tx, NULL, &rect);
		}
	}
}

void RenderBeings(SDL_Renderer* const rend, Beings_array* const bs, SDL_Texture** const tx, Player* const p) {
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
		BLADE_SIZE * 0.85F
	};
    for (Being* b = bs->array; b != (bs->array + bs->num); ++b) {
		SDL_FPoint point;
		if(GetRenderPointFromTrue(b->position.x, b->position.y, p, &point)){
			rect.x = point.x - PLAYER_SIZE * 0.5F;
			rect.y = point.y - PLAYER_SIZE * 0.5F;

			float being_direction = b->direction - p->direction;

			float sine = SineSafe(being_direction);
			float cosine = CosiSafe(being_direction);
			rect_blade.x = point.x + (b->blade.position.x * cosine + b->blade.position.y * sine) - BLADE_SIZE * 0.5F;
			rect_blade.y = point.y + (b->blade.position.x * sine - b->blade.position.y * cosine) - BLADE_SIZE * 0.85F;
			SDL_RenderTextureRotated(rend, *(tx + tx_being), NULL, &rect, (double)RadToDeg(being_direction), NULL, SDL_FLIP_NONE);
			SDL_RenderTextureRotated(rend, *(tx + tx_being_blade), NULL, &rect_blade, (double)RadToDeg(b->blade.direction + being_direction), &blade_rotation_point, SDL_FLIP_NONE);
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

void RenderMap(SDL_Renderer* const rend, SDL_Texture** const tx, Player* const p) {
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
	SDL_RenderTextureRotated(rend, *(tx + tx_compass), NULL, &destination_rect1, -rotation, NULL, SDL_FLIP_NONE);//compass
	SDL_RenderTexture(rend, *(tx + tx_nesw), NULL, &destination_rect0a);
	SDL_RenderTextureRotated(rend, *(tx + tx_arrow), NULL, &destination_rect0b, rotation, NULL, SDL_FLIP_NONE);
	SDL_SetRenderDrawColor(rend, 255, 255, 255, 0);
	SDL_SetRenderViewport(rend, &rect);
	SDL_RenderRect(rend, NULL);
	SDL_SetRenderDrawColor(rend, 255, 127, 127, 0);
	SDL_RenderPoint(rend, p->position.x * (map_size / WORLD_W), p->position.y * (map_size / WORLD_H));
	SDL_SetRenderViewport(rend, NULL);
}

inline bool GetRenderPointFromTrue(const float true_point_x, const float true_point_y, const Player* const p, SDL_FPoint* const rend_point){
	// const SDL_FRect visible_rect = {
	// 	0.0F,
	// 	0.0F,
	// 	VIEWFINDER,
	// 	VIEWFINDER
	// };
	float dx = true_point_x - p->position.x;
	if(SDL_fabsf(dx) > VIEWFINDER) return false;
	float dy = true_point_y - p->position.y;
	if(SDL_fabsf(dy) > VIEWFINDER) return false;
	rend_point->x =	VIEWFINDER_CENTER + (dx * world.cos_player_direction + dy * world.sin_player_direction);
	rend_point->y =	VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - (dx * world.sin_player_direction - dy * world.cos_player_direction);
	if (SDL_PointInRectFloat(rend_point, &rend_data.visible_rect)) return true;
	return false;
}

void RenderTextInfo(SDL_Renderer* const rend, Player* const p, const Uint64 tps, Beings_array* const beings, Projectiles_array* const projectiles, Projectiles_h_array* const h_projectiles, Segment* const player_seg){
	SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
	SDL_RenderDebugTextFormat(rend, 10, 10, "Fatigue: %d", p->fatigue_points);
	SDL_RenderDebugTextFormat(rend, 10, 20, "Position: %.2f %.2f", p->position.x, p->position.y);
	SDL_RenderDebugTextFormat(rend, 10, 30, "Direction: %.2f", p->direction);

	SDL_RenderDebugTextFormat(rend, 10, 140, "Ticks per sec.: %d", tps);
	SDL_RenderDebugTextFormat(rend, 10, 150, "max FPS: ~%u", (1000000000ULL / FRAME_TIME));

	SDL_RenderDebugTextFormat(rend, 10, 160, "beings: %d", beings->num);
	SDL_RenderDebugTextFormat(rend, 10, 170, "projectiles: %d", projectiles->num);
	SDL_RenderDebugTextFormat(rend, 10, 180, "projectiles: %d", h_projectiles->num);
	SDL_RenderDebugTextFormat(rend, 10, 190, "hp: %d", p->hit_points);
	// SDL_RenderDebugTextFormat(rrer, 10, 190, "seg coord: %.0f %.0f", (*(*(world->segments + 0) + 0)).coordinates.x, (*(*(world->segments + 0) + 0)).coordinates.y);
	// SDL_RenderDebugTextFormat(rrer, 10, 250, "sizeof: %d MB", sizeof(world->segments) / 100000);
	SDL_RenderDebugTextFormat(rend, 10, 260, "player: x: %d y: %d", player_seg->indx.x, player_seg->indx.y);
	//for (unsigned int i = 0U; i < projectiles.num; ++i) {
	//    Projectile* pr = *(projectiles.array + i);
	//    s = GetSegment(world, pr->position.x, pr->position.y);
	//    SDL_RenderDebugTextFormat(renderer, 10, 270 + i * 10, "%u     : %.0f %.0f", i, s->coordinates.x, s->coordinates.y);
	//}
	//for (unsigned int i = 0U; i < beings.num; ++i) {
	//    Being* b = *(beings.array + i);
	//    Segment* s = b->segment;
	//    SDL_RenderDebugTextFormat(renderer, 10, 200 + i * 10, "%u: %.0f %.0f", i, s->coordinates.x, s->coordinates.y);
	//}

	//SDL_RenderDebugTextFormat(renderer, 10, 200, "min_delay: %llu", MINIMAL_DELAY);
}

void RenderPlayerStatus(SDL_Renderer* const rend, Player* const p){
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
	SDL_SetRenderDrawColor(rend, 255U, 255U, 255U, 0U);
	SDL_RenderRect(rend, &rect0a);
	SDL_RenderRect(rend, &rect1a);
	SDL_RenderRect(rend, &rect2a);
	SDL_SetRenderDrawColor(rend, 255U, 0U, 0U, 0U);
	SDL_RenderFillRect(rend, &rect0b);
	SDL_SetRenderDrawColor(rend, 0U, 127U, 255U, 0U);
	SDL_RenderFillRect(rend, &rect1b);
	SDL_SetRenderDrawColor(rend, 0U, 255U, 0U, 0U);
	SDL_RenderFillRect(rend, &rect2b);
}