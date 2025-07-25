#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <enum.h>
#include <game.h>
#include <event.h>
#include <render.h>
#include <World.h>
#include <Player.h>
#include <Projectile.h>
#include <Being.h>

int MainMenuLoop(SDL_Event* const e, Render_data* const rend_data){
    int option = menu_unknown;
    SDL_PumpEvents();
    SDL_FlushEvent(SDL_EVENT_KEY_UP);
    while(1){
        RenderMainMenu(rend_data);
        option = MenuEventsService(e, rend_data);
        if(option != menu_unknown) break;
        SDL_Delay(FRAME_TIME_MS);
    }
    return option;
}

void GameLoop(SDL_Event* const e, Render_data* const rend_data){
	Game_data game_data;
	SetGameData(&game_data);
    float cursor_y;
	float cursor_distance = 0.0F;
	unsigned int ticks_to_update_beings = UPDATE_BEINGS_INTERVAL;
	int tps = 0;
	int tps_count = 0;
	Uint64 now = 0ULL;
	Uint64 timer;
    bool quit = false;
	Uint64 time = SDL_GetTicksNS();
	Uint64 prev_frame_time = time;
	Uint64 TPS_time = SDL_GetTicks();
    StartLevel(&game_data);
    while(!quit){
        timer = SDL_GetTicksNS();
		quit = EventsService(e, &game_data.pc, rend_data);
		SDL_GetMouseState(NULL, &cursor_y);
		UpdatePlayer(&game_data);
		cursor_distance = WINDOW_CENTER_Y + PLAYER_REND_Y_SHIFT - cursor_y;
		Segment* player_seg = GetSegment(&game_data.world, game_data.pc.position.x, game_data.pc.position.y);
		UpdateProjectiles(&game_data, player_seg);
		UpdateHProjectiles(&game_data);
		if(ticks_to_update_beings == 0U){
			if(!(game_data.pc.control_flags & tmp0)){
				UpdateBeings(&game_data);
			}
			ticks_to_update_beings = UPDATE_BEINGS_INTERVAL;
		}else{
			--ticks_to_update_beings;
		}
		if(game_data.pc.hit_points < 1){
            break;
		}

		now = SDL_GetTicksNS();
		if(now > prev_frame_time + FRAME_TIME){
			prev_frame_time = now;
            RenderGame(rend_data, &game_data);
			RenderTextInfo(rend_data->renderer, tps, &game_data, player_seg);
			SDL_RenderPresent(rend_data->renderer);
		}

		if(SDL_GetTicks() - TPS_time >= 1000ULL){
			tps = tps_count;
			tps_count = 0;
			TPS_time = SDL_GetTicks();
		}
		++tps_count;
		time += TICK_TIME;
		now = SDL_GetTicksNS();
		if(now < time){
			SDL_DelayNS(time - now);
		}else if(now - timer < TICK_TIME){
			SDL_DelayNS((TICK_TIME - (now - timer)) >> 1);
		}
    }
	ClearGameData(&game_data);
}

void SetGameData(Game_data* const g_d){
	CreateWorld(&g_d->world, WORLD_W, WORLD_H);
	g_d->projectiles.array = (Projectile*)SDL_malloc(sizeof(Projectile) * MAX_PROJECTILES_NUM);
	g_d->h_projectiles.array = (Projectile_hostile*)SDL_malloc(sizeof(Projectile_hostile) * MAX_PROJECTILES_NUM);
	g_d->beings.array = (Being*)SDL_malloc(sizeof(Being) * MAX_BEINGS_NUM);
	if(g_d->projectiles.array == NULL || g_d->beings.array == NULL || g_d->h_projectiles.array == NULL){SDL_Quit(); exit(1);}
	g_d->projectiles.num = 0U;
	g_d->h_projectiles.num = 0U;
}

void ClearGameData(Game_data* const g_d){
	DestroyBeings(&g_d->beings);
    DestroyHProjectiles(&g_d->h_projectiles);
    DestroyProjectiles(&g_d->projectiles);
	DestroyWorld(&g_d->world);
}