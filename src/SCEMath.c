/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2012  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
 
/* created: 22/12/2006
   updated: 17/03/2012 */

#include <math.h>

#include "SCE/utils/SCEMath.h"

/**
 * \file SCEMath.c
 * \copydoc math
 * \brief Mathematical operations
 * \file SCEMath.h
 * \copydoc math
 * \brief Mathematical operations
 */

/**
 * \defgroup math Maths
 * \ingroup utils
 * \brief Mathematical useful functions
 * \internal
 * 
 * Mathematical useful function not in the C standard library or specific
 * improvments of standard functions.
 */

/** @{ */

/**
 * \brief Checks whether a number is zero or non-zero.
 * \param number a number
 * \return SCE_TRUE if \p number is zero, SCE_FALSE otherwise
 */
int SCE_Math_IsZero (float number)
{
    return SCE_Math_Fabsf (number) < SCE_EPSILONF;
}

/**
 * \brief Computes the square root of the given number
 * \param number a number
 * \returns the square root of the given number
 * 
 * \note This function is slower than the C standard sqrt() function if it
 * hasn't be compiled with SCE_USE_SQRT_HACK.
 * \note When compiled with SCE_USE_SQRT_HACK this function do an approximation.
 */
float SCE_Math_Sqrt (float number)
{
#if SCE_USE_SQRT_HACK
    /* Copyright (C) id software (Quake 3 arena)
       license : GPL */
    return 1.0F/SCE_Math_InvSqrt (number);
#else
# ifdef HAVE_SQRTF
    return sqrtf (number);
#else /* !HAVE_SQRTF */
    return (float)sqrt (number);
#endif /* HAVE_SQRTF */
#endif
}

/**
 * \brief Computes the inverted square root of the given number
 * \param number a number
 * \returns the inverted square root of the given number
 * 
 * \note This function is slower than (1.0f / sqrt()) if it has'nt be compiled
 * with SCE_USE_SQRT_HACK.
 * \note When compiled with SCE_USE_SQRT_HACK this function do an approximation.
 */
float SCE_Math_InvSqrt (float number)
{
#if SCE_USE_SQRT_HACK
    /* Copyright (C) id software (Quake 3 arena)
       license : GPL */
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;  /* evil floating point bit level hacking */
    i  = 0x5f3759df/*a86*/ - ( i >> 1 ); /* what the fuck? */
    y  = * ( float * ) &i;
    /* 1st iteration */
    y  = y * ( threehalfs - ( x2 * y * y ) );
    /* 2nd iteration, this can be removed */
    y  = y * ( threehalfs - ( x2 * y * y ) );

    return y;
#else
    return 1.0F/SCE_Math_Powf (number, 0.5F);
#endif
}


/**
 * \brief Exponentiation by squaring (fast exponentiation)
 * \param x a number
 * \param n the exposant
 * \returns x^n
 * \todo powi is crap for a func name.
 * 
 * This function computes the power of a number. This function do the same as
 * the C standard pow() function except that it works only with integers.
 * With integers, this function has the advantage of speed except with exposant
 * 2.
 * 
 * \note For a x^2 computing, prefer use of the C standard pow() function that
 * is about 10 times faster.
 * 
 * \sa pow()
 */
int SCE_Math_Powi (int x, unsigned int n)
{
    unsigned int d;
    int m = 1;
    if (!n)
        return 1;
    if (n == 1)
        return x;
    while (n > 1) {
        d = n / 2;
        if (2 * d != n)
            m *= x;
        n = d;
        x *= x;
    }
    return x * m;
}

/**
 * \brief Checks if a number is power of two
 * \param n a number
 * \returns 0 if \p n is non-power of two, or the exposant if it is.
 * 
 * Checks if the given number is power of two and return the exposant if it is.
 */
int SCE_Math_PowerOfTwo (unsigned int n)
{
    return (!(n & (n - 1)) && n);
}

/**
 * \brief Nearest upper power of two of a number
 * \param n a number
 * \return \p n if \p n is a power of two, the nearest upper power of
 * two otherwise
 */
unsigned int SCE_Math_NextPowerOfTwo (unsigned int n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

/**
 * \brief Clamp \p a value between \p min and \p max
 * \sa SCE_Math_Clampf()
 */
int SCE_Math_Clamp (int a, int min, int max)
{
    if (a < min) return min;
    else if (a > max) return max;
    else return a;
}
/**
 * \brief Clamp \p a value between \p min and \p max
 * \sa SCE_Math_Clamp()
 */
float SCE_Math_Clampf (float a, float min, float max)
{
    if (a < min) return min;
    else if (a > max) return max;
    else return a;
}


/**
 * \brief Returns the decimal part of a number
 * \param x a number
 * \return \p x - SCE_Math_Floorf (\p x)
 */
float SCE_Math_Fractf (float x)
{
    return x - SCE_Math_Floorf (x);
}


/**
 * \brief Gets the class of a number in a ring
 * \param x a value
 * \param n another value
 * \return the class of \p x in Z/nZ
 */
int SCE_Math_Ring (int x, int n)
{
    return ((x % n) + n) % n;
}


/** @} */
