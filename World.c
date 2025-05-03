#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>

World* createWorld(const float x, const float y) {
	World* w = (World*)SDL_malloc(sizeof(World));
	if (w == NULL) return NULL;
	w->width = x;
	w->height = y;
	for (unsigned int c = 0U; c < SEGMENTS_X; ++c) {
		for (unsigned int r = 0U; r < SEGMENTS_Y; ++r) {
			(*(*(w->segments + c) + r)).coordinates.x = (float)(c * SEGMENTS_SIZE) + BOUNDS_L;
			(*(*(w->segments + c) + r)).coordinates.y = (float)(r * SEGMENTS_SIZE) + BOUNDS_U;
			(*(*(w->segments + c) + r)).contain_player = false;
			(*(*(w->segments + c) + r)).beings.num = 0U;
			(*(*(w->segments + c) + r)).beings.array = (Being**)SDL_malloc(sizeof(Being*) * MAX_SEGM_BEINGS);
		}
	}
	w->visible_rect.x = 0.0F;
	w->visible_rect.y = 0.0F;
	w->visible_rect.w = 1.0F;
	w->visible_rect.h = 1.0F;
	return w;
}

extern inline void destroyWorld(World* w) {
	for (unsigned int c = 0U; c < SEGMENTS_X; ++c) {
		for (unsigned int r = 0U; r < SEGMENTS_Y; ++r) {
			SDL_free((*(*(w->segments + c) + r)).beings.array);
		}
	}
	SDL_free(w);
}

extern inline Segment* getSegment(World* w, const float x, const float y) {
	unsigned int c = (unsigned int)((x - BOUNDS_L) / SEGMENTS_SIZE);
	unsigned int r = (unsigned int)((y - BOUNDS_U) / SEGMENTS_SIZE);
	return *(w->segments + c) + r;
}

void setVisibleRect(World* w, Player* p) {//void setVisibleRect(World* w, Player* p, SDL_Renderer* r) {
	//SDL_FPoint edge_points[4];
	//edge_points->x = VIEW_CENTER_X + SDL_sinf(p->direction - SDL_PI_F * 0.25F) * 1000.0F;
	//edge_points->y = VIEW_CENTER_Y - SDL_cosf(p->direction - SDL_PI_F * 0.25F) * 1000.0F;
	//(edge_points + 1)->x = VIEW_CENTER_X + SDL_sinf(p->direction + SDL_PI_F * 0.25F) * 1000.0F;
	//(edge_points + 1)->y = VIEW_CENTER_Y - SDL_cosf(p->direction + SDL_PI_F * 0.25F) * 1000.0F;
	//(edge_points + 2)->x = VIEW_CENTER_X + SDL_sinf(p->direction + SDL_PI_F * 0.625F) * 500.0F;
	//(edge_points + 2)->y = VIEW_CENTER_Y - SDL_cosf(p->direction + SDL_PI_F * 0.625F) * 500.0F;
	//(edge_points + 3)->x = VIEW_CENTER_X + SDL_sinf(p->direction - SDL_PI_F * 0.625F) * 500.0F;
	//(edge_points + 3)->y = VIEW_CENTER_Y - SDL_cosf(p->direction - SDL_PI_F * 0.625F) * 500.0F;
	//SDL_GetRectEnclosingPointsFloat(edge_points, 4, NULL, &w->visible_rect);
	//SDL_RenderLines(r, edge_points, 4);
	//SDL_RenderLine(r, edge_points->x, edge_points->y, (edge_points + 3)->x, (edge_points + 3)->y);

	SDL_FPoint edge_points[3];
	edge_points->x = p->position.x + SDL_sinf(p->direction - SIGHT_ANGLE) * SIGHT;
	edge_points->y = p->position.y - SDL_cosf(p->direction - SIGHT_ANGLE) * SIGHT;
	(edge_points + 1)->x = p->position.x + SDL_sinf(p->direction + SIGHT_ANGLE) * SIGHT;
	(edge_points + 1)->y = p->position.y - SDL_cosf(p->direction + SIGHT_ANGLE) * SIGHT;
	(edge_points + 2)->x = p->position.x + SDL_sinf(p->direction + SIGHT_BACK_ANGLE) * SIGHT_BACK;
	(edge_points + 2)->y = p->position.y - SDL_cosf(p->direction + SIGHT_BACK_ANGLE) * SIGHT_BACK;
	SDL_GetRectEnclosingPointsFloat(edge_points, 3, NULL, &w->visible_rect);



	//SDL_RenderLines(r, edge_points, 3);
	//SDL_RenderLine(r, edge_points->x, edge_points->y, (edge_points + 2)->x, (edge_points + 2)->y);
	//SDL_RenderRect(r, &w->visible_rect);
}
