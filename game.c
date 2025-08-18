#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <enum.h>
#include <event.h>
#include <render.h>
#include <World.h>
#include <Player.h>
#include <Projectile.h>
#include <Being.h>
#include <game.h>

int MainMenuLoop(SDL_Event* const e, Render_data* const rend_data){
    int option = menu_unknown;
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
	DrawMap(rend_data, &game_data.world);
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
		UpdatePlayer(&game_data);
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
		RareEventsService(&game_data, player_seg);

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

static void SetGameData(Game_data* const g_d){
	g_d->boxes.array = (Box*)SDL_malloc(sizeof(Box) * BOXES_NUM);
	g_d->boxes.num = 0U;
	CreateWorld(g_d, WORLD_W, WORLD_H);
	g_d->projectiles.array = (Projectile*)SDL_malloc(sizeof(Projectile) * MAX_PROJECTILES_NUM);
	g_d->h_projectiles.array = (Projectile_hostile*)SDL_malloc(sizeof(Projectile_hostile) * MAX_PROJECTILES_NUM);
	g_d->beings.array = (Being*)SDL_malloc(sizeof(Being) * MAX_BEINGS_NUM);
	if(g_d->projectiles.array == NULL || g_d->beings.array == NULL || g_d->h_projectiles.array == NULL){SDL_Quit(); exit(1);}
	g_d->projectiles.num = 0U;
	g_d->h_projectiles.num = 0U;
	g_d->keys = 0U;
	g_d->needed_keys = KEYS_NUM;

}

static void ClearGameData(Game_data* const g_d){
	DestroyBoxes(&g_d->boxes);
	DestroyBeings(&g_d->beings);
    DestroyHProjectiles(&g_d->h_projectiles);
    DestroyProjectiles(&g_d->projectiles);
	DestroyWorld(&g_d->world);
}

static void RareEventsService(Game_data* const g_d, Segment* const player_seg){
	static int check_queue = 0;
	if(check_queue == 0 && SDL_fabsf(g_d->pc.position.x - g_d->world.portalA.x) < half(DOOR_SIZE) && SDL_fabsf(g_d->pc.position.y - g_d->world.portalA.y) < half(DOOR_SIZE) && (g_d->pc.control_flags & action)){
		SetPlayerPosition(&g_d->pc, g_d->world.portalB.x, g_d->world.portalB.y);
	 	g_d->pc.control_flags &= ~(action);
	}else if(check_queue == 1 && SDL_fabsf(g_d->pc.position.x - g_d->world.portalB.x) < half(DOOR_SIZE) && SDL_fabsf(g_d->pc.position.y - g_d->world.portalB.y) < half(DOOR_SIZE) && (g_d->pc.control_flags & action)){
		SetPlayerPosition(&g_d->pc, g_d->world.portalA.x, g_d->world.portalA.y);
	 	g_d->pc.control_flags &= ~(action);
	}
	check_queue = (check_queue + 1) % 2;
}

extern inline void AddBoxToArray(Boxes* const bxs, const float position_x, const float position_y){
	unsigned int new_box_indx = bxs->num;
	for(unsigned int i = 0U; i < bxs->num; ++i){
		if(position_x < (bxs->array + i)->location.x){
			new_box_indx = i;
			FreeBoxPlaceInArray(bxs, i);
			break;
		}
	}
	Box* bx = (bxs->array + new_box_indx);
	++bxs->num;
	bx->location.x = position_x;
	bx->location.y = position_y;
	AddToBox(bx, 0, box_coins, (void*)32);
	for(unsigned int i = 1U; i < BOX_SLOTS; ++i){
		AddToBox(bx, i, box_void, NULL);
	}
}

static inline void AddToBox(Box* const bx, const unsigned int slot, const int type, void* const value){
	(bx->elements + slot)->type = type;
	(bx->elements + slot)->value = value;
}

static inline void FreeBoxPlaceInArray(Boxes* const bxs, const unsigned int indx){//make room for new box
	for(unsigned int i = bxs->num - 1U; i > indx; --i){
		*(bxs->array + i) = *(bxs->array + i - 1U);
	}
}

static void DestroyBoxes(Boxes* const bxs){
	SDL_free(bxs->array);
	bxs->num = 0U;
}