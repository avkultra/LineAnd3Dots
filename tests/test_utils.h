#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <vector>
#include <cmath>
#include <algorithm>
#include "../src/Point/point.h"
#include "../src/MainWindow/const.h"

class CTester
{
public:
    std::vector<CPoint> points;
    int startId;

    double compAngl(const CPoint& from, const CPoint& to)
    {
        return atan2(to.m_dY - from.m_dY, to.m_dX - from.m_dX);
    }

    bool simCiclePath(double startAngle)
    {
        if (points.empty())
            return false;

        std::vector<bool> visited(points.size(), false);
        visited[startId] = true;

        int currentId = startId;
        double currentAngle = startAngle;
        int visitedCount = 1;

        for (size_t step = 0; step < points.size() * 2; ++step)
        {
            int nextId = -1;
            double minDelta = dMinDelta;

            for (size_t i = 0; i < points.size(); ++i)
            {
                if ((int)i == currentId)
                    continue;

                double angle = compAngl(points[currentId], points[i]);
                double delta = angle - currentAngle;

                while (delta <= 0)
                {
                    delta += 2 * M_PI;
                }

                while (delta >= 2 * M_PI)
                {
                    delta -= 2 * M_PI;
                }

                if (delta < minDelta - dMinRad)
                {
                    minDelta = delta;
                    nextId = i;
                }
            }

            if (nextId == -1)
                return false;

            currentAngle = compAngl(points[currentId], points[nextId]);
            currentId = nextId;

            if (!visited[currentId])
            {
                visited[currentId] = true;
                ++visitedCount;
            }

            if (visitedCount == (int)points.size())
                return true;
        }
        return false;
    }

    double findLine()
    {
        if (points.empty()) return 0;

        const CPoint& start = points[startId];
        std::vector<double> testAngles;

        for (size_t i = 0; i < points.size(); ++i)
        {
            const CPoint& p = points[i];
            if (p.m_id == startId)
                continue;
            double ang = compAngl(start, p);
            testAngles.push_back(ang);
            testAngles.push_back(ang + dOffset);
            testAngles.push_back(ang - dOffset);
        }

        for (int i = 0; i < 360; ++i)
        {
            testAngles.push_back(i * M_PI / 180.0);
        }

        std::sort(testAngles.begin(), testAngles.end());
        // Убирем дубликаты из списка, если разнница < dMinRad = 1e-9;
        testAngles.erase(std::unique(testAngles.begin(), testAngles.end(),
            [](double a, double b) { return fabs(a - b) < dMinRad; }), testAngles.end());

        for (size_t j = 0; j < testAngles.size(); ++j)
        {
            double angle = testAngles[j];
            if (simCiclePath(angle))
            {
                return angle;
            }
        }

        return -dMinDelta;
    }
};

namespace TestData
{
    inline std::vector<CPoint> triangle()
    {
        return { CPoint(0,0,0), CPoint(100,0,1), CPoint(50,86.6025,2) };
    }

    inline std::vector<CPoint> square()
    {
        return { CPoint(100,0,0), CPoint(0,100,1), CPoint(-100,0,2), CPoint(0,-100,3) };
    }

    inline std::vector<CPoint> hexagon()
    {
        return
        {
            CPoint(0,0,0), CPoint(100,0,1), CPoint(50,86.6025,2),
            CPoint(-50,86.6025,3), CPoint(-100,0,4), CPoint(-50,-86.6025,5),
            CPoint(50,-86.6025,6)
        };
    }
}

#endif // TEST_UTILS_H
