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

extern inline void AddBeingToArray(Beings_array* const bs, const unsigned int type_id, const float x, const float y, Segment* const s){
    static int being_id = 0U;
    Being* b = (bs->array + bs->num);
    b->position = (SDL_FPoint){x, y};
    b->direction = 0.0F;
    b->type_id = type_id;
    if(IsAlly(b)){
        AddBeingToSegment(s, b, &s->ally_beings);
    }else{
        AddBeingToSegment(s, b, &s->beings);
    }
    b->velocity = (being_types + b->type_id)->velocity;
    b->armour = (being_types + b->type_id)->armour;
    b->hit_points = (being_types + b->type_id)->hit_points;
    b->status = being_idle;
    b->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
    b->weapon = (Weapon){BEING_WEAPON_BASE_PLCMNT, (being_types + b->type_id)->impact};
    b->id = being_id++;
    b->effects_num = 0U;
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
    b->position = (SDL_FPoint){x, y};
}

static inline void SetBeingPositionInNewSegment(Being* const b, const float x, const float y, Segment* const s){
    b->position = (SDL_FPoint){x, y};
    MoveBeingToSegment(b, s);
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

static bool ResolveBeingCollisionInNewSegment(Being* const b, Segment* const s, float* const new_x, float* const new_y, const float x_shift, const float y_shift){
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
    float new_x = b->position.x + b->special_move_shift.x;
    float new_y = b->position.y + b->special_move_shift.y;
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
        AddHProjectileToArray(prs, &b->position, b->direction, PROJECTILE_VELOCITY, &b->weapon.impact);
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

static inline void UpdateBeingStrike(Being* const b, Player* const p, float const distance, float const distance_x, float const distance_y, World* const w){
    if(b->status_ticks_left == 0){
        b->weapon.placement = (Placement)BEING_WEAPON_BASE_PLCMNT;
        b->status = being_follow;
        b->status_ticks_left = BEING_RELOAD_TICKS;
        return;
    }
    static const float blade_length = BLADE_SIZE * 0.85F;
    if(distance >= BEING_HALT_DISTANCE){
        MoveStrikingBeing(b, distance, distance_x, distance_y, w);
    }else if(distance < BEING_MIN_DISTANCE){
        MoveBackStrikingBeing(b, distance, distance_x, distance_y, w);
    }
    if(b->status_ticks_left > 0){
        ShiftHBlade(&b->weapon, &(Placement)BEING_BLD_SHIFT_ATTACK);
        if(b->status_ticks_left == 1){
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
        ShiftHBlade(&b->weapon, &(Placement)BEING_BLD_SHIFT_PREPATE);
    }else if(b->status_ticks_left == -BEING_ATTACK_STEPS){
        ShiftHBlade(&b->weapon, &(Placement)BEING_BLD_SHIFT_PREPATE);
        b->status_ticks_left = BEING_ATTACK_STEPS;
        return;
    }else{
        ShiftHBlade(&b->weapon, &(Placement)BEING_BLD_SHIFT_RESET);
    }
    ++b->status_ticks_left;
}

static inline void UpdateBeingFollow(Being* const b, float const distance, float const distance_x, float const distance_y, World* const w){
    if(b->status_ticks_left == 0){
        b->status = being_idle;
        return;
    }
    if(distance < BEING_HALT_DISTANCE){
        --b->status_ticks_left;
        return;
    }
    float velocity_xy = distance / b->velocity;
    float x_shift = distance_x / velocity_xy;
    float y_shift = distance_y / velocity_xy;
    float new_x = b->position.x + x_shift;
    float new_y = b->position.y + y_shift;
    if(distance > RANGE_SEGMENTS * (WORLD_SIZE / SEGMENTS_X)){
        HaltBeing(b, BEING_WALK_TICKS);
        return;
    }
    Segment* new_segment = GetSegment(w, new_x, new_y);
    bool collision = false;
    if(new_segment == NULL){
        StartBeingWalkWithRandTurn45Deg(b, BEING_WALK_TICKS, x_shift, y_shift);
        new_x = b->position.x + b->special_move_shift.x;
        new_y = b->position.y + b->special_move_shift.y;
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
    for(Being* b = g_d->beings.array; b != (g_d->beings.array + g_d->beings.num); ++b){
        if(b->status == being_dead){
            if(b != (g_d->beings.array + g_d->beings.num - 1U)){ 
                if(IsAlly(b)){
                    UpdateSegmentBeingPointer((g_d->beings.array + g_d->beings.num - 1U), b, &(g_d->beings.array + g_d->beings.num - 1U)->segment->beings);
                }else{
                    UpdateSegmentBeingPointer((g_d->beings.array + g_d->beings.num - 1U), b, &(g_d->beings.array + g_d->beings.num - 1U)->segment->ally_beings);
                }
                *b = *(g_d->beings.array + g_d->beings.num - 1U);
            }
            --b;
            --g_d->beings.num;
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
    b->special_rotation_shift = (bool)SDL_rand(2) ? FULL_ANGLE / b->status_ticks_left : -FULL_ANGLE / b->status_ticks_left;
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
    b->direction += b->special_rotation_shift;
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
    Player* target = plys->array;
    b->target_indx = 0U;
    float distance_squared0 = pow2(b->position.x - target->position.x) + pow2(b->position.y - target->position.y);
    for(unsigned int i = 1U; i < plys->num; ++i){
        const float distance_squared1 = pow2(b->position.x - (plys->array + i)->position.x) + pow2(b->position.y - (plys->array + i)->position.y);
        if(distance_squared0 > distance_squared1){
            target = plys->array + i;
            distance_squared0 = distance_squared1;
            b->target_indx = i;
        }
    }
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
        for(unsigned int i = 0U; i < b->segment->beings.num; ++i){
            Being* b2 = *(b->segment->beings.array + i);
            if(b2 == b || b2->status == being_walk) continue;
            if(BeingCollideWithBeing(b2, b->position.x, b->position.y, BeingSize(b))){
                StartBeingRandWalk(b, BEING_WALK_TICKS / 2);
                return;
            }
        }
        FindTargetForBeing(b, &g_d->champions);
        p = g_d->champions.array + b->target_indx;
        distance_x = p->position.x - b->position.x;
        distance_y = p->position.y - b->position.y;
        distance_squared = distance_x * distance_x + distance_y * distance_y;
        if(distance_squared < pow2(IDLE_BEING_ACTION_DISTANCE)){
            if(distance_squared < pow2(BEING_ATTACK_DISTANCE)){
                b->status = being_strike;
                b->status_ticks_left = -(BEING_ATTACK_STEPS * 2);
                return;
            }
        }
        if(b->status_ticks_left > 0){
            --b->status_ticks_left;
            return;
        }
        b->status = being_follow;
        b->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
        return;
    }else{
        p = g_d->champions.array + b->target_indx;
        distance_x = p->position.x - b->position.x;
        distance_y = p->position.y - b->position.y;
        distance_squared = distance_x * distance_x + distance_y * distance_y;
    }
    const float distance = SDL_sqrtf(distance_squared);
    if(distance_squared < pow2(half(PLAYER_SIZE) + half(BeingSize(b)))){
        float velocity_xy = distance / (PLAYER_VELOCITY * 1.875F);
        MovePlayer(g_d, b->target_indx, distance_x / velocity_xy, distance_y / velocity_xy);
    }
    if(b->status == being_stunned){
        UpdateBeingStunned(b, &g_d->world);
        return;
    }
    if(!(b->status == being_strike && ((b->status_ticks_left > 0 && b->status_ticks_left < BEING_ATTACK_STEPS / 2) || (b->status_ticks_left < 0 && b->status_ticks_left >= -(BEING_ATTACK_STEPS - 1))))){
        b->direction = SDL_atan2f(-distance_y, -distance_x) - SDL_PI_F * 0.5F;
    }
    if(b->status == being_strike){
        UpdateBeingStrike(b, p, distance, distance_x, distance_y, &g_d->world);
        return;
    }
    if(b->status == being_follow){
        UpdateBeingFollow(b, distance, distance_x, distance_y, &g_d->world);
    }
}

static void UpdateBeing1(Being* const b, Game_data* const g_d){
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
        for(unsigned int i = 0U; i < b->segment->beings.num; ++i){
            Being* b2 = *(b->segment->beings.array + i);
            if(b2 == b || b2->status == being_walk) continue;
            if(BeingCollideWithBeing(b2, b->position.x, b->position.y, BeingSize(b))){
                b->status = being_follow;
                b->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
                return;
            }
        }
        FindTargetForBeing(b, &g_d->champions);
        p = g_d->champions.array + b->target_indx;
        distance_x = p->position.x - b->position.x;
        distance_y = p->position.y - b->position.y;
        distance_squared = distance_x * distance_x + distance_y * distance_y;
        if(distance_squared < pow2(IDLE_BEING_ACTION_DISTANCE)){
            if(distance_squared < pow2(BEING_ATTACK_DISTANCE)){
                b->status = being_strike;
                b->status_ticks_left = -(BEING_ATTACK_STEPS * 2);
                return;
            }
            if(distance_squared < pow2(BEING_SHOOT_DISTANCE)){
                b->status = being_shoot;
                b->status_ticks_left = BEING_RELOAD;
                return;
            }
        }
        if(b->status_ticks_left > 0){
            --b->status_ticks_left;
            return;
        }
        b->status = being_follow;
        b->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
        return;
    }else{
        p = g_d->champions.array + b->target_indx;
        distance_x = p->position.x - b->position.x;
        distance_y = p->position.y - b->position.y;
        distance_squared = distance_x * distance_x + distance_y * distance_y;
    }
    const float distance = SDL_sqrtf(distance_squared);
    if(distance_squared < pow2(half(PLAYER_SIZE) + half(BeingSize(b)))){
        float velocity_xy = distance / (PLAYER_VELOCITY * 1.875F);
        MovePlayer(g_d, b->target_indx, distance_x / velocity_xy, distance_y / velocity_xy);
    }
    if(b->status == being_stunned){
        UpdateBeingStunned(b, &g_d->world);
        return;
    }
    if(!(b->status == being_strike && ((b->status_ticks_left > 0 && b->status_ticks_left < BEING_ATTACK_STEPS / 2) || (b->status_ticks_left < 0 && b->status_ticks_left >= -(BEING_ATTACK_STEPS - 1))))){
        b->direction = SDL_atan2f(-distance_y, -distance_x) - SDL_PI_F * 0.5F;
    }
    if(b->status == being_shoot){
        UpdateBeingShoot(b, &g_d->projectiles);
        return;
    }
    if(b->status == being_strike){
        UpdateBeingStrike(b, p, distance, distance_x, distance_y, &g_d->world);
        return;
    }
    if(b->status == being_follow){
        UpdateBeingFollow(b, distance, distance_x, distance_y, &g_d->world);
    }
}

static void UpdateBeing2(Being* const b, Game_data* const g_d){
    
}