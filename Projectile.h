#ifndef PROJECTILE_H_
#define PROJECTILE_H_

Projectile* createProjectile(const SDL_FPoint*, const float, const float);
void destroyProjectile(Projectile*);
void destroyProjectiles(Projectiles_array*);
bool projectileHitsBeing(Projectile*, Being*);
void addProjectileToArray(Projectiles_array*, Projectile*);
void destroyProjectileInArray(Projectiles_array*, const unsigned int);
void moveProjectile(Projectile*);
void updateProjectiles(World*, Projectiles_array*);

#endif