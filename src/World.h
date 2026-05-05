#ifndef WORLD_H_
#define WORLD_H_

typedef struct Placement Placement;
typedef struct position16 position16;
typedef struct Segment_beings Segment_beings;
typedef struct Key_location Key_location;
typedef struct Segment Segment;
typedef struct Shop Shop;
typedef struct World World;

enum segment_flags{
    segment_in_sight = 1 << 0,
    segment_known = 1 << 1
};
enum key_info{
    key_location_unknown,
    key_location_known,
    key_owned
};

struct Placement{
    SDL_FPoint position;
    float direction;
};
struct position16{
    _Float16 x;
    _Float16 y;
};
struct Segment_beings{
    Uint16 beings_ind[MAX_SEGM_BEINGS];
    Uint8 num;
};
struct Key_location{
    Uint8 x;
    Uint8 y;
};
struct Segment{
    Uint8 flags;
    Segment_beings beings;
    Segment_beings ally_beings;
    struct coordinates {Uint16 x; Uint16 y;} indx;
};
struct Shop{
    SDL_FPoint location;
    Uint8 scrolls[SHOP_SCROLLS_NUM];
};
struct World{
    Segment*** segments;
    Uint64 plan[BIG_SEGMENTS_X];
    SDL_FPoint portalA;
    SDL_FPoint portalB;
    SDL_FPoint door;
    Key_location* key_locations;
    Shop shops[SHOPS_NUM];
};

unsigned int GetBigSegCoordinate(const float);
bool IsInUncoveredBigSeg(const Uint64* const, const unsigned int, const unsigned int);
bool IsInNoticedBigSeg(const Uint64* const, const unsigned int, const unsigned int);
void UncoverBigSeg(Uint64* const, const unsigned int, const unsigned int);
void MarkAsNoticedBigSeg(Uint64* const, const unsigned int, const unsigned int);
bool IsVoidBigSeg(const Uint64* const, const unsigned int, const unsigned int);
void CreateWorld(Game_data* const);
static void PlaceBoxes(Game_data* const);
static void PlaceShops(World* const);
void DestroyWorld(World* const);
Segment* GetSegmentUnsafe(const World* const, const float, const float);
Segment* GetSegmentSafe(const World* const, const float, const float);
Segment* GetSegmentByIndxUnsafe(const World* const, const unsigned int, const unsigned int);
Segment* GetSegmentByIndxSafe(const World* const, const unsigned int, const unsigned int);
void StartNewLevel(Game_data* const);
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
