#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <World.h>
#include <function.h>
#include <Player.h>
#include <Being.h>
#include <game.h>

void CreateWorld(Game_data* const g_d, const float x, const float y){
	bool world_plan_base[BIG_SEGMENTS_X][BIG_SEGMENTS_X] = WORLD_BASE;
	int small_wordl_plan[BIG_SEGMENTS_X / 4][BIG_SEGMENTS_X / 4] = WORLD_SMALL_BASE;
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
	SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "p0: %d:%d", portal0.x, portal0.y);
	SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "p1: %d:%d", portal1.x, portal1.y);
	SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "d: %d:%d", door_position.x, door_position.y);
	int fields_left = 36;
	int try = 0b0000;
	SDL_Point fields[37];
	fields->x = 3U;
	fields->y = 3U;
	int indx = 0;
	unsigned int new_col = fields->x;
	unsigned int new_row = fields->y;
	while(fields_left){
		if(try == 0b1111){
			--indx;
			try = 0b0000;
			continue;
		}
		rand0 = SDL_rand(4);
		while(try & 1 << rand0){
			rand0 = SDL_rand(4);
		}
		if(rand0 == 0){
			new_row = (fields + indx)->y - 1U;
		}else if(rand0 == 1){
			new_col = (fields + indx)->x + 1U;
		}else if(rand0 == 2){
			new_row = (fields + indx)->y + 1U;
		}else{
			new_col = (fields + indx)->x - 1U;
		}
		if(new_col < 0U || new_col > 6U || new_row < 0U || new_row > 6U){
			if((fields + indx)->x == portal0.x && (fields + indx)->y == portal0.y){
				new_col = portal1.x;
				new_row = portal1.y;
			}else if((fields + indx)->x == portal1.x && (fields + indx)->y == portal1.y){
				new_col = portal0.x;
				new_row = portal0.y;
			}else{
				try |= 1 << rand0;
				new_col = (fields + indx)->x;
				new_row = (fields + indx)->y;
				continue;
			}
		}
		int status = *(*(small_wordl_plan + new_col) + new_row);
		if(status != 1){
			try |= 1 << rand0;
			new_col = (fields + indx)->x;
			new_row = (fields + indx)->y;
			continue;
		}
		if(rand0 == 0){
			*(*(world_plan_base + 2 + (fields + indx)->x * 4    ) + 2 + (fields + indx)->y * 4 - 2) = true;
		}else if(rand0 == 1){
			*(*(world_plan_base + 2 + (fields + indx)->x * 4 + 2) + 2 + (fields + indx)->y * 4    ) = true;
		}else if(rand0 == 2){
			*(*(world_plan_base + 2 + (fields + indx)->x * 4    ) + 2 + (fields + indx)->y * 4 + 2) = true;
		}else{
			*(*(world_plan_base + 2 + (fields + indx)->x * 4 - 2) + 2 + (fields + indx)->y * 4    ) = true;
		}
		*(*(small_wordl_plan + new_col) + new_row) = 2;
		++indx;
		(fields + indx)->x = new_col;
		(fields + indx)->y = new_row;
		try = 0b0000;
		--fields_left;
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

extern inline Segment* GetSegment(World* const world, const float x, const float y){
	const unsigned int c = (unsigned int)(x / SEGMENT_SIZE);
	const unsigned int r = (unsigned int)(y / SEGMENT_SIZE);
	return *(*(world->segments + c) + r);
}

extern inline Segment* GetSegmentByIndx(World* const world, const unsigned int c, const unsigned int r){
	return *(*(world->segments + c) + r);
}

extern inline void UpdateSegmentBeingPointer(Being* old, Being* new){
	*(old->segment->beings.array + old->indx) = new;
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
	for(unsigned int i = 0U; i < MAX_PLAYERS_NUM; ++i){
		CreatePlayer((g_d->champions.array + i), start_position.x, start_position.y);
		(g_d->champions.array + i)->segment = GetSegment(&g_d->world, (g_d->champions.array + i)->position.x, (g_d->champions.array + i)->position.y);
		++g_d->champions.num;
	}
	Uint64 start_time = SDL_GetTicks();
	while (g_d->beings.num < MAX_START_BEINGS_NUM){//test beings
		float x = (float)(SDL_rand((Sint32)(WORLD_W - SEGMENT_SIZE * 4.0F))) + SEGMENT_SIZE * 2.0F;
		float y = (float)(SDL_rand((Sint32)(WORLD_H - SEGMENT_SIZE * 4.0F))) + SEGMENT_SIZE * 2.0F;
		if(SDL_fabsf(start_position.x - x) > 1000.0F && SDL_fabsf(start_position.y - y) > 1000.0F){//not too close
			Segment* s = GetSegment(&g_d->world, x, y);
			if(s != NULL && s->beings.num < MAX_SEGM_BEINGS){
				AddBeingToArray(&g_d->beings, (unsigned int)SDL_rand(2), x, y, s);
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

extern inline float SegmentPositionX(Segment* const s){
	return s->indx.x * SEGMENT_SIZE;
}

extern inline float SegmentPositionY(Segment* const s){
	return s->indx.y * SEGMENT_SIZE;
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