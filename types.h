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
	Uint16 hit_targets[MAX_HITS];
	Uint8 penetration;
	Uint8 hits;
	Placement step_shift;
	Sint8 key;
	Sint16 steps;
	Sint16 step;
	Uint8 chain;
	Uint8 chain_next;
	bool abide;
	bool freehand;
	Uint16 idle_ticks;
	float charge;	
};
struct Render_data{
	float viewfinder;
	SDL_FRect visible_rect;
	SDL_Rect viewfinder_rect;
	Uint16 window_w;
	Uint16 window_h;
	float sin_player_direction;
	float cos_player_direction;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* textures[TEXTURES_NUM];
};
struct Lasting_effect{
	Uint8 id;
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
struct Being{
    SDL_FPoint position;
	float direction;
    float velocity;
    int hit_points;
    Segment* segment;
	union target{
		Player* player;
		Being* being;
	}target;
	Uint8 indx;
	Uint8 type_id;
	Sint8 status;
	int status_ticks_left;
	Armour armour;
	Impact impact;
	SDL_FPoint special_move_shift;
	struct rend_fly_help_data{
		_Float16 start_angle;
		Sint16 ticks;
	}rend_fly_help_data;
	Uint16 main_indx;
	Uint8 effects_num;
	Lasting_effect effects[MAX_BEING_EFFECTS];
};
struct Beings_array{
	Being* array;
	Uint16* indices;
    Uint16 num;
};
struct Segment_beings{
	Uint16 beings_ind[MAX_SEGM_BEINGS];
    Uint8 num;
};
//Projectile
struct Projectile{
	SDL_FPoint position;
	SDL_FPoint shift_per_tick;
	Uint8 type_id;
	union data{
		struct penetrating{
			Uint16 hit_targets[MAX_HITS];
			Impact impact;
			Uint8 penetration;
			Uint8 hits;
		}penetrating;
		struct basic{
			Impact impact;
		}basic;
		struct special{
			Uint8 effect_id;
		}special;
	}data;
};
struct Projectiles_array{
	Projectile* array;
	Uint16 num;
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
	Sint16 fatigue;
	Sint16 shoot;
	Sint16 push;
	Sint16 dodge;
	Sint16 cast;
};
struct Player{
	Uint32 flags;
	SDL_FPoint position;
	Blade blade;
	Impact blade_attack;
	Impact range_attack;
	Segment* segment;
	Segment* last_seen_in;
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
	Uint8 selected_scroll;
	Uint8 scrolls[SCROLLS_NUM];
	Uint8 scrolls_quick_access[QUICK_SCROLLS];
	Uint8 effects_num;
	Lasting_effect effects[MAX_PC_EFFECTS];
};
struct Players{
	Player* array;
	Uint8 num;
};
//Game
struct Box_element{
	Uint8 type;
	Uint16 value;
};
struct Box{
	SDL_FPoint location;
	Box_element elements[BOX_SLOTS];
};
struct Boxes{
	Box* array;
	Uint16 num;
};
struct Game_data{
	Players champions;
	Beings_array beings;
	Projectiles_array projectiles;
	World world;
	Boxes boxes;
	Uint8 human_indx;
	Uint8 keys;
	Uint8 needed_keys;
	Sint16 enemy_morale;
	Uint8 effects_num;
	Lasting_effect effects[MAX_GAME_EFFECTS];
};
// int a = sizeof(Segment);
#endif