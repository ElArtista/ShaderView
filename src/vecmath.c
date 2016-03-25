#include "vecmath.h"
#include <math.h>
#include <string.h>

void mat4_identity(mat4x4 mat)
{
    int i = 0, j = 0;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            mat[i][j] = i == j ? 1.0f : 0.0f;
}

void mat4_add(mat4x4 mat1, mat4x4 mat2, mat4x4 result)
{
    int i = 0, j = 0;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            result[i][j] = mat1[i][j] + mat2[i][j];
}

void mat4_remove(mat4x4 mat1, mat4x4 mat2, mat4x4 result)
{
    int i = 0, j = 0;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            result[i][j] = mat2[i][j] - mat1[i][j];
}

void mat4_multi(mat4x4 mat, float factor, mat4x4 result)
{
    int i = 0, j = 0;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            result[i][j] = mat[i][j] * factor;
}

void mat4_inverse(mat4x4 mat, mat4x4 result)
{
    int i = 0, j = 0;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            result[i][j] = mat[j][i];
}

void mat4_persp(float fovy, float aspect, float z_near, float z_far, mat4x4 result)
{
    const float tanHalfFovy = tanf(fovy / (float)(2));
    memset(result, 0, 16);

    result[0][0] = (float)(1) / (aspect * tanHalfFovy);
    result[1][1] = (float)(1) / (tanHalfFovy);
    result[2][3] = - (float)(1);
    result[2][2] = - (z_far + z_near) / (z_far - z_near);
    result[3][2] = - ((float)(2) * z_far * z_near) / (z_far - z_near);
}

void mat4_ortho(float left, float right, float bottom, float top, mat4x4 result)
{
    mat4_identity(result);
    result[0][0] = (float)(2) / (right - left);
    result[1][1] = (float)(2) / (top - bottom);
    result[2][2] = - 1.0f;
    result[3][0] = - (right + left) / (right - left);
    result[3][1] = - (top + bottom) / (top - bottom);
}
