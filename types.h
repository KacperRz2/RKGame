#ifndef TYPES_H_
#define TYPES_H_

//other
typedef struct Walk Walk;
typedef struct Blade Blade;
typedef struct Status_frame Status_frame;
//Being
typedef struct Being Being;
typedef struct Beings_array Beings_array;
typedef struct Segment_beings Segment_beings;
//Projectile
typedef struct Projectile Projectile;
typedef struct Projectiles_array Projectiles_array;
//World
typedef struct Segment Segment;
typedef struct World World;
//Player
typedef struct Player Player;

//other
struct Walk {
	int time_left;
	SDL_FPoint shift;
};
struct Blade {
	SDL_FPoint position;
	float direction;
	int damage;
	unsigned int penetration;
	void* hit_targets[MAX_HITS];
	unsigned int hits;
};
struct Status_frame {
	SDL_FPoint position;
	float direction;
};
//Being
struct Being {
    SDL_FPoint position;
    Segment* segment;
	unsigned int indx;
    float velocity;
    int hit_points;
	//bool flank;
    Walk walk;
};
struct Beings_array {
	Being* array;
    unsigned int num;
};
struct Segment_beings {
	Being* array[MAX_SEGM_BEINGS];
    unsigned int num;
};
//Projectile
struct Projectile {
	SDL_FPoint position;
	SDL_FPoint shift_per_tick;
	// unsigned int time_left;
	int damage;
	unsigned int penetration;
	void* hit_targets[MAX_HITS];
	unsigned int hits;
};
struct Projectiles_array {
	Projectile* array;
	unsigned int num;
};
//World
struct Segment {
	Segment_beings beings;
	// bool available;
	SDL_Point indx;
};
struct World {
	Segment*** segments;//[SEGMENTS_Y][SEGMENTS_X];
	float sin_player_direction;
	float cos_player_direction;
	float width;
	float height;
};
//Player
struct Player {
	// Segment* segment;
	Uint32 control_flags;
	SDL_FPoint position;
	Blade blade;
	float direction;
	float velocity;
	float max_velocity;
	float range;
	int hit_points;
	int fatigue_points;
	int magic_points;
	int max_h_p;
	int max_fatigue;
	int max_magic;
	int fatigue_block_time;
	float armour;
};

#endif