#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Being.h>
#include <World.h>
#include <function.h>
#include <Player.h>

inline void AddBeingToSegment(Segment* s, Being* b) {
    b->segment = s;
    *(s->beings.array + s->beings.num) = b;
    b->indx = s->beings.num;
    ++s->beings.num;
}

// Being* CreateBeing(const float x, const float y) {
//     Being* b = (Being*)SDL_malloc(sizeof(Being));
//     if (b == NULL) return NULL;
//     // b->velocity = PLAYER_VELOCITY * 1.875F;
//     b->velocity = PLAYER_VELOCITY * 2.125F;
//     b->position.x = x;
//     b->position.y = y;
//     AddBeingToSegment(GetSegment(x, y), b);
//     b->hit_points = 100;
//     //b->flank = (bool)SDL_rand(2);
//     b->walk.time_left = 0;
//     return b;
// }

inline void RemoveBeingFromSegment(Being* b) {
    if (b->indx != b->segment->beings.num - 1) {
        *(b->segment->beings.array + b->indx) = *(b->segment->beings.array + b->segment->beings.num - 1);
        (*(b->segment->beings.array + b->indx))->indx = b->indx;
    }
    --b->segment->beings.num;
}

// inline void DestroyBeing(Being* b) {
//     RemoveBeingFromSegment(b);
// 	SDL_free(b);
// }

void DestroyBeings(Beings_array* bs) {
    // for (unsigned int i = 0U; i < bs->num; ++i) {
    //     Being* b = (bs->array + i);
    //     SDL_free(b);
    // }
    SDL_free(bs->array);
    bs->num = 0U;
}

// extern inline void AddBeingToArray(Beings_array* bs, Being* b) {
extern inline void AddBeingToArray(Beings_array* bs, const float x, const float y) {
    Being* b = (bs->array + bs->num);
    b->position.x = x;
    b->position.y = y;
    b->velocity = PLAYER_VELOCITY * 2.125F;
    b->position.x = x;
    b->position.y = y;
    AddBeingToSegment(GetSegment(x, y), b);
    b->hit_points = 100;
    b->walk.time_left = 0;
    ++bs->num;
}

inline void DestroyBeingInArray(Beings_array* bs, const unsigned int indx) {
    // SDL_free(*(bs->array + indx));
    if (indx != bs->num - 1) {
        *(bs->array + indx) = *(bs->array + bs->num - 1);
    }
    --bs->num;
}

inline void MoveBeing(Being* b, const float x, const float y) {
    b->position.x += x;
    b->position.y += y;
}

inline void SetBeingPosition(Being* b, const float x, const float y) {
    b->position.x = x;
    b->position.y = y;
}

inline void MoveBeingX(Being* b, const float x) {
    b->position.x += x;
}

inline void MoveBeingY(Being* b, const float y) {
    b->position.y += y;
}

inline void MoveBeingToSegment(Being* b, Segment* s) {
    RemoveBeingFromSegment(b);
    AddBeingToSegment(s, b);
}

inline void StartBeingWalk(Being* b, const int time, const float x_shift, const float y_shift) {
    b->walk.time_left = time;
    b->walk.shift.x = x_shift;
    b->walk.shift.y = y_shift;
}

inline void StartBeingWalkWithRandTurn(Being* b, const int time, const float x_shift, const float y_shift) {

    if ((bool)SDL_rand(2)) {

        StartBeingWalk(b, time, -y_shift, x_shift);
    }
    else {

        StartBeingWalk(b, time, y_shift, -x_shift);
    }
}

inline void StartBeingWalkWithRandTurn45Deg(Being* b, const int time, const float x_shift, const float y_shift) {

    if ((bool)SDL_rand(2)) {

        StartBeingWalk(b, time, (x_shift + y_shift) * SQRT2DIV2, (y_shift - x_shift) * SQRT2DIV2);
    }
    else {

        StartBeingWalk(b, time, (x_shift - y_shift) * SQRT2DIV2, (y_shift + x_shift) * SQRT2DIV2);
    }
}

extern inline bool CollideWithBeing(Being* b, const float x, const float y){
    // if (SDL_fabsf(x - b->position.x) < (float)PLAYER_SIZE) {
    //     if (SDL_fabsf(y - b->position.y) < (float)PLAYER_SIZE) {
    //         return true;
    //     }
    // }
    if(SDL_sqrtf((x - b->position.x) * (x - b->position.x) + (y - b->position.y) * (y - b->position.y)) < (float)PLAYER_SIZE){
        return true;
    }
    return false;
}

bool ResolveBeingCollisionInNewSegment(Being* b, Segment* s, float* new_x, float* new_y, const float x_shift, const float y_shift) {

    for (unsigned int i = 0U; i < s->beings.num; ++i) {

        Being* b2 = *(s->beings.array + i);

        if (b2 == b) continue;

        if(CollideWithBeing(b2, *new_x, *new_y)){
            StartBeingWalkWithRandTurn45Deg(b, 128, x_shift, y_shift);
            *new_x = b->position.x + b->walk.shift.x;
            *new_y = b->position.y + b->walk.shift.y;
            return true;
        }

        // float dist_x = *new_x - b2->position.x;

        // if (SDL_fabsf(dist_x) < (float)PLAYER_SIZE) {

        //     float dist_y = *new_y - b2->position.y;

        //     if (SDL_fabsf(dist_y) < (float)PLAYER_SIZE) {

        //         StartBeingWalkWithRandTurn45Deg(b, 128, x_shift, y_shift);
        //         *new_x = b->position.x + b->walk.shift.x;
        //         *new_y = b->position.y + b->walk.shift.y;
        //         return true;
        //     }
        // }
    }
    return false;
}

inline void TurnBeingWalk(Being* b) {
    float tmp_x = b->walk.shift.x;
    if ((bool)SDL_rand(2)) {

        b->walk.shift.x = -b->walk.shift.y;
        b->walk.shift.y = tmp_x;
    }
    else {

        b->walk.shift.x = b->walk.shift.y;
        b->walk.shift.y = -tmp_x;
    }
}

void UpdateBeingWalk(Being* b) {

    if (b->walk.time_left < 0) {
        ++b->walk.time_left;
        return;
    }
    float new_x = b->position.x + b->walk.shift.x;
    float new_y = b->position.y + b->walk.shift.y;
    // SetInBounds(&new_x, &new_y);
    Segment* new_segment = GetSegment(new_x, new_y);

    if (new_segment != b->segment) {
        if (!new_segment->available || new_segment->beings.num >= MAX_SEGM_BEINGS) {
            TurnBeingWalk(b);
            return;
        }
    }
    SetBeingPosition(b, new_x, new_y);
    MoveBeingToSegment(b, new_segment);
    --b->walk.time_left;
}

void UpdateBeings(Beings_array* bs, Player* p, Segment* player_seg) {

    // Segment* player_seg = GetSegment(player_position->x, player_position->y);

    for (unsigned int i = 0U; i < bs->num; ++i) {

        Being* b = (bs->array + i);

        if (b->hit_points <= 0) {
            DestroyBeingInArray(bs, i);
            --i;
            continue;
        }

        if (SDL_abs(player_seg->indx.x - b->segment->indx.x) > RANGE_SEGMENTS) {
            continue;
        }
        if (SDL_abs(player_seg->indx.y - b->segment->indx.y) > RANGE_SEGMENTS) {
            continue;
        }
        float distance_x = p->position.x - b->position.x;
        float distance_y = p->position.y - b->position.y;
        float distance = SDL_sqrtf(distance_x * distance_x + distance_y * distance_y);
        float velocity_xy;
        
        if (distance < 75.0F) {
            if(distance < PLAYER_SIZE){
                velocity_xy = distance / (PLAYER_VELOCITY * 1.5F);
                // player_position->x = player_position->x + distance_x / velocity_xy;
                // player_position->y = player_position->y + distance_y / velocity_xy;
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
        Segment* new_segment = GetSegment(new_x, new_y);
        bool collision = false;

        if (distance < 768.0F) {

            collision = ResolveBeingCollisionInNewSegment(b, new_segment, &new_x, &new_y, x_shift, y_shift);
        }
        // if (SetInBounds(&new_x, &new_y) || collision) {
        if (collision) {

            new_segment = GetSegment(new_x, new_y);
        }

        if (new_segment != b->segment) {
            if (new_segment->available){
                if (new_segment->beings.num >= MAX_SEGM_BEINGS) {
                    if ((bool)SDL_rand(2)) {
                        StartBeingWalkWithRandTurn(b, 128, x_shift * 0.5F, y_shift * 0.5F);
                    }
                    else {
                        StartBeingWalkWithRandTurn(b, -128, 0.0F, 0.0F);
                    }
                    continue;
                }
            }else{
                if ((bool)SDL_rand(2)) {
                    StartBeingWalkWithRandTurn(b, 128, x_shift * 0.5F, y_shift * 0.5F);
                }
                else {
                    StartBeingWalkWithRandTurn(b, -128, 0.0F, 0.0F);
                }
                continue;
            }
        }
        SetBeingPosition(b, new_x, new_y);
        MoveBeingToSegment(b, new_segment);
    }
}

extern inline void DamageBeing(Being* b, int damage) {
    b->hit_points -= damage;
    if(b->hit_points < 1){
        RemoveBeingFromSegment(b);
    }
}
