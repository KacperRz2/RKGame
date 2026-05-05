#include <common.h>

extern inline unsigned int GetBigSegCoordinate(const float x){
	return (unsigned int)(x / BIG_SEGMENT_SIZE);
}

extern inline bool IsInUncoveredBigSeg(const Uint64* const wrld_plan, const unsigned int x, const unsigned int y){
	return (bool)((*(wrld_plan + y) & ((1ULL << (x * 2U)) | (1ULL << (x * 2U + 1U)))) == ((1ULL << (x * 2U)) | (1ULL << (x * 2U + 1U))));
}

extern inline bool IsInNoticedBigSeg(const Uint64* const wrld_plan, const unsigned int x, const unsigned int y){
	return (bool)(*(wrld_plan + y) & (1ULL << (x * 2U + 1U)));
}

extern inline void UncoverBigSeg(Uint64* const wrld_plan, const unsigned int x, const unsigned int y){
	*(wrld_plan + y) |= (1ULL << (x * 2U)) | (1ULL << (x * 2U + 1U));
}

extern inline void MarkAsNoticedBigSeg(Uint64* const wrld_plan, const unsigned int x, const unsigned int y){
	*(wrld_plan + y) |= (1ULL << (x * 2U + 1U));
}

extern inline bool IsVoidBigSeg(const Uint64* const wrld_plan, const unsigned int x, const unsigned int y){
	return (bool)(x >= BIG_SEGMENTS_X || y >= BIG_SEGMENTS_X || ((*(wrld_plan + y) & ((1ULL << (x * 2U)) | (1ULL << (x * 2U + 1U)))) == (1ULL << (x * 2U))));
}

static void SetBigSegmentPlanWalls(const unsigned int bigc, const unsigned int bigr, Sint8(* big_seg_plan)[BIG_SEGMENT_SEGMENTS_X]){
	//1
	int block_size_c = SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 + 1);
	int block_size_r = SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2);
	int shift = BIG_SEGMENT_SEGMENTS_X / 2 + 1 - block_size_c <= BIG_SEGMENT_SEGMENTS_X / 2 - block_size_r ? SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 + 1 - block_size_c) : SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 - block_size_r);
	for(unsigned int c = shift; c < block_size_c + shift; ++c){
		for(unsigned int r = shift; r < block_size_r + shift; ++r){
			*(*(big_seg_plan + c) + r) = -1;
		}
	}
	if(block_size_c > 2 && block_size_r > 2 && SDL_rand(2)){
		for(unsigned int c = shift + 1U; c < block_size_c + shift - 1U; ++c){
			for(unsigned int r = shift + 1U; r < block_size_r + shift - 1U; ++r){
				*(*(big_seg_plan + c) + r) = 0;
			}
		}
		*(*(big_seg_plan + (block_size_c + shift - 1U)) + (shift + 1U)) = 0;
		*(*(big_seg_plan + (shift + 1U)) + (block_size_r + shift - 1U)) = 0;
		if((block_size_c > 3 && block_size_r > 3) && (block_size_c > 4 || block_size_r > 4)){
			if(block_size_r > block_size_c){
				*(*(big_seg_plan + (block_size_c + shift - 1U)) + (block_size_r + shift - 2U)) = 0;
			}else{
				*(*(big_seg_plan + (block_size_c + shift - 2U)) + (block_size_r + shift - 1U)) = 0;
			}
		}
	}
	//2
	block_size_c = SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2);
	block_size_r = SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 + 1);
	shift = BIG_SEGMENT_SEGMENTS_X / 2 - block_size_c <= BIG_SEGMENT_SEGMENTS_X / 2 + 1 - block_size_r ? SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 - block_size_c) : SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 + 1 - block_size_r);
	for(unsigned int c = BIG_SEGMENT_SEGMENTS_X - block_size_c - shift; c < BIG_SEGMENT_SEGMENTS_X - shift; ++c){
		for(unsigned int r = shift; r < block_size_r + shift; ++r){
			*(*(big_seg_plan + c) + r) = -1;
		}
	}
	if(block_size_c > 2 && block_size_r > 2 && SDL_rand(2)){
		for(unsigned int c = BIG_SEGMENT_SEGMENTS_X - block_size_c - shift + 1U; c < BIG_SEGMENT_SEGMENTS_X - shift - 1U; ++c){
			for(unsigned int r = shift + 1U; r < block_size_r + shift - 1U; ++r){
				*(*(big_seg_plan + c) + r) = 0;
			}
		}
		*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X - block_size_c - shift)) + (shift + 1U)) = 0;
		*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X - shift - 2U)) + (block_size_r + shift - 1U)) = 0;
		if((block_size_c > 3 && block_size_r > 3) && (block_size_c > 4 || block_size_r > 4)){
			if(block_size_r > block_size_c){
				*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X - block_size_c - shift)) + (block_size_r + shift - 2U)) = 0;
			}else{
				*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X - block_size_c - shift + 1U)) + (block_size_r + shift - 1U)) = 0;
			}
		}
	}
	//3
	block_size_c = SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 + 1);
	block_size_r = SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2);
	shift = BIG_SEGMENT_SEGMENTS_X / 2 + 1 - block_size_c <= BIG_SEGMENT_SEGMENTS_X / 2 - block_size_r ? SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 + 1 - block_size_c) : SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 - block_size_r);
	for(unsigned int c = BIG_SEGMENT_SEGMENTS_X - block_size_c - shift; c < BIG_SEGMENT_SEGMENTS_X - shift; ++c){
		for(unsigned int r = BIG_SEGMENT_SEGMENTS_X - block_size_r - shift; r < BIG_SEGMENT_SEGMENTS_X - shift; ++r){
			*(*(big_seg_plan + c) + r) = -1;
		}
	}
	if(block_size_c > 2 && block_size_r > 2 && SDL_rand(2)){
		for(unsigned int c = BIG_SEGMENT_SEGMENTS_X - block_size_c - shift + 1U; c < BIG_SEGMENT_SEGMENTS_X - shift - 1U; ++c){
			for(unsigned int r = BIG_SEGMENT_SEGMENTS_X - block_size_r - shift + 1U; r < BIG_SEGMENT_SEGMENTS_X - shift - 1U; ++r){
				*(*(big_seg_plan + c) + r) = 0;
			}
		}
		*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X - shift - 2U)) + (BIG_SEGMENT_SEGMENTS_X - block_size_r - shift)) = 0;
		*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X - block_size_c - shift)) + (BIG_SEGMENT_SEGMENTS_X - shift - 2U)) = 0;
		if((block_size_c > 3 && block_size_r > 3) && (block_size_c > 4 || block_size_r > 4)){
			if(block_size_r > block_size_c){
				*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X - block_size_c - shift)) + (BIG_SEGMENT_SEGMENTS_X - block_size_r - shift + 1U)) = 0;
			}else{
				*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X - block_size_c - shift + 1U)) + (BIG_SEGMENT_SEGMENTS_X - block_size_r - shift)) = 0;
			}
		}
	}
	//4
	block_size_c = SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2);
	block_size_r = SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 + 1);
	shift = BIG_SEGMENT_SEGMENTS_X / 2 - block_size_c <= BIG_SEGMENT_SEGMENTS_X / 2 + 1 - block_size_r ? SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 - block_size_c) : SDL_rand(BIG_SEGMENT_SEGMENTS_X / 2 + 1 - block_size_r);
	for(unsigned int c = shift; c < block_size_c + shift; ++c){
		for(unsigned int r = BIG_SEGMENT_SEGMENTS_X - block_size_r - shift; r < BIG_SEGMENT_SEGMENTS_X - shift; ++r){
			*(*(big_seg_plan + c) + r) = -1;
		}
	}
	if(block_size_c > 2 && block_size_r > 2 && SDL_rand(2)){
		for(unsigned int c = shift + 1U; c < block_size_c + shift - 1U; ++c){
			for(unsigned int r = BIG_SEGMENT_SEGMENTS_X - block_size_r - shift + 1U; r < BIG_SEGMENT_SEGMENTS_X - shift - 1U; ++r){
				*(*(big_seg_plan + c) + r) = 0;
			}
		}
		*(*(big_seg_plan + (shift + 1U)) + (BIG_SEGMENT_SEGMENTS_X - block_size_r - shift)) = 0;
		*(*(big_seg_plan + (block_size_c + shift - 1U)) + (BIG_SEGMENT_SEGMENTS_X - shift - 2U)) = 0;
		if((block_size_c > 3 && block_size_r > 3) && (block_size_c > 4 || block_size_r > 4)){
			if(block_size_r > block_size_c){
				*(*(big_seg_plan + (block_size_c + shift - 1U)) + (BIG_SEGMENT_SEGMENTS_X - block_size_r - shift + 1U)) = 0;
			}else{
				*(*(big_seg_plan + (block_size_c + shift - 2U)) + (BIG_SEGMENT_SEGMENTS_X - block_size_r - shift)) = 0;
			}
		}
	}
	if(SDL_rand(2)){
		for(unsigned int i = 0U; i < BIG_SEGMENT_SEGMENTS_X / 2U - 1U; ++i){
			*(*(big_seg_plan + i)) = -1;
		}
		for(unsigned int i = 1U; i < BIG_SEGMENT_SEGMENTS_X / 2U - 1U; ++i){
			*(*(big_seg_plan) + i) = -1;
		}
	}
	if(SDL_rand(2)){
		for(unsigned int i = BIG_SEGMENT_SEGMENTS_X / 2U + 1U; i < BIG_SEGMENT_SEGMENTS_X; ++i){
			*(*(big_seg_plan + i)) = -1;
		}
		for(unsigned int i = 1U; i < BIG_SEGMENT_SEGMENTS_X / 2U; ++i){
			*(*(big_seg_plan + BIG_SEGMENT_SEGMENTS_X - 1U) + i) = -1;
		}
	}
	if(SDL_rand(2)){
		for(unsigned int i = BIG_SEGMENT_SEGMENTS_X / 2U; i < BIG_SEGMENT_SEGMENTS_X; ++i){
			*(*(big_seg_plan + i) + BIG_SEGMENT_SEGMENTS_X - 1U) = -1;
		}
		for(unsigned int i = BIG_SEGMENT_SEGMENTS_X / 2U + 1U; i < BIG_SEGMENT_SEGMENTS_X - 1U; ++i){
			*(*(big_seg_plan + BIG_SEGMENT_SEGMENTS_X - 1U) + i) = -1;
		}
	}
	if(SDL_rand(2)){
		for(unsigned int i = BIG_SEGMENT_SEGMENTS_X / 2U + 1U; i < BIG_SEGMENT_SEGMENTS_X; ++i){
			*(*(big_seg_plan) + i) = -1;
		}
		for(unsigned int i = 1U; i < BIG_SEGMENT_SEGMENTS_X / 2U - 1U; ++i){
			*(*(big_seg_plan + i) + BIG_SEGMENT_SEGMENTS_X - 1U) = -1;
		}
	}
	*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X / 2 - 1))) = 0;
	*(*(big_seg_plan) + (BIG_SEGMENT_SEGMENTS_X / 2)) = 0;
	if(bigc == BIG_SEGMENTS_X - 1U){
		for(unsigned int r = 0U; r < BIG_SEGMENT_SEGMENTS_X; ++r){
			*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X - 1)) + r) = -1;
			*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X - 2)) + r) = 0;
			*(*(big_seg_plan + (BIG_SEGMENT_SEGMENTS_X - 3)) + r) = 0;
		}
	}else if(bigc == 0U){
		for(unsigned int r = 0U; r < BIG_SEGMENT_SEGMENTS_X; ++r){
			*(*(big_seg_plan) + r) = -1;
			*(*(big_seg_plan + 1) + r) = 0;
			*(*(big_seg_plan + 2) + r) = 0;
		}
	}
	if(bigr == 0U){
		for(unsigned int c = 0U; c < BIG_SEGMENT_SEGMENTS_X; ++c){
			*(*(big_seg_plan + c)) = -1;
			*(*(big_seg_plan + c) + 1) = 0;
			*(*(big_seg_plan + c) + 2) = 0;
		}
	}else if(bigr == BIG_SEGMENTS_X - 1U){
		for(unsigned int c = 0U; c < BIG_SEGMENT_SEGMENTS_X; ++c){
			*(*(big_seg_plan + c) + (BIG_SEGMENT_SEGMENTS_X - 1)) = -1;
			*(*(big_seg_plan + c) + (BIG_SEGMENT_SEGMENTS_X - 2)) = 0;
			*(*(big_seg_plan + c) + (BIG_SEGMENT_SEGMENTS_X - 3)) = 0;
		}
	}
}

static void SetBigSegmentSegments(World* const world, const unsigned int bigc, const unsigned int bigr){
	Sint8 big_seg_plan[BIG_SEGMENT_SEGMENTS_X][BIG_SEGMENT_SEGMENTS_X];
	SDL_zeroa(big_seg_plan);
	SetBigSegmentPlanWalls(bigc, bigr, big_seg_plan);
	for(unsigned int i = 0U; i < BIG_SEGMENT_SEGMENTS_X; ++i){
		const unsigned int c = bigc * BIG_SEGMENT_SEGMENTS_X + i;
		for(unsigned int j = 0U; j < BIG_SEGMENT_SEGMENTS_X; ++j){
			const unsigned int r = bigr * BIG_SEGMENT_SEGMENTS_X + j;
			if(*(*(big_seg_plan + i) + j) != BIG_SEG_PLAN_NULL_SEG){
				*(*(world->segments + c) + r) = (Segment*)SDL_malloc(sizeof(Segment));
				(*(*(world->segments + c) + r))->indx.x = c;
				(*(*(world->segments + c) + r))->indx.y = r;
				(*(*(world->segments + c) + r))->beings.num = 0U;
				(*(*(world->segments + c) + r))->ally_beings.num = 0U;
				(*(*(world->segments + c) + r))->flags = 0x0U;
			}else{
				*(*(world->segments + c) + r) = NULL;
			}
		}
	}
}

static void SetDoor(SDL_Point* const door_plan_position, SDL_FPoint* const door_world_position, const int rand){
	if(rand == 0){
		door_plan_position->x = SMALL_PLAN_SIZE / 2U;
		door_plan_position->y = 0U;
	}else if(rand == 1){
		door_plan_position->x = SMALL_PLAN_SIZE - 1U;
		door_plan_position->y = SMALL_PLAN_SIZE / 2U;
	}else if(rand == 2){
		door_plan_position->x = SMALL_PLAN_SIZE / 2U;
		door_plan_position->y = SMALL_PLAN_SIZE - 1U;
	}else{
		door_plan_position->x = 0U;
		door_plan_position->y = SMALL_PLAN_SIZE / 2U;
	}
	door_world_position->x = GetDoorPositionXorY(door_plan_position->x);
	door_world_position->y = GetDoorPositionXorY(door_plan_position->y);
}

static void PlaceWorldDoors(World* const world, SDL_Point* const portal0, SDL_Point* const portal1){
	const int rand0 = SDL_rand(4);
	const int rand1 = (rand0 + SDL_rand(3) + 1) % 4;
	int rand2 = (rand0 + SDL_rand(2) + 1) % 4;
	while(rand2 == rand1){
		rand2 = (rand0 + SDL_rand(3) + 1) % 4;
	}
	SetDoor(portal0, &world->portalA, rand0);
	SetDoor(portal1, &world->portalB, rand1);
	SetDoor(&ZERO_POINT, &world->door, rand2);
}

static void SetWorldPartsConnection(const int rand, bool(* world_plan_base)[BIG_SEGMENTS_X], const SDL_Point* const fields, const int indx){
	enum direction{up, right, down, left};
	const unsigned int step = HUGE_SEGMENT_BIG_SEGMENTS_X + 1U;
	const unsigned int shift = HUGE_SEGMENT_BIG_SEGMENTS_X / 2U + 1U;
	if(rand == up){
		*(*(world_plan_base + shift + (fields + indx)->x * step) + shift + (fields + indx)->y * step - shift) = true;
	}else if(rand == right){
		*(*(world_plan_base + shift + (fields + indx)->x * step + shift) + shift + (fields + indx)->y * step) = true;
	}else if(rand == down){
		*(*(world_plan_base + shift + (fields + indx)->x * step) + shift + (fields + indx)->y * step + shift) = true;
	}else{
		*(*(world_plan_base + shift + (fields + indx)->x * step - shift) + shift + (fields + indx)->y * step) = true;
	}
}

static void GenerateWorldPartsConnections(bool(* world_plan_base)[BIG_SEGMENTS_X], int(* small_wordl_plan)[SMALL_PLAN_SIZE], const SDL_Point* const portal0, const SDL_Point* const portal1){
	enum small_plan_field{
		small_plan_null,
		small_plan_unconnected,
		small_plan_connected
	};
	enum direction{up, right, down, left};
	int fields_left = NOT_NULL_HUGE_SEGS_NUM - 1;
	int try = 0b0000;
	SDL_Point fields[NOT_NULL_HUGE_SEGS_NUM];
	do{
		*fields = (SDL_Point){
			SDL_rand(SMALL_PLAN_SIZE),
			SDL_rand(SMALL_PLAN_SIZE)
		};
	}while(*(*(small_wordl_plan + fields->x) + fields->y) == small_plan_null);
	*(*(small_wordl_plan + fields->x) + fields->y) = small_plan_connected;
	int indx = 0;
	while(fields_left){
		if(try == 0b1111){
			--indx;
			try = 0b0000;
			continue;
		}
		int new_col = (fields + indx)->x;
		int new_row = (fields + indx)->y;
		int rand;
		do{
			rand = SDL_rand(4);
		}while(try & 1 << rand);
		if(rand == up){
			--new_row;
		}else if(rand == right){
			++new_col;
		}else if(rand == down){
			++new_row;
		}else{
			--new_col;
		}
		if(new_col < 0 || new_col >= SMALL_PLAN_SIZE || new_row < 0 || new_row >= SMALL_PLAN_SIZE){
			if((fields + indx)->x == portal0->x && (fields + indx)->y == portal0->y){
				new_col = portal1->x;
				new_row = portal1->y;
			}else if((fields + indx)->x == portal1->x && (fields + indx)->y == portal1->y){
				new_col = portal0->x;
				new_row = portal0->y;
			}else{
				try |= 1 << rand;
				continue;
			}
		}
		if(*(*(small_wordl_plan + new_col) + new_row) != small_plan_unconnected){
			try |= 1 << rand;
			continue;
		}
		SetWorldPartsConnection(rand, world_plan_base, fields, indx);
		*(*(small_wordl_plan + new_col) + new_row) = small_plan_connected;
		++indx;
		(fields + indx)->x = new_col;
		(fields + indx)->y = new_row;
		try = 0b0000;
		--fields_left;
	}
}

static void SetWordPartsShapes(bool(* world_plan_base)[BIG_SEGMENTS_X]){
	const unsigned int shift = HUGE_SEGMENT_BIG_SEGMENTS_X / 2U + 1U;
	const unsigned int step = HUGE_SEGMENT_BIG_SEGMENTS_X + 1U;
	for(unsigned int c = shift; c < BIG_SEGMENTS_X; c += step){
		for(unsigned int r = shift; r < BIG_SEGMENTS_X; r += step){
			if(*(*(world_plan_base + c) + r) == true && *(*(world_plan_base + c + 1U) + r + 1U) == true){
				switch(SDL_rand(21)){
					case 0:
						*(*(world_plan_base + c) + r) = false;
						break;
					case 1:
						*(*(world_plan_base + c) + r) = false;
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						break;
					case 2:
						*(*(world_plan_base + c) + r) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						break;
					case 3:
						*(*(world_plan_base + c) + r) = false;
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						break;
					case 4:
						*(*(world_plan_base + c) + r) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 5:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						break;
					case 6:
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						break;
					case 7:
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						break;
					case 8:
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 9:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 10:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						break;
					case 11:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 12:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 13:
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 14:
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 15:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 16:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						break;
					case 17:
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						break;
					case 18:
						*(*(world_plan_base + c - 1U) + r + 1U) = false;
						*(*(world_plan_base + c - 1U) + r - 1U) = false;
						break;
					case 19:
						*(*(world_plan_base + c + 1U) + r + 1U) = false;
						*(*(world_plan_base + c + 1U) + r - 1U) = false;
						break;
					default: break;
				}
			}
		}
	}
}

static void MarkAsVoidBigSeg(Uint64* const wrld_plan, const unsigned int x, const unsigned int y){
	*(wrld_plan + y) |= (1ULL << (x * 2U));
}

static void RemoveTinyPassages(World* const world){
	for(unsigned int c = 1U; c < SEGMENTS_X - 1U; ++c){
		for(unsigned int r = 1U; r < SEGMENTS_X - 1U; ++r){
			Segment* const seg = GetSegmentByIndxUnsafe(world, c, r);
			if(!seg){
				continue;
			}
			if(GetSegmentByIndxUnsafe(world, c - 1U, r - 1U) && !(GetSegmentByIndxUnsafe(world, c, r - 1U) || GetSegmentByIndxUnsafe(world, c - 1U, r))){
				if(SDL_rand(2)){
					SDL_free(*(*(world->segments + c) + r));
					*(*(world->segments + c) + r) = NULL;
				}else{
					SDL_free(*(*(world->segments + c - 1U) + r - 1U));
					*(*(world->segments + c - 1U) + r - 1U) = NULL;
				}
			}else if(GetSegmentByIndxUnsafe(world, c + 1U, r - 1U) && !(GetSegmentByIndxUnsafe(world, c, r - 1U) || GetSegmentByIndxUnsafe(world, c + 1U, r))){
				if(SDL_rand(2)){
					SDL_free(*(*(world->segments + c) + r));
					*(*(world->segments + c) + r) = NULL;
				}else{
					SDL_free(*(*(world->segments + c + 1U) + r - 1U));
					*(*(world->segments + c + 1U) + r - 1U) = NULL;
				}
			}
		}
	}
}

static void CreateWorldSegments(World* const world, bool(* world_plan_base)[BIG_SEGMENTS_X]){
	SDL_zeroa(world->plan);
	world->segments = (Segment***)SDL_malloc(sizeof(Segment**) * SEGMENTS_X);
	for(unsigned int c = 0U; c < SEGMENTS_X; ++c){
		*(world->segments + c) = (Segment**)SDL_malloc(sizeof(Segment*) * SEGMENTS_Y);
	}
	for(unsigned int bigc = 0U; bigc < BIG_SEGMENTS_X; ++bigc){
		for(unsigned int bigr = 0U; bigr < BIG_SEGMENTS_X; ++bigr){
			if(*(*(world_plan_base + bigc) + bigr) == true){
				SetBigSegmentSegments(world, bigc, bigr);
			}else{
				for(unsigned int c = bigc * BIG_SEGMENT_SEGMENTS_X; c < bigc * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X; ++c){
					for(unsigned int r = bigr * BIG_SEGMENT_SEGMENTS_X; r < bigr * BIG_SEGMENT_SEGMENTS_X + BIG_SEGMENT_SEGMENTS_X; ++r){
						*(*(world->segments + c) + r) = NULL;
					}
				}
				MarkAsVoidBigSeg(world->plan, bigc, bigr);
			}
		}
	}
	RemoveTinyPassages(world);
}

void CreateWorld(Game_data* const gd){
	SDL_srand(gd->seed);
	bool world_plan_base[BIG_SEGMENTS_X][BIG_SEGMENTS_X] = WORLD_BASE;
	int small_wordl_plan[SMALL_PLAN_SIZE][SMALL_PLAN_SIZE] = WORLD_SMALL_BASE;
	World* const world = &gd->world;
	SDL_Point portal0, portal1;
	PlaceWorldDoors(world, &portal0, &portal1);
	GenerateWorldPartsConnections(world_plan_base, small_wordl_plan, &portal0, &portal1);
	SetWordPartsShapes(world_plan_base);
	CreateWorldSegments(world, world_plan_base);
	PlaceShops(&gd->world);
	SDL_srand(0ULL);
}

int SDLCALL compareBoxes(const void* a, const void* b){
    const Box* A = (const Box*)a;
    const Box* B = (const Box*)b;
    if(A->location.x < B->location.x){
        return -1;
    }else if(B->location.x < A->location.x){
        return 1;
    }
	return 0;
}

static void PlaceBoxes(Game_data* const gd){
	const int min_segs_from_edge = 2;
	while(gd->boxes.num < BOXES_NUM){
		unsigned int c = SDL_rand(SEGMENTS_X - min_segs_from_edge * 2) + min_segs_from_edge;
		unsigned int r = SDL_rand(SEGMENTS_Y - min_segs_from_edge * 2) + min_segs_from_edge;
		Segment* seg = GetSegmentByIndxUnsafe(&gd->world, c, r);
		if(seg){
			const unsigned int new_box_indx = gd->boxes.num++;
			(gd->boxes.array + new_box_indx)->location.x = SegmentCenterX(seg) + (SDL_randf() - 0.5F) * (SEGMENT_SIZE - BOX_SIZE);
			(gd->boxes.array + new_box_indx)->location.y = SegmentCenterY(seg) + (SDL_randf() - 0.5F) * (SEGMENT_SIZE - BOX_SIZE);
		}
	}
	SDL_qsort(gd->boxes.array, gd->boxes.num, sizeof(Box), compareBoxes);
	for(unsigned int i = 0U; i < gd->boxes.num; ++i){
		SDL_FPoint box_location = (gd->boxes.array + i)->location;
		(gd->boxes.array + i)->location = ZERO_POINT_F;
		const int box_indx = GetNearbyBoxIndx(&gd->boxes, &box_location, BOX_SIZE);
		if(box_indx != NOT_FOUND){
			do{
				box_location.y += BOX_SIZE;
				if(box_location.y >= WORLD_H - SEGMENT_SIZE * 2.0F){
					box_location.y = SEGMENT_SIZE * 2.0F + half(BOX_SIZE);
				}
			}while(!GetSegmentUnsafe(&gd->world, box_location.x, box_location.y) || SDL_fabsf(box_location.y - (gd->boxes.array + box_indx)->location.y) < BOX_SIZE);
		}
		(gd->boxes.array + i)->location = box_location;
	}
}

int SDLCALL compareShops(const void* a, const void* b){
    const Shop* A = (const Shop*)a;
    const Shop* B = (const Shop*)b;
    if(A->location.x < B->location.x){
        return -1;
    }else if(B->location.x < A->location.x){
        return 1;
    }
	if(A->location.y < B->location.y){
		return -1;
	}else if(B->location.y < A->location.y){
		return 1;
	}
	return 0;
}

static void PlaceShops(World* const wld){
	const unsigned int shops_huge_segments[SHOPS_NUM][2] = SHOPS_HUGE_SEGS;
	unsigned int a, b;
	if(SDL_rand(2)){
		a = 0U;
		b = 1U;
	}else{
		a = 1U;
		b = 0U;
	}
	for(unsigned int i = 0U; i < SHOPS_NUM; ++i){
		unsigned int x = HugeSegCenterBigSegCoordinate(*(*(shops_huge_segments + i) + a));
		unsigned int y = HugeSegCenterBigSegCoordinate(*(*(shops_huge_segments + i) + b));
		if(!IsVoidBigSeg(wld->plan, x, y)){
			x = SDL_rand(2) ? x + 1U : x - 1U;
			do{
				++y;
			}while(!IsVoidBigSeg(wld->plan, x, y));
		}
		const float shift = SDL_randf() * (half(BIG_SEGMENT_SIZE) - SHOP_SIZE - SEGMENT_SIZE * 2.0F) + half(SHOP_SIZE) + SEGMENT_SIZE;
		(wld->shops + i)->location.x = SDL_rand(2) ? BigSegPosition(x) + shift : BigSegPosition(x) + BIG_SEGMENT_SIZE - shift;
		(wld->shops + i)->location.y = BigSegPosition(y) - 1.0F;
		while(!GetSegmentUnsafe(wld, (wld->shops + i)->location.x, (wld->shops + i)->location.y)){
			(wld->shops + i)->location.y -= SEGMENT_SIZE;
		}
	}
	SDL_qsort(wld->shops, SHOPS_NUM, sizeof(Shop), compareShops);
}

void DestroyWorld(World* const wld){
	for(unsigned int c = 0U; c < SEGMENTS_X; ++c){
	for(unsigned int r = 0U; r < SEGMENTS_Y; ++r){
			SDL_free(*(*(wld->segments + c) + r));
		}
		SDL_free(*(wld->segments + c));
	}
	SDL_free(wld->segments);
	SDL_free(wld->key_locations);
}

extern inline Segment* GetSegmentUnsafe(const World* const wld, const float x, const float y){
	const unsigned int c = (unsigned int)(x / SEGMENT_SIZE);
	const unsigned int r = (unsigned int)(y / SEGMENT_SIZE);
	return *(*(wld->segments + c) + r);
}

extern inline Segment* GetSegmentSafe(const World* const wld, const float x, const float y){
	const unsigned int c = (unsigned int)(x / SEGMENT_SIZE);
	const unsigned int r = (unsigned int)(y / SEGMENT_SIZE);
	if(c >= SEGMENTS_X || r >= SEGMENTS_Y){
		return NULL;
	}
	return *(*(wld->segments + c) + r);
}

extern inline Segment* GetSegmentByIndxUnsafe(const World* const wld, const unsigned int c, const unsigned int r){
	return *(*(wld->segments + c) + r);
}

extern inline Segment* GetSegmentByIndxSafe(const World* const wld, const unsigned int c, const unsigned int r){
	if(c >= SEGMENTS_X || r >= SEGMENTS_Y){
		return NULL;
	}
	return *(*(wld->segments + c) + r);
}

static void FillShops(World* const wld){
	for(unsigned int i = 0U; i < SHOPS_NUM; ++i){
		for(unsigned int j = 0U; j < SHOP_SCROLLS_NUM; ++j){
			*((wld->shops + i)->scrolls + j) = GetRandomScroll();
		}
	}
}

static void PlaceBeings(Game_data* const gd, const SDL_FPoint* const start_position){
	while(gd->beings.num < MAX_START_BEINGS_NUM){
		const float x = SEGMENT_SIZE + SDL_randf() * (WORLD_SIZE - SEGMENT_SIZE * 2.0F);
		const float y = SEGMENT_SIZE + SDL_randf() * (WORLD_SIZE - SEGMENT_SIZE * 2.0F);
		if(pow2(start_position->x - x) + pow2(start_position->y - y) > pow2(BIG_SEGMENT_SIZE)){
			TryCreateIdleBeing(gd, GetRandomBeingId(), x, y, human(gd));
		}
	}
}

void StartNewLevel(Game_data* const gd){
	PlaceBoxes(gd);
	FillBoxes(gd);
	FillShops(&gd->world);
	gd->keys_status = (Uint8*)SDL_malloc(sizeof(Uint8) * gd->needed_keys);
	for(unsigned int i = 0U; i < gd->needed_keys; ++i){
		*(gd->keys_status + i) = key_location_unknown;
	}
	const SDL_FPoint start_position = GetStartPosition(&gd->world);
	for(unsigned int i = 0U; i < START_PLAYERS_NUM; ++i){
		CreatePlayer((gd->champions.array + i), start_position.x, start_position.y);
		(gd->champions.array + i)->segment = GetSegmentUnsafe(&gd->world, (gd->champions.array + i)->position.x, (gd->champions.array + i)->position.y);
		(gd->champions.array + i)->last_seen_in = (gd->champions.array + i)->segment;
		++gd->champions.num;
	}
	const unsigned int seg_x = GetBigSegCoordinate(start_position.x);
	const unsigned int seg_y = GetBigSegCoordinate(start_position.y);
	MarkAsNoticedBigSeg(gd->world.plan, seg_x, seg_y);
	PlaceBeings(gd, &start_position);
}

static SDL_FPoint GetStartPosition(World* const w){
	SDL_FPoint result;
	if(!(w->door.x < WORLD_SIZE / 4 || w->portalA.x < WORLD_SIZE / 4 || w->portalB.x < WORLD_SIZE / 4)){
		result.x = SEGMENT_SIZE * 2.25F;
		result.y = half(WORLD_SIZE);
	}else if(!(w->door.y < WORLD_SIZE / 4 || w->portalA.y < WORLD_SIZE / 4 || w->portalB.y < WORLD_SIZE / 4)){
		result.x = half(WORLD_SIZE);
		result.y = SEGMENT_SIZE * 2.25F;
	}else if(!(w->door.x > WORLD_SIZE / 4 * 3 || w->portalA.x > WORLD_SIZE / 4 * 3 || w->portalB.x > WORLD_SIZE / 4 * 3)){
		result.x = WORLD_SIZE - SEGMENT_SIZE * 2.25F;
		result.y = half(WORLD_SIZE);
	}else if(!(w->door.y > WORLD_SIZE / 4 * 3 || w->portalA.y > WORLD_SIZE / 4 * 3 || w->portalB.y > WORLD_SIZE / 4 * 3)){
		result.x = half(WORLD_SIZE);
		result.y = WORLD_SIZE - SEGMENT_SIZE * 2.25F;
	}
	return result;
}

extern inline float SegmentPositionX(const Segment* const s){
	return s->indx.x * SEGMENT_SIZE;
}

extern inline float SegmentPositionY(const Segment* const s){
	return s->indx.y * SEGMENT_SIZE;
}

extern inline float SegmentCenterX(const Segment* const s){
	return (s->indx.x + 0.5F) * SEGMENT_SIZE;
}

extern inline float SegmentCenterY(const Segment* const s){
	return (s->indx.y + 0.5F) * SEGMENT_SIZE;
}

extern inline float BigSegPosition(const unsigned int x){
	return x * BIG_SEGMENT_SIZE;
}

extern inline float BigSegCenter(const unsigned int x){
	return (x + 0.5F) * BIG_SEGMENT_SIZE;
}

static inline float SegPosition(const unsigned int x){
	return x * SEGMENT_SIZE;
}

static inline unsigned int HugeSegCenterBigSegCoordinate(const unsigned int x){
	return x * 4U + 2U;
}

static float GetDoorPositionXorY(const unsigned int small_plan_position){
	if(small_plan_position == 0){
		return SEGMENT_SIZE * 1.25F;
	}
	if(small_plan_position == 3){
		return half(WORLD_SIZE);
	}
	if(small_plan_position == 6){
		return WORLD_SIZE - SEGMENT_SIZE * 1.25F;
	}
	return SEGMENT_SIZE * 1.25F;
}

extern inline bool IsClearSight(const SDL_FPoint* const from, const SDL_FPoint* const to, const Segment* const to_s, const World* const w){
	const Segment* s = GetSegmentUnsafe(w, from->x, from->y);
	if(s == to_s){return true;}
	const float distance_x = to->x - from->x;
	const float distance_y = to->y - from->y;
	const float distance = SDL_sqrtf(pow2(distance_x) + pow2(distance_y));
    const float shift_x = distance_x / distance;
    const float shift_y = distance_y / distance;
	const float help_to_x = shift_x < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	const float help_to_y = shift_y < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	SDL_FPoint control_point = *from;
	do{
		const float help_x = SDL_fmodf(control_point.x, SEGMENT_SIZE);
		const float help_y = SDL_fmodf(control_point.y, SEGMENT_SIZE);
		const float shifts_to_next_seg_x = (help_to_x - help_x) / shift_x;
		const float shifts_to_next_seg_y = (help_to_y - help_y) / shift_y;
		const float multipl = shifts_to_next_seg_x < shifts_to_next_seg_y ? shifts_to_next_seg_x : shifts_to_next_seg_y;
		control_point.x += shift_x * multipl;
		control_point.y += shift_y * multipl;
		s = GetSegmentUnsafe(w, control_point.x, control_point.y);
		if(s == NULL){
			return false;
		}
	}while(s != to_s);
	return true;
}

extern inline bool IsClearSightWithKnownDistance(const SDL_FPoint* const from, const Segment* const to_s, const World* const w, const float d_x, const float d_y, const float distance_squared){
	const Segment* s = GetSegmentUnsafe(w, from->x, from->y);
	if(s == to_s){return true;}
	const float distance = SDL_sqrtf(distance_squared);
    const float shift_x = d_x / distance;
    const float shift_y = d_y / distance;
	const float help_to_x = shift_x < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	const float help_to_y = shift_y < 0.0F ? -1.0F : SEGMENT_SIZE + 1.0F;
	SDL_FPoint control_point = *from;
	do{
		const float help_x = SDL_fmodf(control_point.x, SEGMENT_SIZE);
		const float help_y = SDL_fmodf(control_point.y, SEGMENT_SIZE);
		const float shifts_to_next_seg_x = (help_to_x - help_x) / shift_x;
		const float shifts_to_next_seg_y = (help_to_y - help_y) / shift_y;
		const float multipl = shifts_to_next_seg_x < shifts_to_next_seg_y ? shifts_to_next_seg_x : shifts_to_next_seg_y;
		control_point.x += shift_x * multipl;
		control_point.y += shift_y * multipl;
		s = GetSegmentUnsafe(w, control_point.x, control_point.y);
		if(s == NULL){
			return false;
		}
	}while(s != to_s);
	return true;
}

extern inline bool SegmentInSight(const SDL_FPoint* const from, const SDL_FPoint* const point_in_seg, const Segment* const to_s, const World* const w){
	const SDL_FPoint to = {point_in_seg->x - SDL_fmodf(point_in_seg->x, SEGMENT_SIZE) + 0.5F, point_in_seg->y - SDL_fmodf(point_in_seg->y, SEGMENT_SIZE) + 0.5F};
	if(GetSegmentUnsafe(w, from->x, from->y) == to_s){return true;}
	const float distance_x = to.x - from->x;
	const float distance_y = to.y - from->y;
	SDL_FPoint p0, p1, p2;
	p0 = (SDL_FPoint){to.x, to.y + SEGMENT_SIZE - 1.0F};
	p1 = (SDL_FPoint){to.x + SEGMENT_SIZE - 1.0F, to.y + SEGMENT_SIZE - 1.0F};
	p2 = (SDL_FPoint){to.x + SEGMENT_SIZE - 1.0F, to.y};
	return (IsClearSight(from, &to, to_s, w) || IsClearSight(from, &p0, to_s, w) || IsClearSight(from, &p1, to_s, w) || IsClearSight(from, &p2, to_s, w));
}

static void PlaceKeysAndMapsInBoxes(Game_data* const gd){
	gd->world.key_locations = (Key_location*)SDL_malloc(sizeof(Key_location) * gd->needed_keys);
	Box* bx;
	for(unsigned int i = 0U; i < gd->needed_keys; ++i){
		do{
			bx = (gd->boxes.array + SDL_rand(gd->boxes.num));
		}while(bx->elements->type == box_key);
		AddToBox(bx, 0U, box_key, i);
		*(gd->world.key_locations + i) = (Key_location){
			(Uint8)(bx->location.x / WORLD_SIZE * 255.0F),
			(Uint8)(bx->location.y / WORLD_SIZE * 255.0F)
		};
		for(unsigned int j = 0U; j < ONE_KEY_MAPS_NUM; ++j){
			do{
				bx = (gd->boxes.array + SDL_rand(gd->boxes.num));
			}while(bx->elements->type == box_key || bx->elements->type == box_map);
			AddToBox(bx, 0U, box_map, i);
		}
	}
}

static void FillBoxes(Game_data* const gd){
	for(unsigned int i = 0U; i < gd->boxes.num; ++i){
		(gd->boxes.array + i)->elements->type = box_mp;
	}
	gd->needed_keys = SDL_rand(MAX_KEYS - MIN_KEYS + 1) + MIN_KEYS;
	PlaceKeysAndMapsInBoxes(gd);
	for(unsigned int i = 0U; i < gd->boxes.num; ++i){
		Box* bx = (gd->boxes.array + i);
		unsigned int slot = 0U;
		if(bx->elements->type == box_key || bx->elements->type == box_map){
			++slot;
		}
		AddToBox(bx, slot++, box_coins, SDL_rand(BOX_MAX_COINS));
		for(unsigned int j = slot; j < BOX_SLOTS - 1U; ++j){
			if(SDL_rand(BOX_SCROLL_CHANCE_FACTOR)){
				AddToBox(bx, slot++, box_scroll, GetRandomScroll());
			}
		}
		AddToBox(bx, slot, box_mp, SDL_rand(BOX_MAX_MP));
	}
}

extern inline void GetNeighbourSegments(Segment** const array, const World* const wld, const Segment* const seg){
	unsigned int i = 0U;
	for(unsigned int c = seg->indx.x - 1U; c <= seg->indx.x + 1U; ++c){
	for(unsigned int r = seg->indx.y - 1U; r <= seg->indx.y + 1U; ++r){
		*(array + i++) = GetSegmentByIndxUnsafe(wld, c, r);
	}}
}

extern inline void GetNeighbourSegmentsFar(Segment** const array, const World* const wld, const Segment* const seg, const int range){
	unsigned int i = 0U;
	for(int c = (int)seg->indx.x - range; c <= (int)seg->indx.x + range; ++c){
	for(int r = (int)seg->indx.y - range; r <= (int)seg->indx.y + range; ++r){
		*(array + i++) = GetSegmentByIndxSafe(wld, c, r);
	}}
}

extern inline void Get4NearestSegments(Segment** const array, const World* const wld, const Segment* const seg, const float x, const float y){
	unsigned int i = 0U;
	const unsigned int col = SDL_fmodf(x, SEGMENT_SIZE) < half(SEGMENT_SIZE) ? seg->indx.x - 1U : seg->indx.x;
	const unsigned int row = SDL_fmodf(y, SEGMENT_SIZE) < half(SEGMENT_SIZE) ? seg->indx.y - 1U : seg->indx.y;
	for(unsigned int c = col; c < col + 2U; ++c){
	for(unsigned int r = row; r < row + 2U; ++r){
		*(array + i++) = GetSegmentByIndxUnsafe(wld, c, r);
	}}
}

extern inline void Get2NearestSegments(Segment** const array, const World* const wld, Segment* const seg, const float x, const float y){
	*array = seg;
	unsigned int col = seg->indx.x;
	unsigned int row = seg->indx.y;
	const float x_in_seg = SDL_fmodf(x, SEGMENT_SIZE);
	const float y_in_seg = SDL_fmodf(y, SEGMENT_SIZE);
	const int x_part = x_in_seg < half(SEGMENT_SIZE) ? 0 : 1;
	const int y_part = y_in_seg < half(SEGMENT_SIZE) ? 0 : 1;
	if(x_part){
		if(y_part){
			if(x_in_seg > y_in_seg){
				++col;
			}else{
				++row;
			}
		}else{
			if(x_in_seg > SEGMENT_SIZE - y_in_seg){
				++col;
			}else{
				--row;
			}
		}
	}else{
		if(y_part){
			if(x_in_seg < SEGMENT_SIZE - y_in_seg){
				--col;
			}else{
				++row;
			}
		}else{
			if(x_in_seg < y_in_seg){
				--col;
			}else{
				--row;
			}
		}
	}
	*(array + 1U) = GetSegmentByIndxUnsafe(wld, col, row);
}

extern inline void GetSurroundingSegmentsFar(Segment** const array, const World* const wld, const Segment* const seg, const int range){
	unsigned int i = 0U;
	int row = (int)seg->indx.y - range;
	for(int c = (int)seg->indx.x - range; c <= (int)seg->indx.x + range; ++c){
		*(array + i++) = GetSegmentByIndxSafe(wld, c, row);
	}
	int col = (int)seg->indx.x + range;
	for(int r = (int)seg->indx.y - range + 1; r <= (int)seg->indx.y + range; ++r){
		*(array + i++) = GetSegmentByIndxSafe(wld, col, r);
	}
	row = (int)seg->indx.y + range;
	for(int c = (int)seg->indx.x + range - 1; c >= (int)seg->indx.x - range; --c){
		*(array + i++) = GetSegmentByIndxSafe(wld, c, row);
	}
	col = (int)seg->indx.x - range;
	for(int r = (int)seg->indx.y + range - 1; r >= (int)seg->indx.y - range + 1; --r){
		*(array + i++) = GetSegmentByIndxSafe(wld, col, r);
	}
}

extern inline Segment* GetDistantSegmentBySpiral(const World* const wld, const Segment* const seg, const int step){
	const Sint8 spiral_shift_x[] = SPIRAL_X_SHIFTS;
	const Sint8 spiral_shift_y[] = SPIRAL_Y_SHIFTS;
	return GetSegmentByIndxSafe(wld, seg->indx.x + *(spiral_shift_x + step), seg->indx.y + *(spiral_shift_y + step));
}
