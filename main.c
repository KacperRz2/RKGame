#include <header.h>

int main(int argc, char* argv[]) {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* textures[TEXTURES_NUM];
	SDL_Event event;
	SDL_Event* const event_ptr = &event;
	float cursor_y;
	float* const cursor_y_ptr = &cursor_y;
	const SDL_FPoint zero_point = {0.0F, 0.0F};
	// Projectile* projectiles_ptr_array[MAX_PROJECTILES_NUM];
	// for (unsigned int i = 0U; i < MAX_PROJECTILES_NUM; ++i) {
	// 	*(projectiles_ptr_array + i) = (projectiles_array + i);
	// }
	// Projectile projectiles_array[MAX_PROJECTILES_NUM];
	// Being beings_array[MAX_BEINGS_NUM];
	Projectiles_array projectiles;
	Projectiles_array* const projectiles_ptr = &projectiles;
	projectiles.array = (Projectile*)SDL_malloc(sizeof(Projectile) * MAX_PROJECTILES_NUM);//projectiles_array;
	projectiles.num = 0U;
	// sizeof(beings_array) / 1000;
	Beings_array beings;
	Beings_array* const beings_ptr = &beings;
	beings.array = (Being*)SDL_malloc(sizeof(Being) * MAX_BEINGS_NUM);//beings_array;
	beings.num = 0U;
	// projectiles.array = (Projectile**)SDL_malloc(sizeof(Projectile*) * MAX_PROJECTILES_NUM);
	// for (unsigned int i = 0U; i < MAX_PROJECTILES_NUM; ++i) {
	// 	*(projectiles.array + i) = CreateProjectile(&zero_point, 0.0F, 3.0F, 10, 3U);
	// }
	// projectiles.num = 0U;
	// Beings_array beings;
	// beings.array = (Being**)SDL_malloc(sizeof(Being*) * MAX_BEINGS_NUM);
	// for (unsigned int i = 0U; i < MAX_BEINGS_NUM; ++i) {
	// 	*(beings.array + i) = CreateBeing(zero_point.x, zero_point.y);
	// }
	// beings.num = 0U;
	float rotation = 0.0F;
	float cursor_distance = 0.0F;
	unsigned int ticks_to_update_beings = 1U;
	bool quit = false;

	for(unsigned int i = 0U; i < ANGLE_PARTS; ++i){
		*(si + i) = SDL_sinf(MIN_ANGLE * i);
		*(co + i) = SDL_cosf(MIN_ANGLE * i);
	}

	struct Graphics_initiation_data* graphics_initiation_data = (struct Graphics_initiation_data*)SDL_malloc(sizeof(struct Graphics_initiation_data));
	graphics_initiation_data->window = &window;
	graphics_initiation_data->renderer = &renderer;
	for (int i = 0; i < TEXTURES_NUM; ++i) {
		*(graphics_initiation_data->textures + i) = textures + i;
	}
	if (GraphicsInitiation(graphics_initiation_data)) return 1;
	SDL_free(graphics_initiation_data);

	Uint64 time = SDL_GetTicksNS();
	Uint64 now = 0ULL;
	Uint64 frame_time = time;
	Uint64 TPS_time = SDL_GetTicks();
	int tps = 0;
	int tps_count = 0;
	Uint64 timer;
	
	const SDL_FRect destination_rect1 = {
		10.0F,
		40.0F,
		50.0F,
		50.0F
	};
	const SDL_Rect viewfinder = {
		(int)((WINDOW_W - VIEWFINDER) * 0.5F),
		(int)((WINDOW_H - VIEWFINDER) * 0.5F),
		(int)(VIEWFINDER),
		(int)(VIEWFINDER)
	};
	const SDL_Rect* const viewfinder_ptr = &viewfinder;

	CreateWorld(WORLD_W, WORLD_H);
	Player p;
	// void* hits[MAX_HITS];
	Player* const player = &p;
	Blade* const player_blade = &player->blade;
	CreatePlayer(player, WORLD_W / SEGMENTS_X * 2.0F, WORLD_H / SEGMENTS_Y * 2.0F);;
	// player->blade.hit_targets = hits;
	
	// Player* player = CreatePlayer(WORLD_W / SEGMENTS_X * 2.0F, WORLD_H / SEGMENTS_Y * 2.0F);
	if (player == NULL || world == NULL) return 1;
	AddBeingToArray(beings_ptr, WORLD_W / SEGMENTS_X * 3.0F, WORLD_H / SEGMENTS_Y * 3.0F);

	// while (beings.num < MAX_BEINGS_NUM) {
	// 	float x = (float)(SDL_rand((Sint32)(WORLD_W - WORLD_W / SEGMENTS_X * 4.0F))) + WORLD_W / SEGMENTS_X * 2.0F;
	// 	float y = (float)(SDL_rand((Sint32)(WORLD_H - WORLD_H / SEGMENTS_Y * 4.0F))) + WORLD_H / SEGMENTS_Y * 2.0F;
	// 	Segment* s = GetSegment(x, y);
	// 	if (s->beings.num < MAX_SEGM_BEINGS && s->available) {
	// 		AddBeingToArray(&beings, CreateBeing(x, y));
	// 	}
	// }

	while (!quit) {
		timer = SDL_GetTicksNS();
		quit = EventsService(event_ptr, player);

		// if (beings.num < MAX_BEINGS_NUM / 2048) {
		if (beings.num < MAX_BEINGS_NUM) {
			float x = (float)(SDL_rand((Sint32)(WORLD_W - WORLD_W / SEGMENTS_X * 4.0F))) + WORLD_W / SEGMENTS_X * 2.0F;
			float y = (float)(SDL_rand((Sint32)(WORLD_H - WORLD_H / SEGMENTS_Y * 4.0F))) + WORLD_H / SEGMENTS_Y * 2.0F;
			if (SDL_fabsf(player->position.x - x) > 2000.0F) {
				if (SDL_fabsf(player->position.y - y) > 2000.0F) {
					Segment* s = GetSegment(x, y);
					if(s->available){
						if (s->beings->num < MAX_SEGM_BEINGS && s->available) {
							AddBeingToArray(beings_ptr, x, y);
						}
					}
				}
			}
		}

		SDL_GetMouseState(NULL, cursor_y_ptr);
		UpdatePlayer(player, projectiles_ptr);
		cursor_distance = WINDOW_CENTER_Y + PLAYER_REND_Y_SHIFT - cursor_y;

		Segment* player_seg = GetSegment(player->position.x, player->position.y);
		UpdateProjectiles(projectiles_ptr, player_seg);
		if (ticks_to_update_beings == 0U) {
			if (!(player->control_flags & 1 << 6)) {
				UpdateBeings(beings_ptr, player, player_seg);
			}
			ticks_to_update_beings = 1U;
		}
		else {
			--ticks_to_update_beings;
		}

		if (SDL_GetTicksNS() > frame_time) {

			frame_time += FRAME_TIME;

			SetSineCosine(player);

			rotation = RadToDeg(player->direction);

			SDL_SetRenderTarget(renderer, *textures);//Gun Sight

			RenderGunSightElements(renderer, cursor_distance, player->range);

			SDL_SetRenderTarget(renderer, NULL);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);
			SDL_SetRenderViewport(renderer, viewfinder_ptr);
			SDL_SetRenderDrawColor(renderer, 50, 50, 50, 0);
			SDL_RenderFillRect(renderer, NULL);

			RenderProjectiles(renderer, projectiles_ptr, *(textures + 3), player);

			if (!(player->control_flags & 1 << 6)) {
				RenderBeings(renderer, beings_ptr, *(textures + 4), player);
			}
			
			SDL_RenderTexture(renderer, *(textures + 5), NULL, NULL);//viewfinder
			// RenderPlayer(renderer, *(textures + 1));
			RenderPlayer(renderer, textures, player_blade);
			RenderGunSight(renderer, cursor_y, *textures);

			SDL_SetRenderViewport(renderer, NULL);

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderDebugTextFormat(renderer, 10, 10, "Fatigue: %d", player->fatigue_points);
			SDL_RenderDebugTextFormat(renderer, 10, 20, "Position: %.2f %.2f", player->position.x, player->position.y);
			SDL_RenderDebugTextFormat(renderer, 10, 30, "Direction: %.2f", player->direction);

			SDL_RenderDebugTextFormat(renderer, 10, 140, "Ticks per sec.: %d", tps);
			SDL_RenderDebugTextFormat(renderer, 10, 150, "max FPS: ~%d", (int)(1000000000ULL / FRAME_TIME));

			SDL_RenderDebugTextFormat(renderer, 10, 160, "beings: %d", beings.num);
			SDL_RenderDebugTextFormat(renderer, 10, 170, "projectiles: %d", projectiles.num);
			// SDL_RenderDebugTextFormat(renderer, 10, 180, "beings in seg0x0: %d", (*(*(world->segments + 0) + 0)).beings.num);
			// SDL_RenderDebugTextFormat(renderer, 10, 190, "seg coord: %.0f %.0f", (*(*(world->segments + 0) + 0)).coordinates.x, (*(*(world->segments + 0) + 0)).coordinates.y);
			// SDL_RenderDebugTextFormat(renderer, 10, 250, "sizeof: %d MB", sizeof(world->segments) / 100000);
			SDL_RenderDebugTextFormat(renderer, 10, 260, "player: x: %d y: %d", player_seg->indx.x, player_seg->indx.y);
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

			SDL_RenderTextureRotated(renderer, *(textures + 2), NULL, &destination_rect1, -rotation, NULL, SDL_FLIP_NONE);//compass

			RenderMap(renderer, player);

			SDL_RenderPresent(renderer);
		}

		if (SDL_GetTicks() - TPS_time >= 1000ULL) {
			tps = tps_count;
			tps_count = 0;
			TPS_time = SDL_GetTicks();
		}
		++tps_count;

		time += TICK_TIME;
		now = SDL_GetTicksNS();
		if (now < time) {
			SDL_DelayNS(time - now);
		}
		else if (now - timer < TICK_TIME) {
			SDL_DelayNS((TICK_TIME - (now - timer)) >> 1);
		}
	}
	// DestroyPlayer(player);
	DestroyProjectiles(projectiles_ptr);
	DestroyBeings(beings_ptr);
	DestroyWorld();
	// SDL_free(beings.array);
	// SDL_free(projectiles.array);
	for (int i = 0; i < TEXTURES_NUM; ++i) {
		SDL_DestroyTexture(*(textures + i));
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
