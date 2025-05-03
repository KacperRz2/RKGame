#ifndef PLAYER_H_
#define PLAYER_H_

Player* createPlayer(const float, const float);
inline void movePlayer(Player*, const float, const float);
inline void setPlayerInBounds(Player*);
void updatePlayerMove(Player*);
inline void updatePlayerDirection(Player*);
inline void destroyPlayer(Player*);
inline void updatePlayerPoints(Player*);
inline void updatePlayer(Player*, Projectiles_array*);

#endif