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
 
/* created: 16/05/2007
   updated: 10/04/2008 */

#include "SCE/utils/SCEInert.h"

void SCE_Inert_Init (SCE_SInertVar *ivar)
{
    ivar->var = ivar->real = ivar->toadd = 0.0f;
    ivar->coeff = 1.0;
    ivar->accum = 0;
}


void SCE_Inert_Accum (SCE_SInertVar *ivar, int accum)
{
    ivar->accum = accum;
}

void SCE_Inert_SetCoefficient (SCE_SInertVar *ivar, float coeff)
{
    ivar->coeff = coeff;
}

void SCE_Inert_Set (SCE_SInertVar *ivar, float val)
{
    ivar->toadd = val;
}

float SCE_Inert_Get (SCE_SInertVar *ivar)
{
    return ivar->var;
}

void SCE_Inert_Compute (SCE_SInertVar *ivar)
{
    if (ivar->accum)
        ivar->real += ivar->toadd;
    else
        ivar->real = ivar->toadd;

    if (!ivar->accum && (ivar->toadd < 0.000002f && ivar->toadd > -0.000002f))
        ivar->real = 0.0f;

    ivar->var += (ivar->real - ivar->var) * ivar->coeff;

    ivar->toadd = 0.0f;
}

void SCE_Inert_Null (SCE_SInertVar *ivar)
{
    if (ivar->accum)
        ivar->var += ivar->toadd;
    else
        ivar->var = ivar->toadd;

    ivar->toadd = 0.0f;
}
