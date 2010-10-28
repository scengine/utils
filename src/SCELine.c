/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2010  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
   updated: 28/10/2010 */

#include "SCE/utils/SCELine.h"

/**
 * \file SCELine.c
 * \copydoc lines
 * \brief Mathematical lines managment
 * 
 * \file SCELine.h
 * \copydoc lines
 * \brief Mathematical lines managment
 */

/**
 * \defgroup lines Mathematical lines managment
 * \ingroup utils
 * \brief Mathematical lines managment
 */

/** @{ */

/**
 * \brief Constructs a line
 * \param line first memory to write to
 */
void SCE_Line_Init (SCE_SLine *line)
{
    SCE_Vector2_Operator1 (line->a, =, 0.0f);
    SCE_Vector2_Operator1 (line->b, =, 0.0f);
}

/**
 * \brief Sets a line's coordinates
 * \param line first line to write to
 * \param x1 second first point's x coordinate
 * \param y1 third first point's y coordinate
 * \param x2 fourth second point's x coordinate
 * \param y2 fifth second point's y coordinate
 */
void SCE_Line_Set (SCE_SLine *line, float x1, float y1, float x2, float y2)
{
    line->a[0] = x1; line->a[1] = y1;
    line->b[0] = x2; line->b[1] = y2;
}
/**
 * \brief Sets a line's coordinates from vectors
 * \param line first line to write to
 * \param v1 second first coordinates vector
 * \param v2 third second coordinates vectr
 */
void SCE_Line_Setv (SCE_SLine *line, SCE_TVector2 v1, SCE_TVector2 v2)
{
    SCE_Vector2_Operator1v (line->a, =, v1);
    SCE_Vector2_Operator1v (line->b, =, v2);
}

/**
 * \brief Moves the first point of a line by a given int offset
 * \param line first line to write to
 * \param x second x coordinate offset
 * \param y third y coordinate offset
 */
void SCE_Line_MovePoint1 (SCE_SLine *line, float x, float y)
{
    line->a[0] += x; line->a[1] += y;
}
/**
 * \brief Translates the first point of a line by a given vector
 * \param line first line to write to
 * \param v second vector coordinate offset
 */
void SCE_Line_MovePoint1v (SCE_SLine *line, float *v)
{
    SCE_Vector2_Operator1v (line->a, +=, v);
}
/**
 * \brief Moves the second point of a line by a given int offset
 * \param line first line to write to
 * \param x second x coordinate offset
 * \param y third y coordinate offset
 */
void SCE_Line_MovePoint2 (SCE_SLine *line, float x, float y)
{
    line->b[0] += x; line->b[1] += y;
}
/**
 * \brief Translates the second point of a line by a given vector
 * \param line first line to write to
 * \param v second vector coordinate offset
 */
void SCE_Line_MovePoint2v (SCE_SLine *line, float *v)
{
    SCE_Vector2_Operator1v (line->b, +=, v);
}

/**
 * \brief Checks out if a x coordinate belongs to an interval made of
 * the x coordinates of the line's points
 * \param line first line to check in
 * \param x second x coordinate to check
 */
int SCE_Line_IsInXInterval (SCE_SLine *line, float x)
{
    if (line->b[0] < line->a[0])
        return (line->b[0] <= x && x <= line->a[0]);
    else
        return (line->a[0] <= x && x <= line->b[0]);
}
/**
 * \brief Checks out if a y coordinate belongs to an interval made of
 * the y coordinates of the line's points
 * \param line first line to check in
 * \param y second y coordinate to check
 */
int SCE_Line_IsInYInterval (SCE_SLine *line, float y)
{
    if (line->b[1] < line->a[1])
        return (line->b[1] <= y && y <= line->a[1]);
    else
        return (line->a[1] <= y && y <= line->b[1]);
}

/**
 * \brief Stores the paramaters of the line's equation into a and b (ax + b)
 * \param line first line
 * \param a second first parameter
 * \param b third second parameter
 */
void SCE_Line_GetEquation (SCE_SLine *line, float *a, float *b)
{
    /* a = y1 - y2 / x1 - x2
     * b = y1 - ax1 = y2 - ax2
     */
    *a = line->a[0] - line->b[0];
    if (*a != 0.0f)
        *a = (line->a[1] - line->b[1]) / (line->a[0] - line->b[0]);
    else
        *a = /* inf~ */4000000000.0;
    *b = line->b[1] - *a * line->b[0];
}

/**
 * \brief Checks if two lines have an intersection point on the map.
 * Stores this point's coordinates into x and y if the intersection is a point.
 * \param l1 first first line
 * \param l2 second second line
 * \param x third intersection point's x coordinate (if exists)
 * \param y fourth intersection point's y coordinate (if exists)
 * \return 0 if there is an intersection, SCE_LINE_COLINEAR if the two lines
 * are colinear, or SCE_LINE_EQUAL if the lines are the same
 */
int SCE_Line_GetIntersection (SCE_SLine *l1, SCE_SLine *l2, float *x, float *y)
{
    float a, b, c, d; /* constantes des fonctions des droites de l1 & l2 */

    SCE_Line_GetEquation (l1, &a, &b);
    SCE_Line_GetEquation (l2, &c, &d);

    if (SCE_Math_Fabsf (a - c) > 0.0) {
        *x = (d - b) / (a - c); /* ax + b = cx + d */
        *y = a*(*x) + b;        /* y = ax + b */
        return 0;
    }
    else if (SCE_Math_Fabsf (d - b) > 0.0)
        return SCE_LINE_COLINEAR;
    else
        return SCE_LINE_EQUAL;
}
int SCE_Line_GetIntersectionv (SCE_SLine *l1, SCE_SLine *l2, SCE_TVector2 p)
{
    return SCE_Line_GetIntersection (l1, l2, &p[0], &p[1]);
}

/**
 * \brief Checks if two lines have intersection point(s).
 * \param l1 first first line
 * \param l2 second second line
 * \return 1 if there is an intersection, 0 otherwise
 */
int SCE_Line_Intersects (SCE_SLine *l1, SCE_SLine *l2)
{
    float x, y;

    switch (SCE_Line_GetIntersection (l1, l2, &x, &y)) {
    case 0:
        /* x doit etre compris dans l'intervalle des deux segments */
        return (SCE_Line_IsInXInterval(l1, x) && SCE_Line_IsInXInterval(l2, x)&&
                SCE_Line_IsInYInterval(l1, y) && SCE_Line_IsInYInterval(l2, y));
    case SCE_LINE_EQUAL:
        return 1;
    }
    return 0;
}



void SCE_Line3_Init (SCE_SLine3 *l)
{
    SCE_Vector3_Set (l->o, 0.0, 0.0, 0.0);
    SCE_Vector3_Set (l->n, 1.0, 0.0, 0.0);
}
/**
 * \brief Makes the line that cross two given points
 * \param l the line to make
 * \param a,b two points
 */
void SCE_Line3_Set (SCE_SLine3 *l, SCE_TVector3 a, SCE_TVector3 b)
{
    SCE_Vector3_Copy (l->o, a);
    SCE_Vector3_Operator2v (l->n, =, b, -, a);
}
void SCE_Line3_SetOrigin (SCE_SLine3 *l, SCE_TVector3 v)
{
    SCE_Vector3_Copy (l->o, v);
}
void SCE_Line3_SetNormal (SCE_SLine3 *l, SCE_TVector3 v)
{
    SCE_Vector3_Copy (l->n, v);
}

/**
 * \brief Applies a matrix to a line and store the result into another
 * \param a store the result here
 * \param b the matrix will be applyied to this line
 * \param m a matrix
 * \sa SCE_Line3_MulCopy()
 */
void SCE_Line3_Mul (SCE_SLine3 *a, SCE_SLine3 *b, SCE_TMatrix4x3 m)
{
    SCE_TMatrix3 rot;
    SCE_TVector3 trans;

    SCE_Matrix3_CopyM4x3 (rot, m);
    SCE_Matrix3_MulV3 (rot, b->n, a->n);

    SCE_Matrix4x3_GetTranslation (m, trans);
    SCE_Vector3_Operator2v (a->o, =, b->o, +, trans);
}
/**
 * \brief Applies a matrix to a line
 * \param l store the result here
 * \param m a matrix
 * \sa SCE_Line3_Mul()
 */
void SCE_Line3_MulCopy (SCE_SLine3 *l, SCE_TMatrix4x3 m)
{
    SCE_TMatrix3 rot;
    SCE_TVector3 trans;

    SCE_Matrix3_CopyM4x3 (rot, m);
    SCE_Matrix3_MulV3Copy (rot, l->n);

    SCE_Matrix4x3_GetTranslation (m, trans);
    SCE_Vector3_Operator1v (l->o, +=, trans);
}

/** @} */
