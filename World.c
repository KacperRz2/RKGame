#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <enum.h>
#include <World.h>
#include <function.h>
#include <Player.h>
#include <Being.h>
#include <game.h>
#include <scroll.h>

void CreateWorld(Game_data* const g_d, const float x, const float y){
	bool world_plan_base[BIG_SEGMENTS_X][BIG_SEGMENTS_X] = WORLD_BASE;
	int small_wordl_plan[SMALL_PLAN_SIZE][SMALL_PLAN_SIZE] = WORLD_SMALL_BASE;
	World* const world = &g_d->world;
	Uint64 seed = 2ULL;
	SDL_srand(seed);
	SDL_Point portal0;
	SDL_Point portal1;
	SDL_Point door_position;
	int rand0 = SDL_rand(4);
	int rand1 = (rand0 + SDL_rand(3) + 1) % 4;
	int rand2 = (rand0 + SDL_rand(2) + 1) % 4;
	while(rand2 == rand1){
		rand2 = (rand0 + SDL_rand(3) + 1) % 4;
	}
	if(rand0 == 0){
		portal0.x = 3U;
		portal0.y = 0U;
	}else if(rand0 == 1){
		portal0.x = 6U;
		portal0.y = 3U;
	}else if(rand0 == 2){
		portal0.x = 3U;
		portal0.y = 6U;
	}else{
		portal0.x = 0U;
		portal0.y = 3U;
	}
	if(rand1 == 0){
		portal1.x = 3U;
		portal1.y = 0U;
	}else if(rand1 == 1){
		portal1.x = 6U;
		portal1.y = 3U;
	}else if(rand1 == 2){
		portal1.x = 3U;
		portal1.y = 6U;
	}else{
		portal1.x = 0U;
		portal1.y = 3U;
	}
	if(rand2 == 0){
		door_position.x = 3U;
		door_position.y = 0U;
	}else if(rand2 == 1){
		door_position.x = 6U;
		door_position.y = 3U;
	}else if(rand2 == 2){
		door_position.x = 3U;
		door_position.y = 6U;
	}else{
		door_position.x = 0U;
		door_position.y = 3U;
	}
	world->portalA.x = GetDoorPositionXorY(portal0.x);
	world->portalA.y = GetDoorPositionXorY(portal0.y);
	world->portalB.x = GetDoorPositionXorY(portal1.x);
	world->portalB.y = GetDoorPositionXorY(portal1.y);
	world->door.x = GetDoorPositionXorY(door_position.x);
	world->door.y = GetDoorPositionXorY(door_position.y);
	// SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "p0: %d:%d", portal0.x, portal0.y);
	// SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "p1: %d:%d", portal1.x, portal1.y);
	// SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "d: %d:%d", door_position.x, door_position.y);
	enum small_plan_field{
		small_plan_null,
		small_plan_unconnected,
		small_plan_connected
	};
	enum direction{up, right, down, left};
	int fields_left = 36;
	int try = 0b0000;
	SDL_Point fields[37];
	do{
		*fields = (SDL_Point){
			SDL_rand(SMALL_PLAN_SIZE),
			SDL_rand(SMALL_PLAN_SIZE)
		};
	}while(*(*(small_wordl_plan + fields->x) + fields->y) == small_plan_null);
	*(*(small_wordl_plan + fields->x) + fields->y) = small_plan_connected;
	int indx = 0;
	while(fields_left){
		if(try == 0b1111){
			if(((fields + indx)->x + 1 >= 0 && (fields + indx)->x + 1 < 7 && *(*(small_wordl_plan + (fields + indx)->x + 1) + (fields + indx)->y) == small_plan_unconnected)
				|| ((fields + indx)->y + 1 >= 0 && (fields + indx)->y + 1 < 7 && *(*(small_wordl_plan + (fields + indx)->x) + (fields + indx)->y + 1) == small_plan_unconnected)
				|| ((fields + indx)->x - 1 >= 0 && (fields + indx)->x - 1 < 7 && *(*(small_wordl_plan + (fields + indx)->x - 1) + (fields + indx)->y) == small_plan_unconnected)
				|| ((fields + indx)->y - 1 >= 0 && (fields + indx)->y - 1 < 7 && *(*(small_wordl_plan + (fields + indx)->x) + (fields + indx)->y - 1) == small_plan_unconnected)
			){
				SDL_LogInfo(SDL_LOG_CATEGORY_RENDER, "WORLD ERROR");
				exit(-100);
			}
			--indx;
			if(indx < 0){
				SDL_LogInfo(SDL_LOG_CATEGORY_ERROR, "indx < 0");
				exit(-101);
			}
			try = 0b0000;
			continue;
		}
		int new_col = (fields + indx)->x;
		int new_row = (fields + indx)->y;
		do{
			rand0 = SDL_rand(4);
		}while(try & 1 << rand0);
		if(rand0 == up){
			--new_row;
		}else if(rand0 == right){
			++new_col;
		}else if(rand0 == down){
			++new_row;
		}else{
			--new_col;
		}
		if(new_col < 0 || new_col > 6 || new_row < 0 || new_row > 6){
			if((fields + indx)->x == portal0.x && (fields + indx)->y == portal0.y){
				new_col = portal1.x;
				new_row = portal1.y;
			}else if((fields + indx)->x == portal1.x && (fields + indx)->y == portal1.y){
				new_col = portal0.x;
				new_row = portal0.y;
			}else{
				try |= 1 << rand0;
				continue;
			}
		}
		if(*(*(small_wordl_plan + new_col) + new_row) != small_plan_unconnected){
			try |= 1 << rand0;
			continue;
		}
		if(rand0 == up){
			*(*(world_plan_base + 2 + (fields + indx)->x * 4    ) + 2 + (fields + indx)->y * 4 - 2) = true;
		}else if(rand0 == right){
			*(*(world_plan_base + 2 + (fields + indx)->x * 4 + 2) + 2 + (fields + indx)->y * 4    ) = true;
		}else if(rand0 == down){
			*(*(world_plan_base + 2 + (fields + indx)->x * 4    ) + 2 + (fields + indx)->y * 4 + 2) = true;
		}else{
			*(*(world_plan_base + 2 + (fields + indx)->x * 4 - 2) + 2 + (fields + indx)->y * 4    ) = true;
		}
		*(*(small_wordl_plan + new_col) + new_row) = small_plan_connected;
		++indx;
		(fields + indx)->x = new_col;
		(fields + indx)->y = new_row;
		try = 0b0000;
		--fields_left;
	}
	for(unsigned int c = 0; c < 7; ++c){
		for(unsigned int r = 0; r < 7; ++r){
			if(*(*(small_wordl_plan + c) + r) == 1){
				SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "field == 1!");
			}
		}
	}
	for(unsigned int c = 2U; c < BIG_SEGMENTS_X; c += 4U){
		for(unsigned int r = 2U; r < BIG_SEGMENTS_X; r += 4U){
			if(*(*(world_plan_base + c) + r) == true && *(*(world_plan_base + c + 1U) + r + 1U) == true){
				switch(SDL_rand(21)){
					case 0:
						*(*(world_plan_base + c) + r) = false;
						break;
					case 1:
						*(*(world_plan_base + c) + r) = false;
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						break;
					case 2:
						*(*(world_plan_base + c) + r) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						break;
					case 3:
						*(*(world_plan_base + c) + r) = false;
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						break;
					case 4:
						*(*(world_plan_base + c) + r) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 5:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						break;
					case 6:
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						break;
					case 7:
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						break;
					case 8:
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 9:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 10:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						break;
					case 11:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 12:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 13:
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 14:
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 15:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 16:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						break;
					case 17:
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						break;
					case 18:
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 19:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						break;
					default:
					break;
				}
			}
		}
	}
	world->width = x;
	world->height = y;
	world->segments = (Segment***)SDL_malloc(sizeof(Segment**) * SEGMENTS_X);
	for(unsigned int c = 0U; c < SEGMENTS_X; ++c){
		*(world->segments + c) = (Segment**)SDL_malloc(sizeof(Segment*) * SEGMENTS_Y);
		for(unsigned int r = 0U; r < SEGMENTS_Y; ++r){
			if(c == 0U || r == 0U || c == SEGMENTS_X - 1U || r == SEGMENTS_Y - 1U){
				*(*(world->segments + c) + r) = NULL;
			}
		}
	}
	for(unsigned int bigc = 0U; bigc < BIG_SEGMENTS_X; ++bigc){
		for(unsigned int bigr = 0U; bigr < BIG_SEGMENTS_X; ++bigr){
			if(*(*(world_plan_base + bigc) + bigr) == true){
				for(unsigned int c = bigc * BIG_SEGMENT_SEGMENTS_X + 1U; c < bigc * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X + 1U; ++c){
					for(unsigned int r = bigr * BIG_SEGMENT_SEGMENTS_X + 1U; r < bigr * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X + 1U; ++r){
						if(!(c == bigc * BIG_SEGMENT_SEGMENTS_X + 1U || c == bigc * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X || r == bigr * BIG_SEGMENT_SEGMENTS_X + 1U || r == bigr * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X) || c == bigc * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X / 2 || c == bigc * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X / 2 + 1U || r == bigr * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X / 2 || r == bigr * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X / 2 + 1U){
							*(*(world->segments + c) + r) = (Segment*)SDL_malloc(sizeof(Segment));
							(*(*(world->segments + c) + r))->indx.x = c;
							(*(*(world->segments + c) + r))->indx.y = r;
							(*(*(world->segments + c) + r))->beings.num = 0U;
							(*(*(world->segments + c) + r))->ally_beings.num = 0U;
							(*(*(world->segments + c) + r))->flags = 0x0U;
						}else{
							*(*(world->segments + c) + r) = NULL;
						}
					}
				}
			}else{
				for(unsigned int c = bigc * BIG_SEGMENT_SEGMENTS_X + 1U; c < bigc * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X + 1U; ++c){
					for(unsigned int r = bigr * BIG_SEGMENT_SEGMENTS_X + 1U; r < bigr * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X + 1U; ++r){
						*(*(world->segments + c) + r) = NULL;
					}
				}
			}
		}
	}
	while(g_d->boxes.num < BOXES_NUM){
		unsigned int c = SDL_rand(SEGMENTS_X - 3) + 2U;
		unsigned int r = SDL_rand(SEGMENTS_Y - 3) + 2U;
		Segment* s = GetSegmentByIndx(world, c, r);
		if(s != NULL){
			AddBoxToArray(&g_d->boxes, SegmentPositionX(s) + half(SEGMENT_SIZE) + (SDL_randf() - 0.5F) * SEGMENT_SIZE, SegmentPositionY(s) + half(SEGMENT_SIZE) + (SDL_randf() - 0.5F) * SEGMENT_SIZE);
		}
	}
	FillBoxes(g_d);
}

void DestroyWorld(World* const world){
	for(unsigned int c = 0U; c < SEGMENTS_X; ++c){
	for(unsigned int r = 0U; r < SEGMENTS_Y; ++r){
			SDL_free(*(*(world->segments + c) + r));
		}
		SDL_free(*(world->segments + c));
	}
	SDL_free(world->segments);
}

extern inline Segment* GetSegment(const World* const world, const float x, const float y){
	const unsigned int c = (unsigned int)(x / SEGMENT_SIZE);
	const unsigned int r = (unsigned int)(y / SEGMENT_SIZE);
	return *(*(world->segments + c) + r);
}

extern inline Segment* GetSegmentByIndx(const World* const world, const unsigned int c, const unsigned int r){
	return *(*(world->segments + c) + r);
}

extern inline Segment* GetSegmentByIndxSafe(const World* const world, const int c, const int r){
	if(c >= SEGMENTS_X || c < 0 || r >= SEGMENTS_Y || r < 0){
		return NULL;
	}
	return *(*(world->segments + c) + r);
}

void StartLevel(Game_data* const g_d){
	for(unsigned int c = 0U; c < SEGMENTS_X; ++c){
	for(unsigned int r = 0U; r < SEGMENTS_Y; ++r){
			if(*(*(g_d->world.segments + c) + r) != NULL){
				(*(*(g_d->world.segments + c) + r))->beings.num = 0U;//reset segments
				(*(*(g_d->world.segments + c) + r))->ally_beings.num = 0U;
			}
		}
	}
	g_d->beings.num = 0U;
	SDL_FPoint start_position = GetStartPosition(&g_d->world);
	for(unsigned int i = 0U; i < START_PLAYERS_NUM; ++i){
		CreatePlayer((g_d->champions.array + i), start_position.x, start_position.y);
		(g_d->champions.array + i)->segment = GetSegment(&g_d->world, (g_d->champions.array + i)->position.x, (g_d->champions.array + i)->position.y);
		(g_d->champions.array + i)->last_seen_in = (g_d->champions.array + i)->segment;
		++g_d->champions.num;
	}
	Uint64 start_time = SDL_GetTicks();
	while (g_d->beings.num < MAX_START_BEINGS_NUM - 64U){//test beings
		float x = (float)(SDL_rand((Sint32)(WORLD_W - SEGMENT_SIZE * 4.0F))) + SEGMENT_SIZE * 2.0F;
		float y = (float)(SDL_rand((Sint32)(WORLD_H - SEGMENT_SIZE * 4.0F))) + SEGMENT_SIZE * 2.0F;
		if(SDL_fabsf(start_position.x - x) > 1000.0F && SDL_fabsf(start_position.y - y) > 1000.0F && SDL_fabsf(start_position.x - x) < WORLD_W * 0.25F && SDL_fabsf(start_position.y - y) < WORLD_H * 0.25F){
			Segment* s = GetSegment(&g_d->world, x, y);
			if(s != NULL && s->beings.num < MAX_SEGM_BEINGS){
				AddBeingToArray(&g_d->beings, (unsigned int)SDL_rand(2), x, y, s, g_d->champions.array + g_d->human_indx);
			}
		}
		if(SDL_GetTicks() - start_time > 1500U) break;
	}
}

static SDL_FPoint GetStartPosition(World* const w){
	SDL_FPoint result;
	if(!(w->door.x < WORLD_SIZE / 4 || w->portalA.x < WORLD_SIZE / 4 || w->portalB.x < WORLD_SIZE / 4)){
		result.x = SEGMENT_SIZE * 2.25F;
		result.y = half(WORLD_SIZE);
	}else if(!(w->door.y < WORLD_SIZE / 4 || w->portalA.y < WORLD_SIZE / 4 || w->portalB.y < WORLD_SIZE / 4)){
		result.x = half(WORLD_SIZE);
		result.y = SEGMENT_SIZE * 2.25F;
	}else if(!(w->door.x > WORLD_SIZE / 4 * 3 || w->portalA.x > WORLD_SIZE / 4 * 3 || w->portalB.x > WORLD_SIZE / 4 * 3)){
		result.x = WORLD_SIZE - SEGMENT_SIZE * 2.25F;
		result.y = half(WORLD_SIZE);
	}else if(!(w->door.y > WORLD_SIZE / 4 * 3 || w->portalA.y > WORLD_SIZE / 4 * 3 || w->portalB.y > WORLD_SIZE / 4 * 3)){
		result.x = half(WORLD_SIZE);
		result.y = WORLD_SIZE - SEGMENT_SIZE * 2.25F;
	}
	return result;
}

extern inline float SegmentPositionX(const Segment* const s){
	return s->indx.x * SEGMENT_SIZE;
}

extern inline float SegmentPositionY(const Segment* const s){
	return s->indx.y * SEGMENT_SIZE;
}

extern inline float SegmentCenterX(const Segment* const s){
	return (s->indx.x + 0.5F) * SEGMENT_SIZE;
}

extern inline float SegmentCenterY(const Segment* const s){
	return (s->indx.y + 0.5F) * SEGMENT_SIZE;
}

static float GetDoorPositionXorY(const unsigned int small_plan_position){
	if(small_plan_position == 0){
		return SEGMENT_SIZE * 1.25F;
	}
	if(small_plan_position == 3){
		return half(WORLD_SIZE);
	}
	if(small_plan_position == 6){
		return WORLD_SIZE - SEGMENT_SIZE * 1.25F;
	}
}

extern inline bool IsClearSight(const SDL_FPoint* const from, const SDL_FPoint* const to, const Segment* const to_s, const World* const w){
	const Segment* s = GetSegment(w, from->x, from->y);
	if(s == to_s){return true;}
	const float distance_x = to->x - from->x;
	const float distance_y = to->y - from->y;
	const float distance = SDL_sqrtf(pow2(distance_x) + pow2(distance_y));
    const float shift_x = distance_x / distance;
    const float shift_y = distance_y / distance;
	const float help_to_x = shift_x < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	const float help_to_y = shift_y < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	SDL_FPoint control_point = *from;
	do{
		const float help_x = SDL_fmodf(control_point.x, SEGMENT_SIZE);
		const float help_y = SDL_fmodf(control_point.y, SEGMENT_SIZE);
		const float shifts_to_next_seg_x = (help_to_x - help_x) / shift_x;
		const float shifts_to_next_seg_y = (help_to_y - help_y) / shift_y;
		const float multipl = shifts_to_next_seg_x < shifts_to_next_seg_y ? shifts_to_next_seg_x : shifts_to_next_seg_y;
		control_point.x += shift_x * multipl;
		control_point.y += shift_y * multipl;
		s = GetSegment(w, control_point.x, control_point.y);
		if(s == NULL){
			return false;
		}
	}while(s != to_s);
	return true;
}

extern inline bool IsClearSightWithKnownDistance(const SDL_FPoint* const from, const Segment* const to_s, const World* const w, const float d_x, const float d_y, const float distance_squared){
	const Segment* s = GetSegment(w, from->x, from->y);
	if(s == to_s){return true;}
	const float distance = SDL_sqrtf(distance_squared);
    const float shift_x = d_x / distance;
    const float shift_y = d_y / distance;
	const float help_to_x = shift_x < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	const float help_to_y = shift_y < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	SDL_FPoint control_point = *from;
	do{
		const float help_x = SDL_fmodf(control_point.x, SEGMENT_SIZE);
		const float help_y = SDL_fmodf(control_point.y, SEGMENT_SIZE);
		const float shifts_to_next_seg_x = (help_to_x - help_x) / shift_x;
		const float shifts_to_next_seg_y = (help_to_y - help_y) / shift_y;
		const float multipl = shifts_to_next_seg_x < shifts_to_next_seg_y ? shifts_to_next_seg_x : shifts_to_next_seg_y;
		control_point.x += shift_x * multipl;
		control_point.y += shift_y * multipl;
		s = GetSegment(w, control_point.x, control_point.y);
		if(s == NULL){
			return false;
		}
	}while(s != to_s);
	return true;
}

extern inline bool SegmentInSight(const SDL_FPoint* const from, const SDL_FPoint* const point_in_seg, const Segment* const to_s, const World* const w){
	const SDL_FPoint to = {point_in_seg->x - SDL_fmodf(point_in_seg->x, SEGMENT_SIZE) + 0.5F, point_in_seg->y - SDL_fmodf(point_in_seg->y, SEGMENT_SIZE) + 0.5F};
	if(GetSegment(w, from->x, from->y) == to_s){return true;}
	const float distance_x = to.x - from->x;
	const float distance_y = to.y - from->y;
	SDL_FPoint p0, p1, p2;
	p0 = (SDL_FPoint){to.x, to.y + SEGMENT_SIZE - 1.0F};
	p1 = (SDL_FPoint){to.x + SEGMENT_SIZE - 1.0F, to.y + SEGMENT_SIZE - 1.0F};
	p2 = (SDL_FPoint){to.x + SEGMENT_SIZE - 1.0F, to.y};
	return (IsClearSightPlus(from, &to, to_s, w) || IsClearSightPlus(from, &p0, to_s, w) || IsClearSightPlus(from, &p1, to_s, w) || IsClearSightPlus(from, &p2, to_s, w));
}

static inline bool IsClearSightPlus(const SDL_FPoint* const from, const SDL_FPoint* const to, const Segment* const to_s, const World* const w){
	const float distance_x = to->x - from->x;
	const float distance_y = to->y - from->y;
	const float distance = SDL_sqrtf(pow2(distance_x) + pow2(distance_y));
    const float shift_x = distance_x / distance;
    const float shift_y = distance_y / distance;
	const float help_to_x = shift_x < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	const float help_to_y = shift_y < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	SDL_FPoint control_point = *from;
	const Segment* s;
	do{
		const float help_x = SDL_fmodf(control_point.x, SEGMENT_SIZE);
		const float help_y = SDL_fmodf(control_point.y, SEGMENT_SIZE);
		const float shifts_to_next_seg_x = (help_to_x - help_x) / shift_x;
		const float shifts_to_next_seg_y = (help_to_y - help_y) / shift_y;
		const float multipl = shifts_to_next_seg_x < shifts_to_next_seg_y ? shifts_to_next_seg_x : shifts_to_next_seg_y;
		control_point.x += shift_x * multipl;
		control_point.y += shift_y * multipl;
		s = GetSegment(w, control_point.x, control_point.y);
		if(s == NULL){
			if(SDL_fabsf(to->x - control_point.x) < SEGMENT_SIZE * SQRT2DIV2 && SDL_fabsf(to->y - control_point.y) < SEGMENT_SIZE * SQRT2DIV2){
				return true;
			}
			return false;
		}
	}while(s != to_s);
	return true;
}

static void FillBoxes(Game_data* const gd){
	gd->needed_keys = 0U;
	do{
		gd->needed_keys = 0U;
		Uint16 boxes_with_map[MAX_MAPS];
		unsigned int maps_num = 0U;
		for(unsigned int i = 0U; i < BOXES_NUM; ++i){
			Box* bx = (gd->boxes.array + i);
			unsigned int slot = 0U;
			if(SDL_rand(BOXES_NUM) < 7 && gd->needed_keys < MAX_KEYS){
				AddToBox(bx, slot++, box_key, 1U);
				*(gd->world.key_locations + gd->needed_keys++) = (Key_location){
					(Uint8)(WORLD_SIZE / bx->location.x * 255),
					(Uint8)(WORLD_SIZE / bx->location.y * 255)
				};
			}else if(SDL_rand(BOXES_NUM) < 14 && maps_num < MAX_MAPS){
				AddToBox(bx, slot++, box_map, 0);
				*(boxes_with_map + maps_num++) = i;
			}
			AddToBox(bx, slot++, box_coins, SDL_rand(BOX_MAX_COINS));
			for(unsigned int j = slot; j < BOX_SLOTS - 1U; ++j){
				if(SDL_rand(3)){
					AddToBox(bx, slot++, box_scroll, GetRandomScroll());
				}
			}
			AddToBox(bx, slot, box_mp, SDL_rand(BOX_MAX_MP));
		}
		Uint16 keys_maps = 0b0000000000000000;
		for(unsigned int i = 0U; i < maps_num; ++i){
			Uint16 key = SDL_rand(gd->needed_keys);
			if(keys_maps != (1U << gd->needed_keys) - 1U){
				while((1 << key) & keys_maps){
					key = SDL_rand(gd->needed_keys);
				}
				keys_maps |= (1 << key);
			}
			(gd->boxes.array + *(boxes_with_map + i))->elements->value = key;
		}
		// SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "keys: %u", gd->needed_keys);
		// SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "maps: %u", maps_num);
	}while(gd->needed_keys < KEYS_NUM);
}