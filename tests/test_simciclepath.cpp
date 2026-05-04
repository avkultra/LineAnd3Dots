#include <gtest/gtest.h>
#include "test_utils.h"

class SimCiclePathTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        tester.startId = 0;
    }

    CTester tester;
};

TEST_F(SimCiclePathTest, EmptyPoints)
{
    EXPECT_FALSE(tester.simCiclePath(0.0));
}

TEST_F(SimCiclePathTest, TriangleCorrectAngle)
{
    tester.points = TestData::triangle();
    EXPECT_TRUE(tester.simCiclePath(30.0 * M_PI / 180.0));
}

TEST_F(SimCiclePathTest, TriangleWrongAngle)
{
    tester.points = { CPoint(0,0,0), CPoint(100,0,1), CPoint(50,86.6,2) };
    EXPECT_FALSE(tester.simCiclePath(180.0 * M_PI / 180.0));
}

TEST_F(SimCiclePathTest, SquareCorrectAngle)
{
    tester.points = TestData::square();
    EXPECT_TRUE(tester.simCiclePath(45.0 * M_PI / 180.0));
}
