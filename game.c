#include <SDL3/SDL.h>
#include <macros.h>
#include <enum.h>
#include <types.h>
#include <event.h>
#include <render.h>
#include <World.h>
#include <Player.h>
#include <Projectile.h>
#include <Being.h>
#include <game.h>
#include <function.h>
#include <Scroll.h>

static inline void PopulateBigSeg(Game_data* const gd, const unsigned int x, const unsigned int y){
	if(IsVoidBigSeg(gd->world.plan, x, y) || IsInPopulatedBigSeg(gd->world.plan, x, y)){
		return;
	}
	MarkAsPopulatedBigSeg(gd->world.plan, x, y);
	UpdateMap(gd->rend_data_ptr, POPULATED_SEG_RGB, x, y);
}

static inline void PlayerInUncoveredBigSeg(Game_data* const gd){
	const unsigned int seg_x = GetBigSegCoordinate(human(gd)->position.x);
	const unsigned int seg_y = GetBigSegCoordinate(human(gd)->position.y);
	if(!IsInUncoveredBigSeg(gd->world.plan, seg_x, seg_y)){
		UncoverBigSeg(gd->world.plan, seg_x, seg_y);
		PopulateBigSeg(gd, seg_x + 1U, seg_y);
		PopulateBigSeg(gd, seg_x, seg_y + 1U);
		PopulateBigSeg(gd, seg_x - 1U, seg_y);
		PopulateBigSeg(gd, seg_x, seg_y - 1U);
		UpdateMap(gd->rend_data_ptr, UNCOVERED_SEG_RGB, seg_x, seg_y);
	}
}

int MainMenuLoop(SDL_Event* const e, Render_data* const rend_data){
    int option = menu_unknown;
	unsigned int menu_position = 0U;
    while(1){
        RenderMainMenu(rend_data, menu_position);
        option = MainMenuEventsService(e, rend_data, &menu_position);
		menu_position += OPTIONS_NUM;
		menu_position %= OPTIONS_NUM;
        if(option != menu_unknown) break;
        SDL_Delay(FRAME_TIME_MS);
    }
    return option;
}

void GameLoop(Game_data* const gd){
	int tps = 0;
	int tps_count = 0;
	Uint64 now = 0ULL;
	Uint64 timer;
    int quit = 0;
	Uint64 time = SDL_GetTicksNS();
	Uint64 prev_frame_time = time;
	Uint64 TPS_time = SDL_GetTicks();
    while(quit != event_quit_game){
        timer = SDL_GetTicksNS();
		if(quit == 0){
			quit = EventsService(gd->ev_ptr, human(gd), gd->rend_data_ptr);
		}else if(quit == event_manage_scrolls){
			quit = ManageScrollsEventsService(gd->ev_ptr, human(gd), gd->rend_data_ptr);
			(human(gd))->help_data.menu_position += SCROLLS_NUM;
			(human(gd))->help_data.menu_position %= SCROLLS_NUM;
		}else if(quit == event_menu){
			quit = MenuEventsService(gd);
			(human(gd))->help_data.menu_position += OPTIONS_NUM;
			(human(gd))->help_data.menu_position %= OPTIONS_NUM;
		}
		const int update_result = UpdateGame(gd);
		if(update_result != update_ok){
			if(update_result == update_shop){
				time = SDL_GetTicksNS();
				prev_frame_time = time;
			}else{
				EndLoop(gd->ev_ptr, gd, update_result);
				break;
			}
		}
		now = SDL_GetTicksNS();
		if(now > prev_frame_time + FRAME_TIME){
			prev_frame_time = now;
            RenderGame(gd->rend_data_ptr, gd, quit);
			if((human(gd))->flags & tmp){
				RenderTextInfo(gd->rend_data_ptr, tps, gd);
			}
			SDL_RenderPresent(gd->rend_data_ptr->renderer);
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
}

static void EndLoop(SDL_Event* const e, Game_data* const gd, const int result){
	if(result == update_defeated){
		for(unsigned int i = 0U; i < 50U; ++i){
			RenderDefeatedScreen(gd->rend_data_ptr);
			SDL_Delay(40U);
		}
		SDL_PumpEvents();
		SDL_FlushEvent(SDL_EVENT_KEY_UP);
		SDL_FlushEvent(SDL_EVENT_MOUSE_BUTTON_UP);
		while(1){
			if(EndingEventsService(e)){
				break;
			}
			RenderDefeatedScreen(gd->rend_data_ptr);
        	SDL_Delay(FRAME_TIME_MS);
		}
	}else{
		for(unsigned int i = 0U; i < 50U; ++i){
			RenderVictoryScreen(gd->rend_data_ptr);
			SDL_Delay(40U);
		}
		SDL_PumpEvents();
		SDL_FlushEvent(SDL_EVENT_KEY_UP);
		SDL_FlushEvent(SDL_EVENT_MOUSE_BUTTON_UP);
		while(1){
			if(EndingEventsService(e)){
				break;
			}
			RenderVictoryScreen(gd->rend_data_ptr);
        	SDL_Delay(FRAME_TIME_MS);
		}
	}
}

void SetGameData(Game_data* const gd){
	gd->flags = 0x0U;
	gd->champions.array = (Player*)SDL_malloc(sizeof(Player) * MAX_PLAYERS_NUM);
	gd->boxes.array = (Box*)SDL_malloc(sizeof(Box) * BOXES_NUM);
	gd->champions.num = 0U;
	gd->boxes.num = 0U;
	gd->human_indx = 0U;
	CreateWorld(gd);
	gd->projectiles.array = (Projectile*)SDL_malloc(sizeof(Projectile) * MAX_PROJECTILES_NUM);
	gd->beings.array = (Being*)SDL_malloc(sizeof(Being) * MAX_BEINGS_NUM);
	gd->beings.indices = (Uint16*)SDL_malloc(sizeof(Uint16) * MAX_BEINGS_NUM);
	gd->rend_data_ptr->visual_effects.array = (Visual_effect*)SDL_malloc(sizeof(Visual_effect) * MAX_VISUAL_EFFECTS_NUM);
	if(gd->projectiles.array == NULL || gd->beings.array == NULL || gd->champions.array == NULL || gd->boxes.array == NULL || gd->beings.indices == NULL || gd->rend_data_ptr->visual_effects.array == NULL){exit(-1); exit(1);}
	for(unsigned int i = 0U; i < MAX_BEINGS_NUM; ++i){
		*(gd->beings.indices + i) = i;
		(gd->beings.array + i)->main_indx = i;
	}
	gd->beings.num = 0U;
	gd->projectiles.num = 0U;
	gd->enemy_morale = MAX_MORALE;
	gd->rend_data_ptr->visual_effects.num = 0U;
	gd->keys = 0U;
	gd->effects_num = 0U;
	gd->horde_data.ticks_from_attack = 0U;
	SDL_SetWindowRelativeMouseMode(gd->rend_data_ptr->window, true);
}

void ClearGameData(Game_data* const gd){
	DestroyBoxes(&gd->boxes);
	DestroyBeings(&gd->beings);
    DestroyProjectiles(&gd->projectiles);
	SDL_free(gd->champions.array);
	SDL_free(gd->rend_data_ptr->visual_effects.array);
	gd->champions.num = 0U;
	gd->rend_data_ptr->visual_effects.num = 0U;
	SDL_free(gd->keys_status);
	DestroyWorld(&gd->world);
}

static int RareEventsService(Game_data* const gd){
	static int check_queue = 0;
	if(check_queue == 0){
		if(SDL_fabsf(human(gd)->position.x - gd->world.portalA.x) < half(DOOR_SIZE) && SDL_fabsf(human(gd)->position.y - gd->world.portalA.y) < half(DOOR_SIZE) && (human(gd)->flags & action)){
			SetPlayerPosition(human(gd), gd->world.portalB.x, gd->world.portalB.y);
			UpdatePlayerNewSegment(&gd->world, human(gd));
			human(gd)->flags &= ~(action);
		}else if(human(gd)->flags & action){
			int shop_indx = GetNearbyShopIndx(gd);
			if(shop_indx != -1){
				EnterShop(gd, human(gd), shop_indx);
				human(gd)->flags &= ~(action);
				return update_shop;
			}
		}
	}else if(check_queue == 1 && SDL_fabsf(human(gd)->position.x - gd->world.portalB.x) < half(DOOR_SIZE) && SDL_fabsf(human(gd)->position.y - gd->world.portalB.y) < half(DOOR_SIZE) && (human(gd)->flags & action)){
		SetPlayerPosition(human(gd), gd->world.portalA.x, gd->world.portalA.y);
		UpdatePlayerNewSegment(&gd->world, human(gd));
	 	human(gd)->flags &= ~(action);
	}else if(check_queue == 2 && (human(gd)->flags & action) && gd->boxes.num > 0U){
		int box_indx = GetNearbyBoxIndx(gd);
		if(box_indx != -1){
			LootBox(gd, box_indx);
	 		human(gd)->flags &= ~(action);
		}
	}else if(check_queue == 3){
		if(gd->keys >= gd->needed_keys && SDL_fabsf(human(gd)->position.x - gd->world.door.x) < half(DOOR_SIZE) && SDL_fabsf(human(gd)->position.y - gd->world.door.y) < half(DOOR_SIZE) && (human(gd)->flags & action)){
			human(gd)->flags &= ~(action);
			return update_victory;
		}
		human(gd)->attention_rect.x = human(gd)->position.x - half(ATTENTION_RECT_SIZE);
		human(gd)->attention_rect.y = human(gd)->position.y - half(ATTENTION_RECT_SIZE);
	}else if(check_queue == 4){
		if(!(gd->flags & gamef_horde_attack)){
			if(gd->horde_data.ticks_from_attack > HORDE_ATTACK_START_TICKS * 2 + SDL_rand(HORDE_ATTACK_START_TICKS * 32)){
				AddGameEffect(gd, (Lasting_effect){game_effect_horde_attack, HORDE_ATTACK_START_TICKS});
				gd->flags |= gamef_horde_attack;
			}else{
				++gd->horde_data.ticks_from_attack;
			}
		}
	}else if(check_queue == 5){
		PlayerInUncoveredBigSeg(gd);
	}
	check_queue = (check_queue + 1) % 6;
	return 0;
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

static inline void FreeBoxPlaceInArray(Boxes* const bxs, const unsigned int indx){
	for(unsigned int i = bxs->num; i > indx; --i){
		*(bxs->array + i) = *(bxs->array + i - 1U);
	}
}

static void DestroyBoxes(Boxes* const bxs){
	SDL_free(bxs->array);
	bxs->num = 0U;
}

static int GetNearbyBoxIndx(Game_data* const gd){
	int left = 0;
	int right = gd->boxes.num;
	int center;
	int new_center = (right - left) / 2 + left;
	do{
		center = new_center;
		if(SDL_fabsf(human(gd)->position.x - (gd->boxes.array + center)->location.x) < half(BOX_SIZE + PLAYER_SIZE)){
			do{
				if(SDL_fabsf(human(gd)->position.y - (gd->boxes.array + center)->location.y) < half(BOX_SIZE + PLAYER_SIZE)){
					return center;
				}
				++center;
			}while(center < gd->boxes.num && SDL_fabsf(human(gd)->position.x - (gd->boxes.array + center)->location.x) < half(BOX_SIZE + PLAYER_SIZE));
			center = new_center - 1;
			while(center >= 0 && SDL_fabsf(human(gd)->position.x - (gd->boxes.array + center)->location.x) < half(BOX_SIZE + PLAYER_SIZE)){
				if(SDL_fabsf(human(gd)->position.y - (gd->boxes.array + center)->location.y) < half(BOX_SIZE + PLAYER_SIZE)){
					return center;
				}
				--center;
			};
			return -1;
		}else if(human(gd)->position.x > (gd->boxes.array + center)->location.x){
			left = center;
		}else{
			right = center;
		}
		new_center = (right - left) / 2 + left;
	}while(new_center != center);
	return -1;
}

static inline void LootBox(Game_data* const gd, const unsigned int box_indx){
	Box_element* elem = (gd->boxes.array + box_indx)->elements;
	int element_type;
	bool empty = true;
	bool found_key = false;
	while(1){
		element_type = elem->type;
		if(element_type == box_scroll){
			if(*(human(gd)->scrolls + elem->value) < 255U){
				++(*(human(gd)->scrolls + elem->value));
				elem->type = box_clear;
			}else{
				SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "No space for scroll %u!", elem->value);
				empty = false;
			}
		}else if(element_type == box_coins){
			human(gd)->coins += (int)elem->value;
			elem->type = box_clear;
		}else if(element_type == box_key){
			found_key = true;
			++gd->keys;
			*(gd->keys_status + elem->value) = key_owned;
			elem->type = box_clear;
		}else if(element_type == box_map){
			if(*(gd->keys_status + elem->value) == key_location_unknown){
				*(gd->keys_status + elem->value) = key_location_known;
			}
			SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "Key location: x: %u, y: %u!", (gd->world.key_locations + elem->value)->x, (gd->world.key_locations + elem->value)->y);
			elem->type = box_clear;
		}else if(element_type == box_mp){
			human(gd)->magic_points += (int)elem->value;
			elem->value = 0U;
			break;
		}
		++elem;
	}
	if(empty){
		DestroyBoxInArray(&gd->boxes, box_indx);
	}
	if(found_key){
		SaveGame(gd);
	}
}

static inline void DestroyBoxInArray(Boxes* const bxs, unsigned int indx){
	while(indx < bxs->num - 1U){
		*(bxs->array + indx) = *(bxs->array + indx + 1U);
		++indx;
	}
	--bxs->num;
}

static int GetNearbyShopIndx(Game_data* const gd){
	int left = 0;
	int right = SHOPS_NUM;
	int center;
	int new_center = (right - left) / 2 + left;
	do{
		center = new_center;
		if(SDL_fabsf(human(gd)->position.x - (gd->world.shops + center)->location.x) < SHOP_ENTRY_RANGE){
			do{
				if(SDL_fabsf(human(gd)->position.y - (gd->world.shops + center)->location.y) < SHOP_ENTRY_RANGE){
					return center;
				}
				++center;
			}while(center < SHOPS_NUM && SDL_fabsf(human(gd)->position.x - (gd->world.shops + center)->location.x) < SHOP_ENTRY_RANGE);
			center = new_center - 1;
			while(center >= 0 && SDL_fabsf(human(gd)->position.x - (gd->world.shops + center)->location.x) < SHOP_ENTRY_RANGE){
				if(SDL_fabsf(human(gd)->position.y - (gd->world.shops + center)->location.y) < SHOP_ENTRY_RANGE){
					return center;
				}
				--center;
			};
			return -1;
		}else if(human(gd)->position.x > (gd->world.shops + center)->location.x){
			left = center;
		}else{
			right = center;
		}
		new_center = (right - left) / 2 + left;
	}while(new_center != center);
	return -1;
}

static void AddScrollToShop(Shop* const sh, const unsigned int scroll_id){
	for(unsigned int i = 0U; i < SHOP_SCROLLS_NUM; ++i){
		if(*(sh->scrolls + i) == scroll_empty){
			*(sh->scrolls + i) = scroll_id;
			return;
		}
	}
}

static inline bool ItemNotChosen(const unsigned int item, const Uint8* const items, const unsigned int items_num){
	for(unsigned int i = 0U; i < items_num; ++i){
		if(*(items + i) == item){
			return false;
		}
	}
	return true;
}

static inline bool IsUnknowKey(const Game_data* const gd){
	for(unsigned int i = 0U; i < gd->needed_keys; ++i){
		if(*(gd->keys_status + i) == key_location_unknown){
			return true;
		}
	}
	return false;
}

static void EnterShop(Game_data* const gd, Player* const pc, const unsigned int shop_indx){
	Render_data* const rend_data = gd->rend_data_ptr;
	SDL_SetWindowRelativeMouseMode(rend_data->window, false);
	SDL_SetWindowMouseRect(rend_data->window, NULL);
    int option = 0;
	Uint8 items_to_sell[MAX_ITEMS_TO_SELL];
	unsigned int items_to_sell_num = 0U;
	Uint8 items_to_get[MAX_ITEMS_TO_SELL];
	unsigned int items_to_get_num = 0U;
	int profit = 0;
	pc->help_data.menu_position = SHOP_POSITIONS;
    while(option != -1){
        RenderShop(rend_data, pc, gd->world.shops + shop_indx, items_to_sell, items_to_sell_num, items_to_get, items_to_get_num, profit);
        if((option = ShopEventsService(gd->ev_ptr, pc, rend_data)) > 0 && pc->help_data.menu_position < SHOP_POSITIONS){
			const unsigned int col = pc->help_data.menu_position % SHOP_COLS;
			const unsigned int row = pc->help_data.menu_position / SHOP_COLS;
			if(col < SHOP_SIDE_COLS && row >= FIRST_SHOP_ROW){
				if(items_to_sell_num < MAX_ITEMS_TO_SELL){
					const unsigned int item_num = col + (row - FIRST_SHOP_ROW) * SHOP_SIDE_COLS;
					if(item_num < scroll_empty && *(pc->scrolls + item_num) > 0U){
						*(items_to_sell + items_to_sell_num++) = item_num;
						--(*(pc->scrolls + item_num));
						profit += ItemPrice(item_num) / SELL_DIVIDER;
					}
				}
			}else if(row == SHOP_ROWS - 1U && pc->coins >= -profit){
				items_to_sell_num = 0U;
				for(unsigned int i = 0U; i < items_to_get_num; ++i){
					const unsigned int item_num = *(items_to_get + i);
					if(item_num < shop_item_invalid){
						++(*(pc->scrolls + item_num));
					}else if(item_num == shop_item_mp10){
						pc->magic_points += 10;
					}else if(item_num == shop_item_mp100){
						pc->magic_points += 100;
					}else if(item_num == shop_item_key){
						++gd->keys;
					}else if(item_num == shop_item_key_location){
						const unsigned int key_indx = SDL_rand(gd->needed_keys);
						if(*(gd->keys_status + key_indx) == key_location_unknown){
							*(gd->keys_status + key_indx) = key_location_known;
						}else{
							unsigned int key_indx1 = (key_indx + 1U) % gd->needed_keys;
							do{
								if(*(gd->keys_status + key_indx1) == key_location_unknown){
									*(gd->keys_status + key_indx1) = key_location_known;
									break;
								}
								key_indx1 = (key_indx1 + 1U) % gd->needed_keys;
							}while(key_indx1 != key_indx);
						}
					}else if(item_num == shop_item_armour){
						pc->max_armour = (Armour)PC_MAX_ARMOUR_II;
					}else if(item_num == shop_item_dodge_speed){
						pc->dodge_velocity_multipl = PC_DODGE_VELOCITY * 2.0F / PLAYER_VELOCITY;
					}
				}
				items_to_get_num = 0U;
				pc->coins += profit;
				profit = 0;
			}else if(col == SHOP_SIDE_COLS + 1U){
				const unsigned int item_num = *(items_to_sell + row);
				if(items_to_sell_num > row){
					++(*(pc->scrolls + item_num));
					*(items_to_sell + row) = *(items_to_sell + --items_to_sell_num);
					profit -= ItemPrice(item_num) / SELL_DIVIDER;
				}
			}else if(col > SHOP_COLS - SHOP_SCROLLS_COLS - 2U){
				if(items_to_get_num < MAX_ITEMS_TO_SELL){
					const unsigned int indx = col - (SHOP_COLS - SHOP_SCROLLS_COLS - 1U) + (row - FIRST_SHOP_ROW - 1U) * SHOP_SCROLLS_COLS;
					const unsigned int item_num = *((gd->world.shops + shop_indx)->scrolls + indx);
					if(item_num < scroll_empty){
						*(items_to_get + items_to_get_num++) = item_num;
						profit -= ItemPrice(item_num);
						*((gd->world.shops + shop_indx)->scrolls + indx) = scroll_empty;
					}
				}
			}else if(col == SHOP_SIDE_COLS + 3U){
				const unsigned int item_num = *(items_to_get + row);
				if(items_to_get_num > row){
					*(items_to_get + row) = *(items_to_get + --items_to_get_num);
					profit += ItemPrice(item_num);
					if(item_num < shop_item_invalid){
						AddScrollToShop(gd->world.shops + shop_indx, item_num);
					}
				}
			}else if(items_to_get_num < MAX_ITEMS_TO_SELL){
				if(row == FIRST_SHOP_ROW + 1U){
					if(col == SHOP_COLS - SHOP_SIDE_COLS - 1U){
						*(items_to_get + items_to_get_num++) = shop_item_mp10;
						profit -= ItemPrice(shop_item_mp10);
					}else if(col == SHOP_COLS - SHOP_SIDE_COLS && gd->keys < gd->needed_keys && ItemNotChosen(shop_item_key, items_to_get, items_to_get_num)){
						*(items_to_get + items_to_get_num++) = shop_item_key;
						profit -= ItemPrice(shop_item_key);
					}else if(col == SHOP_COLS - SHOP_SIDE_COLS + 1U && ItemNotChosen(shop_item_key_location, items_to_get, items_to_get_num) && gd->keys < gd->needed_keys && IsUnknowKey(gd)){
						*(items_to_get + items_to_get_num++) = shop_item_key_location;
						profit -= ItemPrice(shop_item_key_location);
					}
				}else if(row == FIRST_SHOP_ROW + 2U && col == SHOP_COLS - SHOP_SIDE_COLS - 1U){
					*(items_to_get + items_to_get_num++) = shop_item_mp100;
					profit -= ItemPrice(shop_item_mp100);
				}else if(row == FIRST_SHOP_ROW + 5U){
					if(col == SHOP_COLS - SHOP_SIDE_COLS - 1U && pc->max_armour.absorption < PC_MAX_ARMOUR_II_ABS && ItemNotChosen(shop_item_armour, items_to_get, items_to_get_num)){
						*(items_to_get + items_to_get_num++) = shop_item_armour;
						profit -= ItemPrice(shop_item_armour);
					}else if(col == SHOP_COLS - SHOP_SIDE_COLS && pc->dodge_velocity_multipl < PC_DODGE_VELOCITY * 2.0F / PLAYER_VELOCITY && ItemNotChosen(shop_item_dodge_speed, items_to_get, items_to_get_num)){
						*(items_to_get + items_to_get_num++) = shop_item_dodge_speed;
						profit -= ItemPrice(shop_item_dodge_speed);
					}
				}
			}
		}
        SDL_Delay(FRAME_TIME_MS);
    }
	for(unsigned int i = 0U; i < items_to_sell_num; ++i){
		const unsigned int item_num = *(items_to_sell + i);
		++(*(pc->scrolls + item_num));
	}
	for(unsigned int i = 0U; i < items_to_get_num; ++i){
		const unsigned int item_num = *(items_to_get + i);
		if(item_num < shop_item_invalid){
			AddScrollToShop(gd->world.shops + shop_indx, item_num);
		}
	}
	SDL_WarpMouseInWindow(rend_data->window, half(rend_data->window_w), half(rend_data->window_h));
	SDL_SetWindowRelativeMouseMode(rend_data->window, true);
	SetMouseBarrier(rend_data);
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
	static unsigned int ticks_to_rare_update = RARE_UPDATE_INTERVAL;
	UpdatePlayers(gd);
	UpdateProjectiles(gd);
	if(ticks_to_rare_update == 0U){
		UpdateBeings(gd);
		ticks_to_rare_update = RARE_UPDATE_INTERVAL;
	}else{
		const int rare_update_resu = RareEventsService(gd);
		if(rare_update_resu == update_victory){
			return update_victory;
		}
		if(rare_update_resu == update_shop){
			return update_shop;
		}
		UpdateEffects(gd);
		--ticks_to_rare_update;
	}
	if(human(gd)->hit_points < 1){
		return update_defeated;
	}
	return update_ok;
}

int ActivateMenuOption(const unsigned int option, Game_data* const gd){
	if(option == menu_p_continue){
		SDL_SetWindowRelativeMouseMode(gd->rend_data_ptr->window, true);
		return 0;
	}else if(option == menu_p_load){
		ClearGameData(gd);
		LoadGame(gd);
		return 0;
	}else if(option == menu_p_save){
		SaveGame(gd);
		return event_quit_game;
	}else if(option == menu_p_quit){
		return event_quit_game;
	}else if(option == menu_p_settings){
		ToggleFullscreen(gd->rend_data_ptr);
		return event_menu;
	}else{
		return event_menu;
	}
}

extern inline int ItemPrice(const unsigned int item){
    const int prices[] = ITEMS_PRICES;
    return *(prices + item);
}

static inline void UpdateEffects(Game_data* const gd){
	UpdateGameEffects(gd);
	UpdateBeingsEffects(gd);
	UpdatePlayersEffects(gd);
}

static inline SDL_FPoint GetBeingCreationPoint(Game_data* const gd, const float angle){
    const float shift_x = SineSafe(angle);
    const float shift_y = -CosiSafe(angle);
	const float help_to_x = shift_x < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	const float help_to_y = shift_y < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	SDL_FPoint control_point_old = human(gd)->position;
	SDL_FPoint control_point = control_point_old;
	while(1){
		const float help_x = SDL_fmodf(control_point.x, SEGMENT_SIZE);
		const float help_y = SDL_fmodf(control_point.y, SEGMENT_SIZE);
		const float shifts_to_next_seg_x = (help_to_x - help_x) / shift_x;
		const float shifts_to_next_seg_y = (help_to_y - help_y) / shift_y;
		const float multipl = (shifts_to_next_seg_x < shifts_to_next_seg_y ? shifts_to_next_seg_x : shifts_to_next_seg_y);
		control_point.x += shift_x * multipl;
		control_point.y += shift_y * multipl;
		if(GetSegmentUnsafe(&gd->world, control_point.x, control_point.y) == NULL){
			control_point.x = control_point_old.x + shift_x * multipl * 0.875F;
			control_point.y = control_point_old.y + shift_y * multipl * 0.875F;
			if(GetSegmentUnsafe(&gd->world, control_point.x, control_point.y) == NULL){
				return control_point_old;
			}
			return control_point;
		}
		control_point_old = control_point;
	}
}

extern inline int HasEffect(Lasting_effect* const efs, const unsigned int efs_num, const unsigned int effect_id){
    for(unsigned int i = 0U; i < efs_num; ++i){
        if((efs + i)->id == effect_id){
            return i;
        }
    }
    return -1;
}

extern inline void AddLastingEffect(Lasting_effect* const effects, const Lasting_effect effect, const int effects_num){
	*(effects + effects_num) = effect;
}

extern inline void RemoveLastingEffect(Lasting_effect* const effects, const int indx, const int effects_num){
	*(effects + indx) = *(effects + effects_num - 1);
}

static void AddGameEffect(Game_data* const gd, const Lasting_effect effect){
	if(gd->effects_num < MAX_GAME_EFFECTS){
		AddLastingEffect(gd->effects, effect, gd->effects_num++);
	}
}

static void RemoveGameEffect(Game_data* const gd, const int indx){
	RemoveLastingEffect(gd->effects, indx, gd->effects_num--);
}

static inline void UpdateGameEffects(Game_data* const gd){
	for(unsigned int i = 0U; i < gd->effects_num; ++i){
		UpdateGameEffect(gd, i);
	}
}

static inline void UpdateGameEffect(Game_data* const gd, const int effect_indx){
    const void (*effect[])(Game_data* const, const int) = GAME_LASTING_EFFECTS;
    (*(effect + (gd->effects + effect_indx)->id))(gd, (gd->effects + effect_indx)->ticks_left--);
	if((gd->effects + effect_indx)->ticks_left < 1){
		RemoveGameEffect(gd, effect_indx);
	}
}

void HordeAttack(Game_data* const gd, const int ticks_left){
	if(ticks_left == HORDE_ATTACK_START_TICKS){
		const float shift = FULL_ANGLE / HORDE_ATTACK_POINTS;
		float angle = SDL_randf() * shift;
		for(unsigned int i = 0U; i < HORDE_ATTACK_POINTS; ++i){
			*(gd->horde_data.creation_points + i) = GetBeingCreationPoint(gd, angle);
			angle += shift;
		}
		gd->enemy_morale = MAX_MORALE;
	}else if(ticks_left < 2){
		gd->flags &= ~(gamef_horde_attack);
		gd->horde_data.ticks_from_attack = 0U;
		gd->enemy_morale = MAX_MORALE;
	}else if((ticks_left < (HORDE_ATTACK_START_TICKS / 2) && !SDL_rand((HORDE_ATTACK_START_TICKS - ticks_left) / 16)) || (ticks_left >= (HORDE_ATTACK_START_TICKS / 2) && !SDL_rand(ticks_left / 16))){
		const unsigned int point_indx = SDL_rand(HORDE_ATTACK_POINTS);
		unsigned int point_indx1 = point_indx;
		do{
			if(TryCreateHostileBeing(gd, GetRandomBeingId(), (gd->horde_data.creation_points + point_indx1)->x, (gd->horde_data.creation_points + point_indx1)->y, human(gd))){
				break;
			}
			point_indx1 = (point_indx1 + 1U) % HORDE_ATTACK_POINTS;
		}while(point_indx1 != point_indx);
	}
}

struct Save_data{
	struct nums{
		Uint8 champions;
		Uint16 beings;
		Uint16 projectiles;
		Uint16 boxes;
		Uint8 needed_keys;
	}nums;
	unsigned int flags;
	Player champions[MAX_PLAYERS_NUM];
	Being beings[MAX_BEINGS_NUM];
	Projectile projectiles[MAX_PROJECTILES_NUM];
	Uint64 plan[BIG_SEGMENTS_X];
	Key_location key_locations[MAX_KEYS];
	Shop shops[SHOPS_NUM];
	Box boxes[BOXES_NUM];
	Uint8 human_indx;
	Uint8 keys;
	int enemy_morale;
	Uint8 keys_status[MAX_KEYS];
	Uint8 effects_num;
	Lasting_effect effects[MAX_GAME_EFFECTS];
	union horde_data horde_data;
	Uint64 seed;
};

void SaveGame(const Game_data* const gd){
    SDL_Storage *user = SDL_OpenUserStorage("RzK", "KGame", 0);
    if(user == NULL){
        exit(-1);
    }
    while(!SDL_StorageReady(user)){
        SDL_Delay(1);
    }
	struct Save_data* save_data = (struct Save_data*)SDL_malloc(sizeof(struct Save_data));
	save_data->nums = (struct nums){
		gd->champions.num,
		gd->beings.num,
		gd->projectiles.num,
		gd->boxes.num,
		gd->needed_keys
	};
	save_data->flags = gd->flags;
	SDL_memcpy(save_data->champions, gd->champions.array, sizeof(Player) * save_data->nums.champions);
	for(unsigned int i = 0U; i < save_data->nums.beings; ++i){
		*(save_data->beings + i) = *(gd->beings.array + *(gd->beings.indices + i));
	}
	SDL_memcpy(save_data->projectiles, gd->projectiles.array, sizeof(Projectile) * save_data->nums.projectiles);
	SDL_memcpy(save_data->plan, gd->world.plan, sizeof(Uint64) * BIG_SEGMENTS_X);
	SDL_memcpy(save_data->key_locations, gd->world.key_locations, sizeof(Key_location) * save_data->nums.needed_keys);
	SDL_memcpy(save_data->shops, gd->world.shops, sizeof(Shop) * SHOPS_NUM);
	SDL_memcpy(save_data->boxes, gd->boxes.array, sizeof(Box) * save_data->nums.boxes);
	save_data->human_indx = gd->human_indx;
	save_data->keys = gd->keys;
	save_data->enemy_morale = gd->enemy_morale;
	SDL_memcpy(save_data->keys_status, gd->keys_status, sizeof(Uint8) * save_data->nums.needed_keys);
	save_data->effects_num = gd->effects_num;
	SDL_memcpy(save_data->effects, gd->effects, sizeof(Lasting_effect) * MAX_GAME_EFFECTS);
	if(gd->flags & gamef_horde_attack){
		SDL_memcpy(save_data->horde_data.creation_points, gd->horde_data.creation_points, sizeof(SDL_FPoint) * HORDE_ATTACK_POINTS);
	}else{
		save_data->horde_data.ticks_from_attack = gd->horde_data.ticks_from_attack;
	}
	save_data->seed = gd->seed;
    Uint64 save_len = sizeof(struct Save_data);
    if(!SDL_WriteStorageFile(user, "save", save_data, save_len)){
        exit(-1);
    }
	SDL_free(save_data);
    SDL_CloseStorage(user);
}

void LoadGame(Game_data* const gd){
    SDL_Storage *user = SDL_OpenUserStorage("RzK", "KGame", 0);
    if(user == NULL){
        exit(-1);
    }
    while(!SDL_StorageReady(user)){
        SDL_Delay(1);
    }
    Uint64 save_len = 0U;
    if(SDL_GetStorageFileSize(user, "save", &save_len) && save_len > 0U){
		struct Save_data* save_data = (struct Save_data*)SDL_malloc(save_len);
        if(SDL_ReadStorageFile(user, "save", save_data, save_len)){
            gd->seed = save_data->seed;
			SetGameData(gd);
			gd->champions.num = save_data->nums.champions;
			gd->beings.num = save_data->nums.beings;
			gd->projectiles.num = save_data->nums.projectiles;
			gd->boxes.num = save_data->nums.boxes;
			gd->needed_keys = save_data->nums.needed_keys;
			gd->flags = save_data->flags;
			SDL_memcpy(gd->champions.array, save_data->champions, sizeof(Player) * save_data->nums.champions);
			for(unsigned int i = 0U; i < save_data->nums.champions; ++i){
				(gd->champions.array + i)->segment = GetSegmentUnsafe(&gd->world, (gd->champions.array + i)->position.x, (gd->champions.array + i)->position.y);
				(gd->champions.array + i)->last_seen_in = (gd->champions.array + i)->segment;
			}
			gd->human_indx = save_data->human_indx;
			for(unsigned int i = 0U; i < save_data->nums.beings; ++i){
				*(gd->beings.array + i) = *(save_data->beings + i);
				(gd->beings.array + i)->target.player = human(gd);
				(gd->beings.array + i)->main_indx = i;
				Segment* seg = GetSegmentUnsafe(&gd->world, (gd->beings.array + i)->position.x, (gd->beings.array + i)->position.y);
				if(seg){
					AddBeingToSegment(seg, (gd->beings.array + i), IsAlly(gd->beings.array + i) ? &seg->ally_beings : &seg->beings);
				}
			}
			SDL_memcpy(gd->projectiles.array, save_data->projectiles, sizeof(Projectile) * save_data->nums.projectiles);
			SDL_memcpy(gd->world.plan, save_data->plan, sizeof(Uint64) * BIG_SEGMENTS_X);
			gd->keys_status = (Uint8*)SDL_malloc(sizeof(Uint8) * gd->needed_keys);
			gd->world.key_locations = (Key_location*)SDL_malloc(sizeof(Key_location) * gd->needed_keys);
			SDL_memcpy(gd->world.key_locations, save_data->key_locations, sizeof(Key_location) * save_data->nums.needed_keys);
			SDL_memcpy(gd->world.shops, save_data->shops, sizeof(Shop) * SHOPS_NUM);
			SDL_memcpy(gd->boxes.array, save_data->boxes, sizeof(Box) * save_data->nums.boxes);
			gd->keys = save_data->keys;
			gd->enemy_morale = save_data->enemy_morale;
			SDL_memcpy(gd->keys_status, save_data->keys_status, sizeof(Uint8) * save_data->nums.needed_keys);
			gd->effects_num = save_data->effects_num;
			SDL_memcpy(gd->effects, save_data->effects, sizeof(Lasting_effect) * MAX_GAME_EFFECTS);
			if(gd->flags & gamef_horde_attack){
				SDL_memcpy(gd->horde_data.creation_points, save_data->horde_data.creation_points, sizeof(SDL_FPoint) * HORDE_ATTACK_POINTS);
			}else{
				gd->horde_data.ticks_from_attack = save_data->horde_data.ticks_from_attack;
			}
        }else{
        	exit(-1);
        }
        SDL_free(save_data);
    }else{
        exit(-1);
    }
    SDL_CloseStorage(user);
}