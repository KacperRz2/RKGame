#include <SDL3/SDL.h>
#include <macros.h>
#include <types.h>
#include <Being.h>
#include <World.h>
#include <function.h>
#include <Player.h>
#include <Projectile.h>
#include <enum.h>
#include <game.h>

const Being_type being_types[] = BEING_TYPES;

static inline void UpdateBeing(Being* const b, Game_data* const g_d){
    (being_types + b->type_id)->update(b, g_d);
}

extern inline float BeingSize(Being* const b){
    return (being_types + b->type_id)->size;
}

static inline void AddBeingToSegment(Segment* const s, Being* const b, Segment_beings* const bs){
    b->segment = s;
    *(bs->array + bs->num) = b;
    b->indx = bs->num;
    ++bs->num;
}

static inline void RemoveBeingFromSegment(Being* const b, Segment_beings* const bs){
    if(b->indx != bs->num - 1){
        *(bs->array + b->indx) = *(bs->array + bs->num - 1);
        (*(bs->array + b->indx))->indx = b->indx;
    }
    --bs->num;
}

void DestroyBeings(Beings_array* const bs){
    SDL_free(bs->array);
    bs->num = 0U;
}

extern inline void AddBeingToArray(Beings_array* const bs, const unsigned int type_id, const float x, const float y, Segment* const s, Player* const target){
    Being* b = *(bs->ptrs + bs->num++);
    b->position = (SDL_FPoint){x, y};
    b->type_id = type_id;
    b->target.player = target;
    if(IsAlly(b)){
        AddBeingToSegment(s, b, &s->ally_beings);
    }else{
        AddBeingToSegment(s, b, &s->beings);
    }
    b->velocity = (being_types + b->type_id)->velocity;
    b->armour = (being_types + b->type_id)->armour;
    b->hit_points = (being_types + b->type_id)->hit_points;
    HaltBeing(b, 1);
    b->weapon = (Weapon){BEING_WEAPON_BASE_PLCMNT, (being_types + b->type_id)->impact};
    b->effects_num = 0U;
}

static inline void MoveBeing(Being* const b, const float x, const float y){
    b->position.x += x;
    b->position.y += y;
}

static inline void SetBeingPosition(Being* const b, const float x, const float y){
    b->position = (SDL_FPoint){x, y};
}

static inline void SetBeingPositionInNewSegment(Being* const b, const float x, const float y, Segment* const s){
    b->position = (SDL_FPoint){x, y};
    MoveBeingToSegment(b, s);
}

static inline void SetAllyPositionInNewSegment(Being* const b, const float x, const float y, Segment* const s){
    b->position = (SDL_FPoint){x, y};
    MoveAllyToSegment(b, s);
}

static inline void MoveBeingX(Being* const b, const float x){
    b->position.x += x;
}

static inline void MoveBeingY(Being* const b, const float y){
    b->position.y += y;
}

static inline void MoveBeingToSegment(Being* const b, Segment* const s){
    RemoveBeingFromSegment(b, &b->segment->beings);
    AddBeingToSegment(s, b, &s->beings);
}

static inline void MoveAllyToSegment(Being* const b, Segment* const s){
    RemoveBeingFromSegment(b, &b->segment->ally_beings);
    AddBeingToSegment(s, b, &s->ally_beings);
}

static inline void StartBeingWalk(Being* const b, const int time, const float x_shift, const float y_shift){
    b->status = being_walk;
    b->status_ticks_left = time;
    b->special_move_shift = (SDL_FPoint){x_shift, y_shift};
}

static inline void StartBeingRandWalk(Being* const b, const int time){
    const float angle = SDL_randf() * FULL_ANGLE;
    b->status = being_walk;
    b->status_ticks_left = time;
    b->special_move_shift = (SDL_FPoint){b->velocity * sine(angle), -b->velocity * cosi(angle)};
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

static inline bool BeingCollideWithBeing(Being* const b, const float x, const float y, const float being_size){
    if(pow2(x - b->position.x) + pow2(y - b->position.y) < (float)pow2(half(BeingSize(b) + being_size))){
        return true;
    }
    return false;
}

static inline bool ResolveBeingCollisionInNewSegment(Being* const b, Segment* const s, float* const new_x, float* const new_y, const float x_shift, const float y_shift){
    for(unsigned int i = 0U; i < s->beings.num; ++i){
        Being* b2 = *(s->beings.array + i);
        if(b2 == b || b2->status == being_walk) continue;
        if(BeingCollideWithBeing(b2, *new_x, *new_y, BeingSize(b))){
            StartBeingWalkWithRandTurn45Deg(b, BEING_WALK_TICKS, x_shift, y_shift);
            *new_x = b->position.x + b->special_move_shift.x;
            *new_y = b->position.y + b->special_move_shift.y;
            return true;
        }
    }
    return false;
}

static inline void TurnBeingWalk(Being* const b){
    float tmp_x = b->special_move_shift.x;
    if((bool)SDL_rand(2)){
        b->special_move_shift = (SDL_FPoint){-b->special_move_shift.y, tmp_x};
    }else{
        b->special_move_shift = (SDL_FPoint){b->special_move_shift.y, -tmp_x};
    }
}

static inline void UpdateBeingWalk(Being* const b, World* const w){
    if(b->status_ticks_left == 0){
        b->status = being_idle;
        return;
    }
    const float new_x = b->position.x + b->special_move_shift.x;
    const float new_y = b->position.y + b->special_move_shift.y;
    Segment* new_segment = GetSegment(w, new_x, new_y);
    if(new_segment != b->segment){
        if(new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS){
            TurnBeingWalk(b);
            return;
        }
        SetBeingPositionInNewSegment(b, new_x, new_y, new_segment);
    }else{
        SetBeingPosition(b, new_x, new_y);
    }
    --b->status_ticks_left;
}

static inline void UpdateBeingShoot(Being* const b, Projectiles_array* const prs){
    if(b->status_ticks_left == 0){
        b->status_ticks_left = BEING_RELOAD_TICKS;
        b->status = being_idle;
        return;
    }
    if(b->status_ticks_left == 4 && prs->num < MAX_PROJECTILES_NUM){
        float x, y;
        GetShift(&b->position, &b->target.player->position, PROJECTILE_VELOCITY, &x, &y);
        AddHProjectileToArray(prs, &b->position, x, y, &b->weapon.impact);
    }
    --b->status_ticks_left;
}

static inline void MoveStrikingBeing(Being* const b, float const distance, float const distance_x, float const distance_y, World* const w){
    float velocity_xy = distance / b->velocity;
    float x_shift = distance_x / velocity_xy;
    float y_shift = distance_y / velocity_xy;
    float new_x = b->position.x + x_shift;
    float new_y = b->position.y + y_shift;
    SetBeingPositionIfAllowed(b, new_x, new_y, w);
}

static inline void MoveBackStrikingBeing(Being* const b, float const distance, float const distance_x, float const distance_y, World* const w){
    float velocity_xy = distance / b->velocity;
    float x_shift = distance_x / velocity_xy;
    float y_shift = distance_y / velocity_xy;
    float new_x = b->position.x - x_shift;
    float new_y = b->position.y - y_shift;
    SetBeingPositionIfAllowed(b, new_x, new_y, w);
}

static inline void SetBeingPositionIfAllowed(Being* const b, float const x, float const y, World* const w){
    Segment* new_segment = GetSegment(w, x, y);
    if(new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS){
        return;
    }
    for(unsigned int i = 0U; i < new_segment->beings.num; ++i){
        Being* b2 = *(new_segment->beings.array + i);
        if(b2 == b) continue;
        if(BeingCollideWithBeing(b2, x, y, BeingSize(b))){
            return;
        }
    }
    if(new_segment == b->segment){
        SetBeingPosition(b, x, y);
        return;
    }
    SetBeingPositionInNewSegment(b, x, y, new_segment);
}

static inline void UpdateBeingStrike(Being* const b, Player* const p, float const distance_squared, float const distance_x, float const distance_y, World* const w){
    if(b->status_ticks_left == 0){
        b->weapon.placement = (Placement)BEING_WEAPON_BASE_PLCMNT;
        b->status = being_follow;
        b->status_ticks_left = BEING_RELOAD_TICKS;
        return;
    }
    static const float blade_length = BLADE_SIZE * 0.85F;
    if(distance_squared >= pow2(BEING_HALT_DISTANCE)){
        const float distance = SDL_sqrtf(distance_squared);
        MoveStrikingBeing(b, distance, distance_x, distance_y, w);
    }else if(distance_squared < pow2(BEING_MIN_DISTANCE)){
        const float distance = SDL_sqrtf(distance_squared);
        MoveBackStrikingBeing(b, distance, distance_x, distance_y, w);
    }
    if(b->status_ticks_left > 0){
        ShiftHBlade(&b->weapon, &(Placement)BEING_BLD_SHIFT_ATTACK);
        if(b->status_ticks_left == BEING_ATTACK_STEPS / 2){
            b->direction = arctan2(-distance_y, -distance_x) - SDL_PI_F * 0.5F;
        }else if(b->status_ticks_left == 1){
            float bl_sine;
            float bl_cosine;
            Placement blade_location = GetHBladeLocation(b, &bl_sine, &bl_cosine);
            SDL_FPoint dangerous_point = {blade_location.position.x + bl_sine * blade_length, blade_location.position.y - bl_cosine * blade_length};
            if(PointInPlayer(dangerous_point.x, dangerous_point.y, p)){
                if(p->flags & block && (sine(p->direction) * SineSafe(b->direction)) + (-cosi(p->direction) * -CosiSafe(b->direction)) <= 0){
                    HitBarrier(p, &b->weapon.impact);
                }else{
                    DamagePlayer(p, &b->weapon.impact);
                }
            }
            b->status_ticks_left = -(BEING_ATTACK_STEPS - 1);
            return;
        }
        --b->status_ticks_left;
        return;
    }
    if(b->status_ticks_left < -BEING_ATTACK_STEPS){
        ShiftHBlade(&b->weapon, &(Placement)BEING_BLD_SHIFT_PREPARE);
    }else if(b->status_ticks_left == -BEING_ATTACK_STEPS){
        ShiftHBlade(&b->weapon, &(Placement)BEING_BLD_SHIFT_PREPARE);
        b->status_ticks_left = BEING_ATTACK_STEPS;
        return;
    }else{
        ShiftHBlade(&b->weapon, &(Placement)BEING_BLD_SHIFT_RESET);
    }
    ++b->status_ticks_left;
}

static inline void UpdateBeingFollow(Being* const b, float const distance_squared, float const distance_x, float const distance_y, World* const w){
    if(b->status_ticks_left == 0){
        b->status = being_idle;
        return;
    }
    if(distance_squared < pow2(BEING_HALT_DISTANCE)){
        --b->status_ticks_left;
        return;
    }
    BEING_DEFAULT_LEFT_TICKS;
    if(distance_squared < pow2(BEING_SHOOT_DISTANCE) || b->status_ticks_left == BEING_DEFAULT_LEFT_TICKS){
        const float distance = SDL_sqrtf(distance_squared);
        const float velocity_xy = distance / b->velocity;
        b->special_move_shift.x = distance_x / velocity_xy;
        b->special_move_shift.y = distance_y / velocity_xy;
    }
    const float x_shift = b->special_move_shift.x;
    const float y_shift = b->special_move_shift.y;
    float new_x = b->position.x + x_shift;
    float new_y = b->position.y + y_shift;
    Segment* new_segment = GetSegment(w, new_x, new_y);
    bool collision = false;
    if(new_segment == NULL){
        StartBeingWalkWithRandTurn45Deg(b, BEING_WALK_TICKS, x_shift, y_shift);
        new_x = b->position.x + b->special_move_shift.x;
        new_y = b->position.y + b->special_move_shift.y;
        collision = true;
    }else if(distance_squared < pow2(CHECK_COLLISION_DISTANCE)){
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
    SetBeingPositionInNewSegment(b, new_x, new_y, new_segment);
    --b->status_ticks_left;
}

static inline void ShiftHBlade(Weapon* const bl, const Placement* const shift){
	bl->placement.position.x += shift->position.x;
	bl->placement.position.y += shift->position.y;
	bl->placement.direction += shift->direction;
}

static inline Placement GetHBladeLocation(Being* const b, float* const sine, float* const cosine){
	float direct = b->direction + b->weapon.placement.direction;
	*sine = SineSafe(direct);
	*cosine = CosiSafe(direct);
	Placement ret = {{b->position.x + *sine * b->weapon.placement.position.x, b->position.y - *cosine * b->weapon.placement.position.y}, direct};
	return ret;
}

static inline void HaltBeing(Being* const b, const int time){
    b->status = being_idle;
    b->status_ticks_left = -time;
}

static inline bool IsAlly(const Being* const b){
    if(b->type_id > being_1){
        return true;
    }
    return false;
}

void UpdateBeings(Game_data* const g_d){
    for(unsigned int i = 0U; i < g_d->beings.num; ++i){
        Being* b = *(g_d->beings.ptrs + i);
        if(b->status == being_dead){
            *(g_d->beings.ptrs + i) = *(g_d->beings.ptrs + --g_d->beings.num);
            *(g_d->beings.ptrs + g_d->beings.num) = b;
            --i;
            continue;
        }
        (being_types + b->type_id)->update(b, g_d);
    }
}

extern inline bool DamageBeing(Being* const b, const Impact* const impact){
    b->hit_points -= CalculateDamage(impact, &b->armour);
    if(b->hit_points < 1){
        b->status = being_dead;
        RemoveBeingFromSegment(b, &b->segment->beings);
        return true;
    }
    return false;
}

extern inline bool DamageAlly(Being* const b, const Impact* const impact){
    b->hit_points -= CalculateDamage(impact, &b->armour);
    if(b->hit_points < 1){
        b->status = being_dead;
        RemoveBeingFromSegment(b, &b->segment->ally_beings);
        return true;
    }
    return false;
}

extern inline void ResetBeingBlade(Being* const b){
    b->weapon.placement = (Placement)BEING_WEAPON_BASE_PLCMNT;
}

extern inline void StunBeing(Being* const b, const int duration){
    b->status = being_stunned;
    b->status_ticks_left = duration;
}

extern inline void CatapultBeing(Being* const b, const float shift_x, const float shift_y, const int duration){
    if(b->status == being_strike){
        ResetBeingBlade(b);
    }
    b->status = being_fly;
    b->status_ticks_left = duration;
    b->special_move_shift = (SDL_FPoint){shift_x, shift_y};
    b->rend_fly_help_data.start_angle = b->direction;
    b->rend_fly_help_data.ticks = (bool)SDL_rand(2) ? duration : -duration;
}

static inline void UpdateBeingStunned(Being* const b, World* const w){
    if(b->status_ticks_left == 0){
        b->status = being_idle;
        return;
    }
    --b->status_ticks_left;
}

static inline void UpdateBeingFly(Being* const b, World* const w){
    if(b->status_ticks_left == 0){
        StunBeing(b, BEING_STUN_DURATION * b->armour.unstability);
        return;
    }
    float new_x = b->position.x + b->special_move_shift.x;
    float new_y = b->position.y + b->special_move_shift.y;
    Segment* new_segment = GetSegment(w, new_x, new_y);
    if(new_segment != b->segment){
        if (new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS){
            StunBeing(b, BEING_STUN_DURATION);
        }else{
            SetBeingPositionInNewSegment(b, new_x, new_y, new_segment);
        }
    }else{
        SetBeingPosition(b, new_x, new_y);
    }
    --b->status_ticks_left;
}

static inline void FindTargetForBeing(Being* const b, Players* const plys){
    b->target.player = plys->array;
    float distance_squared0 = pow2(b->position.x - plys->array->position.x) + pow2(b->position.y - plys->array->position.y);
    for(unsigned int i = 1U; i < plys->num; ++i){
        const float distance_squared1 = pow2(b->position.x - (plys->array + i)->position.x) + pow2(b->position.y - (plys->array + i)->position.y);
        if(distance_squared0 > distance_squared1){
            b->target.player = plys->array + i;
            distance_squared0 = distance_squared1;
        }
    }
}

static inline bool IdleBeingCollision(Being* const b, Players* plys){
    for(unsigned int i = 0U; i < b->segment->beings.num; ++i){
        Being* b1 = *(b->segment->beings.array + i);
        if(b1 == b || b1->status == being_walk) continue;
        if(BeingCollideWithBeing(b1, b->position.x, b->position.y, BeingSize(b))){
            FindTargetForBeing(b, plys);
            if(pow2(b->target.player->position.x - b->position.x) + pow2(b->target.player->position.y - b->position.y) <= pow2(BEING_HALT_DISTANCE)){
                StartBeingRandWalk(b, BEING_DEFAULT_LEFT_TICKS);
                return true;
            }
            b->status = being_follow;
            b->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
            return true;
        }
    }
    return false;
}

static inline bool FindAllyTargetFar(Being* const b, World* const w, const unsigned int distance){
    int c = b->segment->indx.x - distance;
    int r = b->segment->indx.y - distance;
    while(c <= b->segment->indx.x + distance){
        Segment* s = GetSegmentByIndxSafe(w, c++, r);
        if(s != NULL && s->beings.num > 0U){
            b->target.being = *(s->beings.array);
            return true;
        }
    }
    while(r <= b->segment->indx.y + distance){
        Segment* s = GetSegmentByIndxSafe(w, c, r++);
        if(s != NULL && s->beings.num > 0U){
            b->target.being = *(s->beings.array);
            return true;
        }
    }
    while(c >= b->segment->indx.x - distance){
        Segment* s = GetSegmentByIndxSafe(w, c--, r);
        if(s != NULL && s->beings.num > 0U){
            b->target.being = *(s->beings.array);
            return true;
        }
    }
    while(r > b->segment->indx.y - distance){
        Segment* s = GetSegmentByIndxSafe(w, c, r--);
        if(s != NULL && s->beings.num > 0U){
            b->target.being = *(s->beings.array);
            return true;
        }
    }
    return false;
}

static inline bool FindAllyTarget(Being* const b, World* const w){
    if(b->segment->beings.num > 0U){
        b->target.being = *(b->segment->beings.array);
        return true;
    }
    unsigned int i = 1U;
    do{
        if(FindAllyTargetFar(b, w, i)){
            return true;
        }
    }while(++i < 4U);
    return false;
}

static inline void UpdateAllyFollow(Being* const b, World* const w){
    if(b->status_ticks_left == 0){
        b->status = being_idle;
        return;
    }
    if(b->status_ticks_left == BEING_DEFAULT_LEFT_TICKS){
        float distance_x = b->target.player->position.x - b->position.x;
        float distance_y = b->target.player->position.y - b->position.y;
        float distance_squared = pow2(distance_x) + pow2(distance_y);
        if(distance_squared < pow2(BEING_HALT_DISTANCE * 2.0F)){
            b->special_move_shift.x = 0.0F;
            b->special_move_shift.y = 0.0F;
            --b->status_ticks_left;
            return;
        }
        distance_x = (bool)SDL_rand(2) ? b->target.player->position.x + SEGMENT_SIZE * (SDL_randf() + 0.5F) - b->position.x : b->target.player->position.x - SEGMENT_SIZE * (SDL_randf() + 0.5F) - b->position.x;
        distance_y = (bool)SDL_rand(2) ? b->target.player->position.y + SEGMENT_SIZE * (SDL_randf() + 0.5F) - b->position.y : b->target.player->position.y - SEGMENT_SIZE * (SDL_randf() + 0.5F) - b->position.y;
        distance_squared = pow2(distance_x) + pow2(distance_y);
        const float distance = SDL_sqrtf(distance_squared);
        const float velocity_xy = distance / b->velocity;
        b->special_move_shift.x = distance_x / velocity_xy;
        b->special_move_shift.y = distance_y / velocity_xy;
    }
    float new_x = b->position.x + b->special_move_shift.x;
    float new_y = b->position.y + b->special_move_shift.y;
    Segment* new_segment = GetSegment(w, new_x, new_y);
    if(new_segment == NULL){
        StartBeingWalkWithRandTurn45Deg(b, BEING_WALK_TICKS, b->special_move_shift.x, b->special_move_shift.y);
        new_x = b->position.x + b->special_move_shift.x;
        new_y = b->position.y + b->special_move_shift.y;
        new_segment = GetSegment(w, new_x, new_y);
    }
    if(new_segment != b->segment){
        if(new_segment == NULL){
            StartBeingWalkWithRandTurn(b, BEING_WALK_TICKS, b->special_move_shift.x * 0.5F, b->special_move_shift.y * 0.5F);
            return;
        }else if(new_segment->ally_beings.num >= MAX_SEGM_BEINGS){
            HaltBeing(b, BEING_WALK_TICKS);
            return;
        }
    }else{
        SetBeingPosition(b, new_x, new_y);
        --b->status_ticks_left;
        return;
    }
    SetAllyPositionInNewSegment(b, new_x, new_y, new_segment);
    --b->status_ticks_left;
}

static inline void UpdateAllyWalk(Being* const b, World* const w){
    if(b->status_ticks_left == 0){
        b->status = being_idle;
        return;
    }
    const float new_x = b->position.x + b->special_move_shift.x;
    const float new_y = b->position.y + b->special_move_shift.y;
    Segment* new_segment = GetSegment(w, new_x, new_y);
    if(new_segment != b->segment){
        if(new_segment == NULL || new_segment->ally_beings.num >= MAX_SEGM_BEINGS){
            TurnBeingWalk(b);
            return;
        }
        SetAllyPositionInNewSegment(b, new_x, new_y, new_segment);
    }else{
        SetBeingPosition(b, new_x, new_y);
    }
    --b->status_ticks_left;
}

static inline void UpdateBeing0(Being* const b, Game_data* const g_d){
    if(b->status == being_walk){
        UpdateBeingWalk(b, &g_d->world);
        return;
    }
    if(b->status == being_fly){
        UpdateBeingFly(b, &g_d->world);
        return;
    }
    Player* p;
    float distance_x;
    float distance_y;
    float distance_squared;
    if(b->status == being_idle){
        if(IdleBeingCollision(b, &g_d->champions)){
            return;
        }
        FindTargetForBeing(b, &g_d->champions);
        p = b->target.player;
        distance_x = p->position.x - b->position.x;
        distance_y = p->position.y - b->position.y;
        distance_squared = pow2(distance_x) + pow2(distance_y);
        if(distance_squared < pow2(BEING_ATTACK_DISTANCE)){
            b->status = being_strike;
            b->status_ticks_left = -(BEING_ATTACK_STEPS * 2);
            return;
        }
        if(b->status_ticks_left < 0){
            ++b->status_ticks_left;
            return;
        }
        b->status = being_follow;
        b->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
        return;
    }else{
        p = b->target.player;
        distance_x = p->position.x - b->position.x;
        distance_y = p->position.y - b->position.y;
        distance_squared = pow2(distance_x) + pow2(distance_y);
    }
    if(distance_squared < pow2(half(PLAYER_SIZE) + half(BeingSize(b)))){
        const float distance = SDL_sqrtf(distance_squared);
        const float velocity_xy = distance / (PLAYER_VELOCITY * 1.875F);
        MovePlayer(g_d, p, distance_x / velocity_xy, distance_y / velocity_xy);
    }
    if(b->status == being_stunned){
        UpdateBeingStunned(b, &g_d->world);
        return;
    }
    if(b->status == being_strike){
        UpdateBeingStrike(b, p, distance_squared, distance_x, distance_y, &g_d->world);
        return;
    }
    if(b->status == being_follow){
        UpdateBeingFollow(b, distance_squared, distance_x, distance_y, &g_d->world);
    }
}

static inline void UpdateBeing1(Being* const b, Game_data* const g_d){
    if(b->status == being_walk){
        UpdateBeingWalk(b, &g_d->world);
        return;
    }
    if(b->status == being_fly){
        UpdateBeingFly(b, &g_d->world);
        return;
    }
    if(b->status == being_shoot){
        UpdateBeingShoot(b, &g_d->projectiles);
        return;
    }
    Player* p;
    float distance_x;
    float distance_y;
    float distance_squared;
    if(b->status == being_idle){
        if(IdleBeingCollision(b, &g_d->champions)){
            return;
        }
        FindTargetForBeing(b, &g_d->champions);
        p = b->target.player;
        distance_x = p->position.x - b->position.x;
        distance_y = p->position.y - b->position.y;
        distance_squared = pow2(distance_x) + pow2(distance_y);
        if(distance_squared < pow2(BEING_SHOOT_DISTANCE)){
            if(distance_squared < pow2(BEING_ATTACK_DISTANCE)){
                b->status = being_strike;
                b->status_ticks_left = -(BEING_ATTACK_STEPS * 2);
                return;
            }
            b->status = being_shoot;
            b->status_ticks_left = BEING_RELOAD;
            return;
        }
        if(b->status_ticks_left < 0){
            ++b->status_ticks_left;
            return;
        }
        b->status = being_follow;
        b->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
        return;
    }else{
        p = b->target.player;
        distance_x = p->position.x - b->position.x;
        distance_y = p->position.y - b->position.y;
        distance_squared = pow2(distance_x) + pow2(distance_y);
    }
    if(distance_squared < pow2(half(PLAYER_SIZE) + half(BeingSize(b)))){
        const float distance = SDL_sqrtf(distance_squared);
        const float velocity_xy = distance / (PLAYER_VELOCITY * 1.875F);
        MovePlayer(g_d, p, distance_x / velocity_xy, distance_y / velocity_xy);
    }
    if(b->status == being_stunned){
        UpdateBeingStunned(b, &g_d->world);
        return;
    }
    if(b->status == being_strike){
        UpdateBeingStrike(b, p, distance_squared, distance_x, distance_y, &g_d->world);
        return;
    }
    if(b->status == being_follow){
        UpdateBeingFollow(b, distance_squared, distance_x, distance_y, &g_d->world);
    }
}

static inline void UpdateAlly0(Being* const b, Game_data* const g_d){
    if(b->status == being_idle){
        if(FindAllyTarget(b, &g_d->world)){
            b->status = being_attack_being;
            b->status_ticks_left = BEING_RELOAD_TICKS;
        }else{
            if(b->status_ticks_left < 0){
                ++b->status_ticks_left;
                return;
            }
            b->target.player = g_d->champions.array + g_d->human_indx;
            b->status = being_follow;
            b->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
        }
        return;
    }
    if(b->status == being_attack_being){
        if(b->status_ticks_left == 0){
            float x, y;
            GetShift(&b->position, &b->target.being->position, PROJECTILE_VELOCITY, &x, &y);
            AddPCProjectileToArray(&g_d->projectiles, &b->position, x, y, &b->weapon.impact, 2U);
            b->status = being_idle;
        }else{
            --b->status_ticks_left;
        }
        return;
    }
    if(b->status == being_follow){
        UpdateAllyFollow(b, &g_d->world);
        return;
    }
    if(b->status == being_walk){
        UpdateAllyWalk(b, &g_d->world);
        return;
    }
    if(b->status == being_stunned){
        b->status = being_idle;
    }
}