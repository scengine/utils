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
 
/* Cree le : 16/05/2007
   derniere modification : 10/04/2008 */

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

/* .. et matrice.. ? :D */


/* initialise une structure */
void SCE_Inert_Init (SCE_SInertVar*);

/* defini si la valeur devra s'accumuler avec Set */
void SCE_Inert_Accum (SCE_SInertVar*, int);

/* defini le coefficient d'inertie */
void SCE_Inert_SetCoefficient (SCE_SInertVar*, float);

/* demande une incrementation de la variable */
void SCE_Inert_Set (SCE_SInertVar*, float);
/* macro bourrin */
#define SCE_Inert_Operator(ivar, op, val)\
    ((ivar)->toadd op (val))
/* recupere la variable */
float SCE_Inert_Get (SCE_SInertVar*);

/* calcul une incrementation, avec inertie */
void SCE_Inert_Compute (SCE_SInertVar*);
/* meme effet que ComputeVar, mais sans inertie */
void SCE_Inert_Null (SCE_SInertVar*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
