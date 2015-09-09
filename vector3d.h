#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include "transformmatrix.h"


struct Vector3D
{
    float x;
    float y;
    float z;

    Vector3D operator-() const;
    Vector3D operator+(const Vector3D &vec) const;
    Vector3D operator-(const Vector3D &vec) const;
    Vector3D operator/(float factor);

    void applyTransform(const TransformMatrix &mat);

    static float distance(const Vector3D &v1, const Vector3D &v2);
    static Vector3D min(const Vector3D &v1, const Vector3D &v2);
    static Vector3D max(const Vector3D &v1, const Vector3D &v2);
};

#endif // VECTOR3D_H
