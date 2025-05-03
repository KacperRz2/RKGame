#ifndef PLAYER_H_
#define PLAYER_H_

Player* createPlayer(const float, const float);
void movePlayer(Player*, const float, const float);
void setPlayerInBounds(Player*);
void updatePlayerMove(Player*);
void updatePlayerDirection(Player*);
void destroyPlayer(Player*);
void updatePlayerPoints(Player*);
void updatePlayer(Player*, Projectiles_array*);

#endif