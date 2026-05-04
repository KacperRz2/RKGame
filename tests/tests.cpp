#include <gtest/gtest.h>
#include <SDL3/SDL.h>

extern "C" {
    #include "../src/function.h"
    #include "../src/macros.h"
}

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
