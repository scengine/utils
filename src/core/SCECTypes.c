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
 
/* Cree le : 7 mars 2006
   derniere modification le 16/06/2007 */

#include <SCE/SCEMinimal.h>

/* CTypes.h est inclu dans Minimal.h */

size_t SCE_CSizeofType (SCEenum type)
{
    switch (type)
    {
    case SCE_DOUBLE:
        return sizeof (GLdouble);

    case SCE_FLOAT:
        return sizeof (GLfloat);

    case SCE_INT:
    case SCE_UNSIGNED_INT:
        return sizeof (GLint);

    case SCE_SHORT:
    case SCE_UNSIGNED_SHORT:
        return sizeof (GLshort);

    case SCE_BYTE:
    case SCE_UNSIGNED_BYTE:
        return sizeof (GLbyte);

#ifdef SCE_DEBUG
    default:
        Logger_Log (SCE_INVALID_ARG);
        Logger_LogMsg ("unknown data type");
#endif
    }

    return 0;
}


/* ajoute le 16/06/2007 */
int SCE_CGetInteger (SCEenum t)
{
    int v;
    glGetIntegerv (t, &v);
    return v;
}

/* ajoute le 16/06/2007 */
float SCE_CGetFloat (SCEenum t)
{
    float v;
    glGetFloatv (t, &v);
    return v;
}

/* ajoute le 16/06/2007 */
void SCE_CGetIntegerv (SCEenum t, int *v)
{
    glGetIntegerv (t, v);
}

/* ajoute le 16/06/2007 */
void SCE_CGetFloatv(SCEenum t, float *v)
{
    glGetFloatv (t, v);
}
