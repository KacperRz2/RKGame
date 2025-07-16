#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Being.h>
#include <World.h>
#include <function.h>
#include <Player.h>
#include <Projectile.h>
#include <enum.h>

extern const Being_type test_being;

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
    // for (unsigned int i = 0U; i < b->segment->beings.num; ++i) {
    //     Being* bb = *(b->segment->beings.array + i);
    //     if(*(bb->segment->beings.array + bb->indx) != bb){
    //         SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "%u>:(", i);
    //         // SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "status:%d %d hp:%d", b->status, b->status_ticks_left, b->hit_points);
    //     }
    // }
}

void DestroyBeings(Beings_array* const bs) {
    SDL_free(bs->array);
    bs->num = 0U;
}

extern inline void AddBeingToArray(Beings_array* const bs, const float x, const float y, Segment* const s) {
    Being* b = (bs->array + bs->num);
    b->position.x = x;
    b->position.y = y;
    b->direction = 0.0F;
    b->type = &test_being;
    AddBeingToSegment(s, b);
    b->hit_points = b->type->hit_points;
    b->status = idle;
    b->status_ticks_left = 32;
	b->blade.position.x = 16.0F;
 	b->blade.position.y = -8.0F;
    b->blade.direction = 0.0F;
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
    // b->walk.time_left = time;
    b->status = walk;
    b->status_ticks_left = time;
    b->walk_shift.x = x_shift;
    b->walk_shift.y = y_shift;
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
    // if(SDL_sqrtf((x - b->position.x) * (x - b->position.x) + (y - b->position.y) * (y - b->position.y)) < (float)PLAYER_SIZE){
    if(pow2(x - b->position.x) + pow2(y - b->position.y) < (float)pow2(PLAYER_SIZE)){
        return true;
    }
    return false;
}

bool ResolveBeingCollisionInNewSegment(Being* const b, Segment* const s, float* const new_x, float* const new_y, const float x_shift, const float y_shift) {

    for (unsigned int i = 0U; i < s->beings.num; ++i) {

        Being* b2 = *(s->beings.array + i);

        if (b2 == b || b2->status == walk) continue;

        if(CollideWithBeing(b2, *new_x, *new_y)){
            StartBeingWalkWithRandTurn45Deg(b, 128, x_shift, y_shift);
            *new_x = b->position.x + b->walk_shift.x;
            *new_y = b->position.y + b->walk_shift.y;
            return true;
        }
    }
    return false;
}

inline void TurnBeingWalk(Being* const b) {
    float tmp_x = b->walk_shift.x;
    if ((bool)SDL_rand(2)) {
        b->walk_shift.x = -b->walk_shift.y;
        b->walk_shift.y = tmp_x;
    }else {
        b->walk_shift.x = b->walk_shift.y;
        b->walk_shift.y = -tmp_x;
    }
}

inline void UpdateBeingWalk(Being* const b) {
    if (b->status_ticks_left == 0) {
        b->status = idle;
        return;
    }
    float new_x = b->position.x + b->walk_shift.x;
    float new_y = b->position.y + b->walk_shift.y;
    Segment* new_segment = GetSegment(new_x, new_y);

    if (new_segment != b->segment && (new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS)) {
        TurnBeingWalk(b);
        return;
    }
    SetBeingPosition(b, new_x, new_y);
    MoveBeingToSegment(b, new_segment);
    --b->status_ticks_left;
}

inline void UpdateBeingIdle(Being* const b, const float distance_squared) {
    if (distance_squared < pow2(VIEWFINDER)) {
        if (distance_squared < pow2(140.0F)) {
            b->status = strike;
            b->status_ticks_left = -(BEING_ATTACK_STEPS * 2);
            return;
        }
        if(distance_squared < pow2(700.0F)){
            b->status = shoot;
            b->status_ticks_left = BEING_RELOAD;
            return;
        }
    }
    if(b->status_ticks_left > 0){
        --b->status_ticks_left;
        return;
    }
    b->status = follow;
    b->status_ticks_left = 32;
}

inline bool UpdateBeingShoot(Being* const b, Projectiles_h_array* const prs) {
    if (b->status_ticks_left == 0) {
        b->status_ticks_left = 128;
        b->status = follow;
        return false;
    }
    if (b->status_ticks_left == 4 && prs->num < MAX_PROJECTILES_NUM){
        AddHProjectileToArray(prs, &b->position, b->direction, 3.0F, b->type->damage_far);
    }
    --b->status_ticks_left;
    return true;
}

inline void MoveStrikingBeing(Being* const b, float const distance, float const distance_x, float const distance_y){
    float velocity_xy = distance / b->type->velocity;
    float x_shift = distance_x / velocity_xy;
    float y_shift = distance_y / velocity_xy;
    float new_x = b->position.x + x_shift;
    float new_y = b->position.y + y_shift;
    Segment* new_segment = GetSegment(new_x, new_y);
    if(new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS){
        return;
    }
    for (unsigned int i = 0U; i < new_segment->beings.num; ++i) {
        Being* b2 = *(new_segment->beings.array + i);
        if (b2 == b) continue;
        if(CollideWithBeing(b2, new_x, new_y)){
            return;
        }
    }
    if (new_segment == b->segment) {
        SetBeingPosition(b, new_x, new_y);
        return;
    }
    SetBeingPosition(b, new_x, new_y);
    MoveBeingToSegment(b, new_segment);
}

inline void UpdateBeingStrike(Being* const b, Player* const p, float const distance, float const distance_x, float const distance_y) {
    static const Blade_hostile blade_base_frame = {{16.0F, -8.0F}, 0.0F};
    if (b->status_ticks_left == 0) {
        b->blade = blade_base_frame;
        b->status = follow;
        b->status_ticks_left = 128;
        return;
    }
    static const Status_frame shift_prepare = {{(20.0F - 16.0F) / BEING_ATTACK_STEPS, (-16.0F - -8.0F) / BEING_ATTACK_STEPS}, (0.5F - 0.0F) / BEING_ATTACK_STEPS};
    static const Status_frame shift_attack = {{(0.0F - 20.0F) / BEING_ATTACK_STEPS, (24.0F - -16.0F) / BEING_ATTACK_STEPS}, (0.0F - 0.5F) / BEING_ATTACK_STEPS};
    static const Status_frame shift_reset = {{(16.0F - 0.0F) / BEING_ATTACK_STEPS, (-8.0F - 24.0F) / BEING_ATTACK_STEPS}, (0.0F - 0.0F) / BEING_ATTACK_STEPS};
    static const float blade_length = BLADE_SIZE * 0.85F;

    if(distance >= 70.0F){
        MoveStrikingBeing(b, distance, distance_x, distance_y);
    }

    if(b->status_ticks_left > 0){
        ShiftHBlade(&b->blade, &shift_attack);
        if(b->status_ticks_left == 1){
            float sine;
            float cosine;
            Status_frame blade_location = GetHBladeLocation(b, &sine, &cosine);
            SDL_FPoint dangerous_point = {blade_location.position.x + sine * blade_length, blade_location.position.y - cosine * blade_length};
            if(PointInPlayer(dangerous_point.x, dangerous_point.y, p)){
                // DamagePlayer(p, b->type->damage_close);
                p->hit_points -= b->type->damage_close;
            }
            b->status_ticks_left = -(BEING_ATTACK_STEPS - 1);
            return;
        }
        --b->status_ticks_left;
        return;
    }
    if(b->status_ticks_left < -BEING_ATTACK_STEPS){
        ShiftHBlade(&b->blade, &shift_prepare);
    }else if(b->status_ticks_left == -BEING_ATTACK_STEPS){
        ShiftHBlade(&b->blade, &shift_prepare);
        b->status_ticks_left = BEING_ATTACK_STEPS;
        return;
    }else{
        ShiftHBlade(&b->blade, &shift_reset);
    }
    ++b->status_ticks_left;
}

inline void UpdateBeingFollow(Being* const b, float const distance, float const distance_x, float const distance_y) {
    if (b->status_ticks_left == 0) {
        b->status = idle;
        return;
    }
    if(distance < 70.0F){
        --b->status_ticks_left;
        return;
    }
    float velocity_xy = distance / b->type->velocity;
    float x_shift = distance_x / velocity_xy;
    float y_shift = distance_y / velocity_xy;
    float new_x = b->position.x + x_shift;
    float new_y = b->position.y + y_shift;
    if (distance > RANGE_SEGMENTS * (WORLD_SIZE / SEGMENTS_X)) {
        SetBeingPosition(b, new_x, new_y);
        --b->status_ticks_left;
        return;
    }
    Segment* new_segment = GetSegment(new_x, new_y);
    bool collision = false;
    if(new_segment == NULL){
        StartBeingWalkWithRandTurn45Deg(b, 128, x_shift, y_shift);
        new_x = b->position.x + b->walk_shift.x;
        new_y = b->position.y + b->walk_shift.y;
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
                HaltBeing(b, -128);
            }
            return;
        }
    }else{
        SetBeingPosition(b, new_x, new_y);
        --b->status_ticks_left;
        return;
    }
    SetBeingPosition(b, new_x, new_y);
    MoveBeingToSegment(b, new_segment);
    --b->status_ticks_left;
}

inline void ShiftHBlade(Blade_hostile* const bl, const Status_frame* const shift){
	bl->position.x += shift->position.x;
	bl->position.y += shift->position.y;
	bl->direction += shift->direction;
}

inline Status_frame GetHBladeLocation(Being* const b, float* const sine, float* const cosine){
	float direct = b->direction + b->blade.direction;
	*sine = SineSafe(direct);
	*cosine = CosiSafe(direct);
	Status_frame ret = {{b->position.x + *sine * b->blade.position.x, b->position.y - *cosine * b->blade.position.y}, direct};
	return ret;
}

inline void HaltBeing(Being* const b, const int time){
    b->status = idle;
    b->status_ticks_left = time;
}

void UpdateBeings(Beings_array* const bs, Player* const p, Segment* const player_seg, Projectiles_h_array* const prs) {
    for (Being* b = bs->array; b != (bs->array + bs->num); ++b) {

        if (b->status == dead) {
            if(b != (bs->array + bs->num - 1U)){
                UpdateSegmentBeingPointer((bs->array + bs->num - 1U), b);
                *b = *(bs->array + bs->num - 1U);
                --b;
            }
            --bs->num;
            continue;
        }
        // if(b != *(b->segment->beings.array + b->indx)){
        //     SDL_LogInfo(SDL_LOG_CATEGORY_TEST, ">:(");
        // }
        if(b->status == walk){
            UpdateBeingWalk(b);
            continue;
        }
        float distance_x = p->position.x - b->position.x;
        float distance_y = p->position.y - b->position.y;
        float distance_squared = distance_x * distance_x + distance_y * distance_y;
        float distance = 0.0F;
        if(distance_squared < pow2(PLAYER_SIZE)){
            distance = SDL_sqrtf(distance_squared);
            float velocity_xy = distance / (PLAYER_VELOCITY * 1.875F);
            MovePlayer(p, distance_x / velocity_xy, distance_y / velocity_xy);
        }
        if(b->status == idle){
            UpdateBeingIdle(b, distance_squared);
            continue;
        }
        b->direction = SDL_atan2f(-distance_y, -distance_x) - SDL_PI_F * 0.5F;
        if(b->status == shoot && UpdateBeingShoot(b, prs)){
            continue;
        }
        if(distance == 0.0F){
            distance = SDL_sqrtf(distance_squared);
        }
        if(b->status == strike){
            UpdateBeingStrike(b, p, distance, distance_x, distance_y);
            continue;
        }
        if(b->status == follow){
            UpdateBeingFollow(b, distance, distance_x, distance_y);
        }
    }
}

extern inline bool DamageBeing(Being* const b, int damage) {
    b->hit_points -= damage; // SDL_LogInfo(SDL_LOG_CATEGORY_TEST, ">:(");
    if(b->hit_points < 1){
        b->status = dead;
        RemoveBeingFromSegment(b);
        return true;
    }
    return false;
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