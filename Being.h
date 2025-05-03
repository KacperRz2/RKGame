#ifndef BEIGN_H_
#define BEIGN_H_

inline unsigned int addBeingToSegment(Segment*, Being*);
Being* createBeing(World*, const float, const float);
inline void removeBeingFromSegment(Being*);
inline void destroyBeing(Being*);
void destroyBeings(Beings_array*);
inline void addBeingToArray(Beings_array*, Being*);
inline void destroyBeingInArray(Beings_array*, const unsigned int);
inline void moveBeing(Being*, const float, const float);
inline void setBeingPosition(Being*, const float, const float);
inline void moveBeingX(Being*, const float);
inline void moveBeingY(Being*, const float);
inline void moveBeingToSegment(Being*, Segment*);
inline void startBeingWalk(Being*, const int, const float, const float);
void startBeingWalkWithRandTurn(Being*, const unsigned int, const float, const float);
inline void startBeingWalkWithRandTurn45Deg(Being*, const unsigned int, const float, const float);
bool resolveBeingCollisionInNewSegment(Being*, Segment*, float*, float*, const float, const float);
inline void turnBeingWalk(Being*);
void updateBeingWalk(Being*);
void updateBeings(Beings_array*, World*, SDL_FPoint*);
inline void damageBeing(Being*, int);

#endif