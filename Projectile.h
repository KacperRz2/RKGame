#ifndef PROJECTILE_H_
#define PROJECTILE_H_

void DestroyProjectiles(Projectiles_array*);
bool ProjectileHitsBeing(Projectile*, Being*);
void AddProjectileToArray(Projectiles_array*, const SDL_FPoint* const, const float, const float, const int, const unsigned int);
void DestroyProjectileInArray(Projectiles_array*, const unsigned int);
void MoveProjectile(Projectile*);
void UpdateProjectiles(Projectiles_array*, Segment*);

#endif