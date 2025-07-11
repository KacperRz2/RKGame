#ifndef PROJECTILE_H_
#define PROJECTILE_H_

void DestroyProjectiles(Projectiles_array* const);
bool ProjectileHitsBeing(Projectile* const, Being* const);
void AddProjectileToArray(Projectiles_array* const, const SDL_FPoint* const, const float, const float, const int, const unsigned int);
void DestroyProjectileInArray(Projectiles_array* const, const unsigned int);
void MoveProjectile(Projectile* const);
void UpdateProjectiles(Projectiles_array* const, Segment* const);

#endif