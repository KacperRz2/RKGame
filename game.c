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

static inline void PopulateBigSeg(Game_data* const gd, const unsigned int x, const unsigned int y){
	if(IsVoidBigSeg(gd->world.plan, x, y) || IsInPopulatedBigSeg(gd->world.plan, x, y)){
		return;
	}
	MarkAsPopulatedBigSeg(gd->world.plan, x, y);
	for(unsigned int i = 0U; i < MAX_POPULATION_NUM; ++i){
		TryCreateIdleBeing(gd, GetRandomBeingId(), BigSegPosition(x) + SDL_randf() * BIG_SEGMENT_SIZE, BigSegPosition(y) + SDL_randf() * BIG_SEGMENT_SIZE, human(gd));
	}
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
	}
}

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
	game_data.rend_data_ptr = rend_data;
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
			if((game_data.champions.array + game_data.human_indx)->flags & tmp){
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

static void SetGameData(Game_data* const gd){
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
	if(gd->projectiles.array == NULL || gd->beings.array == NULL || gd->champions.array == NULL || gd->boxes.array == NULL || gd->beings.indices == NULL || gd->rend_data_ptr->visual_effects.array == NULL){SDL_Quit(); exit(1);}
	for(unsigned int i = 0U; i < MAX_BEINGS_NUM; ++i){
		*(gd->beings.indices + i) = i;
		(gd->beings.array + i)->main_indx = i;
	}
	gd->beings.num = 0U;
	gd->projectiles.num = 0U;
	gd->rend_data_ptr->visual_effects.num = 0U;
	gd->keys = 0U;
	gd->effects_num = 0U;
	gd->horde_data.ticks_from_attack = 0U;
}

static void ClearGameData(Game_data* const gd){
	DestroyBoxes(&gd->boxes);
	DestroyBeings(&gd->beings);
    DestroyProjectiles(&gd->projectiles);
	SDL_free(gd->champions.array);
	SDL_free(gd->rend_data_ptr->visual_effects.array);
	gd->champions.num = 0U;
	gd->rend_data_ptr->visual_effects.num = 0U;
	DestroyWorld(&gd->world);
}

static void RareEventsService(Game_data* const gd){
	static int check_queue = 0;
	if(check_queue == 0 && SDL_fabsf(human(gd)->position.x - gd->world.portalA.x) < half(DOOR_SIZE) && SDL_fabsf(human(gd)->position.y - gd->world.portalA.y) < half(DOOR_SIZE) && (human(gd)->flags & action)){
		SetPlayerPosition(human(gd), gd->world.portalB.x, gd->world.portalB.y);
	 	human(gd)->flags &= ~(action);
	}else if(check_queue == 1 && SDL_fabsf(human(gd)->position.x - gd->world.portalB.x) < half(DOOR_SIZE) && SDL_fabsf(human(gd)->position.y - gd->world.portalB.y) < half(DOOR_SIZE) && (human(gd)->flags & action)){
		SetPlayerPosition(human(gd), gd->world.portalA.x, gd->world.portalA.y);
	 	human(gd)->flags &= ~(action);
	}else if(check_queue == 2 && (human(gd)->flags & action) && gd->boxes.num > 0U){
		int box_indx = GetNearbyBoxIndx(gd);
		if(box_indx != -1){
			LootBox(gd, box_indx);
	 		human(gd)->flags &= ~(action);
		}
	}else if(check_queue == 3 && gd->keys >= gd->needed_keys && SDL_fabsf(human(gd)->position.x - gd->world.door.x) < half(DOOR_SIZE) && SDL_fabsf(human(gd)->position.y - gd->world.door.y) < half(DOOR_SIZE) && (human(gd)->flags & action)){
		human(gd)->hit_points = 0;
	 	human(gd)->flags &= ~(action);
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
			gd->keys += (int)elem->value;
			elem->type = box_clear;
		}else if(element_type == box_map){
			Key_location* kl = gd->world.key_locations + elem->value;
			SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "Key location: x: %u, y: %u!", kl->x, kl->y);
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
	static unsigned int ticks_to_rare_update = RARE_UPDATE_INTERVAL;
	UpdatePlayers(gd);
	UpdateProjectiles(gd);
	if(ticks_to_rare_update == 0U){
		UpdateBeings(gd);
		ticks_to_rare_update = RARE_UPDATE_INTERVAL;
	}else{
		RareEventsService(gd);
		UpdateEffects(gd);
		--ticks_to_rare_update;
	}
	if(human(gd)->hit_points < 1){
		return update_defeated;
	}
	return update_ok;
}

static inline void UpdateEffects(Game_data* const gd){
	UpdateGameEffects(gd);
	UpdateBeingsEffects(gd);
	UpdatePlayersEffects(gd);
}

static inline SDL_FPoint GetRandomBeingCreationPoint(Game_data* const gd){
	const float angle = SDL_randf() * FULL_ANGLE;
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
		for(unsigned int i = 0U; i < HORDE_ATTACK_POINTS; ++i){
			*(gd->horde_data.creation_points + i) = GetRandomBeingCreationPoint(gd);
		}
	}else if(ticks_left < 2){
		gd->flags &= ~(gamef_horde_attack);
		gd->horde_data.ticks_from_attack = 0U;
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