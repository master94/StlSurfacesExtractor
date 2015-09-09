#include "vector3d.h"


Vector3D Vector3D::operator-() const
{
    Vector3D res;
    res.x = -x;
    res.y = -y;
    res.z = -z;
    return res;
}

Vector3D Vector3D::operator+(const Vector3D &vec) const
{
    Vector3D res;
    res.x = x + vec.x;
    res.y = y + vec.y;
    res.z = z + vec.z;
    return res;
}

Vector3D Vector3D::operator-(const Vector3D &vec) const
{
    return *this + (-vec);
}

Vector3D Vector3D::operator/(float factor)
{
    Vector3D res;
    res.x = x / factor;
    res.y = y / factor;
    res.z = z / factor;
    return res;
}

void Vector3D::applyTransform(const TransformMatrix &mat)
{
    const float tmpX = mat.value(0, 0) * x + mat.value(0, 1) * y + mat.value(0, 2) * z + mat.value(0, 3) * 1;
    const float tmpY = mat.value(1, 0) * x + mat.value(1, 1) * y + mat.value(1, 2) * z + mat.value(1, 3) * 1;
    const float tmpZ = mat.value(2, 0) * x + mat.value(2, 1) * y + mat.value(2, 2) * z + mat.value(2, 3) * 1;

    x = tmpX;
    y = tmpY;
    z = tmpZ;
}

float Vector3D::distance(const Vector3D &v1, const Vector3D &v2)
{
    return pow(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2), 0.5);
}

Vector3D Vector3D::min(const Vector3D &v1, const Vector3D &v2)
{
    Vector3D v;
    v.x = std::min(v1.x, v2.x);
    v.y = std::min(v1.y, v2.y);
    v.z = std::min(v1.z, v2.z);
    return v;
}

Vector3D Vector3D::max(const Vector3D &v1, const Vector3D &v2)
{
    Vector3D v;
    v.x = std::max(v1.x, v2.x);
    v.y = std::max(v1.y, v2.y);
    v.z = std::max(v1.z, v2.z);
    return v;
}
