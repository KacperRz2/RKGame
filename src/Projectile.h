#ifndef PROJECTILE_H_
#define PROJECTILE_H_

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