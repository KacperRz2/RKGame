#ifndef WORLD_H_
#define WORLD_H_

void CreateWorld(Game_data* const, const float, const float);
void DestroyWorld(World* const);
Segment* GetSegment(const World* const, const float, const float);
Segment* GetSegmentByIndx(const World* const, const unsigned int, const unsigned int);
Segment* GetSegmentByIndxSafe(const World* const, const int, const int);
void StartLevel(Game_data* const);
static SDL_FPoint GetStartPosition(World* const);
float SegmentPositionX(Segment* const);
float SegmentPositionY(Segment* const);
static float GetDoorPositionXorY(const unsigned int);
bool IsClearSight(const SDL_FPoint* const, const SDL_FPoint* const, const Segment* const, const World* const);
bool IsClearSightWithKnownDistance(const SDL_FPoint* const, const Segment* const, const World* const, const float, const float, const float);
bool IsClearPathWithKnownDistance(const SDL_FPoint* const, const Segment* const, const World* const, const float, const float, const float);

#endif