#include <gtest/gtest.h>
#include "test_utils.h"

class FindLineTest : public ::testing::Test
{
protected:
    CTester tester;
};

TEST_F(FindLineTest, EmptyPoints)
{
    EXPECT_EQ(tester.findLine(), 0.0);
}

TEST_F(FindLineTest, Triangle)
{
    tester.points = TestData::triangle();
    double result = tester.findLine();
    EXPECT_GT(result, -1e8);
    EXPECT_TRUE(tester.simCiclePath(result));
}

TEST_F(FindLineTest, Hexagon)
{
    tester.points = TestData::hexagon();
    double result = tester.findLine();
    EXPECT_GT(result, -1e8);
}
