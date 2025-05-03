#include <header.h>

int main(int argc, char* argv[]) {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* textures[TEXTURES_NUM];
	SDL_Event event;
	float cursor_y;
	Projectiles_array projectiles;
	projectiles.array = (Projectile**)SDL_malloc(sizeof(Projectile*) * MAX_PROJECTILES_NUM);
	projectiles.num = 0U;
	Beings_array beings;
	beings.array = (Being**)SDL_malloc(sizeof(Being*) * MAX_BEINGS_NUM);
	beings.num = 0U;
	float rotation = 0.0F;
	float cursor_distance = 0.0F;
	unsigned int ticks_to_update_beings = 1U;
	bool quit = false;

	struct Graphics_initiation_data* graphics_initiation_data = (struct Graphics_initiation_data*)SDL_malloc(sizeof(struct Graphics_initiation_data));
	graphics_initiation_data->window = &window;
	graphics_initiation_data->renderer = &renderer;
	for (int i = 0; i < TEXTURES_NUM; ++i) {
		*(graphics_initiation_data->textures + i) = textures + i;
	}
	if (graphicsInitiation(graphics_initiation_data)) return 1;
	SDL_free(graphics_initiation_data);

	Uint64 time = SDL_GetTicksNS();
	Uint64 now = 0ULL;
	Uint64 frame_time = time;
	Uint64 TPS_time = SDL_GetTicks();
	int tps = 0;
	int tps_count = 0;
	Uint64 timer;

	const SDL_FRect destination_rect = {
		-((VIEW_W - WINDOW_W) * 0.5F),
		-((VIEW_H - WINDOW_H) * 0.5F) + PLAYER_REND_Y_SHIFT,
		VIEW_W,
		VIEW_H
	};
	const SDL_FRect destination_rect1 = {
		10.0F,
		40.0F,
		50.0F,
		50.0F
	};
	const SDL_FRect clear_rect = {
		0.0F,
		0.0F,
		270.0F,
		300.0F
	};
	SDL_FRect present_world_part = {
		0.0F,
		0.0F,
		VIEW_W,
		VIEW_H
	};

	World* world = createWorld(WORLD_W, WORLD_H);
	drawStaticWorld(renderer, *textures);
	Player* player = createPlayer(BOUNDS_L + 1000.0F, BOUNDS_U + 1000.0F);
	if (player == NULL || world == NULL) return 1;
	addBeingToArray(&beings, createBeing(world, 1500.0F, 1500.0F));

	while (beings.num < MAX_BEINGS_NUM) {
		float x = (float)(SDL_rand((Sint32)WORLD_W) + (int)BOUNDS_L);
		float y = (float)(SDL_rand((Sint32)WORLD_H) + (int)BOUNDS_U);
		if (getSegment(world, x, y)->beings.num < MAX_SEGM_BEINGS) {
			addBeingToArray(&beings, createBeing(world, x, y));
		}
	}

	while (!quit) {
		timer = SDL_GetTicksNS();
		quit = eventsService(&event, player);

		if (beings.num < MAX_BEINGS_NUM) {
			float x = (float)(SDL_rand((Sint32)WORLD_W) + (int)BOUNDS_L);
			float y = (float)(SDL_rand((Sint32)WORLD_H) + (int)BOUNDS_U);
			if (SDL_fabsf(player->position.x - x) > 2000.0F) {
				if (SDL_fabsf(player->position.y - y) > 2000.0F) {
					if (getSegment(world, x, y)->beings.num < MAX_SEGM_BEINGS) {
						addBeingToArray(&beings, createBeing(world, x, y));
					}
				}
			}
		}

		SDL_GetMouseState(NULL, &cursor_y);
		updatePlayer(player, &projectiles);
		cursor_distance = WINDOW_CENTER_Y + PLAYER_REND_Y_SHIFT - cursor_y;

		updateProjectiles(world, &projectiles);
		if (ticks_to_update_beings == 0U) {
			if (!(player->control_flags & 1 << 6)) {
				updateBeings(&beings, world, &player->position);
			}
			ticks_to_update_beings = 1U;
		}
		else {
			--ticks_to_update_beings;
		}

		if (SDL_GetTicksNS() > frame_time) {

			present_world_part.x = player->position.x - PLAYER_RENDER_X;
			present_world_part.y = player->position.y - PLAYER_RENDER_Y;

			frame_time += FRAME_TIME;

			setVisibleRect(world, player);

			rotation = radToDeg(player->direction);

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			SDL_RenderFillRect(renderer, &clear_rect);

			SDL_SetRenderTarget(renderer, *(textures + 2));

			SDL_RenderTexture(renderer, *textures, &world->visible_rect, &world->visible_rect);

			renderProjectiles(renderer, &projectiles, *(textures + 5));

			if (!(player->control_flags & 1 << 6)) {
				renderBeings(renderer, &beings, *(textures + 6), world->visible_rect);
			}

			SDL_SetRenderTarget(renderer, *(textures + 1));

			renderGunSightElements(renderer, cursor_distance, player->range);

			SDL_SetRenderTarget(renderer, NULL);

			SDL_RenderTextureRotated(renderer, *(textures + 2), &present_world_part, &destination_rect, -rotation, NULL, SDL_FLIP_NONE);

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderDebugTextFormat(renderer, 10, 10, "Fatigue: %d", player->fatigue_points);
			SDL_RenderDebugTextFormat(renderer, 10, 20, "Position: %.2f %.2f", player->position.x, player->position.y);
			SDL_RenderDebugTextFormat(renderer, 10, 30, "Direction: %.2f", player->direction);

			SDL_RenderDebugTextFormat(renderer, 10, 100, "x: %.2f", world->visible_rect.x);
			SDL_RenderDebugTextFormat(renderer, 10, 110, "y: %.2f", world->visible_rect.y);
			SDL_RenderDebugTextFormat(renderer, 10, 120, "w: %.2f", world->visible_rect.w);
			SDL_RenderDebugTextFormat(renderer, 10, 130, "h: %.2f", world->visible_rect.h);

			SDL_RenderDebugTextFormat(renderer, 10, 140, "Ticks per sec.: %d", tps);
			SDL_RenderDebugTextFormat(renderer, 10, 150, "max FPS: ~%d", (int)(1000000000ULL / FRAME_TIME));

			SDL_RenderDebugTextFormat(renderer, 10, 160, "beings: %d", beings.num);
			SDL_RenderDebugTextFormat(renderer, 10, 170, "projectiles: %d", projectiles.num);
			SDL_RenderDebugTextFormat(renderer, 10, 180, "beings in seg0x0: %d", (*(*(world->segments + 0) + 0)).beings.num);
			SDL_RenderDebugTextFormat(renderer, 10, 190, "seg coord: %.0f %.0f", (*(*(world->segments + 0) + 0)).coordinates.x, (*(*(world->segments + 0) + 0)).coordinates.y);
			Segment* s = getSegment(world, player->position.x, player->position.y);
			SDL_RenderDebugTextFormat(renderer, 10, 260, "player: %.0f %.0f", s->coordinates.x, s->coordinates.y);
			//for (unsigned int i = 0U; i < projectiles.num; ++i) {
			//    Projectile* pr = *(projectiles.array + i);
			//    s = getSegment(world, pr->position.x, pr->position.y);
			//    SDL_RenderDebugTextFormat(renderer, 10, 270 + i * 10, "%u     : %.0f %.0f", i, s->coordinates.x, s->coordinates.y);
			//}
			//for (unsigned int i = 0U; i < beings.num; ++i) {
			//    Being* b = *(beings.array + i);
			//    Segment* s = b->segment;
			//    SDL_RenderDebugTextFormat(renderer, 10, 200 + i * 10, "%u: %.0f %.0f", i, s->coordinates.x, s->coordinates.y);
			//}

			//SDL_RenderDebugTextFormat(renderer, 10, 200, "min_delay: %llu", MINIMAL_DELAY);

			SDL_RenderTextureRotated(renderer, *(textures + 4), NULL, &destination_rect1, -rotation, NULL, SDL_FLIP_NONE);

			renderPlayer(renderer, *(textures + 3));
			renderGunSight(renderer, cursor_y, *(textures + 1));

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

	destroyPlayer(player);
	destroyProjectiles(&projectiles);
	destroyBeings(&beings);
	destroyWorld(world);
	SDL_free(beings.array);
	SDL_free(projectiles.array);
	for (int i = 0; i < TEXTURES_NUM; ++i) {
		SDL_DestroyTexture(*(textures + i));
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
