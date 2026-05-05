#ifndef PROJECTILE_H_
#define PROJECTILE_H_

typedef struct Projectile Projectile;

enum projectile_type{
    projectile_penetrat,
    projectile_hostile,
    projectile_special
};
enum special_projectile_effect{
    projectile_warlock,
    projectile_fire
};

struct Projectile{
    SDL_FPoint position;
    SDL_FPoint shift_per_tick;
    Uint8 type_id;
    union{
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
            unsigned int ticks;
        }special;
    };
};

void DestroyProjectiles(Projectiles_array* const);
static bool ProjectileHitsBeing(Projectile* const, Being* const);
void AddPCProjectileToArray(Projectiles_array* const, const SDL_FPoint* const, const float, const float, const Impact* const, const Uint8);
void AddHProjectileToArray(Projectiles_array* const, const SDL_FPoint* const, const float, const float, const Impact* const);
void AddSpecialProjectileToArray(Projectiles_array* const, const SDL_FPoint* const, const float, const float, const Uint8, const unsigned int);
static void DestroyProjectileInArray(Projectiles_array* const, const unsigned int);
static void MoveProjectile(Projectile* const);
void UpdateProjectiles(Game_data* const);
static bool ProjectileHitsPlayer(Projectile* const, Game_data* const, const unsigned int);

static bool UpdatePCProjectile(Projectile* const, Game_data* const);
static bool UpdateHostileProjectile(Projectile* const, Game_data* const);
static bool UpdateSpecialProjectile(Projectile* const, Game_data* const);

static bool WarlockProjectile(Projectile* const, Game_data* const);
static bool FireProjectile(Projectile* const, Game_data* const);

#endif
