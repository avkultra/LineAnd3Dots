#include <gtest/gtest.h>
#include <cmath>
#include "test_utils.h"

class GeometryTest : public ::testing::Test
{
protected:
    CTester tester;
};

TEST_F(GeometryTest, AngleZeroDegrees)
{
    CPoint from(0,0,0);
    CPoint to(100,0,1);
    EXPECT_NEAR(tester.compAngl(from, to), 0.0, 1e-6);
}

TEST_F(GeometryTest, AngleNinetyDegrees)
{
    CPoint from(0,0,0);
    CPoint to(0,100,1);
    EXPECT_NEAR(tester.compAngl(from, to), M_PI_2, 1e-6);
}
