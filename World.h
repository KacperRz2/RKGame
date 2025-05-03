#ifndef WORLD_H_
#define WORLD_H_

World* createWorld(const float, const float);
void destroyWorld(World*);
Segment* getSegment(World*, const float, const float);
void setVisibleRect(World*, Player*);

#endif