#ifndef TYPES_H_
#define TYPES_H_

//other
typedef struct Walk Walk;
typedef struct Blade Blade;
typedef struct Status_frame Status_frame;
typedef struct Render_data Render_data;
//Being
typedef struct Blade_hostile Blade_hostile;
typedef struct Being_type Being_type;
typedef struct Being Being;
typedef struct Beings_array Beings_array;
typedef struct Segment_beings Segment_beings;
//Projectile
typedef struct Projectile Projectile;
typedef struct Projectiles_array Projectiles_array;
typedef struct Projectile_hostile Projectile_hostile;
typedef struct Projectiles_h_array Projectiles_h_array;
//World
typedef struct Segment Segment;
typedef struct World World;
//Player
typedef struct Player Player;
//Game
typedef struct Game_data Game_data;
typedef struct Box_element Box_element;
typedef struct Box Box;
typedef struct Boxes Boxes;

//other
struct Walk{
	int time_left;
	SDL_FPoint shift;
};
struct Blade{
	SDL_FPoint position;
	float direction;
	int damage;
	unsigned int penetration;
	unsigned int hit_targets[MAX_HITS];
	unsigned int hits;
};
struct Status_frame{
	SDL_FPoint position;
	float direction;
};
struct Render_data{
	float viewfinder;
	SDL_FRect visible_rect;
	SDL_Rect viewfinder_rect;
	int window_w;
	int window_h;
	float sin_player_direction;
	float cos_player_direction;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* textures[TEXTURES_NUM];
};
//Being
struct Being_type{
	float size;
    float velocity;
    int hit_points;
	int damage_close;
	int damage_far;
};
struct Blade_hostile{
	SDL_FPoint position;
	float direction;
	// int attack_tiks_left;
};
struct Being{
    SDL_FPoint position;
	float direction;
    Segment* segment;
	unsigned int indx;
    int hit_points;
	const Being_type* type;
	int status;
	int status_ticks_left;
	Blade_hostile blade;
	SDL_FPoint special_move_shift;
	float special_rotation_shift;
	unsigned int id;
};
struct Beings_array{
	Being* array;
    unsigned int num;
};
struct Segment_beings{
	Being* array[MAX_SEGM_BEINGS];
    unsigned int num;
};
//Projectile
struct Projectile{
	SDL_FPoint position;
	SDL_FPoint shift_per_tick;
	int damage;
	unsigned int penetration;
	unsigned int hit_targets[MAX_HITS];
	unsigned int hits;
};
struct Projectile_hostile{
	SDL_FPoint position;
	SDL_FPoint shift_per_tick;
	int damage;
};
struct Projectiles_array{
	Projectile* array;
	unsigned int num;
};
struct Projectiles_h_array{
	Projectile_hostile* array;
	unsigned int num;
};
//World
struct Segment{
	Segment_beings beings;
	SDL_Point indx;
};
struct World{
	Segment*** segments;
	float width;
	float height;
	SDL_FPoint portalA;
	SDL_FPoint portalB;
	SDL_FPoint door;
};
//Player
struct Player{
	Uint32 control_flags;
	SDL_FPoint position;
	Blade blade;
	float direction;
	float velocity;
	float max_velocity;
	int hit_points;
	int fatigue_points;
	int magic_points;
	int max_h_p;
	int max_fatigue;
	int fatigue_block_time;
	float armour;
	int coins;
};
//Game
struct Box_element {
	int type;
	void* value;
};
struct Box {
	SDL_FPoint location;
	Box_element elements[BOX_SLOTS];
};
struct Boxes {
	Box* array;
	unsigned int num;
};
struct Game_data{
	Player pc;
	Beings_array beings;
	Projectiles_array projectiles;
	Projectiles_h_array h_projectiles;
	World world;
	Boxes boxes;
	unsigned int keys;
	unsigned int needed_keys;
};

#endif