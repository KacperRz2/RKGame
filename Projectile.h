#ifndef PROJECTILE_H_
#define PROJECTILE_H_

Projectile* createProjectile(const SDL_FPoint*, const float, const float);
inline void destroyProjectile(Projectile*);
void destroyProjectiles(Projectiles_array*);
inline bool projectileHitsBeing(Projectile*, Being*);
inline void addProjectileToArray(Projectiles_array*, Projectile*);
inline void destroyProjectileInArray(Projectiles_array*, const unsigned int);
inline void moveProjectile(Projectile*);
void updateProjectiles(World*, Projectiles_array*);

#endif