#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <World.h>
#include <function.h>
#include <Player.h>
#include <Being.h>

void CreateWorld(World* const world, const float x, const float y){
	const bool test_world_plan[BIG_SEGMENTS_X][BIG_SEGMENTS_X] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0,
		0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
		0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0,
		0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
		0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};
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
			if(*(*(test_world_plan + bigc) + bigr) == true){
				for(unsigned int c = bigc * BIG_SEGMENT_SEGMENTS_X + 1U; c < bigc * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X + 1U; ++c){
					for(unsigned int r = bigr * BIG_SEGMENT_SEGMENTS_X + 1U; r < bigr * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X + 1U; ++r){
						if(SDL_randf() < 0.1F){
							*(*(world->segments + c) + r) = NULL;
						}else{
							*(*(world->segments + c) + r) = (Segment*)SDL_malloc(sizeof(Segment));
							(*(*(world->segments + c) + r))->indx.x = c;
							(*(*(world->segments + c) + r))->indx.y = r;
							(*(*(world->segments + c) + r))->beings.num = 0U;
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
			}
		}
	}
	g_d->beings.num = 0U;
	CreatePlayer(&g_d->pc, WORLD_W / 2.0F, WORLD_H - SEGMENT_SIZE * 2.0F);//test staring position
	AddBeingToArray(&g_d->beings, WORLD_W / 2.0F, WORLD_H - SEGMENT_SIZE * 8.0F, GetSegment(&g_d->world, WORLD_W / 2.0F, WORLD_H - SEGMENT_SIZE * 8.0F));//test being
	Uint64 start_time = SDL_GetTicks();
	while (g_d->beings.num < MAX_BEINGS_NUM){//test beings
		float x = (float)(SDL_rand((Sint32)(WORLD_W - SEGMENT_SIZE * 4.0F))) + SEGMENT_SIZE * 2.0F;
		float y = (float)(SDL_rand((Sint32)(WORLD_H - SEGMENT_SIZE * 4.0F))) + SEGMENT_SIZE * 2.0F;
		if(SDL_fabsf(g_d->pc.position.x - x) > 1000.0F && SDL_fabsf(g_d->pc.position.y - y) > 1000.0F){//not too close
			Segment* s = GetSegment(&g_d->world, x, y);
			if(s != NULL && s->beings.num < MAX_SEGM_BEINGS){
				AddBeingToArray(&g_d->beings, x, y, s);
			}
		}
		if(SDL_GetTicks() - start_time > 1500U) break;
	}
}