#include <common.h>
#include <event.h>
#include <stdlib.h>

static void NoticeBigSeg(Game_data* const, const unsigned int, const unsigned int);
static void PlayerInUncoveredBigSeg(Game_data* const);
static void EndLoop(SDL_Event* const, Game_data* const, const int);
static int RareEventsService(Game_data* const);
static void DestroyBoxes(Boxes* const);
static void LootBox(Game_data* const, const unsigned int);
static void DestroyBoxInArray(Boxes* const, unsigned int);
static int GetNearbyShopIndx(Game_data* const);
static void EnterShop(Game_data* const, Player* const, const unsigned int);
static int UpdateGame(Game_data* const);
static SDL_FPoint GetBeingCreationPoint(Game_data* const, const float);
static void UpdateEffects(Game_data* const);
static void AddGameEffect(Game_data* const, const Lasting_effect);
static void RemoveGameEffect(Game_data* const, const int);
static void UpdateGameEffects(Game_data* const);
static void UpdateGameEffect(Game_data* const, const int);
static void HordeAttack(Game_data* const, const int);

static inline void NoticeBigSeg(Game_data* const gd, const unsigned int x, const unsigned int y){
	if(IsVoidBigSeg(gd->world.plan, x, y) || IsInNoticedBigSeg(gd->world.plan, x, y)){
		return;
	}
	MarkAsNoticedBigSeg(gd->world.plan, x, y);
	UpdateMap(gd->rend_data_ptr, POPULATED_SEG_RGB, x, y);
}

static inline void PlayerInUncoveredBigSeg(Game_data* const gd){
	const unsigned int seg_x = GetBigSegCoordinate(human(gd)->position.x);
	const unsigned int seg_y = GetBigSegCoordinate(human(gd)->position.y);
	if(!IsInUncoveredBigSeg(gd->world.plan, seg_x, seg_y)){
		UncoverBigSeg(gd->world.plan, seg_x, seg_y);
		NoticeBigSeg(gd, seg_x + 1U, seg_y);
		NoticeBigSeg(gd, seg_x, seg_y + 1U);
		NoticeBigSeg(gd, seg_x - 1U, seg_y);
		NoticeBigSeg(gd, seg_x, seg_y - 1U);
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
	Uint64 now = 0ULL;
	Uint64 timer;
    int state = event_ok;
	Uint64 time = SDL_GetTicksNS();
	Uint64 prev_frame_time = time;
    while(state != event_quit_game){
        timer = SDL_GetTicksNS();
		if(state == event_ok){
			state = EventsService(gd->ev_ptr, human(gd), gd->rend_data_ptr);
		}else if(state == event_manage_scrolls){
			state = ManageScrollsEventsService(gd->ev_ptr, human(gd), gd->rend_data_ptr);
		}else if(state == event_menu){
			state = MenuEventsService(gd);
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
            RenderGame(gd->rend_data_ptr, gd, state);
			SDL_RenderPresent(gd->rend_data_ptr->renderer);
		}
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
	if(gd->projectiles.array == NULL || gd->beings.array == NULL || gd->champions.array == NULL || gd->boxes.array == NULL || gd->beings.indices == NULL || gd->rend_data_ptr->visual_effects.array == NULL){exit(-1);}
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
	SDL_GetMouseState(NULL, &gd->rend_data_ptr->mouse_y);
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

static inline int RarePlayerEventsService(Game_data* const gd, const int check_queue){
	Player* const pc = human(gd);
	if(check_queue == check_0){
		if(SDL_fabsf(pc->position.x - gd->world.portalA.x) < half(DOOR_SIZE) && SDL_fabsf(pc->position.y - gd->world.portalA.y) < half(DOOR_SIZE) && (pc->flags & action)){
			SetPlayerPosition(pc, gd->world.portalB.x, gd->world.portalB.y);
			UpdatePlayerNewSegment(&gd->world, pc);
			pc->flags &= ~(action);
		}else if(SDL_fabsf(pc->position.x - gd->world.portalB.x) < half(DOOR_SIZE) && SDL_fabsf(pc->position.y - gd->world.portalB.y) < half(DOOR_SIZE) && (pc->flags & action)){
			SetPlayerPosition(pc, gd->world.portalA.x, gd->world.portalA.y);
			UpdatePlayerNewSegment(&gd->world, pc);
			pc->flags &= ~(action);
		}
	}else if(check_queue == check_1 && (pc->flags & action)){
		const int shop_indx = GetNearbyShopIndx(gd);
		if(shop_indx != NOT_FOUND){
			StopPlayerActions(pc);
			EnterShop(gd, pc, shop_indx);
			return update_shop;
		}
	}else if(check_queue == check_2 && (pc->flags & action) && gd->boxes.num > 0U){
		const int box_indx = GetNearbyBoxIndx(&gd->boxes, &pc->position, PLAYER_SIZE);
		if(box_indx != NOT_FOUND){
			LootBox(gd, box_indx);
	 		pc->flags &= ~(action);
		}
	}else if(check_queue == check_3){
		if(gd->keys >= gd->needed_keys && SDL_fabsf(pc->position.x - gd->world.door.x) < half(DOOR_SIZE) && SDL_fabsf(pc->position.y - gd->world.door.y) < half(DOOR_SIZE) && (pc->flags & action)){
			pc->flags &= ~(action);
			return update_victory;
		}
		pc->attention_rect.x = pc->position.x - half(ATTENTION_RECT_SIZE);
		pc->attention_rect.y = pc->position.y - half(ATTENTION_RECT_SIZE);
	}else if(check_queue == check_4){
		PlayerInUncoveredBigSeg(gd);
	}
	return update_ok;
}

static int RareEventsService(Game_data* const gd){
	static int check_queue = check_0;
	check_queue = (check_queue + 1) % checks_num;
	if(check_queue != check_5){
		return RarePlayerEventsService(gd, check_queue);
	}else if(!(gd->flags & gamef_horde_attack)){
		if(gd->horde_data.ticks_from_attack > MIN_TICKS_FROM_HORDE + SDL_rand(MAX_TICKS_FROM_HORDE)){
			AddGameEffect(gd, (Lasting_effect){game_effect_horde_attack, HORDE_ATTACK_START_TICKS});
			gd->flags |= gamef_horde_attack;
		}else{
			++gd->horde_data.ticks_from_attack;
		}
	}
	return update_ok;
}

extern inline void AddToBox(Box* const bx, const unsigned int slot, const int type, const unsigned int value){
	(bx->elements + slot)->type = type;
	(bx->elements + slot)->value = value;
}

static void DestroyBoxes(Boxes* const bxs){
	SDL_free(bxs->array);
	bxs->num = 0U;
}

int GetNearbyBoxIndx(const Boxes* const bxs, const SDL_FPoint* const position, const float range){
	int left = 0;
	int right = bxs->num;
	int center;
	int new_center = (right - left) / 2 + left;
	const float max_distance = half(BOX_SIZE + range);
	do{
		center = new_center;
		if(SDL_fabsf(position->x - (bxs->array + center)->location.x) < max_distance){
			do{
				if(SDL_fabsf(position->y - (bxs->array + center)->location.y) < max_distance){
					return center;
				}
				++center;
			}while(center < bxs->num && SDL_fabsf(position->x - (bxs->array + center)->location.x) < max_distance);
			center = new_center - 1;
			while(center >= 0 && SDL_fabsf(position->x - (bxs->array + center)->location.x) < max_distance){
				if(SDL_fabsf(position->y - (bxs->array + center)->location.y) < max_distance){
					return center;
				}
				--center;
			};
			return NOT_FOUND;
		}else if(position->x > (bxs->array + center)->location.x){
			left = center;
		}else{
			right = center;
		}
		new_center = (right - left) / 2 + left;
	}while(new_center != center);
	return NOT_FOUND;
}

static inline void LootBox(Game_data* const gd, const unsigned int box_indx){
	Box_element* elem = (gd->boxes.array + box_indx)->elements;
	int element_type;
	bool empty = true;
	bool found_key = false;
	while(1){
		element_type = elem->type;
		if(element_type == box_scroll){
			if(*(human(gd)->scrolls + elem->value) < MAX_ONE_TYPE_ITEMS){
				++(*(human(gd)->scrolls + elem->value));
				elem->type = box_clear;
			}else{
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
			AddSpellVisualEffect(&gd->rend_data_ptr->visual_effects, &(gd->boxes.array + box_indx)->location, SPELL0_RGB);
		}else if(element_type == box_map){
			if(*(gd->keys_status + elem->value) == key_location_unknown){
				*(gd->keys_status + elem->value) = key_location_known;
			}
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
			return NOT_FOUND;
		}else if(human(gd)->position.x > (gd->world.shops + center)->location.x){
			left = center;
		}else{
			right = center;
		}
		new_center = (right - left) / 2 + left;
	}while(new_center != center);
	return NOT_FOUND;
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

struct transaction_data{
	Uint8 items_to_sell[MAX_ITEMS_TO_SELL];
	unsigned int items_to_sell_num;
	Uint8 items_to_get[MAX_ITEMS_TO_SELL];
	unsigned int items_to_get_num;
	int profit;
	Player* const pc;
	Shop* const shop;
}transaction_data;

static inline bool SelectedPlayerItem(const unsigned int col, const unsigned int row, struct transaction_data* const td){
	if(col < SHOP_SIDE_COLS && row >= FIRST_SHOP_ROW){
		if(td->items_to_sell_num < MAX_ITEMS_TO_SELL){
			const unsigned int item_num = col + (row - FIRST_SHOP_ROW) * SHOP_SIDE_COLS;
			if(item_num < scroll_empty && *(td->pc->scrolls + item_num) > 0U){
				*(td->items_to_sell + (td->items_to_sell_num)++) = item_num;
				--(*(td->pc->scrolls + item_num));
				td->profit += ItemPrice(item_num) / SELL_DIVIDER;
			}
		}
		return true;
	}
	return false;
}

static inline bool SelectedFinalize(const unsigned int row, struct transaction_data* const td, Game_data* const gd){
	if(row == SHOP_ROWS - 1U){
		if(td->pc->coins >= -(td->profit)){
			td->items_to_sell_num = 0U;
			for(unsigned int i = 0U; i < td->items_to_get_num; ++i){
				const unsigned int item_num = *(td->items_to_get + i);
				if(item_num == shop_item_mp10){
					td->pc->magic_points += 10;
				}else if(item_num == shop_item_mp100){
					td->pc->magic_points += 100;
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
					td->pc->max_armour = (Armour)PC_MAX_ARMOUR_II;
				}else if(item_num == shop_item_dodge_speed){
					td->pc->dodge_velocity_multipl = PC_DODGE_VELOCITY * 2.0F / PLAYER_VELOCITY;
				}
			}
			td->items_to_get_num = 0U;
			td->pc->coins += td->profit;
			td->profit = 0;
		}
		return true;
	}
	return false;
}

static inline bool SelectedReturnToPlayer(const unsigned int col, const unsigned int row, struct transaction_data* const td){
	if(col == SHOP_SIDE_COLS + 1U){
		const unsigned int item_num = *(td->items_to_sell + row);
		if(td->items_to_sell_num > row && *(td->pc->scrolls + item_num) < MAX_ONE_TYPE_ITEMS){
			++(*(td->pc->scrolls + item_num));
			*(td->items_to_sell + row) = *(td->items_to_sell + --(td->items_to_sell_num));
			td->profit -= ItemPrice(item_num) / SELL_DIVIDER;
		}
		return true;
	}
	return false;
}

static inline bool SelectedReturnToShop(const unsigned int col, const unsigned int row, struct transaction_data* const td){
	if(col == SHOP_SIDE_COLS + 3U){
		const unsigned int item_num = *(td->items_to_get + row);
		if(td->items_to_get_num > row){
			*(td->items_to_get + row) = *(td->items_to_get + --(td->items_to_get_num));
			td->profit += ItemPrice(item_num);
			if(item_num < shop_item_invalid){
				--(*(td->pc->scrolls + item_num));
				AddScrollToShop(td->shop, item_num);
			}
		}
		return true;
	}
	return false;
}

static inline bool SelectedShopScroll(const unsigned int col, const unsigned int row, struct transaction_data* const td){
	if(col > SHOP_COLS - SHOP_SCROLLS_COLS - 2U){
		const unsigned int indx = col - (SHOP_COLS - SHOP_SCROLLS_COLS - 1U) + (row - FIRST_SHOP_ROW - 1U) * SHOP_SCROLLS_COLS;
		const unsigned int item_num = *(td->shop->scrolls + indx);
		if(item_num < scroll_empty && *(td->pc->scrolls + item_num) < MAX_ONE_TYPE_ITEMS){
			*(td->items_to_get + (td->items_to_get_num)++) = item_num;
			td->profit -= ItemPrice(item_num);
			*(td->shop->scrolls + indx) = scroll_empty;
			++(*(td->pc->scrolls + item_num));
		}
		return true;
	}
	return false;
}

static inline bool SelectedMagicOrKey(const unsigned int col, const unsigned int row, struct transaction_data* const td, Game_data* const gd){
	if(row == FIRST_SHOP_ROW + 1U){
		if(col == SHOP_COLS - SHOP_SIDE_COLS - 1U){
			*(td->items_to_get + (td->items_to_get_num)++) = shop_item_mp10;
			td->profit -= ItemPrice(shop_item_mp10);
		}else if(col == SHOP_COLS - SHOP_SIDE_COLS && gd->keys < gd->needed_keys && ItemNotChosen(shop_item_key, td->items_to_get, td->items_to_get_num)){
			*(td->items_to_get + (td->items_to_get_num)++) = shop_item_key;
			td->profit -= ItemPrice(shop_item_key);
		}else if(col == SHOP_COLS - SHOP_SIDE_COLS + 1U && ItemNotChosen(shop_item_key_location, td->items_to_get, td->items_to_get_num) && gd->keys < gd->needed_keys && IsUnknowKey(gd)){
			*(td->items_to_get + (td->items_to_get_num)++) = shop_item_key_location;
			td->profit -= ItemPrice(shop_item_key_location);
		}
		return true;
	}else if(row == FIRST_SHOP_ROW + 2U && col == SHOP_COLS - SHOP_SIDE_COLS - 1U){
		*(td->items_to_get + (td->items_to_get_num)++) = shop_item_mp100;
		td->profit -= ItemPrice(shop_item_mp100);
		return true;
	}
	return false;
}

static inline bool SelectedUpgrade(const unsigned int col, const unsigned int row, struct transaction_data* const td){
	if(row == FIRST_SHOP_ROW + 5U){
		if(col == SHOP_COLS - SHOP_SIDE_COLS - 1U && td->pc->max_armour.absorption < PC_MAX_ARMOUR_II_ABS && ItemNotChosen(shop_item_armour, td->items_to_get, td->items_to_get_num)){
			*(td->items_to_get + (td->items_to_get_num)++) = shop_item_armour;
			td->profit -= ItemPrice(shop_item_armour);
		}else if(col == SHOP_COLS - SHOP_SIDE_COLS && td->pc->dodge_velocity_multipl < PC_DODGE_VELOCITY * 2.0F / PLAYER_VELOCITY && ItemNotChosen(shop_item_dodge_speed, td->items_to_get, td->items_to_get_num)){
			*(td->items_to_get + (td->items_to_get_num)++) = shop_item_dodge_speed;
			td->profit -= ItemPrice(shop_item_dodge_speed);
		}
		return true;
	}
	return false;
}

static inline void ExitShop(struct transaction_data* const td, Game_data* const gd){
	for(unsigned int i = 0U; i < td->items_to_sell_num; ++i){
		const unsigned int item_num = *(td->items_to_sell + i);
		++(*(td->pc->scrolls + item_num));
	}
	for(unsigned int i = 0U; i < td->items_to_get_num; ++i){
		const unsigned int item_num = *(td->items_to_get + i);
		if(item_num < shop_item_invalid){
			--(*(td->pc->scrolls + item_num));
			AddScrollToShop(td->shop, item_num);
		}
	}
	Render_data* const rd = gd->rend_data_ptr;
	SDL_WarpMouseInWindow(rd->window, half(rd->window_w), half(rd->window_h));
	SDL_SetWindowRelativeMouseMode(rd->window, true);
	SetMouseBarrier(rd);
	rd->mouse_y = half(rd->window_h);
}

static void EnterShop(Game_data* const gd, Player* const pc, const unsigned int shop_indx){
	Render_data* const rend_data = gd->rend_data_ptr;
	SDL_SetWindowRelativeMouseMode(rend_data->window, false);
	SDL_SetWindowMouseRect(rend_data->window, NULL);
    int option = opt_void;
	struct transaction_data td = {
		.items_to_sell_num = 0U,
		.items_to_get_num = 0U,
		.profit = 0,
		.pc = pc,
		.shop = gd->world.shops + shop_indx
	};
	pc->help_data.menu_position = SHOP_POSITIONS;
	SDL_SetTextureAlphaMod(*(rend_data->textures + tx_icons), 255);
    while(option != opt_exit){
        RenderShop(rend_data, pc, td.shop, td.items_to_sell, td.items_to_sell_num, td.items_to_get, td.items_to_get_num, td.profit);
        if((option = ShopEventsService(gd->ev_ptr, pc, rend_data)) > opt_void && pc->help_data.menu_position < SHOP_POSITIONS){
			const unsigned int col = pc->help_data.menu_position % SHOP_COLS;
			const unsigned int row = pc->help_data.menu_position / SHOP_COLS;
			if(SelectedPlayerItem(col, row, &td)){
			}else if(SelectedFinalize(row, &td, gd)){
			}else if(SelectedReturnToPlayer(col, row, &td)){
			}else if(SelectedReturnToShop(col, row, &td)){
			}else if(td.items_to_get_num < MAX_ITEMS_TO_SELL){
				if(SelectedShopScroll(col, row, &td)){
				}else if(SelectedMagicOrKey(col, row, &td, gd)){
				}else if(SelectedUpgrade(col, row, &td)){}
			}
		}
        SDL_Delay(FRAME_TIME_MS);
    }
	ExitShop(&td, gd);
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
		return event_ok;
	}else if(option == menu_p_load){
		ClearGameData(gd);
		LoadGame(gd);
		return event_ok;
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
//---------------------------------------------------------------------------------------------------------------
extern inline int HasEffect(Lasting_effect* const efs, const unsigned int efs_num, const unsigned int effect_id){
    for(unsigned int i = 0U; i < efs_num; ++i){
        if((efs + i)->id == effect_id){
            return i;
        }
    }
    return NOT_FOUND;
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

static void HordeAttack(Game_data* const gd, const int ticks_left){
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
	}else if((ticks_left < (HORDE_ATTACK_START_TICKS / 2) && !SDL_rand((HORDE_ATTACK_START_TICKS - ticks_left) / HORDE_BEING_CHANCE_FACTOR))
		 || (ticks_left >= (HORDE_ATTACK_START_TICKS / 2) && !SDL_rand(ticks_left / HORDE_BEING_CHANCE_FACTOR))){
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

struct nums{
	Uint8 champions;
	Uint16 beings;
	Uint16 projectiles;
	Uint16 boxes;
	Uint8 needed_keys;
	Uint8 effects;
	Uint16 known_segs;
}nums;

void SaveGame(const Game_data* const gd){
	struct coordinates known_segs[MAX_NOT_NULL_SEGS];
	unsigned int known_segs_num = 0U;
	for(unsigned int bigc = 0U; bigc < BIG_SEGMENTS_X; ++bigc){
		for(unsigned int bigr = 0U; bigr < BIG_SEGMENTS_X; ++bigr){
			if(IsInNoticedBigSeg(gd->world.plan, bigc, bigr)){
				for(unsigned int c = bigc * BIG_SEGMENT_SEGMENTS_X; c < bigc * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X; ++c){
					for(unsigned int r = bigr * BIG_SEGMENT_SEGMENTS_X; r < bigr * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X; ++r){
						const Segment* const seg = GetSegmentByIndxUnsafe(&gd->world, c, r);
						if(seg && (seg->flags & segment_known)){
							*(known_segs + known_segs_num++) = (struct coordinates){c, r};
						}
					}
				}
			}
		}
	}
	struct nums nums = {
		gd->champions.num,
		gd->beings.num,
		gd->projectiles.num,
		gd->boxes.num,
		gd->needed_keys,
		gd->effects_num,
		known_segs_num
	};
    Uint64 save_len = SAVE_LEN(nums);
	void* const save_data = (void*)SDL_malloc(save_len);
	void* ptr = save_data;
	*(Uint64*)ptr = gd->seed;
	ptr += sizeof(Uint64);
	*(struct nums*)ptr = nums;
	ptr += sizeof(nums);
	*(unsigned int*)ptr = gd->flags;
	ptr += sizeof(unsigned int);
	for(unsigned int i = 0U; i < nums.champions; ++i){
		StopPlayerActions(gd->champions.array + i);
	}
	SDL_memcpy(ptr, gd->champions.array, sizeof(Player) * nums.champions);
	ptr += sizeof(Player) * nums.champions;
	*(Uint8*)ptr = gd->human_indx;
	ptr += sizeof(Uint8);
	for(unsigned int i = 0U; i < nums.beings; ++i){
		*(Being*)ptr = *(gd->beings.array + *(gd->beings.indices + i));
		ptr += sizeof(Being);
	}
	SDL_memcpy(ptr, gd->projectiles.array, sizeof(Projectile) * nums.projectiles);
	ptr += sizeof(Projectile) * nums.projectiles;
	SDL_memcpy(ptr, gd->world.plan, sizeof(Uint64) * BIG_SEGMENTS_X);
	ptr += sizeof(Uint64) * BIG_SEGMENTS_X;
	SDL_memcpy(ptr, gd->world.key_locations, sizeof(Key_location) *nums.needed_keys);
	ptr += sizeof(Key_location) *nums.needed_keys;
	SDL_memcpy(ptr, gd->world.shops, sizeof(Shop) * SHOPS_NUM);
	ptr += sizeof(Shop) * SHOPS_NUM;
	SDL_memcpy(ptr, gd->boxes.array, sizeof(Box) * nums.boxes);
	ptr += sizeof(Box) * nums.boxes;
	*(Uint8*)ptr = gd->keys;
	ptr += sizeof(Uint8);
	*(int*)ptr = gd->enemy_morale;
	ptr += sizeof(int);
	SDL_memcpy(ptr, gd->keys_status, sizeof(Uint8) * nums.needed_keys);
	ptr += sizeof(Uint8) * nums.needed_keys;
	SDL_memcpy(ptr, gd->effects, sizeof(Lasting_effect) * nums.effects);
	ptr += sizeof(Lasting_effect) * nums.effects;
	if(gd->flags & gamef_horde_attack){
		SDL_memcpy(ptr, gd->horde_data.creation_points, sizeof(SDL_FPoint) * HORDE_ATTACK_POINTS);
	}else{
		*(unsigned int*)ptr = gd->horde_data.ticks_from_attack;
	}
	ptr += sizeof(union horde_data);
	SDL_memcpy(ptr, known_segs, sizeof(struct coordinates) * nums.known_segs);
    SDL_Storage* user = SDL_OpenUserStorage("RzK", "KGame", 0);
    if(!user){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_OpenUserStorage error: %s", SDL_GetError()); exit(-1);
    }
    while(!SDL_StorageReady(user)){
        SDL_Delay(1);
    }
    if(!SDL_WriteStorageFile(user, "save", save_data, save_len)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_WriteStorageFile error: %s", SDL_GetError()); exit(-1);
    }
	SDL_free(save_data);
    SDL_CloseStorage(user);
}

void LoadGame(Game_data* const gd){
    SDL_Storage *user = SDL_OpenUserStorage("RzK", "KGame", 0);
    if(!user){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_OpenUserStorage error: %s", SDL_GetError()); exit(-1);
    }
    while(!SDL_StorageReady(user)){
        SDL_Delay(1);
    }
    Uint64 save_len = 0U;
	if(!SDL_GetStorageFileSize(user, "save", &save_len)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GetStorageFileSize error: %s", SDL_GetError()); exit(-1);
	}
    if(save_len == 0U){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid save file size"); exit(-1);
	}
	void* const save_data = (void*)SDL_malloc(save_len);
	if(!SDL_ReadStorageFile(user, "save", save_data, save_len)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_ReadStorageFile error: %s", SDL_GetError()); exit(-1);
	}
    SDL_CloseStorage(user);
	void* ptr = save_data;
	gd->seed = *(Uint64*)ptr;
	ptr += sizeof(Uint64);
	SetGameData(gd);
	struct nums nums = *(struct nums*)ptr;
	ptr += sizeof(struct nums);
	gd->champions.num = nums.champions;
	gd->beings.num = nums.beings;
	gd->projectiles.num = nums.projectiles;
	gd->boxes.num = nums.boxes;
	gd->needed_keys = nums.needed_keys;
	gd->effects_num = nums.effects;
	gd->flags = *(unsigned int*)ptr;
	ptr += sizeof(unsigned int);
	SDL_memcpy(gd->champions.array, ptr, sizeof(Player) * nums.champions);
	ptr += sizeof(Player) * nums.champions;
	for(unsigned int i = 0U; i < nums.champions; ++i){
		(gd->champions.array + i)->segment = GetSegmentUnsafe(&gd->world, (gd->champions.array + i)->position.x, (gd->champions.array + i)->position.y);
		(gd->champions.array + i)->last_seen_in = (gd->champions.array + i)->segment;
	}
	gd->human_indx = *(Uint8*)ptr;
	ptr += sizeof(Uint8);
	for(unsigned int i = 0U; i < nums.beings; ++i){
		*(gd->beings.array + i) = *(Being*)ptr;
		(gd->beings.array + i)->target.player = human(gd);
		(gd->beings.array + i)->main_indx = i;
		Segment* seg = GetSegmentUnsafe(&gd->world, (gd->beings.array + i)->position.x, (gd->beings.array + i)->position.y);
		if(seg){
			AddBeingToSegment(seg, (gd->beings.array + i), IsAlly(gd->beings.array + i) ? &seg->ally_beings : &seg->beings);
		}
		ptr += sizeof(Being);
	}
	SDL_memcpy(gd->projectiles.array, ptr, sizeof(Projectile) * nums.projectiles);
	ptr += sizeof(Projectile) * nums.projectiles;
	SDL_memcpy(gd->world.plan, ptr, sizeof(Uint64) * BIG_SEGMENTS_X);
	ptr += sizeof(Uint64) * BIG_SEGMENTS_X;
	gd->keys_status = (Uint8*)SDL_malloc(sizeof(Uint8) * gd->needed_keys);
	gd->world.key_locations = (Key_location*)SDL_malloc(sizeof(Key_location) * gd->needed_keys);
	SDL_memcpy(gd->world.key_locations, ptr, sizeof(Key_location) * nums.needed_keys);
	ptr += sizeof(Key_location) * nums.needed_keys;
	SDL_memcpy(gd->world.shops, ptr, sizeof(Shop) * SHOPS_NUM);
	ptr += sizeof(Shop) * SHOPS_NUM;
	SDL_memcpy(gd->boxes.array, ptr, sizeof(Box) * nums.boxes);
	ptr += sizeof(Box) * nums.boxes;
	gd->keys = *(Uint8*)ptr;
	ptr += sizeof(Uint8);
	gd->enemy_morale = *(int*)ptr;
	ptr += sizeof(int);
	SDL_memcpy(gd->keys_status, ptr, sizeof(Uint8) * nums.needed_keys);
	ptr += sizeof(Uint8) * nums.needed_keys;
	SDL_memcpy(gd->effects, ptr, sizeof(Lasting_effect) * nums.effects);
	ptr += sizeof(Lasting_effect) * nums.effects;
	if(gd->flags & gamef_horde_attack){
		SDL_memcpy(gd->horde_data.creation_points, ptr, sizeof(SDL_FPoint) * HORDE_ATTACK_POINTS);
	}else{
		gd->horde_data.ticks_from_attack = *(unsigned int*)ptr;
	}
	ptr += sizeof(union horde_data);
	for(unsigned int i = 0U; i < nums.known_segs; ++i){
		GetSegmentByIndxUnsafe(&gd->world, ((struct coordinates*)ptr)->x, ((struct coordinates*)ptr)->y)->flags |= segment_known;
		ptr += sizeof(struct coordinates);
	}
	SDL_free(save_data);
	SDL_GetMouseState(NULL, &gd->rend_data_ptr->mouse_y);
	DrawMap(gd->rend_data_ptr, &gd->world);
}
