#ifndef WORLD_H_
#define WORLD_H_

World* createWorld(const float, const float);
inline void destroyWorld(World*);
inline Segment* getSegment(World*, const float, const float);
void setVisibleRect(World*, Player*);

#endif