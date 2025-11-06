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
	float angle = 0.0F;
    while(1){
		angle += 0.02F;
		if(angle >= SDL_PI_F * 2.0F){
			angle = 0.0F;
		}
        RenderMainMenu(rend_data, angle);
        option = MenuEventsService(e, rend_data);
        if(option != menu_unknown) break;
        SDL_Delay(FRAME_TIME_MS);
    }
	ResetTextTextureAlpha(rend_data);
    return option;
}

void GameLoop(SDL_Event* const e, Render_data* const rend_data){
	Game_data game_data;
	SetGameData(&game_data);
	DrawMap(rend_data, &game_data.world);
	int tps = 0;
	int tps_count = 0;
	Uint64 now = 0ULL;
	Uint64 timer;
    int quit = 0;
	Uint64 time = SDL_GetTicksNS();
	Uint64 prev_frame_time = time;
	Uint64 TPS_time = SDL_GetTicks();
    StartLevel(&game_data);
    while(quit != 1){
        timer = SDL_GetTicksNS();
		if(quit == 0){
			quit = EventsService(e, game_data.champions.array + game_data.human_indx, rend_data);
		}else if(quit == event_manage_scrolls){
			quit = ManageScrollsEventsService(e, game_data.champions.array + game_data.human_indx, rend_data);
			(game_data.champions.array + game_data.human_indx)->help_data.menu_position += SCROLLS_NUM;
			(game_data.champions.array + game_data.human_indx)->help_data.menu_position %= SCROLLS_NUM;
		}
		if(UpdateGame(&game_data)){
            break;
		}
		now = SDL_GetTicksNS();
		if(now > prev_frame_time + FRAME_TIME){
			prev_frame_time = now;
            RenderGame(rend_data, &game_data, quit);
			if((game_data.champions.array + game_data.human_indx)->flags & tmp1){
				RenderTextInfo(rend_data, tps, &game_data);
			}
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
	g_d->champions.array = (Player*)SDL_malloc(sizeof(Player) * MAX_PLAYERS_NUM);
	g_d->boxes.array = (Box*)SDL_malloc(sizeof(Box) * BOXES_NUM);
	g_d->champions.num = 0U;
	g_d->boxes.num = 0U;
	g_d->human_indx = 0U;
	CreateWorld(g_d, WORLD_W, WORLD_H);
	g_d->projectiles.array = (Projectile*)SDL_malloc(sizeof(Projectile) * MAX_PROJECTILES_NUM);
	g_d->beings.array = (Being*)SDL_malloc(sizeof(Being) * MAX_BEINGS_NUM);
	g_d->beings.indices = (Uint16*)SDL_malloc(sizeof(Uint16) * MAX_BEINGS_NUM);
	if(g_d->projectiles.array == NULL || g_d->beings.array == NULL || g_d->champions.array == NULL || g_d->boxes.array == NULL || g_d->beings.indices == NULL){SDL_Quit(); exit(1);}
	for(unsigned int i = 0U; i < MAX_BEINGS_NUM; ++i){
		*(g_d->beings.indices + i) = i;
		(g_d->beings.array + i)->main_indx = i;
	}
	g_d->beings.num = 0U;
	g_d->projectiles.num = 0U;
	g_d->keys = 0U;
	g_d->effects_num = 0U;
}

static void ClearGameData(Game_data* const g_d){
	DestroyBoxes(&g_d->boxes);
	DestroyBeings(&g_d->beings);
    DestroyProjectiles(&g_d->projectiles);
	SDL_free(g_d->champions.array);
	g_d->champions.num = 0U;
	DestroyWorld(&g_d->world);
}

static void RareEventsService(Game_data* const g_d){
	static int check_queue = 0;
	if(check_queue == 0 && SDL_fabsf((g_d->champions.array + g_d->human_indx)->position.x - g_d->world.portalA.x) < half(DOOR_SIZE) && SDL_fabsf((g_d->champions.array + g_d->human_indx)->position.y - g_d->world.portalA.y) < half(DOOR_SIZE) && ((g_d->champions.array + g_d->human_indx)->flags & action)){
		SetPlayerPosition(g_d->champions.array + g_d->human_indx, g_d->world.portalB.x, g_d->world.portalB.y);
	 	(g_d->champions.array + g_d->human_indx)->flags &= ~(action);
	}else if(check_queue == 1 && SDL_fabsf((g_d->champions.array + g_d->human_indx)->position.x - g_d->world.portalB.x) < half(DOOR_SIZE) && SDL_fabsf((g_d->champions.array + g_d->human_indx)->position.y - g_d->world.portalB.y) < half(DOOR_SIZE) && ((g_d->champions.array + g_d->human_indx)->flags & action)){
		SetPlayerPosition(g_d->champions.array + g_d->human_indx, g_d->world.portalA.x, g_d->world.portalA.y);
	 	(g_d->champions.array + g_d->human_indx)->flags &= ~(action);
	}else if(check_queue == 2 && ((g_d->champions.array + g_d->human_indx)->flags & action) && g_d->boxes.num > 0U){
		int box_indx = GetNearbyBoxIndx(g_d);
		if(box_indx != -1){
			LootBox(g_d, box_indx);
	 		(g_d->champions.array + g_d->human_indx)->flags &= ~(action);
		}
	}else if(check_queue == 3 && g_d->keys >= g_d->needed_keys && SDL_fabsf((g_d->champions.array + g_d->human_indx)->position.x - g_d->world.door.x) < half(DOOR_SIZE) && SDL_fabsf((g_d->champions.array + g_d->human_indx)->position.y - g_d->world.door.y) < half(DOOR_SIZE) && ((g_d->champions.array + g_d->human_indx)->flags & action)){
		(g_d->champions.array + g_d->human_indx)->hit_points = 0;
	 	(g_d->champions.array + g_d->human_indx)->flags &= ~(action);
	}
	check_queue = (check_queue + 1) % 4;
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
	++bxs->num;
	(bxs->array + new_box_indx)->location.x = position_x;
	(bxs->array + new_box_indx)->location.y = position_y;
}

extern inline void AddToBox(Box* const bx, const unsigned int slot, const int type, const unsigned int value){
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
		if(SDL_fabsf((g_d->champions.array + g_d->human_indx)->position.x - (g_d->boxes.array + center)->location.x) < half(BOX_SIZE + PLAYER_SIZE)){
			do{
				if(SDL_fabsf((g_d->champions.array + g_d->human_indx)->position.y - (g_d->boxes.array + center)->location.y) < half(BOX_SIZE + PLAYER_SIZE)){
					return center;
				}
				++center;
			}while(center < g_d->boxes.num && SDL_fabsf((g_d->champions.array + g_d->human_indx)->position.x - (g_d->boxes.array + center)->location.x) < half(BOX_SIZE + PLAYER_SIZE));
			center = new_center - 1;
			while(center >= 0 && SDL_fabsf((g_d->champions.array + g_d->human_indx)->position.x - (g_d->boxes.array + center)->location.x) < half(BOX_SIZE + PLAYER_SIZE)){
				if(SDL_fabsf((g_d->champions.array + g_d->human_indx)->position.y - (g_d->boxes.array + center)->location.y) < half(BOX_SIZE + PLAYER_SIZE)){
					return center;
				}
				--center;
			};
			return -1;
		}else if((g_d->champions.array + g_d->human_indx)->position.x > (g_d->boxes.array + center)->location.x){
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
	bool empty = true;
	while(1){
		element_type = elem->type;
		if(element_type == box_scroll){
			if(*((g_d->champions.array + g_d->human_indx)->scrolls + elem->value) < 255U){
				++(*((g_d->champions.array + g_d->human_indx)->scrolls + elem->value));
				elem->type = box_clear;
			}else{
				SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "No space for scroll %u!", elem->value);
				empty = false;
			}
		}else if(element_type == box_coins){
			(g_d->champions.array + g_d->human_indx)->coins += (int)elem->value;
			elem->type = box_clear;
		}else if(element_type == box_key){
			g_d->keys += (int)elem->value;
			elem->type = box_clear;
		}else if(element_type == box_map){
			Key_location* kl = g_d->world.key_locations + elem->value;
			SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "Key location: x: %u, y: %u!", kl->x, kl->y);
			elem->type = box_clear;
		}else if(element_type == box_mp){
			(g_d->champions.array + g_d->human_indx)->magic_points += (int)elem->value;
			elem->value = 0U;
			break;
		}
		++elem;
	}
	if(empty){
		DestroyBoxInArray(&g_d->boxes, box_indx);
	}
}

static inline void DestroyBoxInArray(Boxes* const bxs, unsigned int indx){
	while(indx < bxs->num - 1U){
		*(bxs->array + indx) = *(bxs->array + indx + 1U);
		++indx;
	}
	--bxs->num;
}

extern inline int CalculateDamage(const Impact* const impact, const Armour* const armour){
	const float magic_damage = impact->magic * armour->magic_multipl;
	const float damage_multipl = armour->multipl + impact->armour_reduction;
	float damage = impact->damage;
	if(damage_multipl < 1.0F){
		damage *= damage_multipl;
	}
	if(impact->armour_reduction < 1.0F){
		damage -= armour->absorption * (1.0F - impact->armour_reduction);
	}
	if(damage < 0.0F){
		return (int)magic_damage;
	}
	return (int)(damage + magic_damage);
}

extern inline float CalculateStunPower(const Impact* const impact, const Armour* const armour){
	return impact->stun * armour->unstability;
}

static int UpdateGame(Game_data* const gd){
	static unsigned int ticks_to_update_beings = UPDATE_BEINGS_INTERVAL;
	//TEST
	if((gd->champions.array + gd->human_indx)->flags & tmp0){
		(gd->champions.array + gd->human_indx)->flags = 0U;
		(gd->champions.array + gd->human_indx)->selected_scroll = scroll_empty;
		gd->human_indx = (gd->human_indx + 1U) % gd->champions.num;
	}
	//TEST
	UpdatePlayers(gd);
	UpdateProjectiles(gd);
	if(ticks_to_update_beings == 0U){
		UpdateBeings(gd);
		ticks_to_update_beings = UPDATE_BEINGS_INTERVAL;
	}else{
		RareEventsService(gd);
		--ticks_to_update_beings;
	}
	if((gd->champions.array + gd->human_indx)->hit_points < 1){
		return update_defeated;
	}
	return update_ok;
}