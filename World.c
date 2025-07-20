#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <World.h>
#include <function.h>
#include <Player.h>
#include <Being.h>

void CreateWorld(World* const world, const float x, const float y){
	world->width = x;
	world->height = y;
	world->segments = (Segment***)SDL_malloc(sizeof(Segment**) * SEGMENTS_X);
	for(unsigned int c = 0U; c < SEGMENTS_X; ++c){
		*(world->segments + c) = (Segment**)SDL_malloc(sizeof(Segment*) * SEGMENTS_Y);
		for(unsigned int r = 0U; r < SEGMENTS_Y; ++r){
			if(c == 0U || r == 0U || c == SEGMENTS_X - 1U || r == SEGMENTS_Y - 1U){
				*(*(world->segments + c) + r) = NULL;
			}else{
				*(*(world->segments + c) + r) = (Segment*)SDL_malloc(sizeof(Segment));
				(*(*(world->segments + c) + r))->indx.x = c;
				(*(*(world->segments + c) + r))->indx.y = r;
				(*(*(world->segments + c) + r))->beings.num = 0U;
			}
		}
	}
}

extern inline void DestroyWorld(World* const world){
	// int tmp;
	for(unsigned int c = 0U; c < SEGMENTS_X; ++c){
	for(unsigned int r = 0U; r < SEGMENTS_Y; ++r){
			SDL_free(*(*(world->segments + c) + r));
		}
		SDL_free(*(world->segments + c));
	}
	SDL_free(world->segments);
}

extern inline Segment* GetSegment(World* const world, const float x, const float y){
	unsigned int c = (unsigned int)(x / SEGMENTS_SIZE);
	unsigned int r = (unsigned int)(y / SEGMENTS_SIZE);
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
				(*(*(g_d->world.segments + c) + r))->beings.num = 0U;
			}
		}
	}

	g_d->beings.num = 0U;
	CreatePlayer(&g_d->pc, WORLD_W / SEGMENTS_X * 2.0F, WORLD_H / SEGMENTS_Y * 2.0F);
	AddBeingToArray(&g_d->beings, WORLD_W / SEGMENTS_X * 3.0F, WORLD_H / SEGMENTS_Y * 3.0F, GetSegment(&g_d->world, WORLD_W / SEGMENTS_X * 3.0F, WORLD_H / SEGMENTS_Y * 3.0F));

	while (g_d->beings.num < MAX_BEINGS_NUM){
		float x = (float)(SDL_rand((Sint32)(WORLD_W - WORLD_W / SEGMENTS_X * 4.0F))) + WORLD_W / SEGMENTS_X * 2.0F;
		float y = (float)(SDL_rand((Sint32)(WORLD_H - WORLD_H / SEGMENTS_Y * 4.0F))) + WORLD_H / SEGMENTS_Y * 2.0F;
		x /= 2.0F;
		y /= 2.0F;
		if(SDL_fabsf(g_d->pc.position.x - x) > 2000.0F && SDL_fabsf(g_d->pc.position.y - y) > 2000.0F){
			Segment* s = GetSegment(&g_d->world, x, y);
			if(s != NULL && s->beings.num < MAX_SEGM_BEINGS){
				AddBeingToArray(&g_d->beings, x, y, s);
			}
		}
	}
}