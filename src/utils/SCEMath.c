/*------------------------------------------------------------------------------
    SCEngine - A 3D real time rendering engine written in the C language
    Copyright (C) 2006-2009  Antony Martin <martin(dot)antony(at)yahoo(dot)fr>

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
 
/* Cree le : 22 decembre 2006
   derniere modification le 22/09/2007 */

#include <SCE/utils/SCEMath.h>


/**
 * \file SCEMath.c
 * \copydoc math
 * \brief Mathematical operations
 * \file SCEMath.h
 * \copydoc math
 * \brief Mathematical operations
 */

/**
 * \defgroup math Mathematical operations
 * \ingroup utils
 * \brief Mathematical useful functions
 * \internal
 * 
 * Mathematical useful function not in the C standard library or specific
 * improvments of standrad functions.
 */

/** @{ */


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
    return sqrt (number);
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
    return 1.0F/pow (number, 0.5F);
#endif
}


/* ajoute le 22/09/2007 */
/**
 * \brief Power
 * \param x a number
 * \param n the exposant
 * \returns x^n
 * 
 * This function computes the power of a number. This function do the same as
 * the C standard pow() function except that it works only with integers.
 * With integers, this function as the advantage of speed except with exposant
 * 2.
 * 
 * \note For a x^2 computing, prefer use of the C standard pow() function that
 * is about 10 times faster.
 * 
 * \see pow()
 */
int SCE_Math_Powi (int x, unsigned int n)
{
    if (n == 0)
        return 1;
    else if (n == 1)
        return x;
    else if (n%2)
        return x * SCE_Math_Powi (x*x, (n-1)/2);
    else
        return SCE_Math_Powi (x*x, n/2);
}

/**
 * \brief Checks if a number is power of two
 * \param n a number
 * \returns 0 if \p n is non-power of two, or the exposant if it is.
 * 
 * Checks if the given number is power of two and return the exposant if it is.
 */
int SCE_Math_PowerOfTwo (int n)
{
    int power = 0;

    while (1)
    {
        if (n % 2)
            break;
        else
        {
            n /= 2;
            power++;
        }
    }

    if (n == 1)
        return power;

    return 0;
}

/** @} */
