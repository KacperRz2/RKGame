#include <common.h>
#include <function.h>
#include <sound.h>

static void UpdatePlayerMove(Game_data* const, const unsigned int);
static void UpdatePlayerDirection(Player* const);
static void UpdatePlayerPoints(Player* const);
static void UpdatePlayer(Game_data* const, const unsigned int);
static void ShiftBlade(Blade* const, Placement*);
static void SetBladePosition(Blade* const, const Placement*);
static void SetShiftToBase(Blade* const, Placement* const, const unsigned int);
static void SetBladePositionToBase(Blade* const);
static void SetShiftToPosition(Blade*, Placement* const, const Placement* const, const unsigned int);
static Placement GetBladeLocation(Player* const);
static bool BladeHitsBeing(Blade* const, Being* const, const SDL_FPoint* const);
static bool UnleashDestruction(Game_data* const, const unsigned int);
static void UpdatePlayerBlade(Game_data* const, const unsigned int);
static void UpdatePlayerFire(Game_data* const, const unsigned int);
static void UpdatePlayerPush(Game_data* const, const unsigned int);
static void UpdatePlayerCast(Game_data* const, const unsigned int);
static void UpdatePlayerHitPoints(Player* const);
static void BlockPlayerFatigue(Player* const, const int);
static void BlockPlayerArmourRegen(Player* const, const int);
static void PlayerGainArmour(Player* const, const float);
static void UpdateCPUPlayerFlags(Game_data* const, const unsigned int);
static Being* BeingNear(Segment* s, Game_data* const);
static void StunPlayer(Player* const, float);
static void RemovePlayerEffect(Player* const, const int);
static void UpdatePlayerEffects(Game_data* const, const unsigned int);
static void UpdatePlayerEffect(Game_data* const, Player* const, const int);
static void SlowPlayer(Game_data* const, Player* const, const int);
static void PlayerHPRegeneration(Game_data* const, Player* const, const int);
static void PlayerFatigueRegeneration(Game_data* const, Player* const, const int);
static void PlayerWeakness(Game_data* const, Player* const, const int);
static void PlayerDodge(Game_data* const, Player* const, const int);
static void DisruptPC(Player *const);

extern inline void StopPlayerActions(Player* const pc){
	pc->flags &= (range_mode | stunned | dodge_time);
}

static void InitScrolls(Player* const pc){
	for(unsigned int i = 0U; i < scroll_empty; ++i){
		*(pc->scrolls + i) = START_SCROLLS_NUM;
	}
	*(pc->scrolls + scroll_empty) = 1U;
	*(pc->scrolls_quick_access) = scroll_empty;
	for(unsigned int i = 1U; i < 9U; ++i){
		*(pc->scrolls_quick_access + i) = i - 1U;
	}
}

void CreatePlayer(Player* const pc, const float x, const float y){
	pc->blade.placement = (Placement)PC_BLADE_BASE_PLCMNT;
	pc->blade.impact = (Impact){};
	pc->blade.penetration = BLADE_PENETRATION;
	pc->blade.hits = 0;
	pc->blade.step_shift = (Placement){{0.0F, 0.0F}, 0.0};
	pc->blade.key = 0;
	pc->blade.steps = 0;
	pc->blade.step = 0;
	pc->blade.chain = 0U;
	pc->blade.chain_next = 0U;
	pc->blade.abide = false;
	pc->blade.loose = false;
	pc->blade.idle_ticks = 0U;
	pc->blade.charge = PC_BLADE_CHARGE_BASE;
	pc->blade_attack = (Impact)PC_BLADE_IMPACT;
	pc->range_attack = (Impact)PC_RANGE_IMPACT;
	SetPlayerPosition(pc, x, y);
	pc->flags = 0x00000000U;
	pc->direction = 0.0F;
	pc->move_direction = 0.0F;
	pc->velocity = 0.0F;
	pc->max_velocity = PLAYER_VELOCITY;
	pc->dodge_velocity_multipl = DODGE_VELOCITY_MULTIP;
	pc->hit_points = PC_HP;
	pc->fatigue_points = PC_FATIGUE;
	pc->magic_points = PC_MAGIC;
	pc->max_fatigue = PC_FATIGUE;
	pc->max_hp = PC_HP;
	pc->block_times = (Block_times){0, 0, 0, 0, 0, 0};
	pc->armour = (Armour)PC_ARMOUR;
	pc->max_armour = (Armour)PC_MAX_ARMOUR;
	pc->coins = PC_START_COINS;
	pc->selected_scroll = scroll_empty;
	InitScrolls(pc);
    pc->effects_num = 0U;
	pc->help_data.menu_position = 0U;
	pc->attention_rect = (SDL_FRect) {
		pc->position.x - half(ATTENTION_RECT_SIZE),
		pc->position.y - half(ATTENTION_RECT_SIZE),
		ATTENTION_RECT_SIZE,
		ATTENTION_RECT_SIZE
	};
}

extern inline void SetPlayerPosition(Player* const pc, const float x, const float y){
	pc->position = (SDL_FPoint){x, y};
}

extern inline void MovePlayer(Game_data* const gd, Player* const pc, const float x, const float y){
	float new_x = pc->position.x + x;
	float new_y = pc->position.y + y;
	const Segment* seg = GetSegmentUnsafe(&gd->world, new_x, new_y);
	if(seg){
		SetPlayerPosition(pc, new_x, new_y);
	}else if((seg = GetSegmentUnsafe(&gd->world, pc->position.x, new_y))){
		pc->position.y = new_y;
	}else if((seg = GetSegmentUnsafe(&gd->world, new_x, pc->position.y))){
		pc->position.x = new_x;
	}
	if(seg != pc->segment){
		UpdatePlayerNewSegment(&gd->world, pc);
	}
}

extern inline void UpdatePlayerNewSegment(World* const wld, Player* const pc){
	pc->last_seen_in = pc->segment;
	pc->segment = GetSegmentUnsafe(wld, pc->position.x, pc->position.y);
}

static inline void SetPlayerDataForwardMove(Player* const pc, const float angle){
	pc->velocity += ACCELERATION;
	pc->move_direction = pc->direction + angle;
}

static inline void SetPlayerDataBackwardMove(Player* const pc, const float angle){
	pc->velocity = (pc->velocity + ACCELERATION) * MOVING_BACK_VELO_MODI;
	pc->move_direction = pc->direction + angle;
}

static const Uint32 mfl = forward | back | right | left;

static inline void UpdatePlayerDodge(Game_data *const gd, Player* const pc){
	if((pc->flags & (dodge | forward)) == dodge){
		if(pc->fatigue_points >= PC_DODGE_FATIG && pc->block_times.dodge < 1){
			PlayGameSound(gd->snd_data_ptr, snd_dodge_last);
			pc->fatigue_points -= PC_DODGE_FATIG;
			BlockPlayerFatigue(pc, PC_DODGE_FATIG_BLOCK_TIME);
			pc->block_times.dodge = PC_DODGE_RELOAD;
			pc->velocity = pc->max_velocity * pc->dodge_velocity_multipl;
			pc->flags |= dodge_time;
			AddOrUpdatePlayerEffect(pc, (Lasting_effect){pc_effect_dodge, DODGE_EFFECT_TICKS});
		}else{
			BlockPlayerFatigue(pc, PC_FAILURE_FATIG_BLOCK_TIME);
			pc->velocity = PC_FAILURE_VELOCITY;
		}
		if(!(pc->flags & mfl)){
			pc->move_direction = pc->direction + SDL_PI_F;
		}
		pc->flags &= ~(dodge);
	}
}

static void UpdatePlayerMove(Game_data* const gd, const unsigned int indx){
	Player* const pc = gd->champions.array + indx;
	const bool runs = ((pc->flags & (run | forward | back)) == (run | forward));
	if(pc->velocity > pc->max_velocity * RUN_MULTIPL || (!runs && pc->velocity > pc->max_velocity) || ((pc->flags & block) && pc->velocity > pc->max_velocity * BLOCK_VELOCITY_MULTIP)){
		pc->velocity *= DECELERATION;
	}else if((pc->flags & mfl) == forward || (pc->flags & mfl) == (mfl & ~(back))){
		SetPlayerDataForwardMove(pc, 0.0F);
	}else if((pc->flags & mfl) == back){
		SetPlayerDataBackwardMove(pc, SDL_PI_F);
	}else if((pc->flags & mfl) == right){
		SetPlayerDataForwardMove(pc, SDL_PI_F * 0.5F);
	}else if((pc->flags & mfl) == left){
		SetPlayerDataForwardMove(pc, SDL_PI_F * 1.5F);
	}else if((pc->flags & mfl) == (forward | right)){
		SetPlayerDataForwardMove(pc, SDL_PI_F * 0.25F);
	}else if((pc->flags & mfl) == (forward | left)){
		SetPlayerDataForwardMove(pc, SDL_PI_F * 1.75F);
	}else if((pc->flags & mfl) == (back | right)){
		SetPlayerDataBackwardMove(pc, SDL_PI_F * 0.75F);
	}else if((pc->flags & mfl) == (back | left)){
		SetPlayerDataBackwardMove(pc, SDL_PI_F * 1.25F);
	}else{
		pc->velocity *= DECELERATION;
		if(pc->velocity < MIN_PC_VELOCITY) pc->velocity = 0.0F;
	}
	if(!(pc->flags & block)){
		UpdatePlayerDodge(gd, pc);
		if(runs){
			if(pc->fatigue_points <= 1){
				pc->flags &= ~(run);
			}else{
				--(pc->fatigue_points);
			}
		}
	}
	if(pc->velocity > 0.0F){
		MovePlayer(gd, pc, SDL_sinf(pc->move_direction) * pc->velocity, -SDL_cosf(pc->move_direction) * pc->velocity);
	}
}

static void UpdatePlayerDirection(Player* const pc){
	if(pc->direction > FULL_ANGLE){
		pc->direction -= FULL_ANGLE;
	}else if(pc->direction < 0.0F){
		pc->direction += FULL_ANGLE;
	}
}

static void UpdatePlayerPoints(Player* const pc){
	if(pc->block_times.fatigue < 1){
		if(pc->fatigue_points < pc->max_fatigue){
			++(pc->fatigue_points);
		}
		if(pc->flags & block){
			BlockPlayerFatigue(pc, BLOCKING_PC_FA_GAIN_INTERV);
		}else{
			BlockPlayerFatigue(pc, PC_FATIGUE_GAIN_INTERVAL);
		}
	}else{
		--(pc->block_times.fatigue);
	}
	if(pc->block_times.armour < 1){
		PlayerGainArmour(pc, PC_ARMOUR_GAIN);
		BlockPlayerArmourRegen(pc, PC_ARMOUR_GAIN_INTERVAL);
	}else{
		--(pc->block_times.armour);
	}
	if(pc->block_times.dodge > 0){
		--pc->block_times.dodge;
	}
}

static inline void UpdatePlayer(Game_data* const gd, const unsigned int indx){
	UpdatePlayerDirection(gd->champions.array + indx);
	UpdatePlayerMove(gd, indx);
	UpdatePlayerCast(gd, indx);
	UpdatePlayerPush(gd, indx);
	UpdatePlayerFire(gd, indx);
	UpdatePlayerBlade(gd, indx);
	UpdatePlayerPoints(gd->champions.array + indx);
}

static inline void ShiftBlade(Blade* const bl, Placement* const shift){
	bl->placement.position.x += shift->position.x;
	bl->placement.position.y += shift->position.y;
	bl->placement.direction += shift->direction;
}

static inline void SetBladePosition(Blade* const bl, const Placement* const position){
	bl->placement = *position;
}

static inline void SetShiftToBase(Blade* const bl, Placement* const step_shift, const unsigned int steps){
	*step_shift = (Placement){
		{(BLADE_BASE_X - bl->placement.position.x) / steps, (BLADE_BASE_Y - bl->placement.position.y) / steps},
		(BLADE_BASE_DIRECTION_PC - bl->placement.direction) / steps
	};
}

static inline void SetBladePositionToBase(Blade* const bl){
	bl->placement = (Placement)PC_BLADE_BASE_PLCMNT;
}

static inline void SetShiftToPosition(Blade* const bl, Placement* const step_shift, const Placement* const frame, const unsigned int steps){
	*step_shift = (Placement){
		{(frame->position.x - bl->placement.position.x) / steps, (frame->position.y - bl->placement.position.y) / steps},
		(frame->direction - bl->placement.direction) / steps
	};
}

static inline Placement GetBladeLocation(Player* const pc){
	const float sine = SineUnsafe(pc->direction);
	const float cosine = CosiUnsafe(pc->direction);
	return (Placement){
		{
			pc->position.x + cosine * pc->blade.placement.position.x + sine * pc->blade.placement.position.y,
			pc->position.y + sine * pc->blade.placement.position.x - cosine * pc->blade.placement.position.y
		},
		pc->direction + pc->blade.placement.direction
	};
}

static inline bool BladeHitsBeing(Blade* const bl, Being* const bg, const SDL_FPoint* const dangerous_points){
	for(unsigned int i = 0U; i < PC_BLADE_CHECKPOINTS; ++i){
		if(SDL_fabsf((dangerous_points + i)->x - bg->position.x) < half(BeingSize(bg)) && SDL_fabsf((dangerous_points + i)->y - bg->position.y) < half(BeingSize(bg))){
			for(unsigned int j = bl->hits; j > 0U; --j){
				if(*(bl->hit_targets + (j - 1U)) == bg->main_indx){
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

static inline void SetBladeInactive(Blade* const bl){
	if(bl->step <= bl->steps){
		SetBladePositionToBase(bl);
		bl->abide = false;
		bl->loose = false;
		bl->charge = PC_BLADE_CHARGE_BASE;
		bl->idle_ticks = PC_BLADE_MAX_IDLE_TICKS;
		bl->step = bl->steps + 1;
		bl->chain_next = 0U;
	}
}

static inline void SetBladeStrikeStart(Blade* const bl, Impact* const blade_attack, const Placement* const destin, const unsigned int chain_len){
	static const float extra_penetration[] = PC_BLADE_PENETRATIONS;
	bl->chain = bl->chain_next;
	bl->key = 0;
	bl->step = 0;
	bl->abide = true;
	bl->idle_ticks = 0U;
	bl->hits = 0U;
	const float multip = 1.0F - bl->charge;
	bl->impact = (Impact){
		blade_attack->damage * multip,
		blade_attack->armour_reduction * multip + *(extra_penetration + bl->chain),
		blade_attack->magic * multip,
		blade_attack->stun * multip
	};
	bl->penetration = (unsigned int)((float)BLADE_PENETRATION * multip);
	bl->charge = PC_BLADE_CHARGE_BASE;
	SetShiftToPosition(bl, &bl->step_shift, destin, bl->steps = PC_BLADE_FIRST_MOVE_STEPS);
	bl->chain_next = (bl->chain_next + 1U) % chain_len;
	bl->loose = false;
}

static inline void SetBladeBackToBegin(Blade* const bl){
	bl->chain_next = 0U;
	SetShiftToBase(bl, &bl->step_shift, bl->steps = PC_BLADE_RETURN_STEPS);
	bl->step = 0;
	bl->loose = true;
}

static inline void SetBladePhaseEnd(Blade* const bl, const unsigned int strike_phases_num, const Placement* const* const blade_moves){
	if(bl->loose){
		bl->loose = false;
	}else if(bl->key > strike_phases_num - 2){
		bl->abide = false;
		bl->loose = true;
		SetShiftToPosition(bl, &bl->step_shift, *(blade_moves + bl->chain_next), bl->steps = PC_BLADE_TO_NEXT_STEPS);
	}else{
		SetShiftToPosition(bl, &bl->step_shift, *(blade_moves + bl->chain) + ++bl->key, bl->steps = PC_BLADE_STRIKE_STEPS);
	}
	bl->step = 0;
}

static inline void BounceBlade(Blade* const bl){
	bl->placement.direction = bl->chain == 1 ? bl->placement.direction - PC_BLADE_BOUNCE_ANGLE : bl->placement.direction + PC_BLADE_BOUNCE_ANGLE;
}

static inline void SetBladeBlocked(Blade* const bl, const Placement* const next_in_chain){
	bl->abide = false;
	bl->hits = 0U;
	BounceBlade(bl);
	SetShiftToPosition(bl, &bl->step_shift, next_in_chain, bl->steps = PC_BLADE_AFTER_BLOCK_STEPS);
	bl->step = 0;
}

static inline void TryPushBeingWithBlade(const Player* const pc, Being* const bg){
	const float stun_power = CalculateStunPower(&pc->blade.impact, &bg->armour);
	if(stun_power >= 1.0F){
		if(bg->status == being_stunned){
			const float angle = GetDirectionToPush(&pc->position, &bg->position);
			const float vel = BASE_FLY_VELOCITY * stun_power;
			CatapultBeing(bg, SineSafe(angle) * vel, -CosiSafe(angle) * vel, BASE_FLY_TICKS * stun_power);
		}else{
			StunBeing(bg, (int)(BEING_DEFAULT_LEFT_TICKS * stun_power));
		}
	}
}

static bool UnleashDestruction(Game_data* const gd, const unsigned int indx){
	Player* const pc = gd->champions.array + indx;
	const Placement blade_true_location = GetBladeLocation(pc);
	const Segment* seg = GetSegmentUnsafe(&gd->world, blade_true_location.position.x, blade_true_location.position.y);
	if(seg == NULL){
		return true;
	}
	const float length_part = BLADE_LEN / (float)PC_BLADE_CHECKPOINTS;
	const float shift_x = SineSafe(blade_true_location.direction) * length_part;
	const float shift_y = -CosiSafe(blade_true_location.direction) * length_part;
	const SDL_FPoint dangerous_points[PC_BLADE_CHECKPOINTS] = {
		{blade_true_location.position.x + shift_x, blade_true_location.position.y + shift_y},
		{blade_true_location.position.x + 2.0F * shift_x, blade_true_location.position.y + 2.0F * shift_y}
	};
	Blade* const bl = &pc->blade;
	Segment* neighbour_segs[4];
	Get4NearestSegments(neighbour_segs, &gd->world, seg, blade_true_location.position.x, blade_true_location.position.y);
	for(unsigned int k = 0U; k < 4; ++k){
		Segment* neighbour = *(neighbour_segs + k);
		if(neighbour == NULL) continue;
		for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
			Being* bg = (gd->beings.array + *(neighbour->beings.beings_ind + i));
			if(!BladeHitsBeing(bl, bg, dangerous_points)) continue;
			AddDamageVisualEffect(gd, dangerous_points);
			if(DamageBeing(bg, &bl->impact, gd->beings.array)){
				if(bl->hits < bl->penetration--){
					--i;
					continue;
				}
			}else{
				if(bg->status != being_fly){
					TryPushBeingWithBlade(pc, bg);
				}
				if(bl->hits < bl->penetration){
					*(bl->hit_targets + bl->hits++) = bg->main_indx;
					continue;
				}
			}
			return true;
		}
	}
	return false;
}

static void UpdatePlayerBlade(Game_data* const gd, const unsigned int indx){
	static const Placement blade_key_frames_0[] = PC_BLADE_FRAMES0;
	static const Placement blade_key_frames_1[] = PC_BLADE_FRAMES1;
	static const Placement blade_key_frames_2[] = PC_BLADE_FRAMES2;
	static const Placement* const blade_moves[] = {blade_key_frames_0, blade_key_frames_1, blade_key_frames_2};
	static const unsigned int sizes[] = {SDL_arraysize(blade_key_frames_0), SDL_arraysize(blade_key_frames_1), SDL_arraysize(blade_key_frames_2)};
	Player* const pc = gd->champions.array + indx;
	Blade* const bl = &pc->blade;
	if(pc->flags & range_mode){
		SetBladeInactive(bl);
		return;
	}
	if(pc->flags & block){
		bl->abide = false;
		bl->charge = PC_BLADE_CHARGE_BASE;
		bl->idle_ticks = PC_BLADE_MAX_IDLE_TICKS;
	}else if(pc->flags & attack){
		bl->charge *= PC_BLADE_CHARGE_MODIFIER;
		BlockPlayerFatigue(pc, 1);
	} 
	if(!bl->abide){
		if(bl->charge != PC_BLADE_CHARGE_BASE){
			if(!(pc->flags & attack)){
				SetBladeStrikeStart(bl, &pc->blade_attack, *(blade_moves + bl->chain_next), SDL_arraysize(sizes));
				return;
			}
		}else if(++bl->idle_ticks > PC_BLADE_MAX_IDLE_TICKS){
			SetBladeBackToBegin(bl);
		}
		if(!bl->loose) return;
	}
	if(bl->step == bl->steps){
		SetBladePhaseEnd(bl, *(sizes + bl->chain), blade_moves);
		if(PC_BLD_LAST_HARMLESS_PHASE == bl->key){
			PlaySoundRand(gd->snd_data_ptr, snd_strike0, snd_strike_last);
		}
	}else{
		ShiftBlade(bl, &bl->step_shift);
		if(!bl->loose && bl->key > PC_BLD_LAST_HARMLESS_PHASE && (bl->loose = UnleashDestruction(gd, indx))){
			SetBladeBlocked(bl, *(blade_moves + bl->chain_next));
			pc->flags |= stop_blade;
		}
	}
	++bl->step;
}

extern inline bool PointInPlayer(const float x, const float y, Player* const pl){
	if(pow2(x - pl->position.x) + pow2(y - pl->position.y) < (float)pow2(half(PLAYER_SIZE))){
		return true;
	}
	return false;
}

extern inline bool CircleMeetsPlayer(const float x, const float y, const float diameter, Player* const pl){
	if(pow2(x - pl->position.x) + pow2(y - pl->position.y) < pow2(half((float)PLAYER_SIZE + diameter))){
		return true;
	}
	return false;
}

extern inline void DamagePlayer(Player* const pc, const Impact* impact){
	pc->hit_points -= CalculateDamage(impact, &pc->armour);
	if(impact->damage > 0.0F){
		if(pc->armour.absorption > 0.0F){
			pc->armour.absorption -= impact->damage;
			if(pc->armour.absorption < 0.0F){
				pc->armour.absorption = 0.0F;
			}
		}
		if(pc->armour.multipl < 1.0F){
			pc->armour.multipl += (1.0F - pc->max_armour.multipl) * (impact->damage / pc->max_armour.absorption);
			if(pc->armour.multipl > 1.0F){
				pc->armour.multipl = 1.0F;
			}
		}
	}
	if(impact->magic > 0.0F){
		if(pc->armour.magic_multipl < 1.0F){
			pc->armour.magic_multipl += (1.0F - pc->max_armour.magic_multipl) * 0.01F;
			if(pc->armour.magic_multipl > 1.0F){
				pc->armour.magic_multipl = 1.0F;
			}
		}
	}
	BlockPlayerArmourRegen(pc, PC_ARMOUR_REGEN_BLOCK);
	StunPlayer(pc, CalculateStunPower(impact, &pc->armour));
}

static void UpdatePlayerFire(Game_data* const gd, const unsigned int indx){
	Player* const pc = gd->champions.array + indx;
	if(pc->block_times.shoot > 0){
		--pc->block_times.shoot;
	}else if(pc->selected_scroll == scroll_empty && (pc->flags & (range_mode | attack | block)) == (range_mode | attack) && gd->projectiles.num < MAX_PROJECTILES_NUM){
		float x, y;
		GetShiftFromAngle(pc->direction + PC_SHOOT_MAX_SPREAD * (SDL_randf() - 0.5F), PROJECTILE_VELOCITY, &x, &y);
		AddPCProjectileToArray(&gd->projectiles, &pc->position, x, y, &pc->range_attack, TEST_PENETRATION);
		pc->block_times.shoot = PC_SHOOT_RELOAD;
		BlockPlayerFatigue(pc, PC_CAST_FATIG_BLOCK_TIME);
    	AddSpellVisualEffect(gd, &pc->position, SPELL0_RGB);
	}
}

static void UpdatePlayerPush(Game_data* const gd, const unsigned int indx){
	Player* const pc = gd->champions.array + indx;
	if(pc->block_times.push > 0){
		--pc->block_times.push;
	}else if((pc->flags & (attack | block)) == (attack | block) && pc->fatigue_points >= PC_PUSH_FATIG){
		PlayGameSound(gd->snd_data_ptr, snd_push_last);
		pc->fatigue_points -= PC_PUSH_FATIG;
		BlockPlayerFatigue(pc, PC_PUSH_FATIG_BLOCK_TIME);
		pc->block_times.push = PC_PUSH_RELOAD;
		Segment* seg = GetSegmentUnsafe(&gd->world, pc->position.x, pc->position.y);
		Segment* neighbour_segs[4];
		Get4NearestSegments(neighbour_segs, &gd->world, seg, pc->position.x, pc->position.y);
		for(unsigned int k = 0U; k < 4U; ++k){
			Segment* neighbour = *(neighbour_segs + k);
			if(neighbour == NULL) continue;
			for(unsigned int i = 0U; i < neighbour->beings.num; ++i){
				Being* bg = (gd->beings.array + *(neighbour->beings.beings_ind + i));
				if(bg->status != being_fly && pow2(pc->position.x - bg->position.x) + pow2(pc->position.y - bg->position.y) < pow2(PC_PUSH_REACH)){
					float angle = GetDirectionToPush(&pc->position, &bg->position);
					if(angle < 0.0F){
						angle += FULL_ANGLE;
					}
					const float difference = SDL_fabsf(pc->direction - angle);
					if(difference <= SDL_PI_F * 0.5F || difference >= SDL_PI_F * 1.5F){
						PlaySoundRand(gd->snd_data_ptr, snd_hit_b0, snd_hit_b_last);
						const float power = PC_PUSH_POWER * bg->armour.unstability;
						if(bg->status == being_stunned){
                			const float vel = BASE_FLY_VELOCITY * power;
							CatapultBeing(bg, SineUnsafe(angle) * vel, -CosiUnsafe(angle) * vel, BASE_FLY_TICKS * power);
						}else{
							StunBeing(bg, (int)(BEING_DEFAULT_LEFT_TICKS * power));
						}
					}
				}
			}
		}
	}
}

extern inline void HaltPlayer(Player* const p){
	p->velocity = 0.0F;
}

extern inline void HitBarrier(Game_data *const gd, Player* const pc, const Impact* const impact){
	PlaySoundRand(gd->snd_data_ptr, snd_block0, snd_block_last);
	pc->fatigue_points -= (int)(impact->stun * BLOCK_COST);
	BlockPlayerFatigue(pc, PC_BLOCK_FATIG_BLOCK_TIME);
	if(pc->fatigue_points < 0){
		pc->flags &= ~(block);
		pc->fatigue_points = 0;
	}
}

extern inline float GetDirectionToPush(const SDL_FPoint* const pushing, const SDL_FPoint* const pushed){
    return arctan2(pushed->y - pushing->y, pushed->x - pushing->x) + SDL_PI_F * 0.5F;
}

extern inline void PlayerCastConsequences(Game_data *const gd, Player *const pc, const int cost, const Uint8 scroll){
	pc->magic_points -= cost;
	--(*(pc->scrolls + scroll));
	pc->block_times.cast = PC_CAST_RELOAD;
	BlockPlayerFatigue(pc, PC_CAST_FATIG_BLOCK_TIME);
}

static void UpdatePlayerCast(Game_data* const gd, const unsigned int indx){
	Player* const pc = gd->champions.array + indx;
	if(pc->block_times.cast > 0){
		--pc->block_times.cast;
	}else if(pc->flags & cast){
		pc->flags &= ~(cast);
		const int cost = ScrollCost(pc->selected_scroll);
		if(*(pc->scrolls + pc->selected_scroll) > 0U && pc->magic_points >= cost && UseScroll(gd, &pc->aim_position, indx)){
			PlayerCastConsequences(gd, pc, cost, pc->selected_scroll);
			if(pc != host(gd)){
				AddAnnouncement(gd, annncmnt_cast, &(Uint64){pc->selected_scroll});
			}
		}
	}
}

extern inline void HealPlayer(Player* const pc, const int points){
	pc->hit_points += points;
	UpdatePlayerHitPoints(pc);
}

static inline void UpdatePlayerHitPoints(Player* const pc){
	if(pc->hit_points > pc->max_hp){
		pc->hit_points = pc->max_hp;
	}
}

static inline void BlockPlayerFatigue(Player* const pc, const int time){
	if(pc->block_times.fatigue < time){
		pc->block_times.fatigue = time;
	}
}

static inline void BlockPlayerArmourRegen(Player* const p, const int time){
	if(p->block_times.armour < time){
		p->block_times.armour = time;
	}
}

static inline void PlayerGainArmour(Player* const pc, const float perc){
	if(pc->armour.absorption < pc->max_armour.absorption){
		pc->armour.absorption += pc->max_armour.absorption * perc;
		if(pc->armour.absorption > pc->max_armour.absorption){
			pc->armour.absorption = pc->max_armour.absorption;
		}
	}
	if(pc->armour.multipl > pc->max_armour.multipl){
		pc->armour.multipl -= (1.0F - pc->max_armour.multipl) * perc;
		if(pc->armour.multipl < pc->max_armour.multipl){
			pc->armour.multipl = pc->max_armour.multipl;
		}
	}
	if(pc->armour.magic_multipl > pc->max_armour.magic_multipl){
		pc->armour.magic_multipl -= (1.0F - pc->max_armour.magic_multipl) * perc;
		if(pc->armour.magic_multipl < pc->max_armour.magic_multipl){
			pc->armour.magic_multipl = pc->max_armour.magic_multipl;
		}
	}
}

void UpdatePlayers(Game_data* const gd){
	host(gd)->aim_position = GetMouseWorldPosition(gd);
	for(unsigned int i = 0U; i < gd->champions.num; ++i){
		if(i != gd->host_indx){
			UpdateCPUPlayerFlags(gd, i);
		}
		UpdatePlayer(gd, i);
    }
}

void UpdatePlayersEffects(Game_data* const gd){
	for(unsigned int i = 0U; i < gd->champions.num; ++i){
		UpdatePlayerEffects(gd, i);
    }
}

static void UpdateCPUPlayerFlags(Game_data* const gd, const unsigned int indx){
	Player* const pc = gd->champions.array + indx;
	unsigned int next_i = (indx + 1U) % gd->champions.num;
	if(next_i == gd->host_indx && gd->champions.num > 2U){
		next_i = (next_i + 1U) % gd->champions.num;
	}
	if(pow2((gd->champions.array + next_i)->position.x - pc->position.x) + pow2((gd->champions.array + next_i)->position.y - pc->position.y) < pow2(PLAYER_SIZE * 2.0F)){
		if(indx % 2U == 0U){
			pc->flags |= left;
			pc->flags &= ~(right);
		}else{
			pc->flags |= right;
			pc->flags &= ~(left);
		}
	}else{
		pc->flags &= ~(right | left);
	}
	Being* target;
	float d_x;
	float d_y;
	float d_squared;
	if((target = BeingNear(pc->segment, gd))){
		d_x = target->position.x - pc->position.x;
		d_y = target->position.y - pc->position.y;
		d_squared = pow2(d_x) + pow2(d_y);
		if(IsClearSightWithKnownDistance(&pc->position, target->segment, &gd->world, d_x, d_y, d_squared)){
			pc->direction = arctan2(-d_y, -d_x) - SDL_PI_F * 0.5F;
			if(d_squared <= pow2(BEING_ATTACK_DISTANCE)){
				pc->flags &= ~(back);
				if(d_squared <= pow2(BEING_HALT_DISTANCE)){
					pc->flags &= ~(forward | run);
				}else{
					pc->flags |= forward;
				}
				if((pc->flags & (attack | range_mode)) != attack){
					pc->flags &= ~(range_mode);
					pc->flags |= attack;
				}else if((pc->flags & (attack | range_mode)) == attack && SDL_randf() < 0x1.0p-5F){
					pc->flags &= ~(attack);
				}
				return;
			}
			d_x = pc->position.x - host(gd)->position.x;
			d_y = pc->position.y - host(gd)->position.y;
			d_squared = pow2(d_x) + pow2(d_y);
			if(d_squared > pow2(SEGMENT_SIZE)){
				const float direction_to_main_player = arctan2(d_y, d_x) - SDL_PI_F * 0.5F;
				float direc_diff = pc->direction - direction_to_main_player;
				if(direc_diff < 0.0F){
					direc_diff += FULL_ANGLE;
				}
				if(direc_diff < SDL_PI_F * 1.5F && direc_diff > SDL_PI_F * 0.5F){
					pc->flags |= back;
					if(direc_diff < SDL_PI_F * 0.75F){
						pc->flags |= left;
					}else if(direc_diff > SDL_PI_F * 1.25F){
						pc->flags |= right;
					}
				}else{
					pc->flags &= ~(back);
				}
			}else{
				pc->flags &= ~(back);
			}
			pc->flags &= ~(forward | run);
			pc->flags |= range_mode | attack;
			return;
		}
	}
	pc->flags &= ~(attack | back);
	d_x = host(gd)->position.x - pc->position.x;
	d_y = host(gd)->position.y - pc->position.y;
	d_squared = pow2(d_x) + pow2(d_y);
	if(!IsClearSightWithKnownDistance(&pc->position, host(gd)->segment, &gd->world, d_x, d_y, d_squared)){
		d_x = SegmentCenterX(host(gd)->last_seen_in) - pc->position.x;
		d_y = SegmentCenterY(host(gd)->last_seen_in) - pc->position.y;
	}
	pc->direction = arctan2(-d_y, -d_x) - SDL_PI_F * 0.5F;
	if(d_squared < pow2(SEGMENT_SIZE)){
		pc->flags &= ~(forward | run);
		return;
	}
	pc->flags |= forward;
	if(d_squared > pow2(SEGMENT_SIZE * 2.0F)){
		pc->flags |= run;
	}else{
		pc->flags &= ~(run);
	}
}

static inline Being* BeingNear(Segment* seg, Game_data* const gd){
	if(seg->beings.num > 0U){
		return gd->beings.array + *seg->beings.beings_ind;
	}
	int c = seg->indx.x;
	int r = seg->indx.y;
	unsigned int i = 0U;
	int sign = 1;
	do{
		unsigned int j = 0U;
		unsigned int k = 0U;
		++i;
		while(j < i){
			c += sign;
			seg = GetSegmentByIndxSafe(&gd->world, c, r);
			if(seg && seg->beings.num > 0U){
				return gd->beings.array + *seg->beings.beings_ind;
			}
			++j;
		}
		while(k < i){
			r += sign;
			seg = GetSegmentByIndxSafe(&gd->world, c, r);
			if(seg && seg->beings.num > 0U){
				return gd->beings.array + *seg->beings.beings_ind;
			}
			++k;
		}
		sign *= -1;
	}while(i < 13);
	return NULL;
}

extern inline void SetQuickScroll(Player* const pc, int num){
	for(unsigned int i = 0U; i < 9U; ++i){
		if(*(pc->scrolls_quick_access + i) == pc->help_data.menu_position){
			if(i != num){
				*(pc->scrolls_quick_access + i) = *(pc->scrolls_quick_access + num);
				break;
			}else{
				return;
			}
		}
	}
	*(pc->scrolls_quick_access + num) = pc->help_data.menu_position;
}

extern inline void TeleportPlayer(Game_data *const gd, Player *const pc, const SDL_FPoint *const position){
	PlayGameSound(gd->snd_data_ptr, snd_teleport_pc_last);
	SetPlayerPosition(pc, position->x, position->y);
	UpdatePlayerNewSegment(&gd->world, pc);
}

static inline void DisruptPC(Player *const pc){
	pc->flags = (pc->flags | disrupted) & ~(dodge | run | block | attack | cast | action);
	pc->blade.charge = PC_BLADE_CHARGE_BASE;
	pc->blade.idle_ticks = PC_BLADE_MAX_IDLE_TICKS;
}

static inline void StunPlayer(Player* const pc, float power){
	power *= 16.0F - (pc->fatigue_points / (float)pc->max_fatigue * 15.0F);
	if(power > 0.5F){
		const int effect_indx = PlayerHasEffect(pc, pc_effect_slow);
		if(effect_indx == NOT_FOUND){
			AddPlayerEffect(pc, (Lasting_effect){pc_effect_slow, PC_DODGE_RELOAD});
			pc->flags |= stunned;
		}else{
			(pc->effects + effect_indx)->ticks_left = PC_DODGE_RELOAD;
		}
		pc->max_velocity /= 0.5F + power;
		pc->block_times.dodge = PC_DODGE_RELOAD;
		if(power > 1.0F){
			DisruptPC(pc);
		}
	}
}

extern inline void AddPlayerEffect(Player* const pc, const Lasting_effect effect){
	if(pc->effects_num < MAX_PC_EFFECTS){
		AddLastingEffect(pc->effects, effect, pc->effects_num++);
	}
}

extern inline void AddOrUpdatePlayerEffect(Player* const pc, const Lasting_effect effect){
	const int effect_indx = PlayerHasEffect(pc, effect.id);
	if(effect_indx == NOT_FOUND){
		AddPlayerEffect(pc, effect);
	}else{
		(pc->effects + effect_indx)->ticks_left = effect.ticks_left;
	}
}

static inline void RemovePlayerEffect(Player* const pc, const int indx){
	RemoveLastingEffect(pc->effects, indx, pc->effects_num--);
}

static inline void UpdatePlayerEffects(Game_data* const gd, const unsigned int indx){
	Player* const pc = gd->champions.array + indx;
	for(unsigned int i = 0U; i < pc->effects_num; ++i){
		UpdatePlayerEffect(gd, pc, i);
	}
}

static inline void UpdatePlayerEffect(Game_data* const gd, Player* const pc, const int effect_indx){
    void (*const effect[])(Game_data* const, Player* const, const int) = PLAYER_LASTING_EFFECTS;
    (*(effect + (pc->effects + effect_indx)->id))(gd, pc, (pc->effects + effect_indx)->ticks_left--);
	if((pc->effects + effect_indx)->ticks_left < 1){
		RemovePlayerEffect(pc, effect_indx);
	}
}

extern inline int PlayerHasEffect(Player* const pc, const int effect_id){
    for(unsigned int i = 0U; i < pc->effects_num; ++i){
        if((pc->effects + i)->id == effect_id){
            return i;
        }
    }
    return NOT_FOUND;
}

static void SlowPlayer(Game_data* const gd, Player* const pc, const int ticks_left){
    if(ticks_left < 2){
        pc->max_velocity = PLAYER_VELOCITY;
		pc->flags &= ~(stunned);
    }
}

static void PlayerHPRegeneration(Game_data* const gd, Player* const pc, const int ticks_left){
	if(!(ticks_left % (HP_REGEN_TICKS / HP_REGEN_TIMES))){
		HealPlayer(pc, pc->max_hp / HP_REGEN_TIMES);
	}
	if(!(ticks_left % DEFAULT_VIS_EFF_INTERV)){
        AddBonusVisualEffect(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
            pc->position.x + (SDL_randf() - 0.5F) * half(PLAYER_SIZE),
            pc->position.y + (SDL_randf() - 0.5F) * half(PLAYER_SIZE)
        });
    }
}

static void PlayerFatigueRegeneration(Game_data* const gd, Player* const pc, const int ticks_left){
	if(pc->fatigue_points < pc->max_fatigue){
		++pc->fatigue_points;
	}
	if(ticks_left % DEFAULT_VIS_EFF_INTERV == 0){
        AddBonusVisualEffect(&gd->rend_data_ptr->visual_effects, &(SDL_FPoint){
            pc->position.x + (SDL_randf() - 0.5F) * half(PLAYER_SIZE),
            pc->position.y + (SDL_randf() - 0.5F) * half(PLAYER_SIZE)
        });
    }
}

static void PlayerWeakness(Game_data* const gd, Player* const pc, const int ticks_left){
	if(ticks_left < 2){
		if(pc->blade_attack.damage < PC_BLADE_DMG){
        	pc->blade_attack.damage = PC_BLADE_DMG;
		}
		if(pc->blade_attack.magic < PC_BLADE_MAGIC){
        	pc->blade_attack.magic = PC_BLADE_MAGIC;
		}
		if(pc->blade_attack.armour_reduction < PC_BLADE_PENETR){
        	pc->blade_attack.armour_reduction = PC_BLADE_PENETR;
		}
		if(pc->range_attack.damage < PC_RANGE_DMG){
        	pc->range_attack.damage = PC_RANGE_DMG;
		}
		if(pc->range_attack.magic < PC_RANGE_MAGIC){
        	pc->range_attack.magic = PC_RANGE_MAGIC;
		}
		if(pc->range_attack.armour_reduction < PC_RANGE_PENETR){
        	pc->range_attack.armour_reduction = PC_RANGE_PENETR;
		}
    }else if(ticks_left % DEFAULT_VIS_EFF_INTERV == 0){
        AddCurseVisualEffect(&gd->rend_data_ptr->visual_effects, &pc->position);
    }
}

static void PlayerDodge(Game_data* const gd, Player* const pc, const int ticks_left){
	if(ticks_left < 2){
		pc->flags &= ~(dodge_time);
    }
}

void MultiplayerUpdatePlayers(Game_data* const gd){
	host(gd)->aim_position = GetMouseWorldPosition(gd);
	if((gd->champions.array + 1)->flags & strike){
		(gd->champions.array + 1)->blade.charge *= PC_BLADE_CHARGE_MODIFIER;
		(gd->champions.array + 1)->flags &= ~(strike);
	}
	for(unsigned int i = 0U; i < gd->champions.num; ++i){
		UpdatePlayer(gd, i);
	}
}

static void ClientUpdatePlayerBlade(Game_data* const gd, const unsigned int indx){
	static const Placement blade_key_frames_0[] = PC_BLADE_FRAMES0;
	static const Placement blade_key_frames_1[] = PC_BLADE_FRAMES1;
	static const Placement blade_key_frames_2[] = PC_BLADE_FRAMES2;
	static const Placement* const blade_moves[] = {blade_key_frames_0, blade_key_frames_1, blade_key_frames_2};
	static const unsigned int sizes[] = {SDL_arraysize(blade_key_frames_0), SDL_arraysize(blade_key_frames_1), SDL_arraysize(blade_key_frames_2)};
	Player* const pc = gd->champions.array + indx;
	Blade* const bl = &pc->blade;
	if(pc->flags & range_mode){
		SetBladeInactive(bl);
		return;
	}
	if(pc->flags & block){
		bl->abide = false;
		bl->charge = PC_BLADE_CHARGE_BASE;
		bl->idle_ticks = PC_BLADE_MAX_IDLE_TICKS;
	}else if(pc->flags & attack){
		bl->charge *= PC_BLADE_CHARGE_MODIFIER;
		BlockPlayerFatigue(pc, 1);
	}
	if(!bl->abide){
		if(bl->charge != PC_BLADE_CHARGE_BASE){
			if(!(pc->flags & attack)){
				pc->flags |= strike;
				SetBladeStrikeStart(bl, &pc->blade_attack, *(blade_moves + bl->chain_next), SDL_arraysize(sizes));
				return;
			}
		}else if(++bl->idle_ticks > PC_BLADE_MAX_IDLE_TICKS){
			SetBladeBackToBegin(bl);
		}
		if(!bl->loose) return;
	}
	if(bl->step == bl->steps){
		SetBladePhaseEnd(bl, *(sizes + bl->chain), blade_moves);
	}else{
		ShiftBlade(bl, &bl->step_shift);
		if(pc->flags & stop_blade){
			bl->loose = true;
			SetBladeBlocked(bl, *(blade_moves + bl->chain_next));
			pc->flags &= ~(stop_blade);
		}
	}
	++bl->step;
}

static inline void ClientUpdatePlayer(Game_data* const gd, const unsigned int indx){
	Player* const pc = gd->champions.array + indx;
	UpdatePlayerDirection(pc);
	const bool runs = ((pc->flags & (run | forward | back)) == (run | forward));
	if(pc->velocity > pc->max_velocity * RUN_MULTIPL || (!runs && pc->velocity > pc->max_velocity) || ((pc->flags & block) && pc->velocity > pc->max_velocity * BLOCK_VELOCITY_MULTIP)){
		pc->velocity *= DECELERATION;
	}else if((pc->flags & mfl) == forward || (pc->flags & mfl) == (mfl & ~(back))){
		SetPlayerDataForwardMove(pc, 0.0F);
	}else if((pc->flags & mfl) == back){
		SetPlayerDataBackwardMove(pc, SDL_PI_F);
	}else if((pc->flags & mfl) == right){
		SetPlayerDataForwardMove(pc, SDL_PI_F * 0.5F);
	}else if((pc->flags & mfl) == left){
		SetPlayerDataForwardMove(pc, SDL_PI_F * 1.5F);
	}else if((pc->flags & mfl) == (forward | right)){
		SetPlayerDataForwardMove(pc, SDL_PI_F * 0.25F);
	}else if((pc->flags & mfl) == (forward | left)){
		SetPlayerDataForwardMove(pc, SDL_PI_F * 1.75F);
	}else if((pc->flags & mfl) == (back | right)){
		SetPlayerDataBackwardMove(pc, SDL_PI_F * 0.75F);
	}else if((pc->flags & mfl) == (back | left)){
		SetPlayerDataBackwardMove(pc, SDL_PI_F * 1.25F);
	}else{
		pc->velocity *= DECELERATION;
		if(pc->velocity < MIN_PC_VELOCITY) pc->velocity = 0.0F;
	}
	if(!(pc->flags & block)){
		if((pc->flags & (dodge | forward)) == dodge){
			if(pc->fatigue_points >= PC_DODGE_FATIG && pc->block_times.dodge < 1){
				pc->fatigue_points -= PC_DODGE_FATIG;
				BlockPlayerFatigue(pc, PC_DODGE_FATIG_BLOCK_TIME);
				pc->block_times.dodge = PC_DODGE_RELOAD;
				pc->velocity = pc->max_velocity * pc->dodge_velocity_multipl;
			}else{
				BlockPlayerFatigue(pc, PC_FAILURE_FATIG_BLOCK_TIME);
				pc->velocity = PC_FAILURE_VELOCITY;
			}
			if(!(pc->flags & mfl)){
				pc->move_direction = pc->direction + SDL_PI_F;
			}
			pc->flags &= ~(dodge);
			gd->flags |= gamef_client_dodge;
		}
		if(runs){
			if(pc->fatigue_points <= 1){
				pc->flags &= ~(run);
			}else{
				--(pc->fatigue_points);
			}
		}
	}
	if(pc->velocity > 0.0F){
		MovePlayer(gd, pc, SDL_sinf(pc->move_direction) * pc->velocity, -SDL_cosf(pc->move_direction) * pc->velocity);
	}
	if(pc->block_times.cast > 0){
		--pc->block_times.cast;
	}else if(pc->flags & cast){
		pc->flags &= ~(cast);
		gd->flags |= gamef_client_cast;
	}
	if(pc->block_times.push > 0){
		--pc->block_times.push;
	}else if((pc->flags & (attack | block)) == (attack | block) && pc->fatigue_points >= PC_PUSH_FATIG){
		pc->fatigue_points -= PC_PUSH_FATIG;
		BlockPlayerFatigue(pc, PC_PUSH_FATIG_BLOCK_TIME);
		pc->block_times.push = PC_PUSH_RELOAD;
	}
	if(pc->block_times.shoot > 0){
		--pc->block_times.shoot;
	}else if(pc->selected_scroll == scroll_empty && (pc->flags & (range_mode | attack | block)) == (range_mode | attack) && gd->projectiles.num < MAX_PROJECTILES_NUM){
		pc->block_times.shoot = PC_SHOOT_RELOAD;
		BlockPlayerFatigue(pc, PC_CAST_FATIG_BLOCK_TIME);
	}
	ClientUpdatePlayerBlade(gd, indx);
	if(pc->block_times.fatigue < 1){
		if(pc->fatigue_points < pc->max_fatigue){
			++(pc->fatigue_points);
		}
		if(pc->flags & block){
			BlockPlayerFatigue(pc, BLOCKING_PC_FA_GAIN_INTERV);
		}else{
			BlockPlayerFatigue(pc, PC_FATIGUE_GAIN_INTERVAL);
		}
	}else{
		--(pc->block_times.fatigue);
	}
	if(pc->block_times.dodge > 0){
		--pc->block_times.dodge;
	}
}

void ClientUpdatePlayers(Game_data* const gd){
	if(host(gd)->flags & disrupted){
		DisruptPC(host(gd));
		host(gd)->flags &= ~(disrupted);
	}
	ClientUpdatePlayer(gd, 0);
}
