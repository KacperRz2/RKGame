#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Being.h>
#include <World.h>
#include <function.h>
#include <Player.h>

inline void AddBeingToSegment(Segment* const s, Being* const b) {
    b->segment = s;
    *(s->beings.array + s->beings.num) = b;
    b->indx = s->beings.num;
    ++s->beings.num;
}

inline void RemoveBeingFromSegment(Being* const b) {
    if (b->indx != b->segment->beings.num - 1) {
        *(b->segment->beings.array + b->indx) = *(b->segment->beings.array + b->segment->beings.num - 1);
        (*(b->segment->beings.array + b->indx))->indx = b->indx;
    }
    --b->segment->beings.num;
}

void DestroyBeings(Beings_array* const bs) {
    SDL_free(bs->array);
    bs->num = 0U;
}

extern inline void AddBeingToArray(Beings_array* const bs, const float x, const float y) {
    Being* b = (bs->array + bs->num);
    b->position.x = x;
    b->position.y = y;
    b->velocity = PLAYER_VELOCITY * 2.5F;
    AddBeingToSegment(GetSegment(x, y), b);
    b->hit_points = 100;
    b->walk.time_left = 0;
    ++bs->num;
}

inline void DestroyBeingInArray(Beings_array* const bs, const unsigned int indx) {
    if (indx != bs->num - 1) {
        *(bs->array + indx) = *(bs->array + bs->num - 1);
    }
    --bs->num;
}

inline void MoveBeing(Being* const b, const float x, const float y) {
    b->position.x += x;
    b->position.y += y;
}

inline void SetBeingPosition(Being* const b, const float x, const float y) {
    b->position.x = x;
    b->position.y = y;
}

inline void MoveBeingX(Being* const b, const float x) {
    b->position.x += x;
}

inline void MoveBeingY(Being* const b, const float y) {
    b->position.y += y;
}

inline void MoveBeingToSegment(Being* const b, Segment* const s) {
    RemoveBeingFromSegment(b);
    AddBeingToSegment(s, b);
}

inline void StartBeingWalk(Being* const b, const int time, const float x_shift, const float y_shift) {
    b->walk.time_left = time;
    b->walk.shift.x = x_shift;
    b->walk.shift.y = y_shift;
}

inline void StartBeingWalkWithRandTurn(Being* const b, const int time, const float x_shift, const float y_shift) {

    if ((bool)SDL_rand(2)) {

        StartBeingWalk(b, time, -y_shift, x_shift);
    }
    else {

        StartBeingWalk(b, time, y_shift, -x_shift);
    }
}

inline void StartBeingWalkWithRandTurn45Deg(Being* const b, const int time, const float x_shift, const float y_shift) {

    if ((bool)SDL_rand(2)) {

        StartBeingWalk(b, time, (x_shift + y_shift) * SQRT2DIV2, (y_shift - x_shift) * SQRT2DIV2);
    }
    else {

        StartBeingWalk(b, time, (x_shift - y_shift) * SQRT2DIV2, (y_shift + x_shift) * SQRT2DIV2);
    }
}

extern inline bool CollideWithBeing(Being* const b, const float x, const float y){
    if(SDL_sqrtf((x - b->position.x) * (x - b->position.x) + (y - b->position.y) * (y - b->position.y)) < (float)PLAYER_SIZE){
        return true;
    }
    return false;
}

bool ResolveBeingCollisionInNewSegment(Being* const b, Segment* const s, float* const new_x, float* const new_y, const float x_shift, const float y_shift) {

    for (unsigned int i = 0U; i < s->beings.num; ++i) {

        Being* b2 = *(s->beings.array + i);

        if (b2 == b) continue;

        if(CollideWithBeing(b2, *new_x, *new_y)){
            StartBeingWalkWithRandTurn45Deg(b, 128, x_shift, y_shift);
            *new_x = b->position.x + b->walk.shift.x;
            *new_y = b->position.y + b->walk.shift.y;
            return true;
        }
    }
    return false;
}

inline void TurnBeingWalk(Being* const b) {
    float tmp_x = b->walk.shift.x;
    if ((bool)SDL_rand(2)) {
        b->walk.shift.x = -b->walk.shift.y;
        b->walk.shift.y = tmp_x;
    }else {
        b->walk.shift.x = b->walk.shift.y;
        b->walk.shift.y = -tmp_x;
    }
}

void UpdateBeingWalk(Being* const b) {

    if (b->walk.time_left < 0) {
        ++b->walk.time_left;
        return;
    }
    float new_x = b->position.x + b->walk.shift.x;
    float new_y = b->position.y + b->walk.shift.y;
    Segment* new_segment = GetSegment(new_x, new_y);

    if (new_segment != b->segment && (new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS)) {
        TurnBeingWalk(b);
        return;
    }
    SetBeingPosition(b, new_x, new_y);
    MoveBeingToSegment(b, new_segment);
    --b->walk.time_left;
}

void UpdateBeings(Beings_array* const bs, Player* const p, Segment* const player_seg) {
    for (Being* b = bs->array; b != (bs->array + bs->num); ++b) {

        if (b->hit_points <= 0) {
            *b = *(bs->array + bs->num-- - 1U);
            --b;
            continue;
        }
        // while(b->hit_points <= 0) {
        //     *b = *(bs->array + bs->num-- - 1U);
        // }
        // for (unsigned int i = 0U; i < bs->num; ++i) {
        // Being* b = (bs->array + i);
        // if (b->hit_points <= 0) {
        //     DestroyBeingInArray(bs, i);
        //     --i;
        //     continue;
        // }
        float distance_x = p->position.x - b->position.x;
        float distance_y = p->position.y - b->position.y;
        float distance = SDL_sqrtf(distance_x * distance_x + distance_y * distance_y);
        float velocity_xy;
        
        if (distance < 70.0F) {
            if(distance < PLAYER_SIZE){
                velocity_xy = distance / (PLAYER_VELOCITY * 1.875F);
                MovePlayer(p, distance_x / velocity_xy, distance_y / velocity_xy);
            }
            continue;
        }
        if (b->walk.time_left) {

            UpdateBeingWalk(b);
            continue;
        }
        velocity_xy = distance / b->velocity;
        float x_shift = distance_x / velocity_xy;
        float y_shift = distance_y / velocity_xy;
        float new_x = b->position.x + x_shift;
        float new_y = b->position.y + y_shift;
        if (distance > RANGE_SEGMENTS * (WORLD_SIZE / SEGMENTS_X)) {
            SetBeingPosition(b, new_x, new_y);
            continue;
        }
        Segment* new_segment = GetSegment(new_x, new_y);
        // if (SDL_abs(player_seg->indx.x - b->segment->indx.x) > RANGE_SEGMENTS || SDL_abs(player_seg->indx.y - b->segment->indx.y) > RANGE_SEGMENTS) {
        bool collision = false;
        if(new_segment == NULL){
            StartBeingWalkWithRandTurn45Deg(b, 128, x_shift, y_shift);
            new_x = b->position.x + b->walk.shift.x;
            new_y = b->position.y + b->walk.shift.y;
            collision = true;
        }else if (distance < 768.0F) {

            collision = ResolveBeingCollisionInNewSegment(b, new_segment, &new_x, &new_y, x_shift, y_shift);
        }
        if (collision) {

            new_segment = GetSegment(new_x, new_y);
        }

        if (new_segment != b->segment) {
            if (new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS) {
                if ((bool)SDL_rand(2)) {
                    StartBeingWalkWithRandTurn(b, 128, x_shift * 0.5F, y_shift * 0.5F);
                }else {
                    StartBeingWalkWithRandTurn(b, -128, 0.0F, 0.0F);
                }
                continue;
            }
        }else{
            SetBeingPosition(b, new_x, new_y);
            continue;
        };
        SetBeingPosition(b, new_x, new_y);
        MoveBeingToSegment(b, new_segment);
    }
}

extern inline void DamageBeing(Being* const b, int damage) {
    b->hit_points -= damage;
    if(b->hit_points < 1){
        RemoveBeingFromSegment(b);
    }
}

// void UpdateAndRenderBeings(Beings_array* const bs, Player* const p, Segment* const player_seg, SDL_Renderer* const rend, SDL_Texture* const tx){
//     static const SDL_FRect visible_rect = {
// 		0.0F,
// 		0.0F,
// 		VIEWFINDER,
// 		VIEWFINDER
// 	};
// 	static SDL_FRect rect = {
// 		0.0F,
// 		0.0F,
// 		PLAYER_SIZE,
// 		PLAYER_SIZE
// 	};
//     for (Being* b = bs->array; b != (bs->array + bs->num); ++b) {

//         if (b->hit_points <= 0) {
//             *b = *(bs->array + bs->num-- - 1U);
//             --b;
//             continue;
//         }
//         float distance_x = p->position.x - b->position.x;
//         float distance_y = p->position.y - b->position.y;
//         float distance = SDL_sqrtf(distance_x * distance_x + distance_y * distance_y);
//         float velocity_xy;
        
//         if (distance < 70.0F) {
//             if(distance < PLAYER_SIZE){
//                 velocity_xy = distance / (PLAYER_VELOCITY * 1.875F);
//                 MovePlayer(p, distance_x / velocity_xy, distance_y / velocity_xy);
//             }
//             rect.x = VIEWFINDER_CENTER + (-distance_x * world.cos_player_direction - distance_y * world.sin_player_direction) - PLAYER_SIZE * 0.5F;
//             rect.y = VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - (-distance_x * world.sin_player_direction + distance_y * world.cos_player_direction) - PLAYER_SIZE * 0.5F;
//             SDL_RenderTexture(rend, tx, NULL, &rect);
//             continue;
//         }
//         if (b->walk.time_left) {

//             UpdateBeingWalk(b);
//             if(distance < VIEWFINDER){
//                 rect.x = VIEWFINDER_CENTER + (-distance_x * world.cos_player_direction - distance_y * world.sin_player_direction) - PLAYER_SIZE * 0.5F;
//                 rect.y = VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - (-distance_x * world.sin_player_direction + distance_y * world.cos_player_direction) - PLAYER_SIZE * 0.5F;
//                 if (SDL_HasRectIntersectionFloat(&rect, &visible_rect)) {
//                     SDL_RenderTexture(rend, tx, NULL, &rect);
//                 }
//             }
//             continue;
//         }
//         velocity_xy = distance / b->velocity;
//         float x_shift = distance_x / velocity_xy;
//         float y_shift = distance_y / velocity_xy;
//         float new_x = b->position.x + x_shift;
//         float new_y = b->position.y + y_shift;
//         if (distance > RANGE_SEGMENTS * (WORLD_SIZE / SEGMENTS_X)) {
//             SetBeingPosition(b, new_x, new_y);
//             continue;
//         }
//         Segment* new_segment = GetSegment(new_x, new_y);
//         bool collision = false;
//         if(new_segment == NULL){
//             StartBeingWalkWithRandTurn45Deg(b, 128, x_shift, y_shift);
//             new_x = b->position.x + b->walk.shift.x;
//             new_y = b->position.y + b->walk.shift.y;
//             collision = true;
//         }else if (distance < 768.0F) {

//             collision = ResolveBeingCollisionInNewSegment(b, new_segment, &new_x, &new_y, x_shift, y_shift);
//         }
//         if (collision) {

//             new_segment = GetSegment(new_x, new_y);
//         }

//         if (new_segment != b->segment) {
//             if (new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS) {
//                 if ((bool)SDL_rand(2)) {
//                     StartBeingWalkWithRandTurn(b, 128, x_shift * 0.5F, y_shift * 0.5F);
//                 }else {
//                     StartBeingWalkWithRandTurn(b, -128, 0.0F, 0.0F);
//                 }
//                 if(distance < VIEWFINDER){
//                     rect.x = VIEWFINDER_CENTER + (-distance_x * world.cos_player_direction - distance_y * world.sin_player_direction) - PLAYER_SIZE * 0.5F;
//                     rect.y = VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - (-distance_x * world.sin_player_direction + distance_y * world.cos_player_direction) - PLAYER_SIZE * 0.5F;
//                     if (SDL_HasRectIntersectionFloat(&rect, &visible_rect)) {
//                         SDL_RenderTexture(rend, tx, NULL, &rect);
//                     }
//                 }
//                 continue;
//             }
//         }else{
//             SetBeingPosition(b, new_x, new_y);
//             if(distance < VIEWFINDER){
//                 rect.x = VIEWFINDER_CENTER + (-distance_x * world.cos_player_direction - distance_y * world.sin_player_direction) - PLAYER_SIZE * 0.5F;
//                 rect.y = VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - (-distance_x * world.sin_player_direction + distance_y * world.cos_player_direction) - PLAYER_SIZE * 0.5F;
//                 if (SDL_HasRectIntersectionFloat(&rect, &visible_rect)) {
//                     SDL_RenderTexture(rend, tx, NULL, &rect);
//                 }
//             }
//             continue;
//         };
//         SetBeingPosition(b, new_x, new_y);
//         MoveBeingToSegment(b, new_segment);
//         if(distance < VIEWFINDER){
//             rect.x = VIEWFINDER_CENTER + (-distance_x * world.cos_player_direction - distance_y * world.sin_player_direction) - PLAYER_SIZE * 0.5F;
//             rect.y = VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - (-distance_x * world.sin_player_direction + distance_y * world.cos_player_direction) - PLAYER_SIZE * 0.5F;
//             if (SDL_HasRectIntersectionFloat(&rect, &visible_rect)) {
//                 SDL_RenderTexture(rend, tx, NULL, &rect);
//             }
//         }
//     }

    // for (Being* b = bs->array; b != (bs->array + bs->num); ++b) {

	// 	float dx = b->position.x - p->position.x;
	// 	if(SDL_fabsf(dx) < VIEWFINDER){
	// 		float dy = b->position.y - p->position.y;
	// 		if(SDL_fabsf(dy) < VIEWFINDER){

	// 			rect.x = VIEWFINDER_CENTER + (dx * world.cos_player_direction + dy * world.sin_player_direction) - PLAYER_SIZE * 0.5F;
	// 			rect.y = VIEWFINDER_CENTER + PLAYER_REND_Y_SHIFT - (dx * world.sin_player_direction - dy * world.cos_player_direction) - PLAYER_SIZE * 0.5F;
				
	// 			if (SDL_HasRectIntersectionFloat(&rect, &visible_rect)) {
	// 				SDL_RenderTexture(rend, tx, NULL, &rect);
	// 			}
	// 		}
	// 	}
	// }
// }