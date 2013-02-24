/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2013  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -----------------------------------------------------------------------------*/
 
/* created: 26/02/2008
   updated: 24/02/2013 */

#include "SCE/utils/SCEType.h"
#include "SCE/utils/SCEVector.h"

#include "SCE/utils/SCERectangle.h"

/**
 * \file SCERectangle.c
 * \copydoc rectangle
 * \file SCERectangle.h
 * \copydoc rectangle
 */

/**
 * \defgroup rectangle Rectangles managment
 * \ingroup utils
 * \brief Rectangle managment functions
 */

/** @{ */

/**
 * \brief Initialize a SCE_SIntRect
 * \param r a SCE_SIntRect to initialize
 */
void SCE_Rectangle_Init (SCE_SIntRect *r)
{
    r->p1[0] = r->p1[1] = r->p2[0] = r->p2[1] = 0;
}
void SCE_Rectangle3_Init (SCE_SIntRect3 *r)
{
    r->p1[0] = r->p1[1] = r->p1[2] = r->p2[0] = r->p2[1] = r->p2[2] = 0;
}
void SCE_Rectangle3_Initl (SCE_SLongRect3 *r)
{
    r->p1[0] = r->p1[1] = r->p1[2] = r->p2[0] = r->p2[1] = r->p2[2] = 0;
}
/**
 * \brief Initialize a SCE_SFloatRect
 * \param r a SCE_SFloatRect to initialize
 */
void SCE_Rectangle_Initf (SCE_SFloatRect *r)
{
    r->p1[0] = r->p1[1] = r->p2[0] = r->p2[1] = 0.0f;
}

/**
 * \brief Defines a rectangle
 * \param r the rectangle to define
 * \param x1 X coordinate of a corner
 * \param y1 Y coordinate of a corner
 * \param x2 X coordinate of the opposite corner
 * \param y2 Y coordinate of the opposite corner
 * \see SCE_Rectangle_Setf()
 * \warning The firsts point coordinates must be lesser than the second point
 * coordinates
 */
void SCE_Rectangle_Set (SCE_SIntRect *r, int x1, int y1, int x2, int y2)
{
    r->p1[0] = x1; r->p2[0] = x2; r->p1[1] = y1; r->p2[1] = y2;
}
void SCE_Rectangle3_Set (SCE_SIntRect3 *r, int x1, int y1, int z1,
                         int x2, int y2, int z2)
{
    r->p1[0] = x1; r->p1[1] = y1; r->p1[2] = z1;
    r->p2[0] = x2; r->p2[1] = y2; r->p2[2] = z2;
}
void SCE_Rectangle3_Setv (SCE_SIntRect3 *r, const int *p1, const int *p2)
{
    r->p1[0] = p1[0]; r->p1[1] = p1[1]; r->p1[2] = p1[2];
    r->p2[0] = p2[0]; r->p2[1] = p2[1]; r->p2[2] = p2[2];
}
void SCE_Rectangle3_Setl (SCE_SLongRect3 *r, long x1, long y1, long z1,
                          long x2, long y2, long z2)
{
    r->p1[0] = x1; r->p1[1] = y1; r->p1[2] = z1;
    r->p2[0] = x2; r->p2[1] = y2; r->p2[2] = z2;
}
void SCE_Rectangle3_Setlv (SCE_SLongRect3 *r, const long *p1, const long *p2)
{
    r->p1[0] = p1[0]; r->p1[1] = p1[1]; r->p1[2] = p1[2];
    r->p2[0] = p2[0]; r->p2[1] = p2[1]; r->p2[2] = p2[2];
}
/**
 * \brief Defines a rectangle
 * \param r the rectangle to define
 * \param x1 X coordinate of the bottom left corner
 * \param y1 Y coordinate of the bottom left corner
 * \param x2 X coordinate of the top right corner
 * \param y2 Y coordinate of the top right corner
 * \see SCE_Rectangle_Set()
 * \warning The firsts point coordinates must be lesser than the second point
 * coordinates
 */
void SCE_Rectangle_Setf (SCE_SFloatRect *r, float x1, float y1,
                         float x2, float y2)
{
    r->p1[0] = x1; r->p2[0] = x2; r->p1[1] = y1; r->p2[1] = y2;
}


void SCE_Rectangle3_IntFromLong (SCE_SIntRect3 *dst, const SCE_SLongRect3 *src)
{
    dst->p1[0] = (int)src->p1[0];
    dst->p1[1] = (int)src->p1[1];
    dst->p1[2] = (int)src->p1[2];
    dst->p2[0] = (int)src->p2[0];
    dst->p2[1] = (int)src->p2[1];
    dst->p2[2] = (int)src->p2[2];
}
void SCE_Rectangle3_LongFromInt (SCE_SLongRect3 *dst, const SCE_SIntRect3 *src)
{
    dst->p1[0] = (long)src->p1[0];
    dst->p1[1] = (long)src->p1[1];
    dst->p1[2] = (long)src->p1[2];
    dst->p2[0] = (long)src->p2[0];
    dst->p2[1] = (long)src->p2[1];
    dst->p2[2] = (long)src->p2[2];
}


/**
 * \brief Defines the origin and the dimensions at once
 * \param r An integer rectangle
 * \param x,y new origin
 * \param w,h new dimensions
 * \sa SCE_Rectangle_Set()
 */
void SCE_Rectangle_SetFromOrigin (SCE_SIntRect *r, int x, int y,
                                  unsigned int w, unsigned int h)
{
    r->p1[0] = x;     r->p1[1] = y;
    r->p2[0] = x + w; r->p2[1] = y + h;
}
void SCE_Rectangle3_SetFromOrigin (SCE_SIntRect3 *r, int x, int y, int z,
                                   SCEuint w, SCEuint h, SCEuint d)
{
    r->p1[0] = x;     r->p1[1] = y;     r->p1[2] = z;
    r->p2[0] = x + w; r->p2[1] = y + h; r->p2[2] = z + d;
}
void SCE_Rectangle3_SetFromOriginl (SCE_SLongRect3 *r, long x, long y, long z,
                                    SCEulong w, SCEulong h, SCEulong d)
{
    r->p1[0] = x;     r->p1[1] = y;     r->p1[2] = z;
    r->p2[0] = x + w; r->p2[1] = y + h; r->p2[2] = z + d;
}
/**
 * \brief Defines the origin and the dimensions at once
 * \param r A floating-point rectangle
 * \param x,y new origin
 * \param w,h new dimensions
 * \sa SCE_Rectangle_Setf()
 */
void SCE_Rectangle_SetFromOriginf (SCE_SFloatRect *r, float x, float y,
                                   float w, float h)
{
    r->p1[0] = x;     r->p1[1] = y;
    r->p2[0] = x + w; r->p2[1] = y + h;
}

/**
 * \brief Defines a rectangle from its center and dimensions
 * \param r an integer rectangle
 * \param x,y center of the rectangle
 * \param w,h dimensions of the rectangle
 */
void SCE_Rectangle_SetFromCenter (SCE_SIntRect *r, int x, int y,
                                  unsigned int w, unsigned int h)
{
    int w2 = w / 2, h2 = h / 2;
    r->p1[0] = x - w2; r->p1[1] = y - h2;
    r->p2[0] = x + w2; r->p2[1] = y + h2;
}
/**
 * \brief Defines a rectangle from its center and dimensions
 * \param r a floating-point rectangle
 * \param x,y center of the rectangle
 * \param w,h dimensions of the rectangle
 */
void SCE_Rectangle_SetFromCenterf (SCE_SFloatRect *r, float x, float y,
                                   float w, float h)
{
    float w2 = w / 2.0, h2 = h / 2.0;
    r->p1[0] = x - w2; r->p1[1] = y - h2;
    r->p2[0] = x + w2; r->p2[1] = y + h2;
}
/**
 * \brief Defines a rectangle from its center and dimensions
 * \param r a floating-point rectangle
 * \param c center of the rectangle
 * \param w,h dimensions of the rectangle
 */
void SCE_Rectangle_SetFromCenterfv (SCE_SFloatRect *r, SCE_TVector2 c,
                                    float w, float h)
{
    SCE_Rectangle_SetFromCenterf (r, c[0], c[1], w, h);
}
/**
 * \brief Defines a rectangle from its center and dimensions
 * \param r an integer rectangle
 * \param x,y center of the rectangle
 * \param w,h dimensions of the rectangle
 */
void SCE_Rectangle3_SetFromCenter (SCE_SIntRect3 *r, int x, int y, int z,
                                   unsigned int w, unsigned int h,
                                   unsigned int d)
{
    int w2 = w / 2, h2 = h / 2, d2 = d / 2;
    r->p1[0] = x - w2; r->p1[1] = y - h2; r->p1[2] = z - d2;
    r->p2[0] = x + w2; r->p2[1] = y + h2; r->p2[2] = z + d2;
}
/**
 * \brief Defines a rectangle from its center and dimensions
 * \param r an integer rectangle
 * \param x,y center of the rectangle
 * \param w,h dimensions of the rectangle
 */
void SCE_Rectangle3_SetFromCenterl (SCE_SLongRect3 *r, long x, long y, long z,
                                    unsigned long w, unsigned long h,
                                    unsigned long d)
{
    long w2 = w / 2, h2 = h / 2, d2 = d / 2;
    r->p1[0] = x - w2; r->p1[1] = y - h2; r->p1[2] = z - d2;
    r->p2[0] = x + w2; r->p2[1] = y + h2; r->p2[2] = z + d2;
}


/**
 * \brief Moves a rectangle
 * \param r the rectangle to move
 * \param x X offset
 * \param y Y offset
 * \see SCE_Rectangle_Movev()
 */
void SCE_Rectangle_Move (SCE_SIntRect *r, int x, int y)
{
    r->p1[0] += x; r->p2[0] += x;
    r->p1[1] += y; r->p2[1] += y;
}
/**
 * \brief Moves a rectangle
 * \param r the rectangle to move
 * \param x X offset
 * \param y Y offset
 * \param z Z offset
 * \see SCE_Rectangle_Move()
 */
void SCE_Rectangle3_Move (SCE_SIntRect3 *r, int x, int y, int z)
{
    r->p1[0] += x; r->p2[0] += x;
    r->p1[1] += y; r->p2[1] += y;
    r->p1[2] += z; r->p2[2] += z;
}
void SCE_Rectangle3_Movel (SCE_SLongRect3 *r, long x, long y, long z)
{
    r->p1[0] += x; r->p2[0] += x;
    r->p1[1] += y; r->p2[1] += y;
    r->p1[2] += z; r->p2[2] += z;
}
/**
 * \brief Moves a rectangle
 * \param r the rectangle to move
 * \param x X offset
 * \param y Y offset
 * \see SCE_Rectangle_Movefv()
 */
void SCE_Rectangle_Movef (SCE_SFloatRect *r, float x, float y)
{
    r->p1[0] += x; r->p2[0] += x;
    r->p1[1] += y; r->p2[1] += y;
}
/**
 * \brief Moves a rectangle
 * \param r the rectangle to move
 * \param v vector offset
 * \see SCE_Rectangle_Move()
 */
void SCE_Rectangle_Movev (SCE_SIntRect *r, int *v)
{
    SCE_Rectangle_Move (r, v[0], v[1]);
}
/**
 * \brief Moves a rectangle
 * \param r the rectangle to move
 * \param v vector offset
 * \see SCE_Rectangle_Movef()
 */
void SCE_Rectangle_Movefv (SCE_SFloatRect *r, SCE_TVector2 v)
{
    SCE_Rectangle_Movef (r, v[0], v[1]);
}

/**
 * \brief Defines new dimensions
 * \param r An integer rectangle
 * \param w,h new dimensions of the rectangle
 */
void SCE_Rectangle_Resize (SCE_SIntRect *r, int w, int h)
{
    r->p2[0] = r->p1[0] + w;
    r->p2[1] = r->p1[1] + h;
}
void SCE_Rectangle3_Resize (SCE_SIntRect3 *r, int w, int h, int d)
{
    r->p2[0] = r->p1[0] + w;
    r->p2[1] = r->p1[1] + h;
    r->p2[2] = r->p1[2] + d;
}
void SCE_Rectangle3_Resizel (SCE_SLongRect3 *r, long w, long h, long d)
{
    r->p2[0] = r->p1[0] + w;
    r->p2[1] = r->p1[1] + h;
    r->p2[2] = r->p1[2] + d;
}
/**
 * \brief Defines new dimensions
 * \param r A floating-point rectangle
 * \param w,h new dimensions of the rectangle
 */
void SCE_Rectangle_Resizef (SCE_SFloatRect *r, float w, float h)
{
    r->p2[0] = r->p1[0] + w;
    r->p2[1] = r->p1[1] + h;
}

/**
 * \brief Adds an offset to the current dimensions
 * \param r An integer rectangle
 * \param w,h offset to add
 */
void SCE_Rectangle_Add (SCE_SIntRect *r, int w, int h)
{
    r->p2[0] += w;
    r->p2[1] += h;
}
/**
 * \brief Adds an offset to the current dimensions
 * \param r A floating-point rectangle
 * \param w,h offset to add
 */
void SCE_Rectangle_Addf (SCE_SFloatRect *r, float w, float h)
{
    r->p2[0] += w;
    r->p2[1] += h;
}
void SCE_Rectangle3_Mul (SCE_SIntRect3 *r, int x, int y, int z)
{
    r->p1[0] *= x; r->p2[0] *= x;
    r->p1[1] *= y; r->p2[1] *= y;
    r->p1[2] *= z; r->p2[2] *= z;
}
void SCE_Rectangle3_Mull (SCE_SLongRect3 *r, long x, long y, long z)
{
    r->p1[0] *= x; r->p2[0] *= x;
    r->p1[1] *= y; r->p2[1] *= y;
    r->p1[2] *= z; r->p2[2] *= z;
}
void SCE_Rectangle3_Div (SCE_SIntRect3 *r, int x, int y, int z)
{
    r->p1[0] /= x; r->p2[0] /= x;
    r->p1[1] /= y; r->p2[1] /= y;
    r->p1[2] /= z; r->p2[2] /= z;
}
void SCE_Rectangle3_Divl (SCE_SLongRect3 *r, long x, long y, long z)
{
    r->p1[0] /= x; r->p2[0] /= x;
    r->p1[1] /= y; r->p2[1] /= y;
    r->p1[2] /= z; r->p2[2] /= z;
}

void SCE_Rectangle3_SubOriginl (SCE_SLongRect3 *r1, const SCE_SLongRect3 *r2)
{
    SCE_Rectangle3_Movel (r1, -r2->p1[0], -r2->p1[1], -r2->p1[2]);
}


/**
 * \brief Gets the rectangle area
 * \param r a rectangle
 * \return the rectangle area
 * \see SCE_Rectangle_GetAreaf()
 */
int SCE_Rectangle_GetArea (const SCE_SIntRect *r)
{
    return (r->p2[0] - r->p1[0]) * (r->p2[1] - r->p1[1]);
}
/**
 * \brief Gets the rectangle area
 * \param r a rectangle
 * \return the rectangle area
 * \see SCE_Rectangle_GetArea(), SCE_Rectangle_GetAreaf()
 */
int SCE_Rectangle3_GetArea (const SCE_SIntRect3 *r)
{
    return (r->p2[0]-r->p1[0]) * (r->p2[1]-r->p1[1]) * (r->p2[2]-r->p1[2]);
}
long SCE_Rectangle3_GetAreal (const SCE_SLongRect3 *r)
{
    return (r->p2[0]-r->p1[0]) * (r->p2[1]-r->p1[1]) * (r->p2[2]-r->p1[2]);
}
/**
 * \brief Gets the rectangle area
 * \param r a rectangle
 * \return the rectangle area
 * \see SCE_Rectangle_GetArea()
 */
float SCE_Rectangle_GetAreaf (const SCE_SFloatRect *r)
{
    return (r->p2[0] - r->p1[0]) * (r->p2[1] - r->p1[1]);
}

/**
 * \brief Gets the rectangle width
 * \param r a rectangle
 * \return the rectangle width
 * \see SCE_Rectangle_GetWidthf(), SCE_Rectangle_GetHeight()
 */
int SCE_Rectangle_GetWidth (const SCE_SIntRect *r)
{
    return r->p2[0] - r->p1[0];
}
/**
 * \brief Gets the rectangle height
 * \param r a rectangle
 * \return the rectangle height
 * \sa SCE_Rectangle_GetHeightf(), SCE_Rectangle_GetWidth()
 */
int SCE_Rectangle_GetHeight (const SCE_SIntRect *r)
{
    return r->p2[1] - r->p1[1];
}

/**
 * \brief Gets the rectangle width
 * \param r a rectangle
 * \return rectangle's width
 * \see SCE_Rectangle3_GetHeight(), SCE_Rectangle_GetWidth()
 */
int SCE_Rectangle3_GetWidth (const SCE_SIntRect3 *r)
{
    return r->p2[0] - r->p1[0];
}
/**
 * \brief Gets the rectangle height
 * \param r a rectangle
 * \return rectangle's height
 * \see SCE_Rectangle3_GetWidth(), SCE_Rectangle_GetWidth()
 */
int SCE_Rectangle3_GetHeight (const SCE_SIntRect3 *r)
{
    return r->p2[1] - r->p1[1];
}
/**
 * \brief Gets the rectangle depth
 * \param r a rectangle
 * \return rectangle's depth
 * \see SCE_Rectangle3_GetWidth(), SCE_Rectangle_GetWidth()
 */
int SCE_Rectangle3_GetDepth (const SCE_SIntRect3 *r)
{
    return r->p2[2] - r->p1[2];
}

/**
 * \brief Gets the rectangle width
 * \param r a rectangle
 * \return rectangle's width
 * \see SCE_Rectangle3_GetHeightl(), SCE_Rectangle_GetWidthl()
 */
long SCE_Rectangle3_GetWidthl (const SCE_SLongRect3 *r)
{
    return r->p2[0] - r->p1[0];
}
/**
 * \brief Gets the rectangle height
 * \param r a rectangle
 * \return rectangle's height
 * \see SCE_Rectangle3_GetWidthl(), SCE_Rectangle_GetWidthl()
 */
long SCE_Rectangle3_GetHeightl (const SCE_SLongRect3 *r)
{
    return r->p2[1] - r->p1[1];
}
/**
 * \brief Gets the rectangle depth
 * \param r a rectangle
 * \return rectangle's depth
 * \see SCE_Rectangle3_GetWidthl(), SCE_Rectangle_GetWidthl()
 */
long SCE_Rectangle3_GetDepthl (const SCE_SLongRect3 *r)
{
    return r->p2[2] - r->p1[2];
}

/**
 * \brief Gets the rectangle width
 * \param r a rectangle
 * \return the rectangle width
 * \see SCE_Rectangle_GetWidth(), SCE_Rectangle_GetHeightf()
 */
float SCE_Rectangle_GetWidthf (const SCE_SFloatRect *r)
{
    return r->p2[0] - r->p1[0];
}
/**
 * \brief Gets the rectangle height
 * \param r a rectangle
 * \return the rectangle height
 * \see SCE_Rectangle_GetHeight(), SCE_Rectangle_GetWidthf()
 */
float SCE_Rectangle_GetHeightf (const SCE_SFloatRect *r)
{
    return r->p2[1] - r->p1[1];
}


void SCE_Rectangle_GetPoints (const SCE_SIntRect *r, int *x1, int *y1,
                              int *x2, int *y2)
{
    *x1 = r->p1[0]; *y1 = r->p1[1]; *x2 = r->p2[0]; *y2 = r->p2[1];
}
void SCE_Rectangle_GetPointsv (const SCE_SIntRect *r, int *p1, int *p2)
{
    p1[0] = r->p1[0]; p1[1] = r->p1[1]; p2[0] = r->p2[0]; p2[1] = r->p2[1];
}
void SCE_Rectangle3_GetPointsv (const SCE_SIntRect3 *r, int *p1, int *p2)
{
    p1[0] = r->p1[0]; p1[1] = r->p1[1]; p1[2] = r->p1[2];
    p2[0] = r->p2[0]; p2[1] = r->p2[1]; p2[2] = r->p2[2];
}
void SCE_Rectangle3_GetPointslv (const SCE_SLongRect3 *r, long *p1, long *p2)
{
    p1[0] = r->p1[0]; p1[1] = r->p1[1]; p1[2] = r->p1[2];
    p2[0] = r->p2[0]; p2[1] = r->p2[1]; p2[2] = r->p2[2];
}
void SCE_Rectangle_GetPointsf (const SCE_SIntRect *r, float *x1, float *y1,
                               float *x2, float *y2)
{
    *x1 = r->p1[0]; *y1 = r->p1[1]; *x2 = r->p2[0]; *y2 = r->p2[1];
}
void SCE_Rectangle_GetPointsfv (const SCE_SIntRect *r, SCE_TVector2 p1,
                                SCE_TVector2 p2)
{
    p1[0] = r->p1[0]; p1[1] = r->p1[1]; p2[0] = r->p2[0]; p2[1] = r->p2[1];
}

int* SCE_Rectangle_GetBottomLeftPoint (SCE_SIntRect *r)
{
    return r->p1;
}
int* SCE_Rectangle_GetTopRightPoint (SCE_SIntRect *r)
{
    return r->p2;
}
float* SCE_Rectangle_GetBottomLeftPointf (SCE_SFloatRect *r)
{
    return r->p1;
}
float* SCE_Rectangle_GetTopRightPointf (SCE_SFloatRect *r)
{
    return r->p2;
}


/**
 * \brief Compares two rectangles
 * \param a a rectangle
 * \param b another rectangle
 * \return SCE_TRUE if the two rectangles are equal, SCE_FALSE otherwise
 */
int SCE_Rectangle_Equal (SCE_SIntRect *a, SCE_SIntRect *b)
{
    return (a->p1[0] == b->p1[0] && a->p1[1] == b->p1[1] &&
            a->p2[0] == b->p2[0] && a->p2[1] == b->p2[1]);
}
/**
 * \brief Compares two rectangles
 * \param a a rectangle
 * \param b another rectangle
 * \return SCE_TRUE if the two rectangles are equal, SCE_FALSE otherwise
 * 
 * \see SCE_Rectangle_Equal()
 */
int SCE_Rectangle_Equalf (SCE_SFloatRect *a, SCE_SFloatRect *b)
{
#define EPS 0.000002 /* "epsilon" (pux) */
    return (SCE_Math_Fabsf (a->p1[0]-b->p1[0]) < EPS &&
            SCE_Math_Fabsf (a->p1[1]-b->p1[1]) < EPS &&
            SCE_Math_Fabsf (a->p2[0]-b->p2[0]) < EPS &&
            SCE_Math_Fabsf (a->p2[1]-b->p2[1]) < EPS);
}

/**
 * \brief Check if a point is inside a rectangle
 * \param r a rectangle
 * \param x X coordinates of the point
 * \param y Y coordinates of the point
 * \return SCE_TRUE if the point is inside teh rectangles, SCE_FALSE otherwise
 */
int SCE_Rectangle_IsIn (SCE_SIntRect *r, int x, int y)
{
    return (x >= r->p1[0] && x < r->p2[0] &&
            y >= r->p1[1] && y < r->p2[1]);
}
/**
 * \brief Check if a point is inside a rectangle
 * \param r a rectangle
 * \param x X coordinates of the point
 * \param y Y coordinates of the point
 * \return SCE_TRUE if the point is inside the rectangles, SCE_FALSE otherwise
 * 
 * \see SCE_Rectangle_IsIn()
 */
int SCE_Rectangle_IsInf (SCE_SFloatRect *r, float x, float y)
{
    return (x >= r->p1[0] && x < r->p2[0] &&
            y >= r->p1[1] && y < r->p2[1]);
}
int SCE_Rectangle3_IsIn (SCE_SIntRect3 *r, int x, int y, int z)
{
    return (x >= r->p1[0] && x < r->p2[0] &&
            y >= r->p1[1] && y < r->p2[1] &&
            z >= r->p1[2] && z < r->p2[2]);
}
int SCE_Rectangle3_IsInl (SCE_SLongRect3 *r, long x, long y, long z)
{
    return (x >= r->p1[0] && x < r->p2[0] &&
            y >= r->p1[1] && y < r->p2[1] &&
            z >= r->p1[2] && z < r->p2[2]);
}
/**
 * \brief Check if a point is inside a rectangle
 * \param r a rectangle
 * \param p vector coordinates of the point
 * \return SCE_TRUE if the point is inside teh rectangles, SCE_FALSE otherwise
 * 
 * \see SCE_Rectangle_IsIn()
 */
int SCE_Rectangle_IsInv (SCE_SIntRect *r, int *p)
{
    return SCE_Rectangle_IsIn (r, p[0], p[1]);
}
/**
 * \brief Check if a point is inside a rectangle
 * \param r a rectangle
 * \param p vector coordinates of the point
 * \return SCE_TRUE if the point is inside the rectangles, SCE_FALSE otherwise
 * \see SCE_Rectangle_IsInf()
 */
int SCE_Rectangle_IsInfv (SCE_SFloatRect *r, float *p)
{
    return SCE_Rectangle_IsInf (r, p[0], p[1]);
}

/**
 * \brief Gets the rectangle's edges
 * \param re a rectangle
 * \param l a SCE_SLine where store the left edge
 * \param r a SCE_SLine where store the right edge
 * \param b a SCE_SLine where store the bottom edge
 * \param t a SCE_SLine where store the top edge
 */
void SCE_Rectangle_MakeLines (SCE_SIntRect *re, SCE_SLine *l, SCE_SLine *r,
                              SCE_SLine *b, SCE_SLine *t)
{
    SCE_Line_Set (l, re->p1[0], re->p1[1], re->p1[0], re->p2[1]);
    SCE_Line_Set (r, re->p2[0], re->p1[1], re->p2[0], re->p2[1]);
    SCE_Line_Set (b, re->p1[0], re->p1[1], re->p2[0], re->p1[1]);
    SCE_Line_Set (t, re->p1[0], re->p2[1], re->p2[0], re->p2[1]);
}
/**
 * \brief Gets the rectangle's edges
 * \param re a rectangle
 * \param l a SCE_SLine where store the left edge
 * \param r a SCE_SLine where store the right edge
 * \param b a SCE_SLine where store the bottom edge
 * \param t a SCE_SLine where store the top edge
 * \see SCE_Rectangle_MakeLines()
 */
void SCE_Rectangle_MakeLinesf (SCE_SFloatRect *re, SCE_SLine *l, SCE_SLine *r,
                               SCE_SLine *b, SCE_SLine *t)
{
    SCE_Line_Set (l, re->p1[0], re->p1[1], re->p1[0], re->p2[1]);
    SCE_Line_Set (r, re->p2[0], re->p1[1], re->p2[0], re->p2[1]);
    SCE_Line_Set (b, re->p1[0], re->p1[1], re->p2[0], re->p1[1]);
    SCE_Line_Set (t, re->p1[0], re->p2[1], re->p2[0], re->p2[1]);
}
/**
 * \brief Gets the rectangle's edges
 * \param r a rectangle
 * \param lines an array of 4 SCE_SLine where store the edges.
 * 
 * This function gets the rectangle's edges and stores them into a given array
 * of SCE_SLine. The lines are stored in the following order: left, right,
 * bottom and top.
 * \see SCE_Rectangle_MakeLines()
 */
void SCE_Rectangle_MakeLinesv (SCE_SIntRect *r, SCE_SLine *lines)
{
    SCE_Rectangle_MakeLines (r, &lines[0], &lines[1], &lines[2], &lines[3]);
}
/**
 * \brief Gets the rectangle's edges
 * \param r a rectangle
 * \param lines an array of 4 SCE_SLine where store the edges.
 * 
 * This function gets the rectangle's edges and stores them into a given array
 * of SCE_SLine. The lines are stored in the following order: left, right,
 * bottom and top.
 * \see SCE_Rectangle_MakeLinesf()
 */
void SCE_Rectangle_MakeLinesfv (SCE_SFloatRect *r, SCE_SLine *lines)
{
    SCE_Rectangle_MakeLinesf (r, &lines[0], &lines[1], &lines[2], &lines[3]);
}

/**
 * \brief Checks if two recangles are crossed
 * \param a a rectangle
 * \param b another rectangle
 * \return SCE_RECT_IN if \p b is inside \p a, SCE_RECT_INTERSECTS if \p a and
 *         \p b are crossed and SCE_RECT_OUT if the two rectangles has no
 *          intersections.
 */
int SCE_Rectangle_Intersects (SCE_SIntRect *a, SCE_SIntRect *b)
{
    int start[2];
    int end[2];
    SCE_SIntRect r;
    
    start[0] = MAX (a->p1[0], b->p1[0]);
    start[1] = MAX (a->p1[1], b->p1[1]);
    end[0] = MIN (a->p2[0], b->p2[0]);
    end[1] = MIN (a->p2[1], b->p2[1]);
    r.p1[0] = start[0];
    r.p1[1] = start[1];
    r.p2[0] = end[0];
    r.p2[1] = end[1];
    
    if ((start[0] > end[0]) || (start[1] > end[1]))
        return SCE_RECT_OUT;
    else if (SCE_Rectangle_Equal (&r, a) || SCE_Rectangle_Equal (&r, b))
        return SCE_RECT_IN;
    else
        return SCE_RECT_INTERSECTS;
}
/**
 * \brief Checks if two recangles are crossed
 * \param a a rectangle
 * \param b another rectangle
 * \return SCE_RECT_IN if \p b is inside \p a, SCE_RECT_INTERSECTS if \p a and
 *         \p b are crossed and SCE_RECT_OUT if the two rectangles has no
 *          intersection.
 * \see SCE_Rectangle_Intersects()
 */
int SCE_Rectangle_Intersectsf (SCE_SFloatRect *a, SCE_SFloatRect *b)
{
    SCE_TVector2 start;
    SCE_TVector2 end;
    SCE_SFloatRect r;
    
    start[0] = MAX (a->p1[0], b->p1[0]);
    start[1] = MAX (a->p1[1], b->p1[1]);
    end[0] = MIN (a->p2[0], b->p2[0]);
    end[1] = MIN (a->p2[1], b->p2[1]);
    r.p1[0] = start[0];
    r.p1[1] = start[1];
    r.p2[0] = end[0];
    r.p2[1] = end[1];
    
    if ((start[0] > end[0]) || (start[1] > end[1]))
        return SCE_RECT_OUT;
    else if (SCE_Rectangle_Equalf (&r, a) || SCE_Rectangle_Equalf (&r, b))
        return SCE_RECT_IN;
    else
        return SCE_RECT_INTERSECTS;
}


void SCE_Rectangle3_Union (const SCE_SIntRect3 *r1, const SCE_SIntRect3 *r2,
                           SCE_SIntRect3 *r)
{
    r->p1[0] = MIN (r1->p1[0], r2->p1[0]);
    r->p1[1] = MIN (r1->p1[1], r2->p1[1]);
    r->p1[2] = MIN (r1->p1[2], r2->p1[2]);

    r->p2[0] = MAX (r1->p2[0], r2->p2[0]);
    r->p2[1] = MAX (r1->p2[1], r2->p2[1]);
    r->p2[2] = MAX (r1->p2[2], r2->p2[2]);
}

int SCE_Rectangle3_Intersection (const SCE_SIntRect3 *r1,
                                 const SCE_SIntRect3 *r2, SCE_SIntRect3 *r)
{
    r->p1[0] = MAX (r1->p1[0], r2->p1[0]);
    r->p1[1] = MAX (r1->p1[1], r2->p1[1]);
    r->p1[2] = MAX (r1->p1[2], r2->p1[2]);

    r->p2[0] = MIN (r1->p2[0], r2->p2[0]);
    r->p2[1] = MIN (r1->p2[1], r2->p2[1]);
    r->p2[2] = MIN (r1->p2[2], r2->p2[2]);

    return r->p2[0] > r->p1[0] &&
           r->p2[1] > r->p1[1] &&
           r->p2[2] > r->p1[2];
}

/**
 * \brief Checks whether a rectangle is inside another
 * \param r1 a rectangle
 * \param r2 another one.
 * \returns SCE_TRUE if \p r2 is inside or perfectly match \p r1,
 * SCE_FALSE otherwise
 */
int SCE_Rectangle3_IsInside (const SCE_SIntRect3 *r1, const SCE_SIntRect3 *r2)
{
    return r1->p1[0] <= r2->p1[0] &&
           r1->p1[1] <= r2->p1[1] &&
           r1->p1[2] <= r2->p1[2] &&
           r1->p2[0] >= r2->p2[0] &&
           r1->p2[1] >= r2->p2[1] &&
           r1->p2[2] >= r2->p2[2];
}



void SCE_Rectangle3_Unionl (const SCE_SLongRect3 *r1, const SCE_SLongRect3 *r2,
                           SCE_SLongRect3 *r)
{
    r->p1[0] = MIN (r1->p1[0], r2->p1[0]);
    r->p1[1] = MIN (r1->p1[1], r2->p1[1]);
    r->p1[2] = MIN (r1->p1[2], r2->p1[2]);

    r->p2[0] = MAX (r1->p2[0], r2->p2[0]);
    r->p2[1] = MAX (r1->p2[1], r2->p2[1]);
    r->p2[2] = MAX (r1->p2[2], r2->p2[2]);
}

int SCE_Rectangle3_Intersectionl (const SCE_SLongRect3 *r1,
                                  const SCE_SLongRect3 *r2, SCE_SLongRect3 *r)
{
    r->p1[0] = MAX (r1->p1[0], r2->p1[0]);
    r->p1[1] = MAX (r1->p1[1], r2->p1[1]);
    r->p1[2] = MAX (r1->p1[2], r2->p1[2]);

    r->p2[0] = MIN (r1->p2[0], r2->p2[0]);
    r->p2[1] = MIN (r1->p2[1], r2->p2[1]);
    r->p2[2] = MIN (r1->p2[2], r2->p2[2]);

    return r->p2[0] > r->p1[0] &&
           r->p2[1] > r->p1[1] &&
           r->p2[2] > r->p1[2];
}

/**
 * \brief Checks whether a rectangle is inside another
 * \param r1 a rectangle
 * \param r2 another one.
 * \returns SCE_TRUE if \p r2 is inside or perfectly match \p r1,
 * SCE_FALSE otherwise
 */
int SCE_Rectangle3_IsInsidel (const SCE_SLongRect3 *r1, const SCE_SLongRect3 *r2)
{
    return r1->p1[0] <= r2->p1[0] &&
           r1->p1[1] <= r2->p1[1] &&
           r1->p1[2] <= r2->p1[2] &&
           r1->p2[0] >= r2->p2[0] &&
           r1->p2[1] >= r2->p2[1] &&
           r1->p2[2] >= r2->p2[2];
}


void SCE_Rectangle3_Pow2 (SCE_SIntRect3 *r, int exponent)
{
    size_t i;
    if (exponent < 0) for (i = 0; i < 3; i++) {
        r->p1[i] >>= -exponent;
        r->p2[i] >>= -exponent;
    } else for (i = 0; i < 3; i++) {
        r->p1[i] <<= exponent;
        r->p2[i] <<= exponent;
    }
}

void SCE_Rectangle3_Pow2l (SCE_SLongRect3 *r, int exponent)
{
    size_t i;
    if (exponent < 0) for (i = 0; i < 3; i++) {
        r->p1[i] >>= -exponent;
        r->p2[i] >>= -exponent;
    } else for (i = 0; i < 3; i++) {
        r->p1[i] <<= exponent;
        r->p2[i] <<= exponent;
    }
}


/**
 * \brief Splits a rectangle into two subrectangles
 * \param r a rectangle
 * \param s split offset, must be less than SCE_Rectangle3_GetWidth()
 * \param a inferior part
 * \param b superior part
 */
void SCE_Rectangle3_SplitWidth (const SCE_SIntRect3 *r, SCEuint s,
                                SCE_SIntRect3 *a, SCE_SIntRect3 *b)
{
    *a = *r;
    *b = *r;
    a->p2[0] -= s;
    b->p1[0] += s;
}
/**
 * \brief Splits a rectangle into two subrectangles
 * \param r a rectangle
 * \param s split offset, must be less than SCE_Rectangle3_GetHeight()
 * \param a inferior part
 * \param b superior part
 */
void SCE_Rectangle3_SplitHeight (const SCE_SIntRect3 *r, SCEuint s,
                                 SCE_SIntRect3 *a, SCE_SIntRect3 *b)
{
    *a = *r;
    *b = *r;
    a->p2[1] -= s;
    b->p1[1] += s;
}
/**
 * \brief Splits a rectangle into two subrectangles
 * \param r a rectangle
 * \param s split offset, must be less than SCE_Rectangle3_GetDepth()
 * \param a inferior part
 * \param b superior part
 */
void SCE_Rectangle3_SplitDepth (const SCE_SIntRect3 *r, SCEuint s,
                                SCE_SIntRect3 *a, SCE_SIntRect3 *b)
{
    *a = *r;
    *b = *r;
    a->p2[2] -= s;
    b->p1[2] += s;
}

void SCE_Rectangle3_SplitMax (const SCE_SIntRect3 *r, SCE_SIntRect3 *a,
                              SCE_SIntRect3 *b)
{
    SCEulong w, h, d;
    w = SCE_Rectangle3_GetWidth (r);
    h = SCE_Rectangle3_GetHeight (r);
    d = SCE_Rectangle3_GetDepth (r);
    if (w >= h && w >= d)
        SCE_Rectangle3_SplitWidth (r, w / 2, a, b);
    else if (h >= w && h >= d)
        SCE_Rectangle3_SplitHeight (r, h / 2, a, b);
    else
        SCE_Rectangle3_SplitDepth (r, d / 2, a, b);
}


/**
 * \brief Splits a rectangle into two subrectangles
 * \param r a rectangle
 * \param s split offset, must be less than SCE_Rectangle3_GetWidthl()
 * \param a inferior part
 * \param b superior part
 */
void SCE_Rectangle3_SplitWidthl (const SCE_SLongRect3 *r, SCEulong s,
                                 SCE_SLongRect3 *a, SCE_SLongRect3 *b)
{
    *a = *r;
    *b = *r;
    a->p2[0] -= s;
    b->p1[0] += s;
}
/**
 * \brief Splits a rectangle into two subrectangles
 * \param r a rectangle
 * \param s split offset, must be less than SCE_Rectangle3_GetHeightl()
 * \param a inferior part
 * \param b superior part
 */
void SCE_Rectangle3_SplitHeightl (const SCE_SLongRect3 *r, SCEulong s,
                                  SCE_SLongRect3 *a, SCE_SLongRect3 *b)
{
    *a = *r;
    *b = *r;
    a->p2[1] -= s;
    b->p1[1] += s;
}
/**
 * \brief Splits a rectangle into two subrectangles
 * \param r a rectangle
 * \param s split offset, must be less than SCE_Rectangle3_GetDepthl()
 * \param a inferior part
 * \param b superior part
 */
void SCE_Rectangle3_SplitDepthl (const SCE_SLongRect3 *r, SCEulong s,
                                 SCE_SLongRect3 *a, SCE_SLongRect3 *b)
{
    *a = *r;
    *b = *r;
    a->p2[2] -= s;
    b->p1[2] += s;
}

void SCE_Rectangle3_SplitMaxl (const SCE_SLongRect3 *r,
                               SCE_SLongRect3 *a, SCE_SLongRect3 *b)
{
    SCEulong w, h, d;
    w = SCE_Rectangle3_GetWidthl (r);
    h = SCE_Rectangle3_GetHeightl (r);
    d = SCE_Rectangle3_GetDepthl (r);
    if (w >= h && w >= d)
        SCE_Rectangle3_SplitWidthl (r, w / 2, a, b);
    else if (h >= w && h >= d)
        SCE_Rectangle3_SplitHeightl (r, h / 2, a, b);
    else
        SCE_Rectangle3_SplitDepthl (r, d / 2, a, b);
}


/** @} */
