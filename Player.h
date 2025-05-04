#ifndef PLAYER_H_
#define PLAYER_H_

Player* CreatePlayer(const float, const float);
void MovePlayer(Player*, const float, const float);
void SetPlayerInBounds(Player*);
void UpdatePlayerMove(Player*);
void UpdatePlayerDirection(Player*);
void DestroyPlayer(Player*);
void UpdatePlayerPoints(Player*);
void UpdatePlayer(Player*, Projectiles_array*);

#endif