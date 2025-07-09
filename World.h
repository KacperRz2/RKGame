#ifndef WORLD_H_
#define WORLD_H_

extern World* const world;

void CreateWorld(const float, const float);
void DestroyWorld();
Segment* GetSegment(const float, const float);
Segment* GetSegmentByIndx(const unsigned int, const unsigned int);
void SetSineCosine(Player*);

#endif