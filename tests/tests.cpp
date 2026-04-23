#include <gtest/gtest.h>
#include <SDL3/SDL.h>

extern "C" {
    #include "../src/function.h"
}

TEST(GetDistanceSquaredTest, ZeroForSamePoints) {
    SDL_FPoint a = {0.0F, 0.0F};
    SDL_FPoint b = {0.0F, 0.0F};
    EXPECT_EQ(GetDistanceSquared(&a, &b), 0);
    a = {1.0F, 1.0F};
    b = {1.0F, 1.0F};
    EXPECT_EQ(GetDistanceSquared(&a, &b), 0);
    a = {-1.0F, -1.0F};
    b = {-1.0F, -1.0F};
    EXPECT_EQ(GetDistanceSquared(&a, &b), 0);
}

TEST(GetDistanceSquaredTest, PositiveResult) {
    SDL_FPoint a = {1.0F, 2.0F};
    SDL_FPoint b = {3.0F, 4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0);
    a = {-1.0F, -2.0F};
    b = {-3.0F, -4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0);
    a = {-1.0F, 2.0F};
    b = {3.0F, -4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0);
    a = {1.0F, -2.0F};
    b = {-3.0F, 4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0);
    a = {-1.0F, 2.0F};
    b = {-3.0F, 4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0);
    a = {1.0F, -2.0F};
    b = {3.0F, -4.0F};
    EXPECT_GT(GetDistanceSquared(&a, &b), 0);
}
