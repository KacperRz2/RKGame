#ifndef TYPES_H_
#define TYPES_H_

//other
typedef struct Walk Walk;
//Being
typedef struct Being Being;
typedef struct Beings_array Beings_array;
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
	Being** array;
    unsigned int num;
};
//Projectile
struct Projectile {
	SDL_FPoint position;
	SDL_FPoint shift_per_tick;
	unsigned int time_left;
	int damage;
};
struct Projectiles_array {
	Projectile** array;
	unsigned int num;
};
//World
struct Segment {
	SDL_FPoint coordinates;
	Beings_array beings;
	bool contain_player;
};
struct World {
	SDL_FRect vision_rects[ANGLE_PARTS];
	SDL_FRect visible_rect;
	Segment segments[SEGMENTS_Y][SEGMENTS_X];
	float sin_player_direction;
	float cos_player_direction;
	float width;
	float height;
	float sight;
	float sight_back;
	float sight_angle;
	float sight_angle_back;
};
//Player
struct Player {
	SDL_FPoint position;
	Segment* segment;
	Uint32 control_flags;
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