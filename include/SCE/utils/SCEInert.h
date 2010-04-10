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
 
/* created: 16/05/2007
   updated: 10/04/2008 */

#ifndef SCEINERT_H
#define SCEINERT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* simple variable avec inertie */
typedef struct sce_sinertvar SCE_SInertVar;
struct sce_sinertvar
{
    float var;          /* variable concernee */
    int accum;          /* booleen, true: accumulation des valeurs */
    float coeff;        /* taux d'inertie */
    /* TODO: kick this stupid variable */
    float toadd;        /* prochaine valeur a additionner a var */
    float real;         /* valeur reelle a laquelle devrait etre positionnee
                           var */
};

#if 0
/* vecteur avec inertie */
typedef struct sce_sinertvec SCE_SInertVec;
struct sce_sinertvec
{
    SCE_TVector3 vec;   /* vecteur concerne */
    float inert;        /* taux d'inertie (pourcentage de deplacement qui sera
                           effectue) */
    SCE_TVector3 toadd; /* prochaine valeur a additionner a var */
    SCE_TVector3 real;  /* valeur reelle a laquelle devrait etre positionnee
                           vec */
};
#endif


void SCE_Inert_Init (SCE_SInertVar*);

void SCE_Inert_Accum (SCE_SInertVar*, int);

void SCE_Inert_SetCoefficient (SCE_SInertVar*, float);

/* TODO: add real function: GetInterpolated(), GetReal() */

void SCE_Inert_Set (SCE_SInertVar*, float);
#define SCE_Inert_Operator(ivar, op, val)\
    ((ivar)->toadd op (val))
float SCE_Inert_Get (SCE_SInertVar*);

void SCE_Inert_Compute (SCE_SInertVar*);
void SCE_Inert_Null (SCE_SInertVar*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
