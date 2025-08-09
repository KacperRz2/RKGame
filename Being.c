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

static inline void AddBeingToSegment(Segment* const s, Being* const b){
    b->segment = s;
    *(s->beings.array + s->beings.num) = b;
    b->indx = s->beings.num;
    ++s->beings.num;
}

static inline void RemoveBeingFromSegment(Being* const b){
    if(b->indx != b->segment->beings.num - 1){
        *(b->segment->beings.array + b->indx) = *(b->segment->beings.array + b->segment->beings.num - 1);
        (*(b->segment->beings.array + b->indx))->indx = b->indx;
    }
    --b->segment->beings.num;
}

void DestroyBeings(Beings_array* const bs){
    SDL_free(bs->array);
    bs->num = 0U;
}

extern inline void AddBeingToArray(Beings_array* const bs, const float x, const float y, Segment* const s){
    static int being_id = 0U;
    Being* b = (bs->array + bs->num);
    b->position.x = x;
    b->position.y = y;
    b->direction = 0.0F;
    b->type = &test_being;
    AddBeingToSegment(s, b);
    b->hit_points = b->type->hit_points;
    b->status = idle;
    b->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
	b->blade.position.x = BLADE_BASE_X;
 	b->blade.position.y = BLADE_BASE_Y;
    b->blade.direction = BLADE_BASE_DIRECTION_BEING;
    b->id = being_id++;
    ++bs->num;
}

static inline void DestroyBeingInArray(Beings_array* const bs, const unsigned int indx){
    if(indx != bs->num - 1){
        *(bs->array + indx) = *(bs->array + bs->num - 1);
    }
    --bs->num;
}

static inline void MoveBeing(Being* const b, const float x, const float y){
    b->position.x += x;
    b->position.y += y;
}

static inline void SetBeingPosition(Being* const b, const float x, const float y){
    b->position.x = x;
    b->position.y = y;
}

static inline void MoveBeingX(Being* const b, const float x){
    b->position.x += x;
}

static inline void MoveBeingY(Being* const b, const float y){
    b->position.y += y;
}

static inline void MoveBeingToSegment(Being* const b, Segment* const s){
    RemoveBeingFromSegment(b);
    AddBeingToSegment(s, b);
}

static inline void StartBeingWalk(Being* const b, const int time, const float x_shift, const float y_shift){
    b->status = walk;
    b->status_ticks_left = time;
    b->walk_shift.x = x_shift;
    b->walk_shift.y = y_shift;
}

static inline void StartBeingWalkWithRandTurn(Being* const b, const int time, const float x_shift, const float y_shift){
    if((bool)SDL_rand(2)){
        StartBeingWalk(b, time, -y_shift, x_shift);
    }else{
        StartBeingWalk(b, time, y_shift, -x_shift);
    }
}

static inline void StartBeingWalkWithRandTurn45Deg(Being* const b, const int time, const float x_shift, const float y_shift){
    if((bool)SDL_rand(2)){
        StartBeingWalk(b, time, (x_shift + y_shift) * SQRT2DIV2, (y_shift - x_shift) * SQRT2DIV2);
    }else{
        StartBeingWalk(b, time, (x_shift - y_shift) * SQRT2DIV2, (y_shift + x_shift) * SQRT2DIV2);
    }
}

static inline bool CollideWithBeing(Being* const b, const float x, const float y){
    if(pow2(x - b->position.x) + pow2(y - b->position.y) < (float)pow2(PLAYER_SIZE)){
        return true;
    }
    return false;
}

static bool ResolveBeingCollisionInNewSegment(Being* const b, Segment* const s, float* const new_x, float* const new_y, const float x_shift, const float y_shift){
    for(unsigned int i = 0U; i < s->beings.num; ++i){
        Being* b2 = *(s->beings.array + i);
        if(b2 == b || b2->status == walk) continue;
        if(CollideWithBeing(b2, *new_x, *new_y)){
            StartBeingWalkWithRandTurn45Deg(b, BEING_WALK_TICKS, x_shift, y_shift);
            *new_x = b->position.x + b->walk_shift.x;
            *new_y = b->position.y + b->walk_shift.y;
            return true;
        }
    }
    return false;
}

static inline void TurnBeingWalk(Being* const b){
    float tmp_x = b->walk_shift.x;
    if((bool)SDL_rand(2)){
        b->walk_shift.x = -b->walk_shift.y;
        b->walk_shift.y = tmp_x;
    }else{
        b->walk_shift.x = b->walk_shift.y;
        b->walk_shift.y = -tmp_x;
    }
}

static inline void UpdateBeingWalk(Being* const b, World* const w){
    if(b->status_ticks_left == 0){
        b->status = idle;
        return;
    }
    float new_x = b->position.x + b->walk_shift.x;
    float new_y = b->position.y + b->walk_shift.y;
    Segment* new_segment = GetSegment(w, new_x, new_y);

    if(new_segment != b->segment && (new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS)){
        TurnBeingWalk(b);
        return;
    }
    SetBeingPosition(b, new_x, new_y);
    MoveBeingToSegment(b, new_segment);
    --b->status_ticks_left;
}

static inline void UpdateBeingIdle(Being* const b, const float distance_squared){
    if(distance_squared < pow2(IDLE_BEING_ACTION_DISTANCE)){
        if(distance_squared < pow2(BEING_ATTACK_DISTANCE)){
            b->status = strike;
            b->status_ticks_left = -(BEING_ATTACK_STEPS * 2);
            return;
        }
        if(distance_squared < pow2(BEING_SHOOT_DISTANCE)){
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
    b->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
}

static inline bool UpdateBeingShoot(Being* const b, Projectiles_h_array* const prs){
    if(b->status_ticks_left == 0){
        b->status_ticks_left = BEING_RELOAD_TICKS;
        b->status = follow;
        return false;
    }
    if(b->status_ticks_left == 4 && prs->num < MAX_PROJECTILES_NUM){
        AddHProjectileToArray(prs, &b->position, b->direction, PROJECTILE_VELOCITY, b->type->damage_far);
    }
    --b->status_ticks_left;
    return true;
}

static inline void MoveStrikingBeing(Being* const b, float const distance, float const distance_x, float const distance_y, World* const w){
    float velocity_xy = distance / b->type->velocity;
    float x_shift = distance_x / velocity_xy;
    float y_shift = distance_y / velocity_xy;
    float new_x = b->position.x + x_shift;
    float new_y = b->position.y + y_shift;
    Segment* new_segment = GetSegment(w, new_x, new_y);
    if(new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS){
        return;
    }
    for(unsigned int i = 0U; i < new_segment->beings.num; ++i){
        Being* b2 = *(new_segment->beings.array + i);
        if(b2 == b) continue;
        if(CollideWithBeing(b2, new_x, new_y)){
            return;
        }
    }
    if(new_segment == b->segment){
        SetBeingPosition(b, new_x, new_y);
        return;
    }
    SetBeingPosition(b, new_x, new_y);
    MoveBeingToSegment(b, new_segment);
}

static inline void UpdateBeingStrike(Being* const b, Player* const p, float const distance, float const distance_x, float const distance_y, World* const w){
    static const Blade_hostile blade_base_frame = {{BLADE_BASE_X, BLADE_BASE_Y}, BLADE_BASE_DIRECTION_BEING};
    if(b->status_ticks_left == 0){
        b->blade = blade_base_frame;
        b->status = follow;
        b->status_ticks_left = BEING_RELOAD_TICKS;
        return;
    }
    static const Status_frame shift_prepare = {{(20.0F - 16.0F) / BEING_ATTACK_STEPS, (-16.0F - -8.0F) / BEING_ATTACK_STEPS}, (0.5F - 0.0F) / BEING_ATTACK_STEPS};
    static const Status_frame shift_attack = {{(0.0F - 20.0F) / BEING_ATTACK_STEPS, (24.0F - -16.0F) / BEING_ATTACK_STEPS}, (0.0F - 0.5F) / BEING_ATTACK_STEPS};
    static const Status_frame shift_reset = {{(BLADE_BASE_X - 0.0F) / BEING_ATTACK_STEPS, (BLADE_BASE_Y - 24.0F) / BEING_ATTACK_STEPS}, (BLADE_BASE_DIRECTION_BEING - 0.0F) / BEING_ATTACK_STEPS};
    static const float blade_length = BLADE_SIZE * 0.85F;
    if(distance >= BEING_HALT_DISTANCE){
        MoveStrikingBeing(b, distance, distance_x, distance_y, w);
    }
    if(b->status_ticks_left > 0){
        ShiftHBlade(&b->blade, &shift_attack);
        if(b->status_ticks_left == 1){
            float sine;
            float cosine;
            Status_frame blade_location = GetHBladeLocation(b, &sine, &cosine);
            SDL_FPoint dangerous_point = {blade_location.position.x + sine * blade_length, blade_location.position.y - cosine * blade_length};
            if(PointInPlayer(dangerous_point.x, dangerous_point.y, p)){
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

static inline void UpdateBeingFollow(Being* const b, float const distance, float const distance_x, float const distance_y, World* const w){
    if(b->status_ticks_left == 0){
        b->status = idle;
        return;
    }
    if(distance < BEING_HALT_DISTANCE){
        --b->status_ticks_left;
        return;
    }
    float velocity_xy = distance / b->type->velocity;
    float x_shift = distance_x / velocity_xy;
    float y_shift = distance_y / velocity_xy;
    float new_x = b->position.x + x_shift;
    float new_y = b->position.y + y_shift;
    if(distance > RANGE_SEGMENTS * (WORLD_SIZE / SEGMENTS_X)){
        // SetBeingPosition(b, new_x, new_y);
        // --b->status_ticks_left;
        HaltBeing(b, BEING_WALK_TICKS);
        return;
    }
    Segment* new_segment = GetSegment(w, new_x, new_y);
    bool collision = false;
    if(new_segment == NULL){
        StartBeingWalkWithRandTurn45Deg(b, BEING_WALK_TICKS, x_shift, y_shift);
        new_x = b->position.x + b->walk_shift.x;
        new_y = b->position.y + b->walk_shift.y;
        collision = true;
    }else if(distance < CHECK_COLLISION_DISTANCE){
        collision = ResolveBeingCollisionInNewSegment(b, new_segment, &new_x, &new_y, x_shift, y_shift);
    }
    if(collision){
        new_segment = GetSegment(w, new_x, new_y);
    }
    if(new_segment != b->segment){
        if(new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS){
            if((bool)SDL_rand(2)){
                StartBeingWalkWithRandTurn(b, BEING_WALK_TICKS, x_shift * 0.5F, y_shift * 0.5F);
            }else{
                HaltBeing(b, BEING_WALK_TICKS);
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

static inline void ShiftHBlade(Blade_hostile* const bl, const Status_frame* const shift){
	bl->position.x += shift->position.x;
	bl->position.y += shift->position.y;
	bl->direction += shift->direction;
}

static inline Status_frame GetHBladeLocation(Being* const b, float* const sine, float* const cosine){
	float direct = b->direction + b->blade.direction;
	*sine = SineSafe(direct);
	*cosine = CosiSafe(direct);
	Status_frame ret = {{b->position.x + *sine * b->blade.position.x, b->position.y - *cosine * b->blade.position.y}, direct};
	return ret;
}

static inline void HaltBeing(Being* const b, const int time){
    b->status = idle;
    b->status_ticks_left = -time;
}

void UpdateBeings(Game_data* const g_d){
    for(Being* b = g_d->beings.array; b != (g_d->beings.array + g_d->beings.num); ++b){
        if(b->status == dead){
            if(b != (g_d->beings.array + g_d->beings.num - 1U)){
                UpdateSegmentBeingPointer((g_d->beings.array + g_d->beings.num - 1U), b);
                *b = *(g_d->beings.array + g_d->beings.num - 1U);
            }
            --b;
            --g_d->beings.num;
            continue;
        }
        if(b->status == walk){
            UpdateBeingWalk(b, &g_d->world);
            continue;
        }
        float distance_x = g_d->pc.position.x - b->position.x;
        float distance_y = g_d->pc.position.y - b->position.y;
        float distance_squared = distance_x * distance_x + distance_y * distance_y;
        float distance = 0.0F;
        if(distance_squared < pow2(PLAYER_SIZE)){
            distance = SDL_sqrtf(distance_squared);
            float velocity_xy = distance / (PLAYER_VELOCITY * 1.875F);
            MovePlayer(g_d, distance_x / velocity_xy, distance_y / velocity_xy);
        }
        if(b->status == idle){
            UpdateBeingIdle(b, distance_squared);
            continue;
        }
        b->direction = SDL_atan2f(-distance_y, -distance_x) - SDL_PI_F * 0.5F;
        if(b->status == shoot && UpdateBeingShoot(b, &g_d->h_projectiles)){
            continue;
        }
        if(distance == 0.0F){
            distance = SDL_sqrtf(distance_squared);
        }
        if(b->status == strike){
            UpdateBeingStrike(b, &g_d->pc, distance, distance_x, distance_y, &g_d->world);
            continue;
        }
        if(b->status == follow){
            UpdateBeingFollow(b, distance, distance_x, distance_y, &g_d->world);
        }
    }
}

extern inline bool DamageBeing(Being* const b, int damage){
    b->hit_points -= damage; // SDL_LogInfo(SDL_LOG_CATEGORY_TEST, ">:(");
    if(b->hit_points < 1){
        b->status = dead;
        RemoveBeingFromSegment(b);
        return true;
    }
    return false;
}