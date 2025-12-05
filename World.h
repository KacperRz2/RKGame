#ifndef WORLD_H_
#define WORLD_H_

unsigned int GetBigSegCoordinate(const float);
bool IsInUncoveredBigSeg(const Uint64* const, const unsigned int, const unsigned int);
bool IsInPopulatedBigSeg(const Uint64* const, const unsigned int, const unsigned int);
void UncoverBigSeg(Uint64* const, const unsigned int, const unsigned int);
void MarkAsPopulatedBigSeg(Uint64* const, const unsigned int, const unsigned int);
bool IsVoidBigSeg(const Uint64* const, const unsigned int, const unsigned int);
void CreateWorld(Game_data* const);
static void PlaceBoxes(Game_data* const);
static void PlaceShops(World* const);
void DestroyWorld(World* const);
Segment* GetSegmentUnsafe(const World* const, const float, const float);
Segment* GetSegmentByIndxUnsafe(const World* const, const unsigned int, const unsigned int);
Segment* GetSegmentByIndxSafe(const World* const, const unsigned int, const unsigned int);
void StartLevel(Game_data* const);
static SDL_FPoint GetStartPosition(World* const);
float SegmentPositionX(const Segment* const);
float SegmentPositionY(const Segment* const);
float SegmentCenterX(const Segment* const);
float SegmentCenterY(const Segment* const);
float BigSegPosition(const unsigned int);
float BigSegCenter(const unsigned int);
static float SegPosition(const unsigned int);
static unsigned int HugeSegCenterBigSegCoordinate(const unsigned int);
static float GetDoorPositionXorY(const unsigned int);
bool IsClearSight(const SDL_FPoint* const, const SDL_FPoint* const, const Segment* const, const World* const);
bool IsClearSightWithKnownDistance(const SDL_FPoint* const, const Segment* const, const World* const, const float, const float, const float);
bool SegmentInSight(const SDL_FPoint* const, const SDL_FPoint* const, const Segment* const, const World* const);
static void FillBoxes(Game_data* const);
void GetNeighbourSegments(Segment** const, const World* const, const Segment* const);
void GetNeighbourSegmentsFar(Segment** const, const World* const, const Segment* const, const int);
void Get4NearestSegments(Segment** const, const World* const, const Segment* const, const float, const float);
void Get2NearestSegments(Segment** const, const World* const, Segment* const, const float, const float);
void GetSurroundingSegmentsFar(Segment** const, const World* const, const Segment* const, const int);
Segment* GetDistantSegmentBySpiral(const World* const, const Segment* const, const int);

#endif