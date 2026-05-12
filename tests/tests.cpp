#include <gtest/gtest.h>
#include <SDL3/SDL.h>

extern "C" {
    #include "../src/function.h"
    #include "../src/macros.h"
}

TEST(RadToDegTest, CorrectResult){
    EXPECT_FLOAT_EQ(RadToDeg(SDL_PI_F * 2.0F), 360.0F);
    EXPECT_FLOAT_EQ(RadToDeg(SDL_PI_F), 180.0F);
    EXPECT_FLOAT_EQ(RadToDeg(SDL_PI_F * 0.5F), 90.0F);
    EXPECT_FLOAT_EQ(RadToDeg(SDL_PI_F * 0.25F), 45.0F);
    EXPECT_FLOAT_EQ(RadToDeg(0.0F), 0.0F);
    EXPECT_FLOAT_EQ(RadToDeg(-SDL_PI_F * 2.0F), -360.0F);
    EXPECT_FLOAT_EQ(RadToDeg(-SDL_PI_F * 0.5F), -90.0F);
    EXPECT_FLOAT_EQ(RadToDeg(SDL_PI_F * 3.0F), 540.0F);
}

TEST(SineUnsafeTest, CorrectResult){
    SetSineCosineArrays();
    EXPECT_NEAR(SineUnsafe(SDL_PI_F), 0.0F, MIN_ANGLE);
    EXPECT_NEAR(SineUnsafe(SDL_PI_F * 0.75F), SDL_sinf(SDL_PI_F * 0.75F), MIN_ANGLE);
    EXPECT_NEAR(SineUnsafe(SDL_PI_F * 0.5F), 1.0F, MIN_ANGLE);
    EXPECT_NEAR(SineUnsafe(SDL_PI_F * 0.25F), SDL_sinf(SDL_PI_F * 0.25F), MIN_ANGLE);
    EXPECT_FLOAT_EQ(SineUnsafe(0.0F), 0.0F);
}

TEST(SineUnsafeDeathTest, BadArgument){
    SetSineCosineArrays();
    EXPECT_DEATH(SineUnsafe(-SDL_PI_F * 2.0F), ".*");
    EXPECT_DEATH(SineUnsafe(-SDL_PI_F * 0.5F), ".*");
}

TEST(SineSafeTest, CorrectResult){
    SetSineCosineArrays();
    EXPECT_FLOAT_EQ(SineSafe(SDL_PI_F * 2.0F), 0.0F);
    EXPECT_NEAR(SineSafe(SDL_PI_F), 0.0F, MIN_ANGLE);
    EXPECT_NEAR(SineSafe(SDL_PI_F * 0.75F), SDL_sinf(SDL_PI_F * 0.75F), MIN_ANGLE);
    EXPECT_NEAR(SineSafe(SDL_PI_F * 0.5F), 1.0F, MIN_ANGLE);
    EXPECT_NEAR(SineSafe(SDL_PI_F * 0.25F), SDL_sinf(SDL_PI_F * 0.25F), MIN_ANGLE);
    EXPECT_FLOAT_EQ(SineSafe(0.0F), 0.0F);
    EXPECT_FLOAT_EQ(SineSafe(-SDL_PI_F * 2.0F), 0.0F);
    EXPECT_NEAR(SineSafe(-SDL_PI_F * 0.5F), -1.0F, MIN_ANGLE);
    EXPECT_NEAR(SineSafe(SDL_PI_F * 3.0F), 0.0F, MIN_ANGLE);
}

TEST(CosiUnsafeTest, CorrectResult){
    SetSineCosineArrays();
    EXPECT_NEAR(CosiUnsafe(SDL_PI_F), -1.0F, MIN_ANGLE);
    EXPECT_NEAR(CosiUnsafe(SDL_PI_F * 0.75F), SDL_cosf(SDL_PI_F * 0.75F), MIN_ANGLE);
    EXPECT_NEAR(CosiUnsafe(SDL_PI_F * 0.5F), 0.0F, MIN_ANGLE);
    EXPECT_NEAR(CosiUnsafe(SDL_PI_F * 0.25F), SDL_cosf(SDL_PI_F * 0.25F), MIN_ANGLE);
    EXPECT_FLOAT_EQ(CosiUnsafe(0.0F), 1.0F);
}

TEST(CosiUnsafeDeathTest, BadArgument){
    SetSineCosineArrays();
    EXPECT_DEATH(CosiUnsafe(-SDL_PI_F * 2.0F), ".*");
    EXPECT_DEATH(CosiUnsafe(-SDL_PI_F * 0.5F), ".*");
}

TEST(CosiSafeTest, CorrectResult){
    SetSineCosineArrays();
    EXPECT_FLOAT_EQ(CosiSafe(SDL_PI_F * 2.0F), 1.0F);
    EXPECT_NEAR(CosiSafe(SDL_PI_F), -1.0F, MIN_ANGLE);
    EXPECT_NEAR(CosiSafe(SDL_PI_F * 0.75F), SDL_cosf(SDL_PI_F * 0.75F), MIN_ANGLE);
    EXPECT_NEAR(CosiSafe(SDL_PI_F * 0.5F), 0.0F, MIN_ANGLE);
    EXPECT_NEAR(CosiSafe(SDL_PI_F * 0.25F), SDL_cosf(SDL_PI_F * 0.25F), MIN_ANGLE);
    EXPECT_FLOAT_EQ(CosiSafe(0.0F), 1.0F);
    EXPECT_FLOAT_EQ(CosiSafe(-SDL_PI_F * 2.0F), 1.0F);
    EXPECT_NEAR(CosiSafe(-SDL_PI_F * 0.5F), 0.0F, MIN_ANGLE);
    EXPECT_NEAR(CosiSafe(SDL_PI_F * 3.0F), -1.0F, MIN_ANGLE);
}

struct arctan2Params{
    float a, b;
};

class arctan2Test : public testing::TestWithParam<arctan2Params>{};

TEST_P(arctan2Test, CorrectResult){
    arctan2Params p = GetParam();
    EXPECT_NEAR(arctan2(p.a, p.b), SDL_atan2f(p.a, p.b), SDL_PI_F * 0.125F);
}

INSTANTIATE_TEST_SUITE_P(
    arctan2Tests,
    arctan2Test,
    testing::Values(
        arctan2Params{8.0F, 8.0F},
        arctan2Params{-8.0F, -8.0F},
        arctan2Params{8.0F, -8.0F},
        arctan2Params{-8.0F, 8.0F},
        arctan2Params{8.0F, 0.0F},
        arctan2Params{0.0F, 8.0F},
        arctan2Params{-8.0F, 0.0F},
        arctan2Params{0.0F, -8.0F},
        arctan2Params{8.0F, 16.0F},
        arctan2Params{16.0F, 8.0F},
        arctan2Params{-8.0F, 16.0F},
        arctan2Params{16.0F, -8.0F},
        arctan2Params{8.0F, -16.0F},
        arctan2Params{-16.0F, 8.0F},
        arctan2Params{-8.0F, -16.0F},
        arctan2Params{-16.0F, -8.0F},
        arctan2Params{0.08F, -16000.0F},
        arctan2Params{-16000.0F, 0.08F},
        arctan2Params{-0.08F, -16000.0F},
        arctan2Params{-16000.0F, -0.08F}
    )
);

struct GetShiftParams{
    SDL_FPoint position, destination;
    float velocity, expect_x, expect_y;
};

class GetShiftTest : public testing::TestWithParam<GetShiftParams>{};

TEST_P(GetShiftTest, CorrectResult){
    GetShiftParams p = GetParam();
    float x, y;
    GetShift(&p.position, &p.destination, p.velocity, &x, &y);
    EXPECT_NEAR(x, p.expect_x, 0.001F);
    EXPECT_NEAR(y, p.expect_y, 0.001F);
}

INSTANTIATE_TEST_SUITE_P(
    GetShiftTests,
    GetShiftTest,
    testing::Values(
        GetShiftParams{
            {1.0F, 2.0F}, {3.0F, 4.0F},
            1.0F, 0.7071F, 0.7071F
        },
        GetShiftParams{
            {-1.0F, -2.0F}, {-3.0F, -4.0F},
            1.0F, -0.7071F, -0.7071F
        },
        GetShiftParams{
            {-1.0F, 2.0F}, {3.0F, -4.0F},
            1.0F, 0.5547F, -0.8321F
        },
        GetShiftParams{
            {1.0F, -2.0F}, {-3.0F, 4.0F},
            1.0F, -0.5547F, 0.8321F
        },
        GetShiftParams{
            {-1.0F, 2.0F}, {-3.0F, 4.0F},
            1.0F, -0.7071F, 0.7071F
        },
        GetShiftParams{
            {1.0F, -2.0F}, {3.0F, -4.0F},
            1.0F, 0.7071F, -0.7071F
        },
        GetShiftParams{
            {1.0F, 2.0F}, {3.0F, 4.0F},
            123.0F, 86.9741F, 86.9741F
        },
        GetShiftParams{
            {-1.0F, -2.0F}, {-3.0F, -4.0F},
            123.0F, -86.9741F, -86.9741F
        },
        GetShiftParams{
            {-1.0F, 2.0F}, {3.0F, -4.0F},
            123.0F, 68.2281F, -102.3422F
        },
        GetShiftParams{
            {1.0F, -2.0F}, {-3.0F, 4.0F},
            123.0F, -68.2281F, 102.3422F
        },
        GetShiftParams{
            {-1.0F, 2.0F}, {-3.0F, 4.0F},
            123.0F, -86.9741F, 86.9741F
        },
        GetShiftParams{
            {1.0F, -2.0F}, {3.0F, -4.0F},
            123.0F, 86.9741F, -86.9741F
        }
    )
);

struct GetShiftFromAngleParams{
    float angle, velocity, expect_x, expect_y;
};

class GetShiftFromAngleTest : public testing::TestWithParam<GetShiftFromAngleParams>{};

TEST_P(GetShiftFromAngleTest, CorrectResult){
    SetSineCosineArrays();
    GetShiftFromAngleParams p = GetParam();
    float x, y;
    GetShiftFromAngle(p.angle, p.velocity, &x, &y);
    EXPECT_NEAR(x, p.expect_x, MIN_ANGLE * p.velocity);
    EXPECT_NEAR(y, p.expect_y, MIN_ANGLE * p.velocity);
}

INSTANTIATE_TEST_SUITE_P(
    GetShiftFromAngleTests,
    GetShiftFromAngleTest,
    testing::Values(
        GetShiftFromAngleParams{SDL_PI_F * 0.5F, 1.0F, 1.0F, 0.0F},
        GetShiftFromAngleParams{SDL_PI_F * 0.25F, 1.0F, 0.7071F, -0.7071F},
        GetShiftFromAngleParams{SDL_PI_F * 0.75F, 1.0F, 0.7071F, 0.7071F},
        GetShiftFromAngleParams{SDL_PI_F * 0.0F, 1.0F, 0.0F, -1.0F},
        GetShiftFromAngleParams{SDL_PI_F * 1.0F, 1.0F, 0.0F, 1.0F},
        GetShiftFromAngleParams{SDL_PI_F * 1.25F, 1.0F, -0.7071F, 0.7071F},
        GetShiftFromAngleParams{SDL_PI_F * 1.456F, 1.0F, -0.9905F, 0.1378F},
        GetShiftFromAngleParams{SDL_PI_F * 0.5F, 123.0F, 123.0F, 0.0F},
        GetShiftFromAngleParams{SDL_PI_F * 0.25F, 123.0F, 86.9741F, -86.9741F},
        GetShiftFromAngleParams{SDL_PI_F * 0.75F, 123.0F, 86.9741F, 86.9741F},
        GetShiftFromAngleParams{SDL_PI_F * 0.0F, 123.0F, 0.0F, -123.0F},
        GetShiftFromAngleParams{SDL_PI_F * 1.0F, 123.0F, 0.0F, 123.0F},
        GetShiftFromAngleParams{SDL_PI_F * 1.456F, 123.0F, -121.8267F, 16.9482F}
    )
);

TEST(GetDistanceSquaredTest, ZeroForSamePoints){
    SDL_FPoint a = {0.0F, 0.0F};
    SDL_FPoint b = {0.0F, 0.0F};
    EXPECT_EQ(GetDistanceSquared(&a, &b), 0.0F);
    a = {1.0F, 1.0F};
    b = {1.0F, 1.0F};
    EXPECT_EQ(GetDistanceSquared(&a, &b), 0.0F);
    a = {-1.0F, -1.0F};
    b = {-1.0F, -1.0F};
    EXPECT_EQ(GetDistanceSquared(&a, &b), 0.0F);
}

TEST(GetDistanceSquaredTest, PositiveResult){
    SDL_FPoint a = {1.0F, 2.0F};
    SDL_FPoint b = {3.0F, 4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0.0F);
    a = {-1.0F, -2.0F};
    b = {-3.0F, -4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0.0F);
    a = {-1.0F, 2.0F};
    b = {3.0F, -4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0.0F);
    a = {1.0F, -2.0F};
    b = {-3.0F, 4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0.0F);
    a = {-1.0F, 2.0F};
    b = {-3.0F, 4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0.0F);
    a = {1.0F, -2.0F};
    b = {3.0F, -4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0.0F);
}

TEST(GetDistanceSquaredTest, CorrectResult){
    SDL_FPoint a = {1.0F, 2.0F};
    SDL_FPoint b = {3.0F, 4.0F};
    EXPECT_FLOAT_EQ(GetDistanceSquared(&a, &b), 8.0F);
    a = {-1.0F, -2.0F};
    b = {-3.0F, -4.0F};
    EXPECT_FLOAT_EQ(GetDistanceSquared(&a, &b), 8.0F);
    a = {-1.0F, 2.0F};
    b = {3.0F, -4.0F};
    EXPECT_FLOAT_EQ(GetDistanceSquared(&a, &b), 52.0F);
    a = {1.0F, -2.0F};
    b = {-3.0F, 4.0F};
    EXPECT_FLOAT_EQ(GetDistanceSquared(&a, &b), 52.0F);
    a = {-1.0F, 2.0F};
    b = {-3.0F, 4.0F};
    EXPECT_FLOAT_EQ(GetDistanceSquared(&a, &b), 8.0F);
    a = {1.0F, -2.0F};
    b = {3.0F, -4.0F};
    EXPECT_FLOAT_EQ(GetDistanceSquared(&a, &b), 8.0F);
}
