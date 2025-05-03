#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <render.h>

int graphicsInitiation(struct Graphics_initiation_data* data) {
	SDL_Surface* surface = NULL;
	char* bmp_path = NULL;
	const char* texture_files[TEXTURE_FILES_NUM] = {
		"img0.bmp",
		"img1.bmp",
		"img2.bmp",
		"being.bmp"
	};

	SDL_SetAppMetadata("KacApp", "1.0", NULL);

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	if (!SDL_CreateWindowAndRenderer("KacWindow", WINDOW_W, WINDOW_H, SDL_WINDOW_FULLSCREEN, data->window, data->renderer)) {
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

	**data->textures = SDL_CreateTexture(*data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)(WORLD_W + BOUNDS_L * 2), (int)(WORLD_H + BOUNDS_U * 2));
	**(data->textures + 1) = SDL_CreateTexture(*data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, GUN_SIGHT_SIZE, GUN_SIGHT_SIZE);
	**(data->textures + 2) = SDL_CreateTexture(*data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)(WORLD_W + BOUNDS_L * 2), (int)(WORLD_H + BOUNDS_L * 2));

	if (**data->textures == NULL || **(data->textures + 1) == NULL || **(data->textures + 2) == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s", SDL_GetError()); return 1;
	}

	SDL_SetWindowRelativeMouseMode(*data->window, true);
	//SDL_HideCursor();
	return 0;
}

void renderGunSightCross(SDL_Renderer* rend) {
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

void renderGunSightElements(SDL_Renderer* rend, const float distance, const float range) {
	SDL_SetRenderDrawColor(rend, 64, 64, 64, 0);
	SDL_RenderClear(rend);
	if (distance > range) {
		SDL_SetRenderDrawColor(rend, 255, 0, 0, 64);
		renderGunSightCross(rend);
	}
	else {
		SDL_FRect rect;
		rect.w = GUN_SIGHT_SIZE * distance / range * 0.9F;
		rect.h = rect.w;
		rect.x = GUN_SIGHT_SIZE * 0.5F - rect.w * 0.5F;
		rect.y = rect.x;
		SDL_SetRenderDrawColor(rend, 0, 255, 0, 64);
		renderGunSightCross(rend);
		SDL_SetRenderDrawColor(rend, 0, 255, 0, 0);
		SDL_RenderFillRect(rend, &rect);
	}
}

void renderPlayer(SDL_Renderer* rend, SDL_Texture* tx) {
	static const SDL_FRect rect = {
		WINDOW_CENTER_X - PLAYER_SIZE * 0.5F,
		WINDOW_CENTER_Y - PLAYER_SIZE * 0.5F + PLAYER_REND_Y_SHIFT,
		(float)PLAYER_SIZE,
		(float)PLAYER_SIZE 
	};
	SDL_RenderTexture(rend, tx, NULL, &rect);
}

void renderGunSight(SDL_Renderer* rend, const float cursor_point_y, SDL_Texture* tx) {
	static SDL_FRect rect = {
		WINDOW_CENTER_X - GUN_SIGHT_SIZE * 0.5F,
		0.0F,
		(float)GUN_SIGHT_SIZE,
		(float)GUN_SIGHT_SIZE
	};
	rect.y = cursor_point_y - GUN_SIGHT_SIZE * 0.5F;
	SDL_RenderTexture(rend, tx, NULL, &rect);
}

void drawStaticWorld(SDL_Renderer* rend, SDL_Texture* tx) {
	SDL_SetRenderTarget(rend, tx);
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	SDL_RenderClear(rend);
	const SDL_Rect viewport = {
		(int)BOUNDS_L,
		(int)BOUNDS_U,
		(int)WORLD_W,
		(int)WORLD_H
	};
	SDL_SetRenderViewport(rend, &viewport);
	SDL_SetRenderDrawColor(rend, 64, 64, 64, 255);
	SDL_RenderFillRect(rend, NULL);
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	float p0, p1, p2;
	p1 = 0.0F;
	if (SEGMENTS_X == SEGMENTS_Y) {
		for (int i = 0; i < SEGMENTS_X + 1; ++i) {
			p0 = (float)i * WORLD_W / SEGMENTS_X;
			p2 = WORLD_H;
			SDL_RenderLine(rend, p0, p1, p0, p2);
			p0 = (float)i * WORLD_H / SEGMENTS_X;
			p2 = WORLD_W;
			SDL_RenderLine(rend, p1, p0, p2, p0);
		}
	}
	SDL_SetRenderTarget(rend, NULL);
}

void renderProjectiles(SDL_Renderer* rend, Projectiles_array* prs, SDL_Texture* tx) {
	static SDL_FRect rect = { 
		0.0F,
		0.0F,
		BULLET_SIZE,
		BULLET_SIZE
	};
	for (unsigned int i = 0U; i < prs->num; ++i) {
		Projectile* pr = *(prs->array + i);
		rect.x = pr->position.x - BULLET_SIZE * 0.5F;
		rect.y = pr->position.y - BULLET_SIZE * 0.5F;
		SDL_RenderTexture(rend, tx, NULL, &rect);
	}
}

void renderBeings(SDL_Renderer* rend, Beings_array* bs, SDL_Texture* tx, SDL_FRect visible_rect) {
	static SDL_FRect rect = {
		0.0F,
		0.0F,
		PLAYER_SIZE,
		PLAYER_SIZE
	};
	for (unsigned int i = 0; i < bs->num; ++i) {
		Being* b = *(bs->array + i);
		if (SDL_PointInRectFloat(&b->position, &visible_rect)) {
			rect.x = b->position.x - PLAYER_SIZE * 0.5F;
			rect.y = b->position.y - PLAYER_SIZE * 0.5F;
			SDL_RenderTexture(rend, tx, NULL, &rect);
		}
	}
}
