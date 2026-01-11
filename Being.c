#include <SDL3/SDL.h>
#include <macros.h>
#include <enum.h>
#include <types.h>
#include <Being.h>
#include <World.h>
#include <function.h>
#include <Player.h>
#include <Projectile.h>
#include <game.h>
#include <render.h>

const Being_type being_types[] = BEING_TYPES;

static inline void UpdateBeing(Being* const b, Game_data* const gd){
    (being_types + b->type_id)->update(b, gd);
}

extern inline float BeingSize(const Being* const bg){
    return (being_types + bg->type_id)->size;
}

extern inline int BeingHP(const Being* const bg){
    return (being_types + bg->type_id)->hit_points;
}

extern inline unsigned int GetRandomBeingId(){
    const int incidences[] = BEINGS_INCIDENCES;
    const int rand_id = SDL_rand(*(incidences + being_warlock));
    for(unsigned int i = 0U; i <= being_commander; ++i){
        if(rand_id < *(incidences + i)){return i;}
    }
    return being_warlock;
}

extern inline void AddBeingToSegment(Segment* const seg, Being* const bg, Segment_beings* const bs){
    bg->segment = seg;
    *(bs->beings_ind + bs->num) = bg->main_indx;
    bg->indx = bs->num++;
}

static inline void RemoveBeingFromSegment(Being* const bg, Segment_beings* const bs, Being* const main_beings){
    if(bg->indx != bs->num - 1U){
        *(bs->beings_ind + bg->indx) = *(bs->beings_ind + bs->num - 1U);
        (main_beings + *(bs->beings_ind + bs->num - 1U))->indx = bg->indx;
    }
    --bs->num;
}

void DestroyBeings(Beings_array* const bs){
    SDL_free(bs->array);
    bs->num = 0U;
}

extern inline void TryCreateIdleBeing(Game_data* const gd, const Uint8 type_id, const float x, const float y, Player* const target){
    Segment* const seg = GetSegmentUnsafe(&gd->world, x, y);
    if(!seg || seg->beings.num >= MAX_SEGM_BEINGS || gd->beings.num >= MAX_BEINGS_NUM){
        return;
    }
    AddIdleBeingToArray(&gd->beings, type_id, x, y, seg, target);
}

extern inline bool TryCreateHostileBeing(Game_data* const gd, const Uint8 type_id, const float x, const float y, Player* const target){
    Segment* const seg = GetSegmentUnsafe(&gd->world, x, y);
    if(seg->beings.num >= MAX_SEGM_BEINGS || gd->beings.num >= MAX_BEINGS_NUM){
        return false;
    }
    AddHordeBeingToArray(&gd->beings, type_id, x, y, seg, target);
    return true;
}

extern inline void AddHordeBeingToArray(Beings_array* const bs, const Uint8 type_id, const float x, const float y, Segment* const s, Player* const target){
    Being* const bg = AddBeingToArray(bs, type_id, x, y, target);
    AddBeingToSegment(s, bg, &s->beings);
    bg->target_last_seen_at = (position16){bg->target.player->position.x, bg->target.player->position.y};
    if(bg->type_id == being_commander){
        AddBeingEffect(bg, (Lasting_effect){being_effect_commander, COMMANDER_EFFECT_TICKS});
    }
}

extern inline void AddIdleBeingToArray(Beings_array* const bs, const Uint8 type_id, const float x, const float y, Segment* const seg, Player* const target){
    Being* const bg = AddBeingToArray(bs, type_id, x, y, target);
    if(IsAlly(bg)){
        AddBeingToSegment(seg, bg, &seg->ally_beings);
        bg->target_last_seen_at = (position16){bg->target.player->position.x, bg->target.player->position.y};
    }else{
        AddBeingToSegment(seg, bg, &seg->beings);
        bg->target_last_seen_at.x = 0.0F;
    }
    if(bg->type_id == being_commander){
        AddBeingEffect(bg, (Lasting_effect){being_effect_commander, COMMANDER_EFFECT_TICKS});
    }
}

static inline Being* AddBeingToArray(Beings_array* const bs, const Uint8 type_id, const float x, const float y, Player* const target){
    Being* bg = (bs->array + *(bs->indices + bs->num++));
    bg->position = (SDL_FPoint){x, y};
    bg->type_id = type_id;
    bg->target.player = target;
    SetBeingTypeData(bg);
    HaltBeing(bg, 1);
    bg->effects_num = 0U;
    return bg;
}

static inline void SetBeingTypeData(Being* const bg){
    bg->velocity = (being_types + bg->type_id)->velocity;
    bg->armour = (being_types + bg->type_id)->armour;
    bg->hit_points = (being_types + bg->type_id)->hit_points;
    bg->impact = (being_types + bg->type_id)->impact;
}

static inline void GetBeingDistances(const Being* const b, const SDL_FPoint* const to, float* const d_x, float* const d_y, float* const d_sq){
    *d_x = to->x - b->position.x;
    *d_y = to->y - b->position.y;
    *d_sq = pow2(*d_x) + pow2(*d_y);
}

static inline void MoveBeing(Being* const b, const float x, const float y){
    b->position.x += x;
    b->position.y += y;
}

static inline void SetBeingPosition(Being* const b, const float x, const float y){
    b->position = (SDL_FPoint){x, y};
}

static inline void SetBeingPositionInNewSegment(Being* const bg, const float x, const float y, Segment* const seg, Being* const main_beings){
    bg->position = (SDL_FPoint){x, y};
    MoveBeingToSegment(bg, seg, main_beings);
}

static inline void SetAllyPositionInNewSegment(Being* const bg, const float x, const float y, Segment* const seg, Being* const main_beings){
    bg->position = (SDL_FPoint){x, y};
    MoveAllyToSegment(bg, seg, main_beings);
}

static inline void MoveBeingX(Being* const bg, const float x){
    bg->position.x += x;
}

static inline void MoveBeingY(Being* const bg, const float y){
    bg->position.y += y;
}

static inline bool CircleMeetsBeing(const float x, const float y, const float diameter, Being* const bg){
	if(pow2(x - bg->position.x) + pow2(y - bg->position.y) < pow2(half((float)PLAYER_SIZE + diameter))){
		return true;
	}
	return false;
}

static inline void MoveBeingToSegment(Being* const bg, Segment* const seg, Being* const main_beings){
    RemoveBeingFromSegment(bg, &bg->segment->beings, main_beings);
    AddBeingToSegment(seg, bg, &seg->beings);
}

static inline void MoveAllyToSegment(Being* const bg, Segment* const seg, Being* const main_beings){
    RemoveBeingFromSegment(bg, &bg->segment->ally_beings, main_beings);
    AddBeingToSegment(seg, bg, &seg->ally_beings);
}

static inline void StartBeingWalk(Being* const bg, const int time, const float x_shift, const float y_shift){
    bg->status = being_walk;
    bg->status_ticks_left = time;
    bg->special_move_shift = (SDL_FPoint){x_shift, y_shift};
}

static inline void StartBeingRandWalk(Being* const bg, const int time){
    const float angle = SDL_randf() * FULL_ANGLE;
    bg->status = being_walk;
    bg->status_ticks_left = time;
    bg->special_move_shift = (SDL_FPoint){bg->velocity * SineUnsafe(angle), -bg->velocity * CosiUnsafe(angle)};
}

static inline void StartBeingWalkWithRandTurn(Being* const bg, const int time, const float x_shift, const float y_shift){
    if(SDL_rand(2)){
        StartBeingWalk(bg, time, -y_shift, x_shift);
    }else{
        StartBeingWalk(bg, time, y_shift, -x_shift);
    }
}

static inline void StartBeingWalkWithRandTurn45Deg(Being* const bg, const int time, const float x_shift, const float y_shift){
    if(bg->main_indx % 2){
        StartBeingWalk(bg, time, (x_shift + y_shift) * SQRT2DIV2, (y_shift - x_shift) * SQRT2DIV2);
    }else{
        StartBeingWalk(bg, time, (x_shift - y_shift) * SQRT2DIV2, (y_shift + x_shift) * SQRT2DIV2);
    }
}

static inline bool BeingCollideWithBeing(Being* const bg, const float x, const float y, const float being_size){
    if(pow2(x - bg->position.x) + pow2(y - bg->position.y) < (float)pow2(half(BeingSize(bg) + being_size))){
        return true;
    }
    return false;
}

static inline bool ResolveBeingCollisionInNewSegment(Game_data* const gd, Being* const bg, Segment* const seg, float* const new_x, float* const new_y, const float x_shift, const float y_shift){
    Segment* segs[2];
    Get2NearestSegments(segs, &gd->world, seg, *new_x, *new_y);
    for(unsigned int k = 0U; k < 2; ++k){
        if(!(*(segs + k))) continue;
        for(unsigned int i = 0U; i < (*(segs + k))->beings.num; ++i){
            Being* bg1 = (gd->beings.array + *((*(segs + k))->beings.beings_ind + i));
            if(bg1 == bg || bg1->status == being_walk) continue;
            if(BeingCollideWithBeing(bg1, *new_x, *new_y, BeingSize(bg))){
                StartBeingWalkWithRandTurn45Deg(bg, BEING_WALK_TICKS, x_shift, y_shift);
                *new_x = bg->position.x + bg->special_move_shift.x;
                *new_y = bg->position.y + bg->special_move_shift.y;
                return true;
            }
        }
    }
    return false;
}

static inline void TurnBeingWalk(Being* const b){
    if(SDL_rand(2)){
        b->special_move_shift = (SDL_FPoint){SDL_copysignf(b->special_move_shift.y, b->special_move_shift.x), SDL_copysignf(b->special_move_shift.x, -b->special_move_shift.y)};
    }else{
        b->special_move_shift = (SDL_FPoint){SDL_copysignf(b->special_move_shift.y, -b->special_move_shift.x), SDL_copysignf(b->special_move_shift.x, b->special_move_shift.y)};
    }
}

static inline void UpdateBeingWalk(Being* const bg, Game_data* const gd){
    if(bg->status_ticks_left == 0){
        bg->status = being_idle;
        return;
    }
    const float new_x = bg->position.x + bg->special_move_shift.x;
    const float new_y = bg->position.y + bg->special_move_shift.y;
    Segment* new_segment = GetSegmentUnsafe(&gd->world, new_x, new_y);
    if(new_segment != bg->segment){
        if(new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS){
            TurnBlockedBeingWalk(bg, gd);
            return;
        }
        SetBeingPositionInNewSegment(bg, new_x, new_y, new_segment, gd->beings.array);
    }else{
        SetBeingPosition(bg, new_x, new_y);
    }
    --bg->status_ticks_left;
}

static inline void UpdateBeingShoot(Being* const bg, Projectiles_array* const prs, const World* const w, const SDL_FPoint* const target_position, Segment* const target_segment){
    if(bg->status_ticks_left == 0){
        bg->status = being_idle;
        return;
    }
    if(bg->status_ticks_left == 32 && prs->num < MAX_PROJECTILES_NUM){
        if(IsClearSight(&bg->position, target_position, target_segment, w)){
            float x, y;
            GetShift(&bg->position, target_position, PROJECTILE_VELOCITY, &x, &y);
            AddHProjectileToArray(prs, &bg->position, x, y, &bg->impact);
        }else{
            bg->status = being_idle;
        }
    }
    --bg->status_ticks_left;
}

static inline void UpdateWarlockShoot(Being* const bg, Projectiles_array* const prs, const World* const wld, const SDL_FPoint* const target_position, Segment* const target_segment){
    if(bg->status_ticks_left == 0){
        bg->status = being_idle;
        return;
    }
    if(bg->status_ticks_left == 32 && prs->num < MAX_PROJECTILES_NUM){
        if(IsClearSight(&bg->position, target_position, target_segment, wld)){
            float x, y;
            GetShift(&bg->position, target_position, PROJECTILE_VELOCITY, &x, &y);
            if(SDL_rand(4)){
                AddHProjectileToArray(prs, &bg->position, x, y, &bg->impact);
            }else{
                AddSpecialProjectileToArray(prs, &bg->position, x, y, projectile_warlock, WARLOCK_SPEC_PROJE_TICKS);
            }
        }else{
            bg->status = being_idle;
        }
    }
    --bg->status_ticks_left;
}

static inline void MoveStrikingBeing(Being* const bg, float const distance, float const distance_x, float const distance_y, Game_data* const gd){
    const float velocity_xy = distance / bg->velocity;
    const float x_shift = distance_x / velocity_xy;
    const float y_shift = distance_y / velocity_xy;
    const float new_x = bg->position.x + x_shift;
    const float new_y = bg->position.y + y_shift;
    SetBeingPositionIfAllowed(bg, new_x, new_y, gd);
}

static inline void MoveBackStrikingBeing(Being* const b, float const distance, float const distance_x, float const distance_y, Game_data* const gd){
    const float velocity_xy = distance / b->velocity;
    const float x_shift = distance_x / velocity_xy;
    const float y_shift = distance_y / velocity_xy;
    const float new_x = b->position.x - x_shift;
    const float new_y = b->position.y - y_shift;
    SetBeingPositionIfAllowed(b, new_x, new_y, gd);
}

static inline void SetBeingPositionIfAllowed(Being* const bg, float const x, float const y, Game_data* const gd){
    Segment* new_segment = GetSegmentUnsafe(&gd->world, x, y);
    if(new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS){
        return;
    }
    Segment* segs[2];
    Get2NearestSegments(segs, &gd->world, new_segment, x, y);
    for(unsigned int k = 0U; k < 2U; ++k){ 
        if(!(*(segs + k))) continue;
        for(unsigned int i = 0U; i < (*(segs + k))->beings.num; ++i){
            Being* bg1 = (gd->beings.array + *((*(segs + k))->beings.beings_ind + i));
            if(bg1 == bg) continue;
            if(BeingCollideWithBeing(bg1, x, y, BeingSize(bg))){
                return;
            }
        }
    }
    if(new_segment == bg->segment){
        SetBeingPosition(bg, x, y);
        return;
    }
    SetBeingPositionInNewSegment(bg, x, y, new_segment, gd->beings.array);
}

static inline void UpdateBeingStrikeAlly(Being* const bg, float const distance_squared, float const distance_x, float const distance_y, Game_data* const gd){
    if(bg->status_ticks_left == 0 || bg->target.being->status == being_dead){
        HaltBeing(bg, BEING_RELOAD_TICKS);
        return;
    }
    if(distance_squared >= pow2(BEING_HALT_DISTANCE)){
        const float distance = SDL_sqrtf(distance_squared);
        MoveStrikingBeing(bg, distance, distance_x, distance_y, gd);
    }else if(distance_squared < pow2(BEING_MIN_DISTANCE)){
        const float distance = SDL_sqrtf(distance_squared);
        MoveBackStrikingBeing(bg, distance, distance_x, distance_y, gd);
    }
    if(bg->status_ticks_left == BEING_ATTACK_STEPS + 1){
        const float b_sine = SineSafe(bg->direction);
        const float b_cosine = CosiSafe(bg->direction);
        const SDL_FPoint dangerous_point = GetHBladeAttackHittingPoint(bg, b_sine, b_cosine);
        if(CircleMeetsBeing(dangerous_point.x, dangerous_point.y, BEING_HIT_CIRCLE_DIAMET, bg->target.being)){
			AddDamageVisualEffect(&gd->rend_data_ptr->visual_effects, &dangerous_point);
            if(!DamageAlly(bg->target.being, &bg->impact, gd->beings.array)){
                StunBeing(bg->target.being, (int)(BEING_DEFAULT_LEFT_TICKS * CalculateStunPower(&bg->impact, &bg->target.being->armour)));
            }
        }
    }
    --bg->status_ticks_left;
}

static inline void UpdateBeingStrike(Being* const bg, Player* const pc, float const distance_squared, float const distance_x, float const distance_y, Game_data* const gd){
    if(bg->status_ticks_left == 0){
        StartBeingFollow(bg, BEING_RELOAD_TICKS, distance_squared, distance_x, distance_y);
        return;
    }
    if(distance_squared >= pow2(BEING_HALT_DISTANCE)){
        const float distance = SDL_sqrtf(distance_squared);
        MoveStrikingBeing(bg, distance, distance_x, distance_y, gd);
    }else if(distance_squared < pow2(BEING_MIN_DISTANCE)){
        const float distance = SDL_sqrtf(distance_squared);
        MoveBackStrikingBeing(bg, distance, distance_x, distance_y, gd);
    }
    if(bg->status_ticks_left == BEING_ATTACK_STEPS + 1){
        const float b_sine = SineSafe(bg->direction);
        const float b_cosine = CosiSafe(bg->direction);
        const SDL_FPoint dangerous_point = GetHBladeAttackHittingPoint(bg, b_sine, b_cosine);
        if(!(pc->flags & dodge_time) && CircleMeetsPlayer(dangerous_point.x, dangerous_point.y, BEING_HIT_CIRCLE_DIAMET, pc)){
            if(pc->flags & block && (SineUnsafe(pc->direction) * b_sine) + (-CosiUnsafe(pc->direction) * -b_cosine) <= 0){
                HitBarrier(pc, &bg->impact);
            }else{
                DamagePlayer(pc, &bg->impact);
			    AddDamageVisualEffect(&gd->rend_data_ptr->visual_effects, &dangerous_point);
            }
        }
    }
    --bg->status_ticks_left;
}

static inline void StartBeingSearch(Being* const bg, const int duration){
    const float distance_x = bg->target_last_seen_at.x - bg->position.x;
    const float distance_y = bg->target_last_seen_at.y - bg->position.y;
    const float distance = SDL_sqrtf(pow2(distance_x) + pow2(distance_y));
    const float velocity_xy = distance / bg->velocity;
    bg->special_move_shift = (SDL_FPoint){distance_x / velocity_xy, distance_y / velocity_xy};
    bg->status = being_search;
    bg->status_ticks_left = duration;
}

static inline void TurnBlockedBeingWalk(Being* const bg, Game_data* const gd){
    float shift = SDL_copysignf(bg->velocity, bg->special_move_shift.x);
    Segment* seg = GetSegmentUnsafe(&gd->world, bg->position.x + shift, bg->position.y);
    if(seg == bg->segment){
        bg->position.x += shift;
        bg->special_move_shift.x = shift;
        bg->special_move_shift.y = 0.0F;
        return;
    }
    if(seg && seg->beings.num < MAX_SEGM_BEINGS){
        bg->position.x += shift;
        MoveBeingToSegment(bg, seg, gd->beings.array);
        bg->special_move_shift.x = shift;
        bg->special_move_shift.y = 0.0F;
        return;
    }
    shift = SDL_copysignf(bg->velocity, bg->special_move_shift.y);
    seg = GetSegmentUnsafe(&gd->world, bg->position.x, bg->position.y + shift);
    if(seg == bg->segment){
        bg->position.y += shift;
        bg->special_move_shift.x = 0.0F;
        bg->special_move_shift.y = shift;
        return;
    }
    if(seg && seg->beings.num < MAX_SEGM_BEINGS){
        bg->position.y += shift;
        MoveBeingToSegment(bg, seg, gd->beings.array);
        bg->special_move_shift.x = 0.0F;
        bg->special_move_shift.y = shift;
        return;
    }
    TurnBeingWalk(bg);
}

static inline void TurnBlockedBeing(Being* const b, Game_data* const gd){
    float shift = SDL_copysignf(b->velocity, b->special_move_shift.x);
    Segment* seg = GetSegmentUnsafe(&gd->world, b->position.x + shift, b->position.y);
    if(seg == b->segment){
        b->position.x += shift;
        return;
    }
    if(seg && seg->beings.num < MAX_SEGM_BEINGS){
        b->position.x += shift;
        MoveBeingToSegment(b, seg, gd->beings.array);
        return;
    }
    shift = SDL_copysignf(b->velocity, b->special_move_shift.y);
    seg = GetSegmentUnsafe(&gd->world, b->position.x, b->position.y + shift);
    if(seg == b->segment){
        b->position.y += shift;
        return;
    }
    if(seg && seg->beings.num < MAX_SEGM_BEINGS){
        b->position.y += shift;
        MoveBeingToSegment(b, seg, gd->beings.array);
        return;
    }
    TurnBeingWalk(b);
}

static inline void TurnBlockedAlly(Being* const bg, Game_data* const gd){
    float shift = SDL_copysignf(bg->velocity, bg->special_move_shift.x);
    Segment* seg = GetSegmentUnsafe(&gd->world, bg->position.x + shift, bg->position.y);
    if(seg == bg->segment){
        bg->position.x += shift;
        return;
    }
    if(seg && seg->ally_beings.num < MAX_SEGM_BEINGS){
        bg->position.x += shift;
        MoveAllyToSegment(bg, seg, gd->beings.array);
        return;
    }
    shift = SDL_copysignf(bg->velocity, bg->special_move_shift.y);
    seg = GetSegmentUnsafe(&gd->world, bg->position.x, bg->position.y + shift);
    if(seg == bg->segment){
        bg->position.y += shift;
        return;
    }
    if(seg && seg->ally_beings.num < MAX_SEGM_BEINGS){
        bg->position.y += shift;
        MoveAllyToSegment(bg, seg, gd->beings.array);
        return;
    }
    TurnBeingWalk(bg);
}

static inline void UpdateBeingSearch(Being* const bg, Game_data* const gd){
    if(bg->status_ticks_left == 0){
        FindTargetForBeing(bg, &gd->champions);
        if(IsClearSight(&bg->position, &bg->target.player->position, bg->target.player->segment, &gd->world)){
            bg->status = being_idle;
        }else{
            bg->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
        }
        return;
    }
    const float new_x = bg->position.x + bg->special_move_shift.x;
    const float new_y = bg->position.y + bg->special_move_shift.y;
    Segment* new_segment = GetSegmentUnsafe(&gd->world, new_x, new_y);
    if(new_segment != bg->segment){
        if(new_segment == NULL){
            TurnBlockedBeing(bg, gd);
            --bg->status_ticks_left;
            return;
        }
        if(new_segment->beings.num >= MAX_SEGM_BEINGS){
            --bg->status_ticks_left;
            return;
        }
        SetBeingPositionInNewSegment(bg, new_x, new_y, new_segment, gd->beings.array);
    }else{
        SetBeingPosition(bg, new_x, new_y);
    }
    --bg->status_ticks_left;
};

static inline void StartBeingFollow(Being* const bg, const int duration, const float distance_squared, const float distance_x, const float distance_y){
    const float distance = SDL_sqrtf(distance_squared);
    const float velocity_xy = distance / bg->velocity;
    bg->special_move_shift = (SDL_FPoint){distance_x / velocity_xy, distance_y / velocity_xy};
    bg->status = being_follow;
    bg->status_ticks_left = duration;
    bg->target_last_seen_at = (position16){bg->target.player->position.x, bg->target.player->position.y};
}

static inline void UpdateBeingFollow(Being* const bg, float const distance_squared, float const distance_x, float const distance_y, Game_data* const gd){
    if(bg->status_ticks_left == 0){
        bg->status = being_idle;
        return;
    }
    if(distance_squared < pow2(BEING_HALT_DISTANCE)){
        --bg->status_ticks_left;
        return;
    }
    if(distance_squared < pow2(BEING_FOCUS_DISTANCE)){
        const float distance = SDL_sqrtf(distance_squared);
        const float velocity_xy = distance / bg->velocity;
        bg->special_move_shift = (SDL_FPoint){distance_x / velocity_xy, distance_y / velocity_xy};
    }
    const float x_shift = bg->special_move_shift.x;
    const float y_shift = bg->special_move_shift.y;
    float new_x = bg->position.x + x_shift;
    float new_y = bg->position.y + y_shift;
    Segment* new_segment = GetSegmentUnsafe(&gd->world, new_x, new_y);
    bool collision = false;
    if(!new_segment || new_segment->beings.num >= MAX_SEGM_BEINGS){
        TurnBlockedBeingWalk(bg, gd);
        bg->status = being_walk;
        return;
    }else if(distance_squared < pow2(CHECK_COLLISION_DISTANCE) && bg->status_ticks_left % 2){
        collision = ResolveBeingCollisionInNewSegment(gd, bg, new_segment, &new_x, &new_y, x_shift, y_shift);
    }
    if(collision){
        new_segment = GetSegmentUnsafe(&gd->world, new_x, new_y);
    }
    if(new_segment != bg->segment){
        if(!new_segment || new_segment->beings.num >= MAX_SEGM_BEINGS){
            if(SDL_rand(2)){
                TurnBlockedBeingWalk(bg, gd);
                bg->status = being_walk;
            }else{
                HaltBeing(bg, BEING_WALK_TICKS);
            }
            return;
        }
    }else{
        SetBeingPosition(bg, new_x, new_y);
        --bg->status_ticks_left;
        return;
    }
    SetBeingPositionInNewSegment(bg, new_x, new_y, new_segment, gd->beings.array);
    --bg->status_ticks_left;
}

static inline SDL_FPoint GetHBladeAttackHittingPoint(Being* const bg, const float sine, const float cosine){
    static const float distance_from_being = WEAPON_ATTACK_Y + BLADE_SIZE * 0.85F - BEING_HIT_CIRCLE_DIAMET;
    return (SDL_FPoint){bg->position.x + sine * distance_from_being, bg->position.y - cosine * distance_from_being};
}

static inline void HaltBeing(Being* const bg, const int time){
    bg->status = being_idle;
    bg->status_ticks_left = -time;
}

extern inline bool IsAlly(const Being* const b){
    return TypeIsAlly(b->type_id);
}

static inline bool TypeIsAlly(const Uint8 type_id){
    if(type_id > being_warlock){
        return true;
    }
    return false;
}

static inline int GetMoraleDrop(const Uint8 type){
    const int morale_costs[] = HOSTILES_MORALE_COSTS;
    return *(morale_costs + type);
}

static inline void KillBeing(Being* const bg, Game_data* const gd, const unsigned int iter){
    const int morale_drop = GetMoraleDrop(bg->type_id);
    gd->enemy_morale = gd->enemy_morale + MAX_MORALE < morale_drop ? -MAX_MORALE : gd->enemy_morale - morale_drop;
    AddDeadVisualEffect(&gd->rend_data_ptr->visual_effects, &bg->position);
    *(gd->beings.indices + iter) = *(gd->beings.indices + --gd->beings.num);
    *(gd->beings.indices + gd->beings.num) = bg->main_indx;
}

static inline bool IsDeadBeing(Being* const bg, Game_data* const gd, const unsigned int iter){
    if(bg->status == being_dead){
        KillBeing(bg, gd, iter);
        return true;
    }
    return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// static void test(Being* bg, Game_data* gd){ 
//     if((!IsAlly(bg) && bg->status != being_in_void && bg->status != being_dead && (gd->beings.array + *(bg->segment->beings.beings_ind + bg->indx) != bg || bg->indx >= bg->segment->beings.num || GetSegmentSafe(&gd->world, bg->position.x, bg->position.y) != bg->segment)) || (bg->status == being_dead && bg->hit_points > 0)){
//         SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "ERROR!!!");
//         exit(-2);
//     }
// }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UpdateBeings(Game_data* const gd){
    if((gd->flags & gamef_horde_attack)){
        for(unsigned int i = 0U; i < gd->beings.num; ++i){
            Being* bg = (gd->beings.array + *(gd->beings.indices + i));
            if(bg->status == being_in_void){
                const unsigned int point_indx = SDL_rand(HORDE_ATTACK_POINTS);
                unsigned int point_indx1 = point_indx;
                do{
                    const float x = (gd->horde_data.creation_points + point_indx1)->x;
                    const float y = (gd->horde_data.creation_points + point_indx1)->y;
                    Segment* const seg = GetSegmentUnsafe(&gd->world, x, y);
                    if(seg->beings.num < MAX_SEGM_BEINGS){
                        SetBeingPosition(bg, x, y);
                        AddBeingToSegment(seg, bg, &seg->beings);
                        HaltBeing(bg, 1);
                        break;
                    }
                    point_indx1 = (point_indx1 + 1U) % HORDE_ATTACK_POINTS;
                }while(point_indx1 != point_indx);
            }else if(SDL_PointInRectFloat(&bg->position, &human(gd)->attention_rect)){
                if(IsDeadBeing(bg, gd, i)){
                    --i;
                    continue;
                }
                (being_types + bg->type_id)->update(bg, gd);
                // test(bg, gd);/////////////////////////
            }
        }
        return;
    }
    if(gd->enemy_morale <= 0){
        for(unsigned int i = 0U; i < gd->beings.num; ++i){
            Being* bg = (gd->beings.array + *(gd->beings.indices + i));
            if(SDL_PointInRectFloat(&bg->position, &human(gd)->attention_rect)){
                if(IsDeadBeing(bg, gd, i)){
                    --i;
                    continue;
                }
                if(IsAlly(bg) || bg->status == being_walk || bg->status == being_fly || bg->status == being_stunned || (bg->status_ticks_left < 0 && bg->status == being_idle) || bg->target_last_seen_at.x == 0.0F){
                    (being_types + bg->type_id)->update(bg, gd);
                    // test(bg, gd);/////////////////////////////////
                }else if(bg->status != being_in_void){
                    BeingFlee(bg, gd);
                }
            }
        }  
        if(!(gd->flags & gamef_morale_break)){
            gd->flags |= gamef_morale_break;
            gd->enemy_morale -= MORALE_BREAK_DROP;
        }
        ++gd->enemy_morale;
        return;
    }
    if(gd->flags & gamef_morale_break){
        gd->flags &= ~(gamef_morale_break);
    }
    for(unsigned int i = 0U; i < gd->beings.num; ++i){
        Being* bg = (gd->beings.array + *(gd->beings.indices + i));
        if(SDL_PointInRectFloat(&bg->position, &human(gd)->attention_rect)){
            if(IsDeadBeing(bg, gd, i)){
                --i;
                continue;
            }
            (being_types + bg->type_id)->update(bg, gd);
            // test(bg, gd);/////////////////////////////////
        }
    }
    if(gd->enemy_morale < MAX_MORALE){
        ++gd->enemy_morale;
    }
}

void UpdateBeingsEffects(Game_data* const gd){
    for(unsigned int i = 0U; i < gd->beings.num; ++i){
        Being* bg = (gd->beings.array + *(gd->beings.indices + i));
        if(IsDeadBeing(bg, gd, i)){
            --i;
            continue;
        }
        UpdateBeingEffects(gd, bg);
        // test(bg, gd);/////////////////////////////////
    }
}

extern inline bool DamageBeing(Being* const bg, const Impact* const impact, Being* const main_beings){
    bg->hit_points -= CalculateDamage(impact, &bg->armour);
    if(bg->hit_points < 1){
        bg->status = being_dead;
        RemoveBeingFromSegment(bg, &bg->segment->beings, main_beings);
        return true;
    }
    return false;
}

extern inline bool DamageAlly(Being* const bg, const Impact* const impact, Being* const main_beings){
    bg->hit_points -= CalculateDamage(impact, &bg->armour);
    if(bg->hit_points < 1){
        bg->status = being_dead;
        RemoveBeingFromSegment(bg, &bg->segment->ally_beings, main_beings);
        return true;
    }
    return false;
}

extern inline void StunBeing(Being* const bg, const int duration){
    bg->status = being_stunned;
    bg->status_ticks_left = duration;
}

extern inline void CatapultBeing(Being* const bg, const float shift_x, const float shift_y, const int duration){
    bg->status = being_fly;
    bg->status_ticks_left = duration;
    bg->special_move_shift = (SDL_FPoint){shift_x, shift_y};
    if(duration < 16){
        bg->rend_fly_help_data.ticks = 0;
    }else{
        bg->rend_fly_help_data.start_angle = bg->direction;
        bg->rend_fly_help_data.ticks = SDL_rand(2) ? duration : -duration;
    }
}

static inline void UpdateBeingStunned(Being* const bg){
    if(bg->status_ticks_left == 0){
        bg->status = being_idle;
        return;
    }
    --bg->status_ticks_left;
}

static inline void UpdateBeingFly(Being* const bg, Game_data* const gd){
    if(bg->status_ticks_left == 0){
        StunBeing(bg, BEING_STUN_DURATION / bg->armour.unstability);
        return;
    }
    float new_x = bg->position.x + bg->special_move_shift.x;
    float new_y = bg->position.y + bg->special_move_shift.y;
    Segment* new_segment = GetSegmentUnsafe(&gd->world, new_x, new_y);
    if(new_segment != bg->segment){
        if(new_segment == NULL || new_segment->beings.num >= MAX_SEGM_BEINGS){
            StunBeing(bg, BEING_STUN_DURATION * bg->armour.unstability);
        }else{
            SetBeingPositionInNewSegment(bg, new_x, new_y, new_segment, gd->beings.array);
        }
    }else{
        SetBeingPosition(bg, new_x, new_y);
    }
    --bg->status_ticks_left;
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

static inline bool IdleBeingCollision(Being* const bg, Game_data* const gd){
    Segment* segs[2];
    Get2NearestSegments(segs, &gd->world, bg->segment, bg->position.x, bg->position.y);
    for(unsigned int k = 0U; k < 2; ++k){ 
        if(!(*(segs + k))) continue;
        for(unsigned int i = 0U; i < (*(segs + k))->beings.num; ++i){
            Being* bg1 = (gd->beings.array + *((*(segs + k))->beings.beings_ind + i));
            if(bg1 == bg || bg1->status == being_walk) continue;
            if(BeingCollideWithBeing(bg1, bg->position.x, bg->position.y, BeingSize(bg))){
                FindTargetForBeing(bg, &gd->champions);
                const float distance_x = bg->target.player->position.x - bg->position.x;
                const float distance_y = bg->target.player->position.y - bg->position.y;
                const float distance_squared = pow2(distance_x) + pow2(distance_y);
                if(distance_squared <= pow2(BEING_HALT_DISTANCE)){
                    StartBeingRandWalk(bg, BEING_DEFAULT_LEFT_TICKS);
                    return true;
                }
                if(IsClearSightWithKnownDistance(&bg->position, bg->target.player->segment, &gd->world, distance_x, distance_y, distance_squared)){
                    StartBeingFollow(bg, BEING_DEFAULT_LEFT_TICKS, distance_squared, distance_x, distance_y);
                }else{
                    StartBeingRandWalk(bg, BEING_WALK_TICKS);
                }
                return true;
            }
        }
    }
    return false;
}

static inline bool FindAllyTarget(Being* const bg, Game_data* const gd){
    if(bg->segment->beings.num > 0U){
        bg->target.being = (gd->beings.array + *(bg->segment->beings.beings_ind));
        return true;
    }
    for(unsigned int i = 1U; i < SPIRAL_STEPS(ALLY_ATTENTION_RANGE); ++i){
        Segment* seg = GetDistantSegmentBySpiral(&gd->world, bg->segment, i);
        if(seg && seg->beings.num > 0U){
            Being* const target = gd->beings.array + *(seg->beings.beings_ind);
            if(IsClearSight(&bg->position, &target->position, seg, &gd->world)){
                bg->target.being = target;
                return true;
            }
        }
    }
    return false;
}

static inline void StartAllyFollow(Being* const bg, const int duration){
    const float target_x = bg->main_indx % 2U == 1U? bg->target.player->position.x + SEGMENT_SIZE * (0x0.1p0F * (float)(bg->main_indx % 0x10U)) : bg->target.player->position.x - SEGMENT_SIZE * (0x0.1p0F * (float)(bg->main_indx % 0x10U));
    const float target_y = bg->main_indx % 4U < 2U ? bg->target.player->position.y + SEGMENT_SIZE * (0x0.1p0F * (float)(bg->main_indx % 0x10U)) : bg->target.player->position.y - SEGMENT_SIZE * (0x0.1p0F * (float)(bg->main_indx % 0x10U));
    const float distance_x = target_x - bg->position.x;
    const float distance_y = target_y - bg->position.y;
    const float distance_squared = pow2(distance_x) + pow2(distance_y);
    if(distance_squared < pow2(BEING_HALT_DISTANCE)){
        HaltBeing(bg, duration);
        return;
    }
    const float distance = SDL_sqrtf(distance_squared);
    const float velocity_xy = distance / bg->velocity;
    bg->special_move_shift = (SDL_FPoint){distance_x / velocity_xy, distance_y / velocity_xy};
    bg->status = being_follow;
    bg->status_ticks_left = duration;
}

static inline void UpdateAllyFollow(Being* const bg, Game_data* const gd){
    if(bg->status_ticks_left == 0){
        bg->status = being_idle;
        return;
    }
    float new_x = bg->position.x + bg->special_move_shift.x;
    float new_y = bg->position.y + bg->special_move_shift.y;
    Segment* new_segment = GetSegmentUnsafe(&gd->world, new_x, new_y);
    if(!new_segment){
        StartBeingWalkWithRandTurn45Deg(bg, BEING_WALK_TICKS, bg->special_move_shift.x, bg->special_move_shift.y);
        new_x = bg->position.x + bg->special_move_shift.x;
        new_y = bg->position.y + bg->special_move_shift.y;
        new_segment = GetSegmentUnsafe(&gd->world, new_x, new_y);
    }
    if(new_segment != bg->segment){
        if(!new_segment){
            StartBeingWalkWithRandTurn(bg, BEING_WALK_TICKS, bg->special_move_shift.x * 0.5F, bg->special_move_shift.y * 0.5F);
            return;
        }else if(new_segment->ally_beings.num >= MAX_SEGM_BEINGS){
            HaltBeing(bg, BEING_WALK_TICKS);
            return;
        }
    }else{
        SetBeingPosition(bg, new_x, new_y);
        --bg->status_ticks_left;
        return;
    }
    SetAllyPositionInNewSegment(bg, new_x, new_y, new_segment, gd->beings.array);
    --bg->status_ticks_left;
}

static inline void UpdateAllySearch(Being* const bg, Game_data* const gd){
    if(bg->status_ticks_left == 0){
        if(IsClearSight(&bg->position, &bg->target.player->position, bg->target.player->segment, &gd->world)){
            bg->status = being_idle;
        }else{
            bg->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
        }
        return;
    }
    const float new_x = bg->position.x + bg->special_move_shift.x;
    const float new_y = bg->position.y + bg->special_move_shift.y;
    Segment* new_segment = GetSegmentUnsafe(&gd->world, new_x, new_y);
    if(new_segment != bg->segment){
        if(new_segment == NULL){
            TurnBlockedAlly(bg, gd);
            --bg->status_ticks_left;
            return;
        }
        if(new_segment->ally_beings.num >= MAX_SEGM_BEINGS){
            --bg->status_ticks_left;
            return;
        }
        SetAllyPositionInNewSegment(bg, new_x, new_y, new_segment, gd->beings.array);
    }else{
        SetBeingPosition(bg, new_x, new_y);
    }
    --bg->status_ticks_left;
};

static inline void UpdateAllyWalk(Being* const bg, Game_data* const gd){
    if(bg->status_ticks_left == 0){
        bg->status = being_idle;
        return;
    }
    const float new_x = bg->position.x + bg->special_move_shift.x;
    const float new_y = bg->position.y + bg->special_move_shift.y;
    Segment* new_segment = GetSegmentUnsafe(&gd->world, new_x, new_y);
    if(new_segment != bg->segment){
        if(new_segment == NULL || new_segment->ally_beings.num >= MAX_SEGM_BEINGS){
            TurnBeingWalk(bg);
            return;
        }
        SetAllyPositionInNewSegment(bg, new_x, new_y, new_segment, gd->beings.array);
    }else{
        SetBeingPosition(bg, new_x, new_y);
    }
    --bg->status_ticks_left;
}

static inline bool AllyNear(Being* const bg, Game_data* const gd){
    if(bg->segment->ally_beings.num > 0U){
        bg->target.being = (gd->beings.array + *(bg->segment->ally_beings.beings_ind));
        return true;
    }
    for(unsigned int i = 1U; i < SPIRAL_STEPS(BEING_ALLY_DETEC_RANGE); ++i){
        Segment* seg = GetDistantSegmentBySpiral(&gd->world, bg->segment, i);
        if(seg && seg->ally_beings.num > 0U){
            Being* const target = gd->beings.array + *(seg->ally_beings.beings_ind);
            if(IsClearSight(&bg->position, &target->position, seg, &gd->world)){
                bg->target.being = target;
                return true;
            }
        }
    }
    return false;
}

static inline Being* GetOtherBeingNearPlayer(Being* const bg, Game_data* const gd){//
    if(bg->target.player->segment->beings.num > 0U){
        Being* target = (gd->beings.array + *(bg->target.player->segment->beings.beings_ind));
        if(target != bg){
            return target;
        }else if(bg->target.player->segment->beings.num > 1U){
            return (gd->beings.array + *(bg->target.player->segment->beings.beings_ind + 1U));
        }
    }
    for(unsigned int i = 1U; i < SPIRAL_STEPS(2U); ++i){
        Segment* seg = GetDistantSegmentBySpiral(&gd->world, bg->segment, i);
        if(seg == NULL || seg->beings.num == 0U) continue;
        Being* target = (gd->beings.array + *(seg->beings.beings_ind));
        if(target != bg){
            return target;
        }else if(seg->beings.num > 1U){
            return (gd->beings.array + *(bg->target.player->segment->beings.beings_ind + 1U));
        }
    }
    return NULL;
}

static inline void UpdateBeingAttackAlly(Being* const bg, Game_data* const gd, const float attack_range){
    const float distance_x = bg->target.being->position.x - bg->position.x;
    const float distance_y = bg->target.being->position.y - bg->position.y;
    const float distance_squared = pow2(distance_x) + pow2(distance_y);
    if(!IsClearSightWithKnownDistance(&bg->position, bg->target.being->segment, &gd->world, distance_x, distance_y, distance_squared)){
        bg->status = being_idle;
        return;
    }
    if(attack_range){
        if(distance_squared < pow2(attack_range)){
            if(distance_squared < pow2(BEING_ATTACK_DISTANCE)){
                bg->status = being_strike_being;
                bg->status_ticks_left = BEING_ATTACK_STEPS * 3;
                return;
            }else{
                bg->status = being_shoot_being;
                bg->status_ticks_left = BEING_RELOAD;
                return;
            }
        }
    }else if(distance_squared < pow2(BEING_ATTACK_DISTANCE)){
        bg->status = being_strike_being;
        bg->status_ticks_left = BEING_ATTACK_STEPS * 3;
        return;
    }
    const float distance = SDL_sqrtf(distance_squared);
    const float velocity_xy = distance / bg->velocity;
    bg->special_move_shift = (SDL_FPoint){distance_x / velocity_xy, distance_y / velocity_xy};
    bg->status = being_follow_being;
    bg->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
}

static inline void MovePlayerIfTooClose(const float d_x, const float d_y, const float distance_squared, const Being* const bg, Game_data* const gd){
    if(distance_squared < pow2(half(PLAYER_SIZE) + half(BeingSize(bg)))){
        const float distance = SDL_sqrtf(distance_squared);
        const float velocity_xy = distance / (PLAYER_VELOCITY * 1.875F);
        MovePlayer(gd, bg->target.player, d_x / velocity_xy, d_y / velocity_xy);
    }
}

static inline bool FleeBeingCollision(Being* const bg, Game_data* const gd){
    for(unsigned int i = 0U; i < bg->segment->beings.num; ++i){
        Being* bg1 = (gd->beings.array + *(bg->segment->beings.beings_ind + i));
        if(bg1 == bg || bg1->status == being_idle || (bg1->status == being_walk && SDL_rand(2))) continue;
        if(BeingCollideWithBeing(bg1, bg->position.x, bg->position.y, BeingSize(bg))){
            StartBeingWalkWithRandTurn45Deg(bg, FLEE_COLLI_WALK_TICKS, bg->special_move_shift.x, bg->special_move_shift.y);
            return true;
        }
    }
    return false;
}

#define DistancesDeclaration    float distance_x, distance_y, distance_squared;
#define SetShifts               if(direction == 0U){\
                                    x_shift = 0U;\
                                    y_shift = -1U;\
                                    x_fshift0 = half(BIG_SEGMENT_SIZE);\
                                    y_fshift0 = 0.0F;\
                                    x_fshift1 = bg->special_move_shift.x;\
                                    y_fshift1 = -SEGMENT_SIZE * 0.125F;\
                                }else if(direction == 1U){\
                                    x_shift = -1U;\
                                    y_shift = 0U;\
                                    x_fshift0 = 0.0F;\
                                    y_fshift0 = half(BIG_SEGMENT_SIZE);\
                                    x_fshift1 = -SEGMENT_SIZE * 0.125F;\
                                    y_fshift1 = bg->special_move_shift.y;\
                                }else if(direction == 2U){\
                                    x_shift = 0U;\
                                    y_shift = 1U;\
                                    x_fshift0 = half(BIG_SEGMENT_SIZE);\
                                    y_fshift0 = BIG_SEGMENT_SIZE;\
                                    x_fshift1 = bg->special_move_shift.x;\
                                    y_fshift1 = SEGMENT_SIZE * 0.125F;\
                                }else{\
                                    x_shift = 1U;\
                                    y_shift = 0U;\
                                    x_fshift0 = BIG_SEGMENT_SIZE;\
                                    y_fshift0 = half(BIG_SEGMENT_SIZE);\
                                    x_fshift1 = SEGMENT_SIZE * 0.125F;\
                                    y_fshift1 = bg->special_move_shift.y;\
                                }

static inline void BeingFlee(Being* const bg, Game_data* const gd){
    if(FleeBeingCollision(bg, gd)){
        return;
    }
    DistancesDeclaration
    unsigned int x_shift, y_shift;
    float x_fshift0, y_fshift0, x_fshift1, y_fshift1;
    unsigned int direction = bg->main_indx % 4U;
    SetShifts
    const unsigned int seg_x = GetBigSegCoordinate(bg->position.x);
    const unsigned int seg_y = GetBigSegCoordinate(bg->position.y);
    const unsigned int pc_seg_x = GetBigSegCoordinate(human(gd)->position.x);
    const unsigned int pc_seg_y = GetBigSegCoordinate(human(gd)->position.y);
    if(seg_x + x_shift == pc_seg_x && seg_y + y_shift == pc_seg_y){
        direction = (direction + 1U) % 4;
        SetShifts
    }
    if(!IsVoidBigSeg(gd->world.plan, seg_x + x_shift, seg_y + y_shift)){
        GetBeingDistances(bg, &(SDL_FPoint){BigSegCenter(seg_x + x_shift) , BigSegCenter(seg_y + y_shift)}, &distance_x, &distance_y, &distance_squared);
    }else{
        GetBeingDistances(bg, &(SDL_FPoint){BigSegPosition(seg_x) + x_fshift0, BigSegPosition(seg_y) + y_fshift0}, &distance_x, &distance_y, &distance_squared);
        if(distance_squared <= pow2(SEGMENT_SIZE * 1.5F) && !GetSegmentUnsafe(&gd->world, bg->position.x + x_fshift1, bg->position.y + y_fshift1)){
            AddBeingEffect(bg, (Lasting_effect){being_effect_open_portal, OPENING_PORTAL_TICKS});
            HaltBeing(bg, OPENING_PORTAL_TICKS * 2);
            AddPortalVisualEffect(&gd->rend_data_ptr->visual_effects, &bg->position);
            return;
        }
    }
    const float distance = SDL_sqrtf(distance_squared);
    const float velocity_xy = distance / bg->velocity;
    StartBeingWalk(bg, BEING_WALK_TICKS, distance_x / velocity_xy, distance_y / velocity_xy);
}
//---------------------------------------------------------------------------------------------------------------
extern inline void AddBeingEffect(Being* const bg, const Lasting_effect effect){
	if(bg->effects_num < MAX_BEING_EFFECTS){
	    AddLastingEffect(bg->effects, effect, bg->effects_num++);
    }
}

extern inline void AddOrUpdateBeingEffect(Being* const bg, const Lasting_effect effect){
	const int effect_indx = BeingHasEffect(bg, effect.id);
	if(effect_indx == -1){
		AddBeingEffect(bg, effect);
	}else{
		(bg->effects + effect_indx)->ticks_left = effect.ticks_left;
	}
}

static inline void RemoveBeingEffect(Being* const bg, const int indx){
	RemoveLastingEffect(bg->effects, indx, bg->effects_num--);
}

static inline void UpdateBeingEffects(Game_data* const gd, Being* const bg){
	for(unsigned int i = 0U; i < bg->effects_num; ++i){
		UpdateBeingEffect(gd, bg, i);
	}
}

static inline void UpdateBeingEffect(Game_data* const gd, Being* const bg, const int effect_indx){
    const void (*effect[])(Game_data* const, Being* const, const int) = BEING_LASTING_EFFECTS;
    (*(effect + (bg->effects + effect_indx)->id))(gd, bg, (bg->effects + effect_indx)->ticks_left--);
	if((bg->effects + effect_indx)->ticks_left < 1 && bg->effects_num > 0U){
		RemoveBeingEffect(bg, effect_indx);
	}
}

extern inline int BeingHasEffect(Being* const bg, const unsigned int effect_id){
    for(unsigned int i = 0U; i < bg->effects_num; ++i){
        if((bg->effects + i)->id == effect_id){
            return i;
        }
    }
    return -1;
}

void SlowBeing(Game_data* const gd, Being* const bg, const int ticks_left){
    if(ticks_left < 2){
        if(bg->velocity < (being_types + bg->type_id)->velocity){
            bg->velocity = (being_types + bg->type_id)->velocity;
        }
    }else if(ticks_left % 64 == 0){
        AddCurseVisualEffect(&gd->rend_data_ptr->visual_effects, &bg->position);
    }
}

void CommanderAura(Game_data* const gd, Being* const bg, const int ticks_left){
    if(ticks_left < 2){
        if(SDL_PointInRectFloat(&bg->position, &human(gd)->attention_rect)){
            const int range = 2;
            const unsigned int array_size = pow2(1 + range * 2);
            Segment* neighbour_segs[array_size];
            GetNeighbourSegmentsFar(neighbour_segs, &gd->world, bg->segment, range);
            for(unsigned int k = 0U; k < array_size; ++k){
                Segment* neighbour = *(neighbour_segs + k);
                if(neighbour == NULL) continue;
                for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
                    Being* bg1 = (gd->beings.array + *(neighbour->beings.beings_ind + i));
                    const int effect_indx = BeingHasEffect(bg1, being_effect_bonus);
                    if(effect_indx == -1){
                        AddBeingEffect(bg1, (Lasting_effect){being_effect_bonus, COMMANDER_EFFECT_TICKS});
                        bg1->impact.damage *= 2.0F;
                        bg1->impact.armour_reduction *= 10.0F;
                        bg1->impact.magic *= 2.0F;
                        bg1->impact.stun *= 2.0F;
                    }else{
                        (bg1->effects + effect_indx)->ticks_left = COMMANDER_EFFECT_TICKS;
                    }
                }
            }
        }
        (bg->effects + BeingHasEffect(bg, being_effect_commander))->ticks_left = COMMANDER_EFFECT_TICKS;
    }
}

void CommanderIsNear(Game_data* const gd, Being* const bg, const int ticks_left){
    if(ticks_left < 2){
        bg->impact = (being_types + bg->type_id)->impact;
    }else if(ticks_left % 64 == 0){
        AddBonusVisualEffect(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
            bg->position.x + (SDL_randf() - 0.5F) * half(BeingSize(bg)),
            bg->position.y + (SDL_randf() - 0.5F) * half(BeingSize(bg))
        });
    }
}

void Burn(Game_data* const gd, Being* const bg, const int ticks_left){
    if(ticks_left % 4 == 0){
        if(DamageBeing(bg, &(Impact){1.0F, 1.0F, 2.0F, 1.0F}, gd->beings.array)){
            bg->effects_num = 0U;
        }
        AddSmallBurnVisualEffect(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
            bg->position.x + (SDL_randf() - 0.5F) * half(BeingSize(bg)),
            bg->position.y + (SDL_randf() - 0.5F) * half(BeingSize(bg))
        });
    }
}

void OpeningPortal(Game_data* const gd, Being* const bg, const int ticks_left){
    if(ticks_left < 2){
        bg->status = being_in_void;
        bg->effects_num = 0U;
        RemoveBeingFromSegment(bg, &bg->segment->beings, gd->beings.array);
        bg->position = ZERO_POINT_F;
    }
}

void ThunderboltChain(Game_data* const gd, Being* const bg, const int ticks_left){
    if(ticks_left < 2){
        Being* bg1 = NULL;
        const unsigned int range = 4U;
        const unsigned int max_i = SPIRAL_STEPS(range);
        const unsigned int rand_seg = SDL_rand(max_i - 1) + 1U;
        unsigned int seg_i = rand_seg;
        do{
            Segment* const seg = GetDistantSegmentBySpiral(&gd->world, bg->segment, seg_i);
            if(seg && seg->beings.num > 0U){
                const int rand = SDL_rand(seg->beings.num);
                int indx = rand;
                do{
                    if(BeingHasEffect(gd->beings.array + *(seg->beings.beings_ind + indx), being_effect_thunderbolt) == -1){
                        bg1 = gd->beings.array + *(seg->beings.beings_ind + indx);
                        goto search_end;
                    }
                    indx = (indx + 1) % seg->beings.num;
                }while(indx != rand);
            }
            if(!seg_i){
                seg_i = (seg_i + 1U) % max_i;
                continue;
            }
            seg_i = (seg_i + 1U) % max_i;
        }while(seg_i != rand_seg);
        Segment* const seg = bg->segment;
        if(seg->beings.num > 1U){
            const int rand = SDL_rand(seg->beings.num);
            int indx = rand;
            do{
                if(bg->indx != indx && BeingHasEffect(gd->beings.array + *(seg->beings.beings_ind + indx), being_effect_thunderbolt) == -1){
                    bg1 = gd->beings.array + *(seg->beings.beings_ind + indx);
                    goto search_end;
                }
                indx = (indx + 1) % seg->beings.num;
            }while(indx != rand);
        }
        search_end:
        if(bg1){
            AddBoltVisualEffect(&gd->rend_data_ptr->visual_effects, &bg1->position, (position16){(_Float16)bg->position.x, (_Float16)bg->position.y});
            AddBeingEffect(bg1, (Lasting_effect){being_effect_thunderbolt, BOLT_CHAIN_TICKS});
        }
        if(DamageBeing(bg, &(Impact){4.0F, 1.0F, 8.0F, 1.0F}, gd->beings.array)){
            bg->effects_num = 0U;
        }else if(bg->status == being_stunned){
            const float thunderbolt_push_power = 6.0F;
            const float power = thunderbolt_push_power * bg->armour.unstability;
            const float angle = FULL_ANGLE * SDL_randf();
            const float vel = BASE_FLY_VELOCITY * power;
            CatapultBeing(bg, SineUnsafe(angle) * vel, -CosiUnsafe(angle) * vel, BASE_FLY_TICKS * power);
        }else if(bg->status != being_fly){
            StunBeing(bg, NAP_TICKS);
        }
        AddSmallBurnVisualEffect(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
            bg->position.x + (SDL_randf() - 0.5F) * half(BeingSize(bg)),
            bg->position.y + (SDL_randf() - 0.5F) * half(BeingSize(bg))
        });
    }
}
//---------------------------------------------------------------------------------------------------------------
#define BeingFollowBeingCode    if(bg->status == being_follow_being){\
                                    GetBeingDistances(bg, &bg->target.being->position, &distance_x, &distance_y, &distance_squared);\
                                    UpdateBeingFollow(bg, distance_squared, distance_x, distance_y, gd);\
                                    return;\
                                }
                                
static inline void UpdateBeingOrdinary(Being* const bg, Game_data* const gd){
    if(bg->status == being_walk){
        UpdateBeingWalk(bg, gd);
        return;
    }
    if(bg->status == being_fly){
        UpdateBeingFly(bg, gd);
        return;
    }
    if(bg->status == being_search){
        UpdateBeingSearch(bg, gd);
        return;
    }
    DistancesDeclaration
    BeingFollowBeingCode
    if(bg->status == being_strike_being){
        GetBeingDistances(bg, &bg->target.being->position, &distance_x, &distance_y, &distance_squared);
        UpdateBeingStrikeAlly(bg, distance_squared, distance_x, distance_y, gd);
        return;
    }
    Player* pc;
    if(bg->status == being_idle){
        if(bg->status_ticks_left < 0){
            ++bg->status_ticks_left;
            return;
        }
        if(IdleBeingCollision(bg, gd)){
            return;
        }
        FindTargetForBeing(bg, &gd->champions);
        pc = bg->target.player;
        GetBeingDistances(bg, &pc->position, &distance_x, &distance_y, &distance_squared);
        if(distance_squared < pow2(BEING_ATTACK_DISTANCE)){
            bg->status = being_strike;
            bg->status_ticks_left = BEING_ATTACK_STEPS * 3;
        }else if(AllyNear(bg, gd)){
            UpdateBeingAttackAlly(bg, gd, 0.0F);
        }else if(IsClearSightWithKnownDistance(&bg->position, pc->segment, &gd->world, distance_x, distance_y, distance_squared)){
            StartBeingFollow(bg, BEING_DEFAULT_LEFT_TICKS, distance_squared, distance_x, distance_y);
        }else if(bg->target_last_seen_at.x != 0.0F){
            StartBeingSearch(bg, BEING_DEFAULT_LEFT_TICKS);
        }else{
            HaltBeing(bg, NAP_TICKS);
        }
        return;
    }else{
        pc = bg->target.player;
        GetBeingDistances(bg, &pc->position, &distance_x, &distance_y, &distance_squared);
    }
    MovePlayerIfTooClose(distance_x, distance_y, distance_squared, bg, gd);
    if(bg->status == being_stunned){
        UpdateBeingStunned(bg);
        return;
    }
    if(bg->status == being_strike){
        UpdateBeingStrike(bg, pc, distance_squared, distance_x, distance_y, gd);
        return;
    }
    if(bg->status == being_follow){
        UpdateBeingFollow(bg, distance_squared, distance_x, distance_y, gd);
    }
}

static inline void UpdateBeingRanger(Being* const bg, Game_data* const gd){
    if(bg->status == being_walk){
        UpdateBeingWalk(bg, gd);
        return;
    }
    if(bg->status == being_fly){
        UpdateBeingFly(bg, gd);
        return;
    }
    if(bg->status == being_shoot_being){
        UpdateBeingShoot(bg, &gd->projectiles, &gd->world, &bg->target.being->position, bg->target.being->segment);
        return;
    }
    if(bg->status == being_search){
        UpdateBeingSearch(bg, gd);
        return;
    }
    DistancesDeclaration
    BeingFollowBeingCode
    if(bg->status == being_strike_being){
        GetBeingDistances(bg, &bg->target.being->position, &distance_x, &distance_y, &distance_squared);
        UpdateBeingStrikeAlly(bg, distance_squared, distance_x, distance_y, gd);
        return;
    }
    Player* pc;
    if(bg->status == being_idle){
        if(bg->status_ticks_left < 0){
            ++bg->status_ticks_left;
            return;
        }
        if(IdleBeingCollision(bg, gd)){
            return;
        }
        FindTargetForBeing(bg, &gd->champions);
        pc = bg->target.player;
        GetBeingDistances(bg, &pc->position, &distance_x, &distance_y, &distance_squared);
        if(distance_squared < pow2(BEING_SHOOT_DISTANCE)){
            if(distance_squared < pow2(BEING_ATTACK_DISTANCE)){
                bg->status = being_strike;
                bg->status_ticks_left = BEING_ATTACK_STEPS * 3;
                return;
            }else{
                if(AllyNear(bg, gd)){
                    UpdateBeingAttackAlly(bg, gd, BEING_SHOOT_DISTANCE);
                    return;
                }else if(IsClearSightWithKnownDistance(&bg->position, pc->segment, &gd->world, distance_x, distance_y, distance_squared)){
                    bg->target_last_seen_at = (position16){bg->target.player->position.x, bg->target.player->position.y};
                    bg->status = being_shoot;
                    bg->status_ticks_left = BEING_RELOAD;
                    return;
                }
            }
        }
        if(AllyNear(bg, gd)){
            UpdateBeingAttackAlly(bg, gd, BEING_SHOOT_DISTANCE);
        }else if(IsClearSightWithKnownDistance(&bg->position, pc->segment, &gd->world, distance_x, distance_y, distance_squared)){
            StartBeingFollow(bg, BEING_DEFAULT_LEFT_TICKS, distance_squared, distance_x, distance_y);
        }else if(bg->target_last_seen_at.x != 0.0F){
            StartBeingSearch(bg, BEING_DEFAULT_LEFT_TICKS);
        }else{
            HaltBeing(bg, NAP_TICKS);
        }
        return;
    }else{
        pc = bg->target.player;
        GetBeingDistances(bg, &pc->position, &distance_x, &distance_y, &distance_squared);
    }
    MovePlayerIfTooClose(distance_x, distance_y, distance_squared, bg, gd);
    if(bg->status == being_shoot){
        UpdateBeingShoot(bg, &gd->projectiles, &gd->world, &bg->target.player->position, bg->target.player->segment);
        return;
    }
    if(bg->status == being_stunned){
        UpdateBeingStunned(bg);
        return;
    }
    if(bg->status == being_strike){
        UpdateBeingStrike(bg, pc, distance_squared, distance_x, distance_y, gd);
        return;
    }
    if(bg->status == being_follow){
        UpdateBeingFollow(bg, distance_squared, distance_x, distance_y, gd);
    }
}

static inline void UpdateBeingCommander(Being* const bg, Game_data* const gd){
    if(bg->status == being_walk){
        UpdateBeingWalk(bg, gd);
        return;
    }
    if(bg->status == being_fly){
        UpdateBeingFly(bg, gd);
        return;
    }
    if(bg->status == being_search){
        UpdateBeingSearch(bg, gd);
        return;
    }
    DistancesDeclaration
    BeingFollowBeingCode
    if(bg->status == being_strike_being){
        GetBeingDistances(bg, &bg->target.being->position, &distance_x, &distance_y, &distance_squared);
        UpdateBeingStrikeAlly(bg, distance_squared, distance_x, distance_y, gd);
        return;
    }
    Player* pc;
    if(bg->status == being_idle){
        if(bg->status_ticks_left < 0){
            ++bg->status_ticks_left;
            return;
        }
        if(IdleBeingCollision(bg, gd)){
            return;
        }
        FindTargetForBeing(bg, &gd->champions);
        pc = bg->target.player;
        GetBeingDistances(bg, &pc->position, &distance_x, &distance_y, &distance_squared);
        if(bg->hit_points <= (being_types + bg->type_id)->hit_points / 4){
            if(distance_squared < pow2(BEING_HALT_DISTANCE)){
                bg->status = being_strike;
                bg->status_ticks_left = BEING_ATTACK_STEPS * 3;
                return;
            }
            Being* being_near = GetOtherBeingNearPlayer(bg, gd);
            if(being_near){
                float distance_x1, distance_y1, distance_squared1;
                GetBeingDistances(bg, &being_near->position, &distance_x1, &distance_y1, &distance_squared1);
                if(distance_squared1 >= pow2(BEING_ATTACK_DISTANCE)){
                    bg->target.being = being_near;
                    const float distance = SDL_sqrtf(distance_squared1);
                    const float velocity_xy = distance / bg->velocity;
                    bg->special_move_shift = (SDL_FPoint){distance_x1 / velocity_xy, distance_y1 / velocity_xy};
                    bg->status = being_follow_being;
                    bg->status_ticks_left = BEING_DEFAULT_LEFT_TICKS;
                    return;
                }
                if(SDL_rand(2)){
                    HaltBeing(bg, BEING_DEFAULT_LEFT_TICKS * 2);
                }else{
                    const float distance = SDL_sqrtf(distance_squared);
                    const float velocity_xy = distance / bg->velocity;
                    StartBeingWalkWithRandTurn(bg, BEING_DEFAULT_LEFT_TICKS * 4, distance_x / velocity_xy, distance_y / velocity_xy);
                }
                return;
            }
            BeingFlee(bg, gd);
            return;
        }
        if(distance_squared < pow2(BEING_ATTACK_DISTANCE)){
            bg->status = being_strike;
            bg->status_ticks_left = BEING_ATTACK_STEPS * 3;
        }else if(AllyNear(bg, gd)){
            UpdateBeingAttackAlly(bg, gd, 0.0F);
        }else if(IsClearSightWithKnownDistance(&bg->position, pc->segment, &gd->world, distance_x, distance_y, distance_squared)){
            StartBeingFollow(bg, BEING_DEFAULT_LEFT_TICKS, distance_squared, distance_x, distance_y);
        }else if(bg->target_last_seen_at.x != 0.0F){
            StartBeingSearch(bg, BEING_DEFAULT_LEFT_TICKS);
        }else{
            HaltBeing(bg, NAP_TICKS);
        }
        return;
    }else{
        pc = bg->target.player;
        GetBeingDistances(bg, &pc->position, &distance_x, &distance_y, &distance_squared);
    }
    MovePlayerIfTooClose(distance_x, distance_y, distance_squared, bg, gd);
    if(bg->status == being_stunned){
        UpdateBeingStunned(bg);
        return;
    }
    if(bg->status == being_strike){
        UpdateBeingStrike(bg, pc, distance_squared, distance_x, distance_y, gd);
        return;
    }
    if(bg->status == being_follow){
        UpdateBeingFollow(bg, distance_squared, distance_x, distance_y, gd);
    }
}

static inline void UpdateBeingWarlock(Being* const bg, Game_data* const gd){
    if(bg->status == being_walk){
        UpdateBeingWalk(bg, gd);
        return;
    }
    if(bg->status == being_fly){
        UpdateBeingFly(bg, gd);
        return;
    }
    if(bg->status == being_shoot_being){
        UpdateBeingShoot(bg, &gd->projectiles, &gd->world, &bg->target.being->position, bg->target.being->segment);
        return;
    }
    if(bg->status == being_search){
        UpdateBeingSearch(bg, gd);
        return;
    }
    DistancesDeclaration
    BeingFollowBeingCode
    if(bg->status == being_strike_being){
        GetBeingDistances(bg, &bg->target.being->position, &distance_x, &distance_y, &distance_squared);
        UpdateBeingStrikeAlly(bg, distance_squared, distance_x, distance_y, gd);
        return;
    }
    Player* pc;
    if(bg->status == being_idle){
        if(bg->status_ticks_left < 0){
            ++bg->status_ticks_left;
            return;
        }
        if(IdleBeingCollision(bg, gd)){
            return;
        }
        FindTargetForBeing(bg, &gd->champions);
        pc = bg->target.player;
        GetBeingDistances(bg, &pc->position, &distance_x, &distance_y, &distance_squared);
        if(distance_squared < pow2(BEING_SHOOT_DISTANCE)){
            if(distance_squared < pow2(BEING_ATTACK_DISTANCE)){
                bg->status = being_strike;
                bg->status_ticks_left = BEING_ATTACK_STEPS * 3;
                return;
            }else{
                if(AllyNear(bg, gd)){
                    UpdateBeingAttackAlly(bg, gd, BEING_SHOOT_DISTANCE);
                    return;
                }else if(IsClearSightWithKnownDistance(&bg->position, pc->segment, &gd->world, distance_x, distance_y, distance_squared)){
                    bg->target_last_seen_at = (position16){bg->target.player->position.x, bg->target.player->position.y};
                    bg->status = being_shoot;
                    bg->status_ticks_left = BEING_RELOAD;
                    return;
                }
            }
        }
        if(AllyNear(bg, gd)){
            UpdateBeingAttackAlly(bg, gd, BEING_SHOOT_DISTANCE);
        }else if(IsClearSightWithKnownDistance(&bg->position, pc->segment, &gd->world, distance_x, distance_y, distance_squared)){
            StartBeingFollow(bg, BEING_DEFAULT_LEFT_TICKS, distance_squared, distance_x, distance_y);
        }else if(bg->target_last_seen_at.x != 0.0F){
            StartBeingSearch(bg, BEING_DEFAULT_LEFT_TICKS);
        }else{
            HaltBeing(bg, NAP_TICKS);
        }
        return;
    }else{
        pc = bg->target.player;
        GetBeingDistances(bg, &pc->position, &distance_x, &distance_y, &distance_squared);
    }
    MovePlayerIfTooClose(distance_x, distance_y, distance_squared, bg, gd);
    if(bg->status == being_shoot){
        UpdateWarlockShoot(bg, &gd->projectiles, &gd->world, &bg->target.player->position, bg->target.player->segment);
        return;
    }
    if(bg->status == being_stunned){
        UpdateBeingStunned(bg);
        return;
    }
    if(bg->status == being_strike){
        UpdateBeingStrike(bg, pc, distance_squared, distance_x, distance_y, gd);
        return;
    }
    if(bg->status == being_follow){
        UpdateBeingFollow(bg, distance_squared, distance_x, distance_y, gd);
    }
}

static inline void UpdateAlly0(Being* const bg, Game_data* const gd){
    if(bg->status == being_idle){
        const bool plr_in_sight = IsClearSight(&bg->position, &human(gd)->position, human(gd)->segment, &gd->world);
        if(plr_in_sight){
            bg->target_last_seen_at = (position16){human(gd)->position.x, human(gd)->position.y};
        }
        if(FindAllyTarget(bg, gd)){
            bg->status = being_attack_being;
            bg->status_ticks_left = BEING_RELOAD_TICKS;
        }else{
            if(bg->status_ticks_left < 0){
                ++bg->status_ticks_left;
                return;
            }
            bg->target.player = human(gd);
            if(plr_in_sight){
                StartAllyFollow(bg, BEING_DEFAULT_LEFT_TICKS);
            }else{
                StartBeingSearch(bg, BEING_DEFAULT_LEFT_TICKS);
            }
        }
        return;
    }
    if(bg->status == being_attack_being){
        if(bg->status_ticks_left == 0){
            float x, y;
            GetShift(&bg->position, &bg->target.being->position, PROJECTILE_VELOCITY, &x, &y);
            AddPCProjectileToArray(&gd->projectiles, &bg->position, x, y, &bg->impact, 2U);
            bg->status = being_idle;
        }else{
            --bg->status_ticks_left;
        }
        return;
    }
    if(bg->status == being_search){
        UpdateAllySearch(bg, gd);
        return;
    }
    if(bg->status == being_follow){
        UpdateAllyFollow(bg, gd);
        return;
    }
    if(bg->status == being_walk){
        UpdateAllyWalk(bg, gd);
        return;
    }
    if(bg->status == being_stunned){
        UpdateBeingStunned(bg);
    }
}