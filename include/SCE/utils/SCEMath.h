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
   derniere modification le 27/02/2008 */

#ifndef SCEMATH_H
#define SCEMATH_H

#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SCE_USE_SQRT_HACK 0

/* PI */
#ifndef M_PI
 #define M_PI 3.14159265358979323846
#endif

/* radian */
#ifndef RAD
 #define RAD 0.0174532925
#endif


#ifndef max
 #define max(a, b) (a > b ? a : b)
#endif

#ifndef min
 #define min(a, b) (a < b ? a : b)
#endif

/* calcul la racine carree d'un nombre */
float SCE_Math_Sqrt (float);
/* calcul l'inverse de la racine carree d'un nombre */
float SCE_Math_InvSqrt (float);

/* renvoie un entier puissance n */
int SCE_Math_Powi (int, unsigned int);

/* fonction renvoyant le nombre de la puissance de 2 a utiliser pour obtenir
   le chiffre donne en parametre.
   la fonction renvoie 0 si le nombre n'est pas un nombre puissance de 2 */
int SCE_Math_PowerOfTwo (int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
