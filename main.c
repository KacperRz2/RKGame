#include <header.h>

int main(int argc, char* argv[]) {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* textures[TEXTURES_NUM];
	SDL_Event event;
	float cursor_y;
	// const SDL_FPoint zero_point = {0.0F, 0.0F};
	Projectiles_array projectiles;
	projectiles.array = (Projectile*)SDL_malloc(sizeof(Projectile) * MAX_PROJECTILES_NUM);
	projectiles.num = 0U;
	Projectiles_h_array h_projectiles;
	h_projectiles.array = (Projectile_hostile*)SDL_malloc(sizeof(Projectile_hostile) * MAX_PROJECTILES_NUM);
	h_projectiles.num = 0U;
	// sizeof(beings_array) / 1000;
	Beings_array beings;
	beings.array = (Being*)SDL_malloc(sizeof(Being) * MAX_BEINGS_NUM);
	beings.num = 0U;
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
	const SDL_Rect viewfinder = {
		(int)((WINDOW_W - VIEWFINDER) * 0.5F),
		(int)((WINDOW_H - VIEWFINDER) * 0.5F),
		(int)(VIEWFINDER),
		(int)(VIEWFINDER)
	};

	CreateWorld(WORLD_W, WORLD_H);
	Player pc;
	Player* const pc_ptr = &pc;
	Blade* const player_blade = &pc.blade;
	CreatePlayer(pc_ptr, WORLD_W / SEGMENTS_X * 2.0F, WORLD_H / SEGMENTS_Y * 2.0F);
	if (projectiles.array == NULL || beings.array == NULL) return 1;
	AddBeingToArray(&beings, WORLD_W / SEGMENTS_X * 3.0F, WORLD_H / SEGMENTS_Y * 3.0F);

	while (beings.num < MAX_BEINGS_NUM) {
		float x = (float)(SDL_rand((Sint32)(WORLD_W - WORLD_W / SEGMENTS_X * 4.0F))) + WORLD_W / SEGMENTS_X * 2.0F;
		float y = (float)(SDL_rand((Sint32)(WORLD_H - WORLD_H / SEGMENTS_Y * 4.0F))) + WORLD_H / SEGMENTS_Y * 2.0F;
		if (SDL_fabsf(pc.position.x - x) > 2000.0F && SDL_fabsf(pc.position.y - y) > 2000.0F) {
			Segment* s = GetSegment(x, y);
			if(s != NULL && s->beings.num < MAX_SEGM_BEINGS){
				AddBeingToArray(&beings, x, y);
			}
		}
	}

	while (!quit) {
		timer = SDL_GetTicksNS();
		quit = EventsService(&event, pc_ptr);

		// if (beings.num < MAX_BEINGS_NUM / 2048) {
		// if (beings.num < MAX_BEINGS_NUM && !(pc.control_flags & tmp0)) {
		// 	float x = (float)(SDL_rand((Sint32)(WORLD_W - WORLD_W / SEGMENTS_X * 4.0F))) + WORLD_W / SEGMENTS_X * 2.0F;
		// 	float y = (float)(SDL_rand((Sint32)(WORLD_H - WORLD_H / SEGMENTS_Y * 4.0F))) + WORLD_H / SEGMENTS_Y * 2.0F;
		// 	if (SDL_fabsf(pc.position.x - x) > 2000.0F && SDL_fabsf(pc.position.y - y) > 2000.0F) {
		// 		Segment* s = GetSegment(x, y);
		// 		if(s != NULL && s->beings.num < MAX_SEGM_BEINGS){
		// 			AddBeingToArray(&beings, x, y);
		// 		}
		// 	}
		// }

		SDL_GetMouseState(NULL, &cursor_y);
		UpdatePlayer(pc_ptr, &projectiles);
		cursor_distance = WINDOW_CENTER_Y + PLAYER_REND_Y_SHIFT - cursor_y;

		Segment* player_seg = GetSegment(pc.position.x, pc.position.y);
		UpdateProjectiles(&projectiles, player_seg);
		UpdateHProjectiles(&h_projectiles, pc_ptr);

			if (ticks_to_update_beings == 0U) {
				if (!(pc.control_flags & tmp0)) {
					UpdateBeings(&beings, pc_ptr, player_seg, &h_projectiles);
				}
				ticks_to_update_beings = 1U;
			}
			else {
				--ticks_to_update_beings;
			}
		if (SDL_GetTicksNS() > frame_time) {

			frame_time += FRAME_TIME;

			SetSineCosine(pc_ptr);

			rotation = RadToDeg(pc.direction);

			SDL_SetRenderTarget(renderer, *textures);//Gun Sight

			RenderGunSightElements(renderer, cursor_distance, RANGE);

			SDL_SetRenderTarget(renderer, NULL);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);
			SDL_SetRenderViewport(renderer, &viewfinder);
			SDL_SetRenderDrawColor(renderer, 50, 50, 50, 0);
			SDL_RenderFillRect(renderer, NULL);

			if (!(pc.control_flags & tmp0)) {
				RenderBeings(renderer, &beings, textures, pc_ptr);
			}

			RenderProjectiles(renderer, &projectiles, *(textures + tx_projectiole), pc_ptr);
			RenderHProjectiles(renderer, &h_projectiles, *(textures + tx_h_projectile), pc_ptr);
			// if (!(pc.control_flags & 1 << 6)) {
			// 	UpdateAndRenderBeings(&beings, pc_ptr, player_seg, renderer, *(textures + tx_being));
			// }
			
			SDL_RenderTexture(renderer, *(textures + tx_viewfinder), NULL, NULL);//viewfinder
			RenderPlayer(renderer, textures, player_blade);
			RenderGunSight(renderer, cursor_y, *textures);

			SDL_SetRenderViewport(renderer, NULL);

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderDebugTextFormat(renderer, 10, 10, "Fatigue: %d", pc.fatigue_points);
			SDL_RenderDebugTextFormat(renderer, 10, 20, "Position: %.2f %.2f", pc.position.x, pc.position.y);
			SDL_RenderDebugTextFormat(renderer, 10, 30, "Direction: %.2f", pc.direction);

			SDL_RenderDebugTextFormat(renderer, 10, 140, "Ticks per sec.: %d", tps);
			SDL_RenderDebugTextFormat(renderer, 10, 150, "max FPS: ~%u", (1000000000ULL / FRAME_TIME));

			SDL_RenderDebugTextFormat(renderer, 10, 160, "beings: %d", beings.num);
			SDL_RenderDebugTextFormat(renderer, 10, 170, "projectiles: %d", projectiles.num);
			SDL_RenderDebugTextFormat(renderer, 10, 180, "projectiles: %d", h_projectiles.num);
			SDL_RenderDebugTextFormat(renderer, 10, 190, "hp: %d", pc.hit_points);
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

			SDL_RenderTextureRotated(renderer, *(textures + tx_compass), NULL, &destination_rect1, -rotation, NULL, SDL_FLIP_NONE);//compass
			SDL_RenderTexture(renderer, *(textures + tx_nesw), NULL, &destination_rect0a);
			SDL_RenderTextureRotated(renderer, *(textures + tx_arrow), NULL, &destination_rect0b, rotation, NULL, SDL_FLIP_NONE);

			RenderMap(renderer, pc_ptr);

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
	DestroyProjectiles(&projectiles);
	DestroyBeings(&beings);
	DestroyWorld();
	for (int i = 0; i < TEXTURES_NUM; ++i) {
		SDL_DestroyTexture(*(textures + i));
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
