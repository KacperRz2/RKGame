#ifndef PROJECTILE_H_
#define PROJECTILE_H_

void DestroyProjectiles(Projectiles_array* const);
static bool ProjectileHitsBeing(Projectile* const, Being* const);
void AddProjectileToArray(Projectiles_array* const, const SDL_FPoint* const, const float, const float, const int, const unsigned int);
static void DestroyProjectileInArray(Projectiles_array* const, const unsigned int);
static void MoveProjectile(Projectile* const);
void UpdateProjectiles(Game_data* const, Segment* const);

void DestroyHProjectiles(Projectiles_h_array* const);
static bool ProjectileHitsPlayerOrLost(Projectile_hostile* const, Player* const);
void AddHProjectileToArray(Projectiles_h_array* const, const SDL_FPoint* const, const float, const float, const int);
static void DestroyHProjectileInArray(Projectiles_h_array* const, const unsigned int);
static void MoveHProjectile(Projectile_hostile* const);
void UpdateHProjectiles(Game_data* const);

#endif