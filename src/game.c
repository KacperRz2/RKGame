#include <common.h>
#include <event.h>
#include <function.h>
#include <stdlib.h>
#include <enet/enet.h>

static void NoticeBigSeg(Game_data* const, const unsigned int, const unsigned int);
static void PlayerInUncoveredBigSeg(Game_data* const, const float, const float);
static void EndLoop(SDL_Event* const, Render_data* const, const int);
static int RareEventsService(Game_data* const);
static void DestroyBoxes(Boxes* const);
static void LootBox(Game_data* const, Player *const, const unsigned int);
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
static int ActivateMenuOption(const unsigned int, Game_data* const);

enum menu_in_game_option{
	menu_ig_settings,
	menu_ig_load,
	menu_ig_continue,
	menu_ig_save,
	menu_ig_quit,
	menu_ig_unknown,
	menu_ig_escape
};
enum settings_option{
	settings_fullscreen,
	settings_quit,
	settings_unknown,
	settings_escape
};
enum new_game_option{
	new_game_single,
	new_game_multipl,
	new_game_quit,
	new_game_unknown,
	new_game_escape
};
enum multipl_option{
	multipl_host,
	multipl_join,
	multipl_quit,
	multipl_unknown,
	multipl_escape
};

static inline void NoticeBigSeg(Game_data* const gd, const unsigned int x, const unsigned int y){
	if(IsVoidBigSeg(gd->world.plan, x, y) || IsInNoticedBigSeg(gd->world.plan, x, y)){
		return;
	}
	MarkAsNoticedBigSeg(gd->world.plan, x, y);
	UpdateMap(gd->rend_data_ptr, POPULATED_SEG_RGB, x, y);
}

static inline void PlayerInUncoveredBigSeg(Game_data* const gd, const float px, const float py){
	const unsigned int seg_x = GetBigSegCoordinate(px);
	const unsigned int seg_y = GetBigSegCoordinate(py);
	if(!IsInUncoveredBigSeg(gd->world.plan, seg_x, seg_y)){
		UncoverBigSeg(gd->world.plan, seg_x, seg_y);
		NoticeBigSeg(gd, seg_x + 1U, seg_y);
		NoticeBigSeg(gd, seg_x, seg_y + 1U);
		NoticeBigSeg(gd, seg_x - 1U, seg_y);
		NoticeBigSeg(gd, seg_x, seg_y - 1U);
		UpdateMap(gd->rend_data_ptr, UNCOVERED_SEG_RGB, seg_x, seg_y);
	}
}

static void SettingsMenuLoop(SDL_Event* const ev, Render_data* const rend_data){
	unsigned int option = settings_unknown;
	unsigned int menu_position = settings_fullscreen;
	while(1){
		RenderSettingsMenu(rend_data, menu_position);
		option = MenuEventsService(ev, rend_data, &menu_position, settings_unknown);
		if(settings_fullscreen == option){
			ToggleFullscreen(rend_data);
			option = settings_unknown;
		}
		if(option != settings_unknown) break;
		SDL_Delay(FRAME_TIME_MS);
	}
}

static int MultiplayerMenuLoop(SDL_Event* const ev, Render_data* const rend_data){
	unsigned int option = multipl_unknown;
	unsigned int menu_position = multipl_host;
	while(1){
		RenderMultiplayerMenu(rend_data, menu_position);
		option = MenuEventsService(ev, rend_data, &menu_position, multipl_unknown);
		if(multipl_host == option){
			return menu_multipl_host;
		}else if(multipl_join == option){
			return menu_multipl;
		}
		if(option != multipl_unknown) break;
		SDL_Delay(FRAME_TIME_MS);
	}
	return menu_unknown;
}

static int NewGameMenuLoop(SDL_Event* const ev, Render_data* const rend_data){
	unsigned int option = new_game_unknown;
	unsigned int menu_position = new_game_single;
	while(1){
		RenderNewGameMenu(rend_data, menu_position);
		option = MenuEventsService(ev, rend_data, &menu_position, new_game_unknown);
		if(new_game_single == option){
			return menu_start;
		}else if(new_game_multipl == option){
			return MultiplayerMenuLoop(ev, rend_data);
		}
		if(option != new_game_unknown) break;
		SDL_Delay(FRAME_TIME_MS);
	}
	return menu_unknown;
}

int MainMenuLoop(SDL_Event* const ev, Render_data* const rend_data){
    unsigned int option = menu_unknown;
	unsigned int menu_position = menu_start;
    while(1){
        RenderMainMenu(rend_data, menu_position);
		option = MenuEventsService(ev, rend_data, &menu_position, OPTIONS_NUM);
		if(menu_settings == option){
			SettingsMenuLoop(ev, rend_data);
			option = menu_unknown;
		}else if(menu_start == option){
			return NewGameMenuLoop(ev, rend_data);
		}
		if(option < menu_unknown) break;
		SDL_Delay(FRAME_TIME_MS);
	}
    return option;
}

static inline void ResetTime(Uint64 *const time, Uint64 *const prev_frame_time){
	*time = SDL_GetTicksNS();
	*prev_frame_time = *time;
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
			state = EventsService(gd->ev_ptr, host(gd), gd->rend_data_ptr);
		}else if(state == event_manage_scrolls){
			state = ManageScrollsEventsService(gd->ev_ptr, host(gd), gd->rend_data_ptr);
		}else if(state == event_menu){
			unsigned int option = MenuEventsService(gd->ev_ptr, gd->rend_data_ptr, &host(gd)->help_data.menu_position, menu_ig_unknown);
			if(menu_ig_unknown != option){
				if(menu_ig_escape == option){
					option = menu_ig_continue;
				}
				state = ActivateMenuOption(option, gd);
			}
		}else if(event_used_pause == state){
			ResetTime(&time, &prev_frame_time);
			state = event_ok;
		}
		const int update_result = UpdateGame(gd);
		if(update_result != update_ok){
			if(update_result == update_shop){
				ResetTime(&time, &prev_frame_time);
			}else{
				EndLoop(gd->ev_ptr, gd->rend_data_ptr, update_result);
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

static void MessageScreenLoop(SDL_Event* const ev, Render_data *const rend_data, void (*RendFunc)(Render_data *const)){
	for(unsigned int i = 0U; i < 50U; ++i){
		RendFunc(rend_data);
		SDL_Delay(40U);
	}
	SDL_PumpEvents();
	SDL_FlushEvent(SDL_EVENT_KEY_UP);
	SDL_FlushEvent(SDL_EVENT_MOUSE_BUTTON_UP);
	while(1){
		if(EndingEventsService(ev)){
			break;
		}
		RendFunc(rend_data);
		SDL_Delay(FRAME_TIME_MS);
	}
}

static void EndLoop(SDL_Event* const ev, Render_data *const rend_data, const int result){
	if(result == update_defeated){
		MessageScreenLoop(ev, rend_data, RenderDefeatedScreen);
	}else{
		MessageScreenLoop(ev, rend_data, RenderVictoryScreen);
	}
}

void CreditsLoop(SDL_Event* const ev, Render_data *const rend_data){
	SDL_SetRenderViewport(rend_data->renderer, NULL);
	MessageScreenLoop(ev, rend_data, RenderCreditsScreen);
}

void SetGameData(Game_data* const gd){
	gd->flags = 0x0U;
	gd->champions.array = (Player*)SDL_malloc(sizeof(Player) * MAX_PLAYERS_NUM);
	gd->boxes.array = (Box*)SDL_malloc(sizeof(Box) * BOXES_NUM);
	gd->champions.num = 0U;
	gd->boxes.num = 0U;
	gd->host_indx = 0U;
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
	gd->announcements.unimportant = NULL;
	gd->announcements.important = NULL;
	gd->announcements.unimportant_num = 0U;
	gd->announcements.important_num = 0U;
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
	SDL_free(gd->announcements.unimportant);
	SDL_free(gd->announcements.important);
	gd->announcements.unimportant_num = 0U;
	gd->announcements.important_num = 0U;
	DestroyWorld(&gd->world);
}

static inline int RarePlayerEventsService(Game_data* const gd, const int check_queue){
	Player* const pc = host(gd);
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
			LootBox(gd, pc, box_indx);
	 		pc->flags &= ~(action);
		}
	}else if(check_queue == check_3){
		if(gd->keys >= gd->needed_keys && SDL_fabsf(pc->position.x - gd->world.door.x) < half(DOOR_SIZE) && SDL_fabsf(pc->position.y - gd->world.door.y) < half(DOOR_SIZE) && (pc->flags & action)){
			pc->flags &= ~(action);
			return update_victory;
		}
		for(unsigned int i = 0U; i < gd->champions.num; ++i){
			(gd->champions.array + i)->attention_rect.x = (gd->champions.array + i)->position.x - half(ATTENTION_RECT_SIZE);
			(gd->champions.array + i)->attention_rect.y = (gd->champions.array + i)->position.y - half(ATTENTION_RECT_SIZE);
		}
	}else if(check_queue == check_4){
		PlayerInUncoveredBigSeg(gd, pc->position.x, pc->position.y);
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

static inline void SetKeyStatus(Game_data *const gd, const Uint16 indx, const Uint8 status){
	*(gd->keys_status + indx) = status;
	struct tmps{Uint32 id; Uint32 status;};
	AddAnnouncement(gd, annncmnt_update_key, &(struct tmps){indx, status});
}

static inline void LootBox(Game_data* const gd, Player *const pc, const unsigned int box_indx){
	Box_element* elem = (gd->boxes.array + box_indx)->elements;
	int element_type;
	bool empty = true;
	bool found_key = false;
	while(1){
		element_type = elem->type;
		if(element_type == box_scroll){
			if(*(pc->scrolls + elem->value) < MAX_ONE_TYPE_ITEMS){
				++(*(pc->scrolls + elem->value));
				if(pc == gd->champions.array + 1){
					struct tmps{Uint32 id; Uint32 num;};
					AddAnnouncement(gd, annncmnt_update_scroll, &(struct tmps){elem->value, *(pc->scrolls + elem->value)});
				}
				elem->type = box_clear;
			}else{
				empty = false;
			}
		}else if(element_type == box_coins){
			pc->coins += (int)elem->value;
			elem->type = box_clear;
		}else if(element_type == box_key){
			found_key = true;
			++gd->keys;
			// *(gd->keys_status + elem->value) = key_owned;
			SetKeyStatus(gd, elem->value, key_owned);
			elem->type = box_clear;
			AddSpellVisualEffect(gd, &(gd->boxes.array + box_indx)->location, SPELL0_RGB);
		}else if(element_type == box_map){
			if(*(gd->keys_status + elem->value) == key_location_unknown){
				SetKeyStatus(gd, elem->value, key_location_known);
				// *(gd->keys_status + elem->value) = key_location_known;
			}
			elem->type = box_clear;
		}else if(element_type == box_mp){
			pc->magic_points += (int)elem->value;
			elem->value = 0U;
			break;
		}
		++elem;
	}
	if(empty){
		DestroyBoxInArray(&gd->boxes, box_indx);
		AddAnnouncement(gd, annncmnt_destroy_box, &(Uint64){box_indx});
	}
	if(found_key){
		if(gd->flags & gamef_multiplayer){
			SaveGame(gd, SAVE_PATHMP);
		}else{
			SaveGame(gd, SAVE_PATH0);
		}
	}
	if(pc == gd->champions.array + 1){
		struct tmps{int mp; int coin;};
		AddAnnouncement(gd, annncmnt_update_mp_and_coin, &(struct tmps){pc->magic_points, pc->coins});
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
		if(SDL_fabsf(host(gd)->position.x - (gd->world.shops + center)->location.x) < SHOP_ENTRY_RANGE){
			do{
				if(SDL_fabsf(host(gd)->position.y - (gd->world.shops + center)->location.y) < SHOP_ENTRY_RANGE){
					return center;
				}
				++center;
			}while(center < SHOPS_NUM && SDL_fabsf(host(gd)->position.x - (gd->world.shops + center)->location.x) < SHOP_ENTRY_RANGE);
			center = new_center - 1;
			while(center >= 0 && SDL_fabsf(host(gd)->position.x - (gd->world.shops + center)->location.x) < SHOP_ENTRY_RANGE){
				if(SDL_fabsf(host(gd)->position.y - (gd->world.shops + center)->location.y) < SHOP_ENTRY_RANGE){
					return center;
				}
				--center;
			};
			return NOT_FOUND;
		}else if(host(gd)->position.x > (gd->world.shops + center)->location.x){
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
						SetKeyStatus(gd, key_indx, key_location_known);
						// *(gd->keys_status + key_indx) = key_location_known;
					}else{
						unsigned int key_indx1 = (key_indx + 1U) % gd->needed_keys;
						do{
							if(*(gd->keys_status + key_indx1) == key_location_unknown){
								// *(gd->keys_status + key_indx1) = key_location_known;
								SetKeyStatus(gd, key_indx1, key_location_known);
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
	if(host(gd)->hit_points < 1){
		return update_defeated;
	}
	return update_ok;
}

static int ActivateMenuOption(const unsigned int option, Game_data* const gd){
	if(option == menu_ig_continue){
		SDL_SetWindowRelativeMouseMode(gd->rend_data_ptr->window, true);
		return event_ok;
	}else if(option == menu_ig_load){
		if(gd->flags & gamef_multiplayer){
			if(gd->flags & gamef_is_serv_host){
				ClearGameData(gd);
				LoadGame(gd, SAVE_PATHMP);
			}else{
				SDL_SetWindowRelativeMouseMode(gd->rend_data_ptr->window, true);
			}
		}else{
			ClearGameData(gd);
			LoadGame(gd, SAVE_PATH0);
		}
		return event_ok;
	}else if(option == menu_ig_save){
		if(gd->flags & gamef_multiplayer){
			SaveGame(gd, SAVE_PATHMP);
		}else{
			SaveGame(gd, SAVE_PATH0);
		}
		return event_quit_game;
	}else if(option == menu_ig_quit){
		return event_quit_game;
	}else if(option == menu_ig_settings){
		SettingsMenuLoop(gd->ev_ptr, gd->rend_data_ptr);
		SDL_SetWindowRelativeMouseMode(gd->rend_data_ptr->window, true);
		return event_used_pause;
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
	SDL_FPoint control_point_old = host(gd)->position;
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
    void (*const effect[])(Game_data* const, const int) = GAME_LASTING_EFFECTS;
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
			if(TryCreateHostileBeing(gd, GetRandomBeingId(), (gd->horde_data.creation_points + point_indx1)->x, (gd->horde_data.creation_points + point_indx1)->y, host(gd))){
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

void SaveGame(const Game_data* const gd, const char *const path){
	if((gd->flags & (gamef_multiplayer | gamef_is_serv_host)) == gamef_multiplayer){
		return;
	}
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
	*(Uint32*)ptr = gd->flags;
	ptr += sizeof(unsigned int);
	for(unsigned int i = 0U; i < nums.champions; ++i){
		StopPlayerActions(gd->champions.array + i);
	}
	SDL_memcpy(ptr, gd->champions.array, sizeof(Player) * nums.champions);
	ptr += sizeof(Player) * nums.champions;
	*(Uint8*)ptr = gd->host_indx;
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
    if(!SDL_WriteStorageFile(user, path, save_data, save_len)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_WriteStorageFile error: %s", SDL_GetError()); exit(-1);
    }
	SDL_free(save_data);
    SDL_CloseStorage(user);
}

void LoadGame(Game_data* const gd, const char *const path){
    SDL_Storage *user = SDL_OpenUserStorage("RzK", "KGame", 0);
    if(!user){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_OpenUserStorage error: %s", SDL_GetError()); exit(-1);
    }
    while(!SDL_StorageReady(user)){
        SDL_Delay(1);
    }
    Uint64 save_len = 0U;
	if(!SDL_GetStorageFileSize(user, path, &save_len)){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GetStorageFileSize error: %s", SDL_GetError()); exit(-1);
	}
    if(save_len == 0U){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid save file size"); exit(-1);
	}
	void* const save_data = (void*)SDL_malloc(save_len);
	if(!SDL_ReadStorageFile(user, path, save_data, save_len)){
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
	gd->flags = *(Uint32*)ptr;
	ptr += sizeof(Uint32);
	SDL_memcpy(gd->champions.array, ptr, sizeof(Player) * nums.champions);
	ptr += sizeof(Player) * nums.champions;
	for(unsigned int i = 0U; i < nums.champions; ++i){
		(gd->champions.array + i)->segment = GetSegmentUnsafe(&gd->world, (gd->champions.array + i)->position.x, (gd->champions.array + i)->position.y);
		(gd->champions.array + i)->last_seen_in = (gd->champions.array + i)->segment;
	}
	gd->host_indx = *(Uint8*)ptr;
	ptr += sizeof(Uint8);
	for(unsigned int i = 0U; i < nums.beings; ++i){
		*(gd->beings.array + i) = *(Being*)ptr;
		(gd->beings.array + i)->target.player = host(gd);
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
	if(gd->flags & gamef_is_serv_host){
		gd->announcements.unimportant = (Announcement*)SDL_malloc(sizeof(Announcement) * MAX_ANNOUNCEMENTS);
		gd->announcements.important = (Announcement*)SDL_malloc(sizeof(Announcement) * MAX_ANNOUNCEMENTS);
		AddAnnouncement(gd, annncmnt_load, &(gd->champions.array + 1)->position);
	}
}
//=======================================================================================================================================
struct pckt_nums{
	Uint16 beings;
	Uint16 projectiles;
	Uint8 announcements;
};

struct Being_pack_data{
	union{
		SDL_FPoint shift;
		int status_ticks_left;
	};
	SDL_FPoint position;
	Uint8 type_id;
	Sint8 pack_flags;
	Uint8 HP8;
};

enum being_pack_flags{
	being_packf_shift = 1 << 0,
	being_packf_strike = 1 << 1,
	being_packf_shoot = 1 << 2,
	being_packf_burn = 1 << 3,
	being_packf_bless = 1 << 4,
	being_packf_curse = 1 << 5,
	being_packf_strike_being = 1 << 6
};

struct Projectile_pack_data{
	SDL_FPoint position;
	SDL_FPoint shift;
	Uint8 pack_flags;
};

enum projectile_pack_flags{
	projectile_packf_penetrat = 1 << 0,
	projectile_packf_hostile = 1 << 1,
	projectile_packf_special = 1 << 2,
	projectile_packf_warlock = 1 << 3,
	projectile_packf_fire = 1 << 4
};

struct Player_pack_data{
	SDL_FPoint position;
	position16 blade_position16;
	Uint32 flags;
	Uint8 direction8;
	Uint8 blade_direction8;
	Uint8 selected_scroll;
};

struct Client_pack_data{
	SDL_FPoint position;
	SDL_FPoint aim_position;
	Uint32 flags;
	float direction;
	Uint8 selected_scroll;
};

struct Client_return_data{
	Uint32 flags;
	float max_velocity;
	int fatigue_points;
	float blade_charge;
	Uint8 HP8;
	Uint8 armour8;
	Uint8 armour_magic8;
};

static void MultiplayerSetGameData(Game_data* const gd, const Uint32 flags){
	SetGameData(gd);
	gd->flags |= flags;
	if(gd->flags & gamef_is_serv_host){
		gd->announcements.unimportant = (Announcement*)SDL_malloc(sizeof(Announcement) * MAX_ANNOUNCEMENTS);
		gd->announcements.important = (Announcement*)SDL_malloc(sizeof(Announcement) * MAX_ANNOUNCEMENTS);
	}
}

static inline int MultiplayerRarePlayerEventsService(Game_data* const gd, const int check_queue){
	for(unsigned int i = 0U; i < gd->champions.num; ++i){
		Player* const pc = gd->champions.array + i;
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

			}
		}else if(check_queue == check_2 && (pc->flags & action) && gd->boxes.num > 0U){
			const int box_indx = GetNearbyBoxIndx(&gd->boxes, &pc->position, PLAYER_SIZE);
			if(box_indx != NOT_FOUND){
				LootBox(gd, pc, box_indx);
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
			PlayerInUncoveredBigSeg(gd, pc->position.x, pc->position.y);
		}
	}
	return update_ok;
}

static int MultiplayerRareEventsService(Game_data* const gd){
	static int check_queue = check_0;
	check_queue = (check_queue + 1) % checks_num;
	if(check_queue != check_5){
		return MultiplayerRarePlayerEventsService(gd, check_queue);
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

static int MultiplayerUpdateGame(Game_data* const gd){
	static unsigned int ticks_to_rare_update = RARE_UPDATE_INTERVAL;
	MultiplayerUpdatePlayers(gd);
	UpdateProjectiles(gd);
	if(ticks_to_rare_update == 0U){
		UpdateBeings(gd);
		ticks_to_rare_update = RARE_UPDATE_INTERVAL;
	}else{
		const int rare_update_resu = MultiplayerRareEventsService(gd);
		if(rare_update_resu == update_victory){
			return update_victory;
		}
		if(rare_update_resu == update_shop){
			return update_shop;
		}
		UpdateEffects(gd);
		--ticks_to_rare_update;
	}
	for(unsigned int i = 0U; i < gd->champions.num; ++i){
		if((gd->champions.array + i)->hit_points < 1){
			return update_defeated;
		}
	}
	return update_ok;
}

static bool ConnectionWaitLoop(ENetHost *const host, ENetPeer **client, ENetEvent *const connection_event, SDL_Event* const ev, Render_data *const rend_data, void (*RendFunc)(Render_data *const)){
	for(unsigned int i = 0U; i < 10U; ++i){
		RendFunc(rend_data);
		SDL_Delay(40U);
	}
	SDL_PumpEvents();
	SDL_FlushEvent(SDL_EVENT_KEY_UP);
	SDL_FlushEvent(SDL_EVENT_MOUSE_BUTTON_UP);
	while(1){
		if(EndingEventsService(ev)){
			return false;
		}
		while(enet_host_service(host, connection_event, 0)){
			switch(connection_event->type){
			case ENET_EVENT_TYPE_CONNECT:
				*client = connection_event->peer;
				connection_event->peer->data = "client";
				return true;
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(connection_event->packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				connection_event->peer->data = NULL;
			default: break;
			}
		}
		RendFunc(rend_data);
		SDL_Delay(FRAME_TIME_MS);
	}
}

void PacketFreeCallback(struct _ENetPacket *packet) {
	SDL_free(packet->data);
}

void HostGameLoop(Game_data *const gd){
	if(enet_initialize() != 0){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "An error occurred while initializing ENet.");
		exit(-1);
	}
	ENetAddress address;
	ENetHost *serhost;
	ENetPeer *client;
	address.host = ENET_HOST_ANY;
	address.port = CONNECTION_PORT;
	serhost = enet_host_create(&address, COOP_PLAYERS_NUM - 1, CONNECTION_CHANNELS, CONNECTION_BANDWIDTH_IN, CONNECTION_BANDWIDTH_OUT);
	if(serhost == NULL){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "An error occurred while trying to create an ENet server host.");
		exit(-1);
	}
	ENetEvent connection_event;
	SDL_SetRenderViewport(gd->rend_data_ptr->renderer, NULL);
	if(!ConnectionWaitLoop(serhost, &client, &connection_event, gd->ev_ptr, gd->rend_data_ptr, RenderConnectingScreen)){
		enet_host_destroy(serhost);
		enet_deinitialize();
		return;
	}
	ENetPacket *seed_packet = enet_packet_create(&gd->seed, sizeof(gd->seed), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(client, 0, seed_packet);
	enet_host_flush(serhost);

	MultiplayerSetGameData(gd, gamef_multiplayer | gamef_is_serv_host);
	SDL_srand(gd->seed);
	StartNewLevel(gd);
	CreatePlayer((gd->champions.array + 1), host(gd)->position.x, host(gd)->position.y);
	++gd->champions.num;
	MovePlayer(gd, (gd->champions.array + 1), (float)PLAYER_SIZE * 2.0F, 0.0F);
	SDL_srand(0ULL);
	DrawMap(gd->rend_data_ptr, &gd->world);
	SaveGame(gd, SAVE_PATHMP);
	Announcement imp_annn = {};
	bool packet_received = false;

	Uint64 now = 0ULL;
	Uint64 timer;
	int state = event_ok;
	Uint64 time = SDL_GetTicksNS();
	Uint64 prev_frame_time = time;
	while(state != event_quit_game){
		timer = SDL_GetTicksNS();
		if(state == event_ok){
			state = EventsService(gd->ev_ptr, host(gd), gd->rend_data_ptr);
		}else if(state == event_manage_scrolls){
			state = ManageScrollsEventsService(gd->ev_ptr, host(gd), gd->rend_data_ptr);
		}else if(state == event_menu){
			unsigned int option = MenuEventsService(gd->ev_ptr, gd->rend_data_ptr, &host(gd)->help_data.menu_position, menu_ig_unknown);
			if(menu_ig_unknown != option){
				if(menu_ig_escape == option){
					option = menu_ig_continue;
				}
				state = ActivateMenuOption(option, gd);
			}
		}else if(event_used_pause == state){
			ResetTime(&time, &prev_frame_time);
			state = event_ok;
		}

		while(enet_host_service(serhost, &connection_event, 0)){
			switch(connection_event.type){
			case ENET_EVENT_TYPE_RECEIVE:
				packet_received = true;
				struct Client_pack_data *const cpdata = (struct Client_pack_data *const)connection_event.packet->data;
				(gd->champions.array + 1)->position = cpdata->position;
				(gd->champions.array + 1)->aim_position = cpdata->aim_position;
				const Uint32 ignor_flags = dodge_time | stunned | stop_blade;
				(gd->champions.array + 1)->flags = (cpdata->flags & ~(ignor_flags)) | ((gd->champions.array + 1)->flags & (ignor_flags));
				(gd->champions.array + 1)->direction = cpdata->direction;
				(gd->champions.array + 1)->selected_scroll = cpdata->selected_scroll;
				enet_packet_destroy(connection_event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				connection_event.peer->data = NULL;
				SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Client disconnected.");
				enet_host_destroy(serhost);
				enet_deinitialize();
				ClearGameData(gd);
				return;
			default: break;
			}
		}

		const int update_result = MultiplayerUpdateGame(gd);

		if(packet_received){
			packet_received = false;
			Uint16 packet_beings_ids[MAX_PACK_BEINGS_NUM];
			Uint16 packet_projectiles_ids[MAX_PACK_PROJECTILES_NUM];
			unsigned int packet_beings_num = 0U;
			unsigned int packet_projectiles_num = 0U;
			const unsigned int packet_annoucements_num = gd->announcements.unimportant_num;
			const float sine = SineUnsafe((gd->champions.array + 1)->direction);
			const float cosi = CosiUnsafe((gd->champions.array + 1)->direction);
			for(unsigned int i = 0U; i < gd->beings.num; ++i){
				Being* bg = (gd->beings.array + *(gd->beings.indices + i));
				if(IsPointOnPCView(gd->rend_data_ptr, bg->position.x, bg->position.y, gd->champions.array + 1, sine, cosi) && packet_beings_num < MAX_PACK_BEINGS_NUM){
					*(packet_beings_ids + packet_beings_num++) = bg->main_indx;
				}
			}
			for(unsigned int i = 0U; i < gd->projectiles.num; ++i){
				Projectile* pr = (gd->projectiles.array + i);
				if(IsPointOnPCView(gd->rend_data_ptr, pr->position.x, pr->position.y, gd->champions.array + 1, sine, cosi) && packet_projectiles_num < MAX_PACK_PROJECTILES_NUM){
					*(packet_projectiles_ids + packet_projectiles_num++) = i;
				}
			}
			struct pckt_nums pckt_nums = {
				packet_beings_num,
				packet_projectiles_num,
				packet_annoucements_num
			};
			const size_t size = PACKET_LEN(pckt_nums);
			void *const data = SDL_malloc(size);
			void* ptr = data;
			*(struct pckt_nums*)ptr = pckt_nums;
			ptr += sizeof(pckt_nums);
			*(struct Player_pack_data*)ptr = (struct Player_pack_data){
				host(gd)->position,
				{host(gd)->blade.placement.position.x, host(gd)->blade.placement.position.y},
				host(gd)->flags,
				(Uint8)(host(gd)->direction / FULL_ANGLE * 255.0F),
				(Uint8)(host(gd)->blade.placement.direction / FULL_ANGLE * 255.0F),
				host(gd)->selected_scroll
			};
			ptr += sizeof(struct Player_pack_data);
			*(struct Client_return_data*)ptr = (struct Client_return_data){
				(gd->champions.array + 1)->flags,
				(gd->champions.array + 1)->max_velocity,
				(gd->champions.array + 1)->fatigue_points,
				(gd->champions.array + 1)->blade.charge,
				(Uint8)((float)(gd->champions.array + 1)->hit_points / (float)(gd->champions.array + 1)->max_hp * 255.0F),
				(Uint8)((gd->champions.array + 1)->armour.multipl * 255.0F),
				(Uint8)((gd->champions.array + 1)->armour.magic_multipl * 255.0F)
			};
			ptr += sizeof(struct Client_return_data);
			(gd->champions.array + 1)->flags &= ~(stop_blade);
			for(unsigned int i = 0U; i < pckt_nums.beings; ++i){
				const Being *const bg = gd->beings.array + *(packet_beings_ids + i);
				*(struct Being_pack_data*)ptr = (struct Being_pack_data){
					.position = bg->position,
					.type_id = bg->type_id,
					.pack_flags =
						(IS_BEING_SHIFTING(bg) ? being_packf_shift
						: (IS_BEING_SHOOTING(bg) ? being_packf_shoot
						: (IS_BEING_STRIKING(bg) ? being_packf_strike
						: (being_strike_being == bg->status ? being_packf_strike_being : 0))))
						| (NOT_FOUND != BeingHasEffect(bg, being_effect_burn) ? being_packf_burn : 0)
						| (NOT_FOUND != BeingHasEffect(bg, being_effect_bonus) ? being_packf_bless : 0)
						| (NOT_FOUND != BeingHasEffect(bg, being_effect_slow) ? being_packf_curse : 0),
					.HP8 = (Uint8)((float)bg->hit_points / (float)BeingHP(bg) * 255.0F)
				};
				if(((struct Being_pack_data*)ptr)->pack_flags & being_packf_shift){
					((struct Being_pack_data*)ptr)->shift = bg->special_move_shift;
				}else{
					((struct Being_pack_data*)ptr)->status_ticks_left = bg->status_ticks_left;
				}
				ptr += sizeof(struct Being_pack_data);
			}
			for(unsigned int i = 0U; i < pckt_nums.projectiles; ++i){
				const Projectile *const pr = (gd->projectiles.array + *(packet_projectiles_ids + i));
				*(struct Projectile_pack_data*)ptr = (struct Projectile_pack_data){
					pr->position,
					pr->shift_per_tick,
					(1 << pr->type_id) | (projectile_special == pr->type_id ? 1 << (pr->special.effect_id + projectile_types_num) : 0)
				};
				ptr += sizeof(struct Projectile_pack_data);
			}
			for(unsigned int i = 0U; i < pckt_nums.announcements; ++i){
				*(Announcement*)ptr = *(gd->announcements.unimportant + i);
				ptr += sizeof(Announcement);
			}
			ENetPacket *packet = enet_packet_create(data, size, ENET_PACKET_FLAG_NO_ALLOCATE);
			packet->freeCallback = PacketFreeCallback;
			enet_peer_send(client, 0, packet);
			if(gd->announcements.important_num > 0U){
				imp_annn = *(gd->announcements.important + (gd->announcements.important_num - 1U));
				if(annncmnt_load == imp_annn.id){
					(gd->champions.array + 1)->position = imp_annn.position;
					const size_t loadsize = sizeof(Box) * gd->boxes.num + sizeof(Player) + sizeof(gd->world.plan);
					void *const loaddata = SDL_malloc(loadsize);
					SDL_memcpy(loaddata, gd->champions.array + 1, sizeof(Player));
					SDL_memcpy(loaddata + sizeof(Player), gd->boxes.array, sizeof(Box) * gd->boxes.num);
					SDL_memcpy(loaddata + sizeof(Player) + sizeof(Box) * gd->boxes.num, gd->world.plan, sizeof(gd->world.plan));
					ENetPacket *data_packet = enet_packet_create(loaddata, loadsize, ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);
					data_packet->freeCallback = PacketFreeCallback;
					enet_peer_send(client, 2, data_packet);
				}else{
					ENetPacket *imp_packet = enet_packet_create(&imp_annn, sizeof(Announcement), ENET_PACKET_FLAG_NO_ALLOCATE | ENET_PACKET_FLAG_RELIABLE);
					enet_peer_send(client, 1, imp_packet);
				}
				--gd->announcements.important_num;
			}
			enet_host_flush(serhost);
			(gd->champions.array + 1)->flags &= ~(disrupted);
			gd->announcements.unimportant_num = 0U;
		}
		if(update_result != update_ok){
			if(update_result == update_shop){
				ResetTime(&time, &prev_frame_time);
			}else if(update_defeated == update_result){
				ClearGameData(gd);
				LoadGame(gd, SAVE_PATHMP);
			}else{
				EndLoop(gd->ev_ptr, gd->rend_data_ptr, update_result);
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
	enet_host_destroy(serhost);
	enet_deinitialize();
	ClearGameData(gd);
}

static void UpdateClientGame(Game_data* const gd){
	static unsigned int ticks_to_rare_update = RARE_UPDATE_INTERVAL;
	static unsigned int counter = 0U;
	++counter;
	host(gd)->aim_position = GetMouseWorldPosition(gd);
	ClientUpdatePlayers(gd);
	for(Projectile* pr = gd->projectiles.array; pr != (gd->projectiles.array + gd->projectiles.num); ++pr){
		const float new_x = pr->position.x + pr->shift_per_tick.x, new_y = pr->position.y + pr->shift_per_tick.y;
		if(GetSegmentUnsafe(&gd->world, new_x, new_y)){
			pr->position.x = new_x;
			pr->position.y = new_y;
			if(projectile_special == pr->type_id){
				if(projectile_fire == pr->special.effect_id){
					if(!(counter % 16U)){
						if(!(counter % 4U)){
							AddBigBurnVisualEffect(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
								pr->position.x + (SDL_randf() - 0.5F) * BULLET_SIZE,
								pr->position.y + (SDL_randf() - 0.5F) * BULLET_SIZE
							});
						}else{
							AddSmallBurnVisualEffect(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
								pr->position.x + (SDL_randf() - 0.5F) * BULLET_SIZE,
								pr->position.y + (SDL_randf() - 0.5F) * BULLET_SIZE
							});
						}
					}
				}else if(projectile_warlock == pr->special.effect_id && !(counter % 3U)){
					AddProjectileVisualEffect(&gd->rend_data_ptr->visual_effects, &pr->position);
				}
			}
		}
	}
	if(ticks_to_rare_update == 0U){
		ClientUpdateBeings(gd);
		ticks_to_rare_update = RARE_UPDATE_INTERVAL;
		static int check_queue = check_0;
		check_queue = (check_queue + 1) % checks_num;
		if(check_0 == check_queue){
			Player *const pc = host(gd);
			if(SDL_fabsf(pc->position.x - gd->world.portalA.x) < half(DOOR_SIZE) && SDL_fabsf(pc->position.y - gd->world.portalA.y) < half(DOOR_SIZE) && (pc->flags & action)){
				SetPlayerPosition(pc, gd->world.portalB.x, gd->world.portalB.y);
				UpdatePlayerNewSegment(&gd->world, pc);
				pc->flags &= ~(action);
			}else if(SDL_fabsf(pc->position.x - gd->world.portalB.x) < half(DOOR_SIZE) && SDL_fabsf(pc->position.y - gd->world.portalB.y) < half(DOOR_SIZE) && (pc->flags & action)){
				SetPlayerPosition(pc, gd->world.portalA.x, gd->world.portalA.y);
				UpdatePlayerNewSegment(&gd->world, pc);
				pc->flags &= ~(action);
			}
		}else if(check_4 == check_queue){
			for(unsigned int i = 0U; i < gd->champions.num; ++i){
				const Player *const pc = gd->champions.array + i;
				PlayerInUncoveredBigSeg(gd, pc->position.x, pc->position.y);
			}
		}
	}else{
		for(unsigned int i = 0U; i < gd->beings.num; ++i){
			const Being *const bg = (gd->beings.array + i);
			for(unsigned int j = 0U; j < bg->effects_num; ++j){
				switch((bg->effects + j)->id){
				case being_effect_burn:
					if(counter % 8U < 2U){
						AddBeingEffectVisual(gd->rend_data_ptr, bg, AddSmallBurnVisualEffect);
					}
					break;
				case being_effect_bonus:
					if(counter % 128U < 2U){
						AddBeingEffectVisual(gd->rend_data_ptr, bg, AddBonusVisualEffect);
					}
					break;
				case being_effect_slow:
					if(counter % 128U < 2U){
						AddCurseVisualEffect(&gd->rend_data_ptr->visual_effects, &bg->position);
					}
					break;
				default: break;
				}
			}
		}
		--ticks_to_rare_update;
	}
}

void ClientGameLoop(Game_data* const gd){
	if(enet_initialize() != 0){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "An error occurred while initializing ENet.");
		exit(-1);
	}
	ENetAddress address;
	ENetHost *client;
	ENetPeer *serhost;
	client = enet_host_create(NULL, COOP_PLAYERS_NUM - 1, CONNECTION_CHANNELS, CONNECTION_BANDWIDTH_IN, CONNECTION_BANDWIDTH_OUT);
	if(client == NULL){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "An error occurred while trying to create an ENet client host.");
		exit(-1);
	}
	ENetEvent connection_event;
	char *file_path = NULL;
	SDL_asprintf(&file_path, "%sserver_name", SDL_GetBasePath());
	size_t sn_size;
	char *const server_name = SDL_LoadFile(file_path, &sn_size);
	SDL_free(file_path);
	if(!server_name){
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't read server name from file: %s", SDL_GetError());
		exit(-1);
	}
	while(sn_size > 0U && ('\n' == *(server_name + sn_size - 1U) || '\r' == *(server_name + sn_size - 1U) || ' ' == *(server_name + sn_size - 1U))) {
		*(server_name + sn_size-- - 1U) = '\0';
	}
	enet_address_set_host(&address, server_name);
	SDL_free(server_name);
	address.port = CONNECTION_PORT;
	serhost = enet_host_connect(client, &address, CONNECTION_CHANNELS, 0);
	if(serhost == NULL){
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "No available peers for initiating an ENet connection.");
		enet_host_destroy(client);
		enet_deinitialize();
		return;
	}
	if(!(enet_host_service(client, &connection_event, 5000) > 0 && connection_event.type == ENET_EVENT_TYPE_CONNECT)){
		enet_peer_reset(serhost);
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Connection to host failed.");
		enet_host_destroy(client);
		enet_deinitialize();
		return;
	}
	bool ok = false;
	while(enet_host_service(client, &connection_event, 0) || !ok){
		switch(connection_event.type){
		case ENET_EVENT_TYPE_RECEIVE:
			ok = true;
			gd->seed = *(Uint64*)connection_event.packet->data;
			enet_packet_destroy(connection_event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			connection_event.peer->data = NULL;
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Client disconnected.");
			enet_host_destroy(client);
			enet_deinitialize();
			return;
		default: break;
		}
	}
	MultiplayerSetGameData(gd, gamef_multiplayer);
	SDL_srand(gd->seed);
	StartNewLevel(gd);
	CreatePlayer((gd->champions.array + 1), host(gd)->position.x, host(gd)->position.y);
	++gd->champions.num;
	MovePlayer(gd, host(gd), (float)PLAYER_SIZE * 2.0F, 0.0F);
	DrawMap(gd->rend_data_ptr, &gd->world);
	SDL_srand(0ULL);
	SaveGame(gd, SAVE_PATHMP);
	struct Client_pack_data cpdata = {};
	Uint64 now = 0ULL;
	Uint64 timer;
	int state = event_ok;
	Uint64 time = SDL_GetTicksNS();
	Uint64 prev_frame_time = time;
	Uint64 prev_connection_time = time;
	while(state != event_quit_game){
		timer = SDL_GetTicksNS();
		if(state == event_ok){
			state = EventsService(gd->ev_ptr, host(gd), gd->rend_data_ptr);
		}else if(state == event_manage_scrolls){
			state = ManageScrollsEventsService(gd->ev_ptr, host(gd), gd->rend_data_ptr);
		}else if(state == event_menu){
			unsigned int option = MenuEventsService(gd->ev_ptr, gd->rend_data_ptr, &host(gd)->help_data.menu_position, menu_ig_unknown);
			if(menu_ig_unknown != option){
				if(menu_ig_escape == option){
					option = menu_ig_continue;
				}
				state = ActivateMenuOption(option, gd);
			}
		}else if(event_used_pause == state){
			ResetTime(&time, &prev_frame_time);
			state = event_ok;
		}
		UpdateClientGame(gd);
		now = SDL_GetTicksNS();
		if(now > prev_connection_time + PACKET_INTERVAL){
			prev_connection_time = now;
			cpdata = (struct Client_pack_data){
				host(gd)->position,
				host(gd)->aim_position,
				host(gd)->flags,
				host(gd)->direction,
				host(gd)->selected_scroll
			};
			if(gd->flags & gamef_client_dodge){
				cpdata.flags |= dodge;
				gd->flags &= ~(gamef_client_dodge);
			}
			if(gd->flags & gamef_client_cast){
				cpdata.flags |= cast;
				gd->flags &= ~(gamef_client_cast);
			}
			ENetPacket *packet = enet_packet_create(&cpdata, sizeof(struct Client_pack_data), ENET_PACKET_FLAG_NO_ALLOCATE);
			enet_peer_send(serhost, 0, packet);
			enet_host_flush(client);
			host(gd)->flags &= ~(strike);
		}
		while(enet_host_service(client, &connection_event, 0)){
			switch(connection_event.type){
			case ENET_EVENT_TYPE_RECEIVE:
				if(0 == connection_event.channelID){
					void* ptr = connection_event.packet->data;
					struct pckt_nums pckt_nums = *(struct pckt_nums*)ptr;
					ptr += sizeof(struct pckt_nums);
					gd->beings.num = pckt_nums.beings;
					gd->projectiles.num = pckt_nums.projectiles;
					(gd->champions.array + 1)->position = ((struct Player_pack_data*)ptr)->position;
					(gd->champions.array + 1)->blade.placement.position = (SDL_FPoint){((struct Player_pack_data*)ptr)->blade_position16.x, ((struct Player_pack_data*)ptr)->blade_position16.y};
					(gd->champions.array + 1)->flags = ((struct Player_pack_data*)ptr)->flags;
					(gd->champions.array + 1)->direction = (float)((struct Player_pack_data*)ptr)->direction8 * FULL_ANGLE / 255.0F;
					(gd->champions.array + 1)->blade.placement.direction = (float)((struct Player_pack_data*)ptr)->blade_direction8 * FULL_ANGLE / 255.0F;
					(gd->champions.array + 1)->selected_scroll = ((struct Player_pack_data*)ptr)->selected_scroll;
					ptr += sizeof(struct Player_pack_data);
					const Uint32 not_read_flags = forward | back | right | left | range_mode | attack | block | run | dodge | cast | strike;
					host(gd)->flags = (((struct Client_return_data*)ptr)->flags & ~(not_read_flags)) | (host(gd)->flags & (not_read_flags));
					host(gd)->max_velocity = ((struct Client_return_data*)ptr)->max_velocity;
					host(gd)->fatigue_points = ((struct Client_return_data*)ptr)->fatigue_points;
					host(gd)->blade.charge = ((struct Client_return_data*)ptr)->blade_charge;
					host(gd)->hit_points = (int)((struct Client_return_data*)ptr)->HP8 * host(gd)->max_hp / 255;
					host(gd)->armour.multipl = (float)((struct Client_return_data*)ptr)->armour8 / 255.0F;
					host(gd)->armour.magic_multipl = (float)((struct Client_return_data*)ptr)->armour_magic8 / 255.0F;
					ptr += sizeof(struct Client_return_data);
					for(unsigned int i = 0U; i < pckt_nums.beings; ++i){
						const struct Being_pack_data *const bpd = (struct Being_pack_data*)ptr;
						Being *const bg = gd->beings.array + i;
						bg->position = bpd->position;
						bg->type_id = bpd->type_id;
						bg->status =
							bpd->pack_flags & being_packf_shift ? being_walk
							: (bpd->pack_flags & being_packf_shoot ? being_shoot
							: (bpd->pack_flags & being_packf_strike ? being_strike
							: (bpd->pack_flags & being_packf_strike_being ? being_strike_being : being_stunned)));
						bg->hit_points = (int)bpd->HP8 * BeingHP(bg) / 255;
						if(being_walk == bg->status){
							bg->special_move_shift = bpd->shift;
						}else{
							bg->status_ticks_left = bpd->status_ticks_left;
						}
						bg->effects_num = 0U;
						if(bpd->pack_flags & being_packf_burn){
							AddBeingEffect(bg, (Lasting_effect){being_effect_burn, 0});
						}
						if(bpd->pack_flags & being_packf_bless){
							AddBeingEffect(bg, (Lasting_effect){being_effect_bonus, 0});
						}
						if(bpd->pack_flags & being_packf_curse){
							AddBeingEffect(bg, (Lasting_effect){being_effect_slow, 0});
						}
						ptr += sizeof(struct Being_pack_data);
					}
					for(unsigned int i = 0U; i < pckt_nums.projectiles; ++i){
						const struct Projectile_pack_data *const ppd = (struct Projectile_pack_data*)ptr;
						(gd->projectiles.array + i)->position = ppd->position;
						(gd->projectiles.array + i)->shift_per_tick = ppd->shift;
						(gd->projectiles.array + i)->type_id =
							ppd->pack_flags & projectile_packf_penetrat ? projectile_penetrat
							: (ppd->pack_flags & projectile_packf_hostile ? projectile_hostile
							: projectile_special);
						if(projectile_special == (gd->projectiles.array + i)->type_id){
							(gd->projectiles.array + i)->special.effect_id = ppd->pack_flags & projectile_packf_warlock ? projectile_warlock : projectile_fire;
						}
						ptr += sizeof(struct Projectile_pack_data);
					}
					for(unsigned int i = 0U; i < pckt_nums.announcements; ++i){
						const Announcement *const an = (Announcement*)ptr;
						switch(an->id){
						case annncmnt_damage:
							AddDamageVisualEffect(gd, &an->position);
							break;
						case annncmnt_explosion:
							AddFireExplosionVisualEffect(gd, &an->position);
							break;
						case annncmnt_kill:
							AddDeadVisualEffect(gd, &an->position);
							break;
						case annncmnt_portal:
							AddPortalVisualEffect(gd, &an->position);
							break;
						case annncmnt_bolt:
							AddBoltVisualEffect(gd, &(SDL_FPoint){an->pos16b0.x, an->pos16b0.y}, an->pos16b1);
							break;
						case annncmnt_cast:
							;Uint8 scroll = (Uint8)an->data64b;
							PlayerCastConsequences(gd, host(gd), ScrollCost(scroll), scroll);
							break;
						case annncmnt_spell:
							AddSpellVisualEffect(gd, &(SDL_FPoint){an->pos16b.x, an->pos16b.y}, SPELL_RED, an->data8b0, an->data8b1);
							break;
						default: break;
						}
						ptr += sizeof(Announcement);
					}
				}else if(1 == connection_event.channelID){
					const Announcement *const an = (Announcement*)connection_event.packet->data;
					switch(an->id){
					case annncmnt_destroy_box:
						DestroyBoxInArray(&gd->boxes, (unsigned int)an->data64b);
						break;
					case annncmnt_update_mp_and_coin:
						host(gd)->magic_points = an->data32b01;
						host(gd)->coins = an->data32b11;
						break;
					case annncmnt_update_scroll:
						*(host(gd)->scrolls + an->data32b0) = an->data32b1;
						break;
					case annncmnt_update_key:
						*(gd->keys_status + an->data32b0) = an->data32b1;
						if(key_owned == an->data32b1){
							++gd->keys;
						}
						break;
					default: break;
					}
				}else{
					gd->boxes.num = (connection_event.packet->dataLength - (sizeof(Player) + sizeof(gd->world.plan))) / sizeof(Box);
					SDL_memcpy(host(gd), connection_event.packet->data, sizeof(Player));
					SDL_memcpy(gd->boxes.array, connection_event.packet->data + sizeof(Player), sizeof(Box) * gd->boxes.num);
					SDL_memcpy(gd->world.plan, connection_event.packet->data + sizeof(Player) + sizeof(Box) * gd->boxes.num, sizeof(gd->world.plan));
					DrawMap(gd->rend_data_ptr, &gd->world);
				}
				enet_packet_destroy(connection_event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				connection_event.peer->data = NULL;
				SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Client disconnected.");
				enet_host_destroy(client);
				enet_deinitialize();
				ClearGameData(gd);
				return;
			default: break;
			}
		}
		now = SDL_GetTicksNS();
		if(now > prev_frame_time + FRAME_TIME){
			prev_frame_time = now;
			RenderClientGame(gd->rend_data_ptr, gd, state);
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
	enet_host_destroy(client);
	enet_deinitialize();
	ClearGameData(gd);
}

static inline bool IsAnnouncementImportant(const Uint8 id){
	if(id < annncmnt_damage){
		return true;
	}
	return false;
}

extern inline void AddAnnouncement(Game_data *const gd, const Uint8 id, const void *const data){
	if(!(gd->flags & gamef_is_serv_host)){return;}
	if(IsAnnouncementImportant(id)){
		if(gd->announcements.important_num < MAX_ANNOUNCEMENTS){
			(gd->announcements.important + gd->announcements.important_num)->id = id;
			SDL_memcpy(gd->announcements.important + gd->announcements.important_num, data, sizeof(Uint64));
			++gd->announcements.important_num;
		}
	}else if(gd->announcements.unimportant_num < MAX_ANNOUNCEMENTS){
		(gd->announcements.unimportant + gd->announcements.unimportant_num)->id = id;
		SDL_memcpy(gd->announcements.unimportant + gd->announcements.unimportant_num, data, sizeof(Uint64));
		++gd->announcements.unimportant_num;
	}
}
