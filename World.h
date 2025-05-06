#ifndef WORLD_H_
#define WORLD_H_

World* CreateWorld(const float, const float);
void DestroyWorld(World*);
Segment* GetSegment(World*, const float, const float);
void SetVisibleRect(World*, Player*, const float, const float, const float, const float);

#endif