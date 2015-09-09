#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Vector3D.h"


struct Triangle
{
    Vector3D p1;
    Vector3D p2;
    Vector3D p3;
};


struct Facet
{
    Vector3D normal;
    Triangle triangle;
};


struct BoundingBox
{
    Vector3D p1;
    Vector3D p2;
};

#endif // TRIANGLE_H
