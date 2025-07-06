#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <World.h>
#include <function.h>

void CreateWorld(const float x, const float y) {
	world = (World*)SDL_malloc(sizeof(World));
	if (world == NULL) return;
	world->width = x;
	world->height = y;
	for (unsigned int c = 0U; c < SEGMENTS_X; ++c) {
		for (unsigned int r = 0U; r < SEGMENTS_Y; ++r) {
			(*(world->segments + c) + r)->indx.x = c;
			(*(world->segments + c) + r)->indx.y = r;
			if(c == 0U || r == 0U || c == SEGMENTS_X - 1U || r == SEGMENTS_Y - 1U){
				(*(world->segments + c) + r)->available = false;
				(*(world->segments + c) + r)->beings.array = NULL;
			}
			else{
				(*(world->segments + c) + r)->available = true;
				(*(world->segments + c) + r)->beings.array = (Being**)SDL_malloc(sizeof(Being*) * MAX_SEGM_BEINGS);
			}
			(*(world->segments + c) + r)->beings.num = 0U;
		}
	}

	world->sin_player_direction = 0.0F;
	world->cos_player_direction = 0.0F;
}

extern inline void DestroyWorld() {
	for (unsigned int c = 0U; c < SEGMENTS_X; ++c) {
		for (unsigned int r = 0U; r < SEGMENTS_Y; ++r) {
			SDL_free((*(world->segments + c) + r)->beings.array);
		}
	}
	SDL_free(world);
}

extern inline Segment* GetSegment(const float x, const float y) {
	unsigned int c = (unsigned int)(x / SEGMENTS_SIZE);
	unsigned int r = (unsigned int)(y / SEGMENTS_SIZE);
	return *(world->segments + c) + r;
}

extern inline Segment* GetSegmentByIndx(const unsigned int c, const unsigned int r) {
	return *(world->segments + c) + r;
}

// extern inline Segment* GetSegmentColumnAndRow(const float x, const float y, unsigned int* column, unsigned int* row) {
// 	*column = (unsigned int)((x - BOUNDS_L) / SEGMENTS_SIZE);
// 	*row = (unsigned int)((y - BOUNDS_U) / SEGMENTS_SIZE);
// 	return *(world->segments + *column) + *row;
// }

extern inline void SetSineCosine(Player* p) {
	world->sin_player_direction = sine(p->direction);
	world->cos_player_direction = cosi(p->direction);
}
