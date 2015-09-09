#ifndef TRANSFORM_MATRIX_H
#define TRANSFORM_MATRIX_H

#include <array>


class TransformMatrix
{
public:
    static TransformMatrix buildTransposeMatrix(float dx, float dy, float dz)
    {
        TransformMatrix matrix;
        matrix.m_data = {
            1, 0, 0, dx,
            0, 1, 0, dy,
            0, 0, 1, dz,
            0, 0, 0, 1
        };
        return matrix;
    }

    TransformMatrix() {}

    void setValue(int row, int col, float value) { m_data[row * DIM + col] = value; }
    float value(int row, int col) const { return m_data[row * DIM + col]; }

private:
    static const int DIM = 4;
    std::array<float, DIM * DIM> m_data;

};

#endif // TRANSFORM_MATRIX_H
