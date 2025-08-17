#ifndef WORLD_H_
#define WORLD_H_

void CreateWorld(World* const, const float, const float);
void DestroyWorld(World* const);
Segment* GetSegment(World* const, const float, const float);
Segment* GetSegmentByIndx(World* const, const unsigned int, const unsigned int);
void UpdateSegmentBeingPointer(Being*, Being*);
void StartLevel(Game_data* const);
static SDL_FPoint GetStartPosition(World* const);
float SegmentPositionX(Segment* const);
float SegmentPositionY(Segment* const);
static float GetDoorPositionXorY(const unsigned int);

#endif