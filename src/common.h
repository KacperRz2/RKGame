#ifndef COMMON_H_
#define COMMON_H_

typedef struct Impact Impact;
typedef struct Armour Armour;
typedef struct Lasting_effect Lasting_effect;
typedef struct Being Being;
typedef struct Player Player;
typedef struct Game_data Game_data;
typedef struct Render_data Render_data;
typedef struct Blade Blade;
typedef struct Beings_array Beings_array;
typedef struct Projectiles_array Projectiles_array;
typedef struct Players Players;

#include <SDL3/SDL.h>

struct Impact{
    float damage;
    float armour_reduction;
    float magic;
    float stun;
};
struct Armour{
    float absorption;
    float multipl;
    float magic_multipl;
    float unstability;
};
struct Lasting_effect{
    Uint8 id;
    int ticks_left;
};

#include <macros.h>
#include <World.h>
#include <render.h>
#include <Projectile.h>
#include <Scroll.h>
#include <Player.h>
#include <Being.h>
#include <game.h>

#endif
