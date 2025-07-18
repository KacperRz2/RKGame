#include <header.h>

int main(int argc, char* argv[]) {
	SetRenderData(1600, 900);
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* textures[TEXTURES_NUM];
	SDL_Event event;
	float cursor_y;
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
	CreateWorld(WORLD_W, WORLD_H);
	Player pc;
	Player* const pc_ptr = &pc;
	Blade* const player_blade = &pc.blade;
	if (projectiles.array == NULL || beings.array == NULL) return 1;
	StartLevel(pc_ptr, &beings);

	while (!quit) {
		timer = SDL_GetTicksNS();
		quit = EventsService(&event, pc_ptr, window);

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
		} else {
			--ticks_to_update_beings;
		}

		if(pc.hit_points < 1){
			projectiles.num = 0;
			h_projectiles.num = 0;
			StartLevel(pc_ptr, &beings);
		}

		if (SDL_GetTicksNS() > frame_time) {

			frame_time += FRAME_TIME;

			SetSineCosine(pc_ptr);

			// SDL_SetRenderTarget(renderer, *textures);//Gun Sight

			// RenderGunSightElements(renderer, cursor_distance, RANGE);

			// SDL_SetRenderTarget(renderer, NULL);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderClear(renderer);

			SDL_SetRenderViewport(renderer, &rend_data.viewfinder_rect);

			SDL_SetRenderDrawColor(renderer, 50, 50, 50, 0);
			SDL_RenderFillRect(renderer, NULL);

			if (!(pc.control_flags & tmp0)) {
				RenderBeings(renderer, &beings, textures, pc_ptr);
			}

			RenderProjectiles(renderer, &projectiles, *(textures + tx_projectiole), pc_ptr);
			RenderHProjectiles(renderer, &h_projectiles, *(textures + tx_h_projectile), pc_ptr);
			
			SDL_RenderTexture(renderer, *(textures + tx_viewfinder), NULL, NULL);//viewfinder
			RenderPlayer(renderer, textures, player_blade);
			// RenderGunSight(renderer, cursor_y, *textures);

			// SDL_SetRenderScale(renderer, 1.0F, 1.0F);
			SDL_SetRenderViewport(renderer, NULL);

			RenderTextInfo(renderer, pc_ptr, tps, &beings, &projectiles, &h_projectiles, player_seg);

			RenderMap(renderer, textures, pc_ptr);

			RenderPlayerStatus(renderer, pc_ptr);

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
