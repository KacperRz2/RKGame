#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Being.h>
#include <World.h>
#include <function.h>

inline void addBeingToSegment(Segment* s, Being* b) {
    b->segment = s;
    *(s->beings.array + s->beings.num) = b;
    b->indx = s->beings.num;
    ++s->beings.num;
}

Being* createBeing(World* w, const float x, const float y) {
    Being* b = (Being*)SDL_malloc(sizeof(Being));
    if (b == NULL) return NULL;
    b->velocity = PLAYER_VELOCITY * 1.875F;
    b->position.x = x;
    b->position.y = y;
    addBeingToSegment(getSegment(w, x, y), b);
    b->hit_points = 100;
    //b->flank = (bool)SDL_rand(2);
    b->walk.time_left = 0;
    return b;
}

inline void removeBeingFromSegment(Being* b) {
    if (b->indx != ((Segment*)(b->segment))->beings.num - 1) {
        *(((Segment*)(b->segment))->beings.array + b->indx) = *(((Segment*)(b->segment))->beings.array + ((Segment*)(b->segment))->beings.num - 1);
        (*(((Segment*)(b->segment))->beings.array + b->indx))->indx = b->indx;
    }
    --((Segment*)(b->segment))->beings.num;
}

inline void destroyBeing(Being* b) {
    removeBeingFromSegment(b);
	SDL_free(b);
}

void destroyBeings(Beings_array* bs) {
    for (unsigned int i = 0U; i < bs->num; ++i) {
        Being* b = *(bs->array + i);
        destroyBeing(b);
    }
    bs->num = 0U;
}

extern inline void addBeingToArray(Beings_array* bs, Being* b) {
    *(bs->array + bs->num) = b;
    ++bs->num;
}

inline void destroyBeingInArray(Beings_array* bs, const unsigned int indx) {
    destroyBeing(*(bs->array + indx));
    if (indx != bs->num - 1) {
        *(bs->array + indx) = *(bs->array + bs->num - 1);
    }
    --bs->num;
}

inline void moveBeing(Being* b, const float x, const float y) {
    b->position.x += x;
    b->position.y += y;
}

inline void setBeingPosition(Being* b, const float x, const float y) {
    b->position.x = x;
    b->position.y = y;
}

inline void moveBeingX(Being* b, const float x) {
    b->position.x += x;
}

inline void moveBeingY(Being* b, const float y) {
    b->position.y += y;
}

inline void moveBeingToSegment(Being* b, Segment* s) {
    removeBeingFromSegment(b);
    addBeingToSegment(s, b);
}

inline void startBeingWalk(Being* b, const int time, const float x_shift, const float y_shift) {
    b->walk.time_left = time;
    b->walk.shift.x = x_shift;
    b->walk.shift.y = y_shift;
}

inline void startBeingWalkWithRandTurn(Being* b, const int time, const float x_shift, const float y_shift) {

    if ((bool)SDL_rand(2)) {

        startBeingWalk(b, time, -y_shift, x_shift);
    }
    else {

        startBeingWalk(b, time, y_shift, -x_shift);
    }
}

inline void startBeingWalkWithRandTurn45Deg(Being* b, const int time, const float x_shift, const float y_shift) {

    if ((bool)SDL_rand(2)) {

        startBeingWalk(b, time, (x_shift + y_shift) * SQRT2DIV2, (y_shift - x_shift) * SQRT2DIV2);
    }
    else {

        startBeingWalk(b, time, (x_shift - y_shift) * SQRT2DIV2, (y_shift + x_shift) * SQRT2DIV2);
    }
}

bool resolveBeingCollisionInNewSegment(Being* b, Segment* s, float* new_x, float* new_y, const float x_shift, const float y_shift) {

    for (unsigned int i = 0U; i < s->beings.num; ++i) {

        Being* b2 = *(s->beings.array + i);

        if (b2 == b) continue;

        float dist_x = *new_x - b2->position.x;

        if (SDL_fabsf(dist_x) < (float)PLAYER_SIZE) {

            float dist_y = *new_y - b2->position.y;

            if (SDL_fabsf(dist_y) < (float)PLAYER_SIZE) {

                startBeingWalkWithRandTurn45Deg(b, 128, x_shift, y_shift);
                *new_x = b->position.x + b->walk.shift.x;
                *new_y = b->position.y + b->walk.shift.y;
                return true;
            }
        }
    }
    return false;
}

inline void turnBeingWalk(Being* b) {
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

void updateBeingWalk(Being* b, World* w) {

    if (b->walk.time_left < 0) {
        ++b->walk.time_left;
        return;
    }
    float new_x = b->position.x + b->walk.shift.x;
    float new_y = b->position.y + b->walk.shift.y;
    setInBounds(&new_x, &new_y);
    Segment* new_segment = getSegment(w, new_x, new_y);

    if (new_segment != b->segment) {
        if (new_segment->beings.num >= MAX_SEGM_BEINGS) {
            turnBeingWalk(b);
            return;
        }
    }
    setBeingPosition(b, new_x, new_y);
    moveBeingToSegment(b, new_segment);
    --b->walk.time_left;
}

void updateBeings(Beings_array* bs, World* w, SDL_FPoint* player_position) {

    for (unsigned int i = 0; i < bs->num; ++i) {

        Being* b = *(bs->array + i);

        if (b->hit_points <= 0) {
            destroyBeingInArray(bs, i);
            --i;
            continue;
        }
        float distance_x = player_position->x - b->position.x;
        float distance_y = player_position->y - b->position.y;
        float distance = SDL_sqrtf(distance_x * distance_x + distance_y * distance_y);

        if (distance < 75.0F) {

            continue;
        }
        if (b->walk.time_left) {

            updateBeingWalk(b, w);
            continue;
        }
        float velocity_xy = distance / b->velocity;
        float x_shift = distance_x / velocity_xy;
        float y_shift = distance_y / velocity_xy;
        float new_x = b->position.x + x_shift;
        float new_y = b->position.y + y_shift;
        Segment* new_segment = getSegment(w, new_x, new_y);
        bool collision = false;

        if (distance < 768.0F) {

            collision = resolveBeingCollisionInNewSegment(b, new_segment, &new_x, &new_y, x_shift, y_shift);
        }
        if (setInBounds(&new_x, &new_y) || collision) {

            new_segment = getSegment(w, new_x, new_y);
        }

        if (new_segment != b->segment) {

            if (new_segment->beings.num >= MAX_SEGM_BEINGS) {

                if ((bool)SDL_rand(2)) {

                    startBeingWalkWithRandTurn(b, 128, x_shift * 0.5F, y_shift * 0.5F);
                }
                else {

                    startBeingWalkWithRandTurn(b, -128, 0.0F, 0.0F);
                }
                continue;
            }
        }
        setBeingPosition(b, new_x, new_y);
        moveBeingToSegment(b, new_segment);
    }
}

extern inline void damageBeing(Being* b, int damage) {
    b->hit_points -= damage;
}


//inline void setBeingInBounds(Being* b) {
//    if (b->position.x > BOUNDS_R - 1.0F) {
//        b->position.x = BOUNDS_R - 2.0F;
//    }
//    else if (b->position.x < BOUNDS_L) {
//        b->position.x = BOUNDS_L + 1.0F;
//    }
//    if (b->position.y > BOUNDS_D - 1.0F) {
//        b->position.y = BOUNDS_D - 2.0F;
//    }
//    else if (b->position.y < BOUNDS_U) {
//        b->position.y = BOUNDS_U + 1.0F;
//    }
//}

//for (unsigned int i = 0U; i < bs->num; ++i) {
//    Being* b = *(bs->array + i);
//    float distance_x = player_position->x - b->position.x;
//    float distance_y = player_position->y - b->position.y;
//    //float distance_x_squared = distance_x * distance_x;
//    //float distance_y_squared = distance_y * distance_y;
//    //float distance_squared = distance_x_squared + distance_y_squared;
//    float distance = SDL_sqrtf(distance_x * distance_x + distance_y * distance_y);
//    float velocity_xy = distance / b->velocity;
//    float x_shift = distance_x / velocity_xy;
//    float y_shift = distance_y / velocity_xy;
//    float new_x = b->position.x + x_shift;
//    float new_y = b->position.y + y_shift;
//    Segment* new_segment = getSegment(w, new_x, new_y);
//    //moveBeing(b, distance_x / velocity_xy, distance_y / velocity_xy);
//    bool collision = false;
//    if (distance < 1000.0F && distance > 75.0F) {
//        for (unsigned int j = 0; j < new_segment->beings.num; ++j) {
//            Being* b2 = *(new_segment->beings.array + j);
//            if (b2 == b) continue;
//            float dist_x = new_x - b2->position.x;
//            if (SDL_fabsf(dist_x) < (float)PLAYER_SIZE) {
//                float dist_y = new_y - b2->position.y;
//                if (SDL_fabsf(dist_y) < (float)PLAYER_SIZE) {
//                    collision = true;
//
//                    if (b->flank) {
//                        moveBeing(b, -y_shift, x_shift);
//                    }
//                    else {
//                        moveBeing(b, y_shift, -x_shift);
//                    }
//
//                    break;
//                }
//            }
//        }
//    }
//    if (!collision && distance > 75.0F) {
//        setBeingPosition(b, new_x, new_y);
//    }
//    setBeingInBounds(b);
//    moveBeingToSegment(b, w);
//}

//for (unsigned int j = i + 1U; j < bs->num; ++j) {
//    Being* b2 = *(bs->array + j);
//    float dist_x = new_x - b2->position.x;
//    if (SDL_fabsf(dist_x) < (float)PLAYER_SIZE) {
//        float dist_y = new_y - b2->position.y;
//        if (SDL_fabsf(dist_y) < (float)PLAYER_SIZE) {
//            collision = true;
//            if (b->flank) {
//                moveBeing(b, -y_shift, x_shift);
//            }
//            else {
//                moveBeing(b, y_shift, -x_shift);
//            }
//            break;
//        }
//    }
//}

//float distance_x2 = player_position->x - b->position.x;
//float distance_y2 = player_position->y - b->position.y;
//float distance2 = SDL_sqrtf(distance_x2 * distance_x2 + distance_y2 * distance_y2);
//if (distance > distance2) {
//    if (b->flank) {
//        moveBeing(b, -y_shift, x_shift);
//    }
//    else {
//        moveBeing(b, y_shift, -x_shift);
//    }
//}
//else {
//    if (b2->flank) {
//        moveBeing(b2, -y_shift, x_shift);
//    }
//    else {
//        moveBeing(b2, y_shift, -x_shift);
//    }
//}

//float tmp_differ = distance_x_squared - distance_y_squared;
//if (SDL_fabsf(tmp_differ) > 0.5F * distance_squared) {
//    if (distance_x_squared > distance_y_squared) {
//        b->position.x += SDL_copysignf(b->velocity, distance_x);
//    }
//    else {
//        b->position.y += SDL_copysignf(b->velocity, distance_y);
//    }
//}
//else {
//    moveBeing(b, SDL_copysignf(b->velocity / SQRT2, distance_x), SDL_copysignf(b->velocity / SQRT2, distance_y));
//}

//if (distance < 1000.0F) {
//    for (unsigned int j = i + 1; j < bs->num; ++j) {
//        int collisions = 0;
//        Being* b2 = *(bs->array + j);
//        float dist_x = b->position.x - b2->position.x;
//        if (SDL_fabsf(dist_x) < (float)PLAYER_SIZE) {
//            float dist_y = b->position.y - b2->position.y;
//            if (SDL_fabsf(dist_y) < (float)PLAYER_SIZE) {
//                if (collisions > 0) {
//                    moveBeing(b, SDL_copysignf(b->velocity, dist_x), SDL_copysignf(b->velocity, dist_y));
//                    break;
//                }
//                moveBeing(b, SDL_copysignf(b->velocity * 0.875F, dist_x), SDL_copysignf(b->velocity * 0.875F, dist_y));
//                ++collisions;
//            }
//        }
//    }
//}