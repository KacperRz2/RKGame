#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <render.h>
#include <function.h>

int GraphicsInitiation(struct Graphics_initiation_data* data) {
	SDL_Surface* surface = NULL;
	char* bmp_path = NULL;
	const char* texture_files[TEXTURE_FILES_NUM] = {
		"img0.bmp",
		"img1.bmp",
		"img2.bmp",
		"being.bmp",
		"img3.bmp",
		"img4.bmp"
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

void RenderGunSightCross(SDL_Renderer* rend) {
	const static SDL_FRect rect0 = { 
		GUN_SIGHT_SIZE * 0.5F - 1.0F,
		1.0F,
		3.0F,
		(float)GUN_SIGHT_SIZE - 2.0F
	};
	const static SDL_FRect rect1 = {
		1.0F,
		GUN_SIGHT_SIZE * 0.5F - 1.0F,
		(float)GUN_SIGHT_SIZE - 2.0F,
		3.0F
	};
	SDL_RenderFillRect(rend, &rect0);
	SDL_RenderFillRect(rend, &rect1);
}

void RenderGunSightElements(SDL_Renderer* rend, const float distance, const float range) {
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

void RenderPlayer(SDL_Renderer* rend, SDL_Texture** tx, Blade* blade) {
	static const SDL_FRect rect = {
		VIEWFINDER_CENTER - PLAYER_SIZE * 0.5F,
		VIEWFINDER_CENTER - PLAYER_SIZE * 0.5F + PLAYER_REND_Y_SHIFT,
		(float)PLAYER_SIZE,
		(float)PLAYER_SIZE
	};
	static SDL_FRect rect_blade = {
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
	SDL_RenderTexture(rend, *(tx + 1), NULL, &rect);
	SDL_RenderTextureRotated(rend, *(tx + 6), NULL, &rect_blade, (double)RadToDeg(blade->direction), &blade_rotation_point, SDL_FLIP_NONE);
}

void RenderGunSight(SDL_Renderer* rend, const float cursor_point_y, SDL_Texture* tx) {
	static SDL_FRect rect = {
		VIEWFINDER_CENTER - GUN_SIGHT_SIZE * 0.5F,
		0.0F,
		(float)GUN_SIGHT_SIZE,
		(float)GUN_SIGHT_SIZE
	};
	rect.y = cursor_point_y - GUN_SIGHT_SIZE * 0.5F;
	SDL_RenderTexture(rend, tx, NULL, &rect);
}

void RenderProjectiles(SDL_Renderer* rend, Projectiles_array* prs, SDL_Texture* tx, Player* p) {
	static const SDL_FRect visible_rect = {
		0.0F,
		0.0F,
		VIEWFINDER,
		VIEWFINDER
	};
	static SDL_FRect rect = { 
		0.0F,
		0.0F,
		BULLET_SIZE,
		BULLET_SIZE
	};
	for (unsigned int i = 0U; i < prs->num; ++i) {
		Projectile* pr = *(prs->array + i);

		float dx = pr->position.x - p->position.x;
		if(SDL_fabsf(dx) < VIEWFINDER){
			float dy = pr->position.y - p->position.y;
			if(SDL_fabsf(dy) < VIEWFINDER){

				rect.x = VIEWFINDER_CENTER + (dx * world->cos_player_direction + dy * world->sin_player_direction) - BULLET_SIZE * 0.5F;
				rect.y = VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - (dx * world->sin_player_direction - dy * world->cos_player_direction) - BULLET_SIZE * 0.5F;
				
				if (SDL_HasRectIntersectionFloat(&rect, &visible_rect)) {
					SDL_RenderTexture(rend, tx, NULL, &rect);
				}
			}
		}
	}
}

void RenderBeings(SDL_Renderer* rend, Beings_array* bs, SDL_Texture* tx, Player* p) {
	static const SDL_FRect visible_rect = {
		0.0F,
		0.0F,
		VIEWFINDER,
		VIEWFINDER
	};
	static SDL_FRect rect = {
		0.0F,
		0.0F,
		PLAYER_SIZE,
		PLAYER_SIZE
	};
	for (unsigned int i = 0; i < bs->num; ++i) {
		Being* b = *(bs->array + i);		
		
		float dx = b->position.x - p->position.x;
		if(SDL_fabsf(dx) < VIEWFINDER){
			float dy = b->position.y - p->position.y;
			if(SDL_fabsf(dy) < VIEWFINDER){

				rect.x = VIEWFINDER_CENTER + (dx * world->cos_player_direction + dy * world->sin_player_direction) - PLAYER_SIZE * 0.5F;
				rect.y = VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - (dx * world->sin_player_direction - dy * world->cos_player_direction) - PLAYER_SIZE * 0.5F;
				
				if (SDL_HasRectIntersectionFloat(&rect, &visible_rect)) {
					SDL_RenderTexture(rend, tx, NULL, &rect);
				}
			}
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

void RenderMap(SDL_Renderer* rend, Player* p) {
	static const int map_size = 300;
	static const SDL_Rect rect = {
		(int)(((WINDOW_W - VIEWFINDER) * 0.5F) + VIEWFINDER) + 10,
		50,
		map_size,
		map_size
	};
	SDL_SetRenderDrawColor(rend, 255, 255, 255, 0);
	SDL_SetRenderViewport(rend, &rect);
	SDL_RenderRect(rend, NULL);
	SDL_SetRenderDrawColor(rend, 255, 127, 127, 0);
	SDL_RenderPoint(rend, p->position.x * (map_size / WORLD_W), p->position.y * (map_size / WORLD_H));
	SDL_SetRenderViewport(rend, NULL);
}
