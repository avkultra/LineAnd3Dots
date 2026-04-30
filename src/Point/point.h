#ifndef POINT_H
#define POINT_H


struct CPoint
{
    double m_dX;
    double m_dY;
    int m_id;
    CPoint(double x = 0, double y = 0, int id = -1);
};

#endif // POINT_H
