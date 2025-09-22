#ifndef TYPES_H_
#define TYPES_H_

//other
typedef struct Blade Blade;
typedef struct Placement Placement;
typedef struct Impact Impact;
typedef struct Render_data Render_data;
typedef struct Lasting_effect Lasting_effect;
typedef struct Armour Armour;
//Being
typedef struct Weapon Weapon;
typedef struct Being_type Being_type;
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
typedef struct Block_times Block_times;
typedef struct Player Player;
typedef struct Players Players;

//Game
typedef struct Game_data Game_data;
typedef struct Box_element Box_element;
typedef struct Box Box;
typedef struct Boxes Boxes;

//other
struct Placement{
	SDL_FPoint position;
	float direction;
};
struct Impact{
	float damage;
	float armour_reduction;
	float magic;
	float stun;
};
struct Blade{
	Placement placement;
	Impact impact;
	unsigned int hit_targets[MAX_HITS];
	Uint8 penetration;
	Uint8 hits;
	Placement step_shift;
	int key;
	int steps;
	int step;
	unsigned int chain;
	unsigned int chain_next;
	bool abide;
	bool freehand;
	unsigned int idle_ticks;
	float charge;	
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
	// Uint32 render_flags;
};
struct Lasting_effect{
	unsigned int id;
	int ticks_left;
};
struct Armour{
	float absorption;
	float multipl;
	float magic_multipl;
	float unstability;
};
//Being
struct Being_type{
	float size;
    float velocity;
    int hit_points;
	Armour armour;
	Impact impact;
	void (*update)(Being* const, Game_data* const);
};
struct Weapon{
	Placement placement;
	Impact impact;
};
struct Being{
    SDL_FPoint position;
	float direction;
    Segment* segment;
	unsigned int indx;
    float velocity;
    int hit_points;
	unsigned int type_id;
	unsigned int target_indx;
	int status;
	int status_ticks_left;
	Armour armour;
	Weapon weapon;
	SDL_FPoint special_move_shift;
	float special_rotation_shift;
	unsigned int id;
	unsigned int effects_num;
	Lasting_effect effects[MAX_BEING_EFFECTS];
};
struct Beings_array{
	Being* array;
    unsigned int num;
};
struct Segment_beings{
	Being* array[MAX_SEGM_BEINGS];
    Uint8 num;
};
//Projectile
struct Projectile{
	SDL_FPoint position;
	SDL_FPoint shift_per_tick;
	unsigned int type_id;
	union data{
		struct penetrating{
			unsigned int hit_targets[MAX_HITS];
			Impact impact;
			Uint8 penetration;
			Uint8 hits;
		}penetrating;
		struct basic{
			Impact impact;
		}basic;
		struct special{
			unsigned int effect_id;
		}special;
	}data;
};
struct Projectiles_array{
	Projectile* array;
	unsigned int num;
};
//World
struct Segment{
	Segment_beings beings;
	Segment_beings ally_beings;
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
struct Block_times{
	int fatigue;
	int shoot;
	int push;
	int dodge;
	int cast;
};
struct Player{
	Uint32 flags;
	SDL_FPoint position;
	Blade blade;
	Impact blade_attack;
	Impact range_attack;
	Segment* segment;
	float direction;
	float move_direction;
	float velocity;
	float max_velocity;
	int hit_points;
	int fatigue_points;
	int magic_points;
	int max_h_p;
	int max_fatigue;
	Block_times block_times;
	Armour armour;
	int coins;
	unsigned int selected_scroll;
	unsigned int scrolls[SCROLLS_NUM];
	unsigned int scrolls_quick_access[QUICK_SCROLLS];
	unsigned int effects_num;
	Lasting_effect effects[MAX_PC_EFFECTS];
};
struct Players{
	Player* array;
	unsigned int num;
};
//Game
struct Box_element{
	int type;
	unsigned int value;
};
struct Box{
	SDL_FPoint location;
	Box_element elements[BOX_SLOTS];
};
struct Boxes{
	Box* array;
	unsigned int num;
};
struct Game_data{
	Players champions;
	Beings_array beings;
	Projectiles_array projectiles;
	World world;
	Boxes boxes;
	unsigned int human_indx;
	unsigned int keys;
	unsigned int needed_keys;
	int enemy_morale;
	unsigned int effects_num;
	Lasting_effect effects[MAX_GAME_EFFECTS];
};

#endif