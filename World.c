#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <World.h>
#include <function.h>

void CreateWorld(const float x, const float y) {
	world.width = x;
	world.height = y;
	world.segments = (Segment***)SDL_malloc(sizeof(Segment**) * SEGMENTS_X);
	for (unsigned int c = 0U; c < SEGMENTS_X; ++c) {
		*(world.segments + c) = (Segment**)SDL_malloc(sizeof(Segment*) * SEGMENTS_Y);
		for (unsigned int r = 0U; r < SEGMENTS_Y; ++r) {
			if(c == 0U || r == 0U || c == SEGMENTS_X - 1U || r == SEGMENTS_Y - 1U){
				*(*(world.segments + c) + r) = NULL;
			}else{
				*(*(world.segments + c) + r) = (Segment*)SDL_malloc(sizeof(Segment));
				(*(*(world.segments + c) + r))->indx.x = c;
				(*(*(world.segments + c) + r))->indx.y = r;
				(*(*(world.segments + c) + r))->beings.num = 0U;
			}
		}
	}

	world.sin_player_direction = 0.0F;
	world.cos_player_direction = 0.0F;
}

extern inline void DestroyWorld() {
	int tmp;
	for (unsigned int c = 0U; c < SEGMENTS_X; ++c) {
	for (unsigned int r = 0U; r < SEGMENTS_Y; ++r) {
			SDL_free(*(*(world.segments + c) + r));
		}
		SDL_free(*(world.segments + c));
	}
	SDL_free(world.segments);
}

extern inline Segment* GetSegment(const float x, const float y) {
	unsigned int c = (unsigned int)(x / SEGMENTS_SIZE);
	unsigned int r = (unsigned int)(y / SEGMENTS_SIZE);
	return *(*(world.segments + c) + r);
}

extern inline Segment* GetSegmentByIndx(const unsigned int c, const unsigned int r) {
	return *(*(world.segments + c) + r);
}

extern inline void SetSineCosine(Player* const p) {
	world.sin_player_direction = sine(p->direction);
	world.cos_player_direction = cosi(p->direction);
}

extern inline void UpdateSegmentBeingPointer(Being* old, Being* new){
	*(old->segment->beings.array + old->indx) = new;
}