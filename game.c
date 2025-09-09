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
	}else if(check_queue == 2 && (g_d->pc.control_flags & action) && g_d->boxes.num > 0U){
		int box_indx = GetNearbyBoxIndx(g_d);
		if(box_indx != -1){
			LootBox(g_d, box_indx);
	 		g_d->pc.control_flags &= ~(action);
		}
	}
	check_queue = (check_queue + 1) % 3;
}

extern inline void AddBoxToArray(Boxes* const bxs, const float position_x, const float position_y){
	unsigned int new_box_indx = bxs->num;
	for(unsigned int i = 0U; i < bxs->num; ++i){
		if(position_x < (bxs->array + i)->location.x || (position_x == (bxs->array + i)->location.x && position_y < (bxs->array + i)->location.y)){
			new_box_indx = i;
			FreeBoxPlaceInArray(bxs, i);
			break;
		}
	}
	++bxs->num;
	Box* bx = (bxs->array + new_box_indx);
	bx->location.x = position_x;
	bx->location.y = position_y;
	AddToBox(bx, 0, box_coins, (void*)32);
	AddToBox(bx, 1, box_mp, (void*)PC_MAGIC);
	for(unsigned int i = 2U; i < BOX_SLOTS; ++i){
		AddToBox(bx, i, box_mp, (void*)0);
	}
}

static inline void AddToBox(Box* const bx, const unsigned int slot, const int type, void* const value){
	(bx->elements + slot)->type = type;
	(bx->elements + slot)->value = value;
}

static inline void FreeBoxPlaceInArray(Boxes* const bxs, const unsigned int indx){//make room for new box
	for(unsigned int i = bxs->num; i > indx; --i){
		*(bxs->array + i) = *(bxs->array + i - 1U);
	}
}

static void DestroyBoxes(Boxes* const bxs){
	SDL_free(bxs->array);
	bxs->num = 0U;
}

static int GetNearbyBoxIndx(Game_data* const g_d){
	int left = 0;
	int right = g_d->boxes.num;
	int center;
	int new_center = (right - left) / 2 + left;
	do{
		center = new_center;
		if(SDL_fabsf(g_d->pc.position.x - (g_d->boxes.array + center)->location.x) < half(BOX_SIZE + PLAYER_SIZE)){
			if(SDL_fabsf(g_d->pc.position.y - (g_d->boxes.array + center)->location.y) < half(BOX_SIZE + PLAYER_SIZE)){
				return center;
			}else if(g_d->pc.position.y > (g_d->boxes.array + center)->location.y){
				left = center;
			}else{
				right = center;
			}
		}else if(g_d->pc.position.x > (g_d->boxes.array + center)->location.x){
			left = center;
		}else{
			right = center;
		}
		new_center = (right - left) / 2 + left;
	}while(new_center != center);
	return -1;
}

static inline void LootBox(Game_data* const g_d, const unsigned int box_indx){
	Box_element* elem = (g_d->boxes.array + box_indx)->elements;
	int element_type;
	while(1){
		element_type = elem->type;
		if(element_type == box_coins){
			g_d->pc.coins += (int)((intptr_t)elem->value);
		}else if(element_type == box_mp){
			g_d->pc.magic_points += (int)((intptr_t)elem->value);
			break;
		}
		++elem;
	}
	DestroyBoxInArray(&g_d->boxes, box_indx);
}

static inline void DestroyBoxInArray(Boxes* const bxs, unsigned int indx){
	while(indx < bxs->num - 1U){
		*(bxs->array + indx) = *(bxs->array + indx + 1U);
		++indx;
	}
	--bxs->num;
}