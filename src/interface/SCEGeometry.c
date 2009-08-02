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

/* created: 25/07/2009
   updated: 01/08/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEResource.h>
#include <SCE/interface/SCEGeometry.h>

static int is_init = SCE_FALSE;
static int resource_type = 0;

int SCE_Init_Geometry (void)
{
    if (is_init)
        return SCE_OK;
    resource_type = SCE_Resource_RegisterType (SCE_TRUE, NULL, NULL);
    if (resource_type < 0)
        goto fail;
    is_init = SCE_TRUE;
    return SCE_OK;
fail:
    SCEE_LogSrc ();
    SCEE_LogSrcMsg ("failed to initialize geometry module");
    return SCE_ERROR;
}
void SCE_Quit_Geometry (void)
{
    resource_type = 0;
    is_init = SCE_FALSE;
}

int SCE_Geometry_GetResourceType (void)
{
    return resource_type;
}

static void SCE_Geometry_FreeArrayUser (void *auser)
{
    SCE_Geometry_ClearArrayUser (auser);
}
void SCE_Geometry_InitArray (SCE_SGeometryArray *array)
{
    SCE_CInitVertexArrayData (&array->data);
    array->canfree_data = SCE_FALSE;
    SCE_List_InitIt (&array->it);
    SCE_List_SetData (&array->it, array);
    SCE_List_Init (&array->users);
    SCE_List_SetFreeFunc (&array->users, SCE_Geometry_FreeArrayUser);
    array->range[0] = array->range[1] = 0;
    array->rangeptr = NULL;
    array->geom = NULL;
}
SCE_SGeometryArray* SCE_Geometry_CreateArray (void)
{
    SCE_SGeometryArray *array = NULL;
    if (!(array = SCE_malloc (sizeof *array)))
        SCEE_LogSrc ();
    else
        SCE_Geometry_InitArray (array);
    return array;
}
SCE_SGeometryArray*
SCE_Geometry_CreateArrayFrom (SCE_CVertexAttributeType attrib, int type,
                              int size, void *data, int canfree)
{
    SCE_SGeometryArray *array = NULL;
    if (!(array = SCE_Geometry_CreateArray ()))
        SCEE_LogSrc ();
    else
        SCE_Geometry_SetArrayData (array, attrib, type, size, data, canfree);
    return array;
}
void SCE_Geometry_DeleteArray (SCE_SGeometryArray *array)
{
    if (array) {
        if (array->geom)
            SCE_List_Removel (&array->it);
        if (array->canfree_data)
            SCE_free (array->data.data);
        SCE_List_Clear (&array->users);
        SCE_free (array);
    }
}
/**
 * \brief Copies \p a2 into \p a1, it doesn't copy users of \p a2 and doesn't
 * put \p a1 into the geometry of \p a2
 */
void SCE_Geometry_CopyArray (SCE_SGeometryArray *a1, SCE_SGeometryArray *a2)
{
    a1->data = a2->data;
}

void SCE_Geometry_InitArrayUser (SCE_SGeometryArrayUser *auser)
{
    auser->array = NULL;
    auser->update = NULL;
    auser->arg = NULL;
    SCE_List_InitIt (&auser->it);
    SCE_List_SetData (&auser->it, auser);
}
SCE_SGeometryArrayUser* SCE_Geometry_CreateArrayUser (void)
{
    SCE_SGeometryArrayUser *auser = NULL;
    if (!(auser = SCE_malloc (sizeof *auser)))
        SCEE_LogSrc ();
    else
        SCE_Geometry_InitArrayUser (auser);
    return auser;
}
void SCE_Geometry_ClearArrayUser (SCE_SGeometryArrayUser *auser)
{
    if (auser->array) {
        SCE_List_Removel (&auser->it);
        auser->array = NULL;
    }
}
void SCE_Geometry_DeleteArrayUser (SCE_SGeometryArrayUser *auser)
{
    if (auser) {
        SCE_Geometry_ClearArrayUser (auser);
        SCE_free (auser);
    }
}

static void SCE_Geometry_FreeArray (void *array)
{
    SCE_Geometry_DeleteArray (array);
}
static void SCE_Geometry_Init (SCE_SGeometry *geom)
{
    SCE_List_Init (&geom->arrays);
    SCE_List_SetFreeFunc (&geom->arrays, SCE_Geometry_FreeArray);
    SCE_List_Init (&geom->modified);
    SCE_List_SetFreeFunc (&geom->modified, SCE_Geometry_FreeArray);
    geom->index_array = NULL;
    geom->canfree_index = SCE_FALSE;
    geom->n_vertices = geom->n_indices = 0;
}
SCE_SGeometry* SCE_Geometry_Create (void)
{
    SCE_SGeometry *geom = NULL;
    if (!(geom = SCE_malloc (sizeof *geom)))
        SCEE_LogSrc ();
    else
        SCE_Geometry_Init (geom);
    return geom;
}
static void SCE_Geometry_DeleteIndexArray (SCE_SGeometry *geom)
{
    if (geom->canfree_index)
        SCE_Geometry_DeleteArray (geom->index_array);
}
void SCE_Geometry_Delete (SCE_SGeometry *geom)
{
    if (geom) {
        SCE_List_Clear (&geom->arrays);
        SCE_List_Clear (&geom->modified);
        SCE_Geometry_DeleteIndexArray (geom);
        SCE_free (geom);
    }
}

/**
 * \brief Gets the geometry array that \p auser uses
 */
SCE_SGeometryArray* SCE_Geometry_GetUserArray (SCE_SGeometryArrayUser *auser)
{
    return auser->array;
}

/**
 * \brief Adds an user to a geometry array
 * \param u the user to add
 * \brief fun callback called when \p array is updated
 *
 * The added pointer will never be freed by SCE_Geometry_DeleteArray() or
 * any other function of the geometry module.
 * \sa SCE_Geometry_RemoveUser(), SCE_Geometry_Modified(),
 * SCE_Geometry_Update(), SCE_Geometry_UpdateArray()
 */
void SCE_Geometry_AddUser (SCE_SGeometryArray *array, SCE_SGeometryArrayUser *u,
                           SCE_FUpdateGeometryArray fun, void *arg)
{
#ifdef SCE_DEBUG
    if (!fun) {
        SCEE_SendMsg ("you must specify an update function");
    }
#endif
    SCE_List_Appendl (&array->users, &u->it);
    u->update = fun;
    u->arg = arg;
}
/**
 * \brief Removes an user from its array
 * \sa SCE_Geometry_AddUser()
 */
void SCE_Geometry_RemoveUser (SCE_SGeometryArrayUser *auser)
{
    SCE_List_Removel (&auser->it);
    auser->array = NULL;
}

/**
 * \brief Defines an array as modified
 * \param range range of modified vertices, [0] is the first modified vertex
 * and [1] the number of modified vertices, if NULL the whole buffer data will
 * be updated.
 *
 * Stores the given geometry array in the modified arrays list of the array's
 * geometry. The \p array user update callback is called when \p array is
 * updated with SCE_Geometry_UpdateArray().
 * \sa SCE_Geometry_Update(), SCE_Geometry_UpdateArray(),
 * SCE_Geometry_AddUser(), SCE_CModifiedVertexBufferData()
 */
void SCE_Geometry_Modified (SCE_SGeometryArray *array, size_t *range)
{
    if (!range)
        array->rangeptr = NULL;
    else {
        array->range[0] = range[0];
        array->range[1] = range[1];
        array->rangeptr = array->range;
    }
    SCE_List_Removel (&array->it);
    SCE_List_Appendl (&array->geom->modified, &array->it);
}
/**
 * \brief Updates an array
 *
 * This function calls all the update callback of each \p arrays's user. It is
 * recommanded to use SCE_Geometry_Modified() and then update the whole
 * geometry by calling SCE_Geometry_Update().
 * \sa SCE_Geometry_Modified(), SCE_Geometry_AddUser(), SCE_Geometry_Update(),
 * SCE_CMod()
 */
void SCE_Geometry_UpdateArray (SCE_SGeometryArray *array)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, &array->users) {
        SCE_SGeometryArrayUser *auser = SCE_List_GetData (it);
        auser->update (auser->arg, array->rangeptr);
    }
    /* move the array back to the main geometry's array list */
    SCE_List_Removel (&array->it);
    SCE_List_Appendl (&array->geom->arrays, &array->it);
}
/**
 * \brief Updates a geometry
 *
 * Calls SCE_Geometry_UpdateArray() for each array of \p geom.
 * \sa SCE_Geomoetry_UpdateArray()
 */
void SCE_Geometry_Update (SCE_SGeometry *geom)
{
    SCE_SListIterator *it = NULL;
    SCE_List_ForEach (it, &geom->modified)
        SCE_Geometry_UpdateArray (SCE_List_GetData (it));
}


/**
 * \brief Sets data for an array
 * \param attrib vertex attribute of the data
 * \param type data type (SCE_FLOAT, etc. SCE_VERTICES_TYPE is recommanded)
 * \param size vertex vector's length (usually 3 for position, 2 for texcoord)
 * \param data pointer to the vertex data
 * \param canfree can \p array free \p data when SCE_Geometry_DeleteArray()
 * or SCE_Geometry_Delete() is called?
 * \sa SCE_Geometry_SetArrayPosition(), SCE_Geometry_SetArrayTexCoord(),
 * SCE_Geometry_SetArrayNormal(), SCE_Geometry_SetArrayTangent(),
 * SCE_Geometry_SetArrayBinormal(), SCE_Geometry_SetArrayIndices(),
 * SCE_CVertexArrayData
 */
void SCE_Geometry_SetArrayData (SCE_SGeometryArray *array,
                                SCE_CVertexAttributeType attrib,
                                int type, int size, void *data, int canfree)
{
    SCE_CVertexArrayData *d = &array->data;
    d->attrib = attrib;
    d->type = type;
    d->size = size;
    d->data = data;
    array->canfree_data = canfree;
}
/**
 * \brief Calls SCE_Geometry_SetArrayData (\p array, SCE_POSITION,
 * SCE_VERTICES_TYPE, \p size, \p data, \p canfree).
 * \sa SCE_Geometry_SetArrayData(), SCE_Geometry_SetArrayNormal()
 */
void SCE_Geometry_SetArrayPosition (SCE_SGeometryArray *array, int size,
                                    SCEvertices *data, int canfree)
{
    SCE_Geometry_SetArrayData (array, SCE_POSITION, SCE_VERTICES_TYPE, size,
                               data, canfree);
}
/**
 * \brief Calls SCE_Geometry_SetArrayData (\p array, SCE_TEXCOORD0 + \p unit,
 * SCE_VERTICES_TYPE, \p size, \p data, \p canfree).
 * \sa SCE_Geometry_SetArrayData(), SCE_Geometry_SetArrayPosition()
 */
void SCE_Geometry_SetArrayTexCoord (SCE_SGeometryArray *array,
                                    unsigned int unit, int size,
                                    SCEvertices *data, int canfree)
{
    SCE_Geometry_SetArrayData (array, SCE_TEXCOORD0 + unit, SCE_VERTICES_TYPE,
                               size, data, canfree);
}
/**
 * \brief Calls SCE_Geometry_SetArrayData (\p array, SCE_NORMAL,
 * SCE_VERTICES_TYPE, 3, \p data, \p canfree).
 * \sa SCE_Geometry_SetArrayData(), SCE_Geometry_SetArrayPosition()
 */
void SCE_Geometry_SetArrayNormal (SCE_SGeometryArray *array, SCEvertices *data,
                                  int canfree)
{
    /* it is useless to specify the parameter 'size' (not used by the GL) */
    SCE_Geometry_SetArrayData (array, SCE_NORMAL, SCE_VERTICES_TYPE, 3,
                               data, canfree);
}
/**
 * \brief Calls SCE_Geometry_SetArrayData (\p array, SCE_TANGENT,
 * SCE_VERTICES_TYPE, 3, \p data, \p canfree).
 * \sa SCE_Geometry_SetArrayData(), SCE_Geometry_SetArrayNormal()
 */
void SCE_Geometry_SetArrayTangent (SCE_SGeometryArray *array, SCEvertices *data,
                                   int canfree)
{
    /* tangent are like normals, so force size of 3 */
    SCE_Geometry_SetArrayData (array, SCE_TANGENT, SCE_VERTICES_TYPE, 3,
                               data, canfree);
}
/**
 * \brief Calls SCE_Geometry_SetArrayData (\p array, SCE_BINORMAL,
 * SCE_VERTICES_TYPE, 3, \p data, \p canfree).
 * \sa SCE_Geometry_SetArrayData(), SCE_Geometry_SetArrayNormal()
 */
void SCE_Geometry_SetArrayBinormal (SCE_SGeometryArray *array, SCEvertices *data,
                                    int canfree)
{
    SCE_Geometry_SetArrayData (array, SCE_BINORMAL, SCE_VERTICES_TYPE, 3,
                               data, canfree);
}

/**
 * \brief Calls SCE_Geometry_SetArrayData (\p array, 42, SCE_INDICES_TYPE,
 * 1, \p data, \p canfree).
 *
 * I know this function is just weird, but it would be boring to make a new
 * structure type just for "index" arrays, like SCE_SGeometryIndexArray,
 * because it would mean that SCE_SGeometryArray is a wrong type name and
 * SCE_SGeometryVertexArray should be better.
 * \sa SCE_Geometry_SetArrayData(), SCE_Geometry_SetArrayNormal()
 */
void SCE_Geometry_SetArrayIndices (SCE_SGeometryArray *array, SCEindices *data,
                                   int canfree)
{
    SCE_Geometry_SetArrayData (array, 42, SCE_INDICES_TYPE, 1, data, canfree);
}

/**
 * \brief Gets the vertex data of an array
 * \sa SCE_Geometry_GetArrayData(), SCE_CVertexArrayData::data
 */
void* SCE_Geometry_GetData (SCE_SGeometryArray *array)
{
    return array->data.data;
}
/**
 * \brief Gets the vertex array of a geometry array
 * \sa SCE_Geometry_GetData(), SCE_CVertexArrayData
 */
SCE_CVertexArrayData* SCE_Geometry_GetArrayData (SCE_SGeometryArray *array)
{
    return &array->data;
}

/**
 * \brief Adds a vertex array to a geometry
 *
 * The pointer \p array will be freed when SCE_Geometry_Delete() called under
 * \p geom.
 * \sa SCE_Geometry_AddArrayDup(), SCE_Geometry_RemoveArray(),
 * SCE_Geometry_SetIndexArray()
 */
void SCE_Geometry_AddArray (SCE_SGeometry *geom, SCE_SGeometryArray *array)
{
    SCE_List_Appendl (&geom->arrays, &array->it);
    array->geom = geom;
}
/**
 * \brief Duplicates and adds an array to a geometry
 * \param canfree can the data pointer of \p array be freed by
 * SCE_Geometry_Delete() ?
 *
 * Does the same of SCE_Geometry_AddArray() except it duplicates the
 * geometry array before add it. The duplicated array is not authorized
 * to delete the vertex data of \p array (see SCE_Geometry_SetArrayData()).
 * The duplicated array hasn't any user (see SCE_Geometry_AddUser()).
 * \sa SCE_Geometry_AddArray(), SCE_Geometry_SetArrayData()
 */
SCE_SGeometryArray* SCE_Geometry_AddArrayDup (SCE_SGeometry *geom,
                                              SCE_SGeometryArray *array,
                                              int canfree)
{
    SCE_SGeometryArray *new = NULL;
    if (!(new = SCE_Geometry_CreateArray ()))
        SCEE_LogSrc ();
    else {
        SCE_Geometry_CopyArray (new, array);
        new->canfree_data = SCE_FALSE;
        SCE_Geometry_AddArray (geom, array);
    }
    return new;
}
/**
 * \todo Merge it with SCE_Geometry_SetIndexArrayDupDup() ?
 * \brief Duplicates an array and its vertex data and adds it to a geometry
 * \param array the array to duplicate
 * \param keep keep the data type as in \p array, otherwise they are converted to
 * an optimized data type (SCE_VERTICES_TYPE)
 * \note This function requires that the number of vertices have been yet
 * specified to \p geom (see SCE_Geometry_SetNumVertices()).
 * \sa SCE_Geometry_SetIndexArrayDupDup(), SCE_Geometry_SetData()
 */
SCE_SGeometryArray* SCE_Geometry_AddArrayDupDup (SCE_SGeometry *geom,
                                                 SCE_SGeometryArray *array,
                                                 int keep)
{
    SCE_SGeometryArray *new = NULL;
    void *newdata = NULL;
    SCEenum type = SCE_VERTICES_TYPE;
    if (keep) {
        type = array->data.type;
        newdata = SCE_Mem_Dup (array->data.data, geom->n_indices *
                               SCE_CSizeof(array->data.type)* array->data.size);
    } else {
        /* TODO: not according to definition of SCEvertices, may be greater */
        newdata = SCE_Mem_ConvertDup (type, array->data.type, array->data.data,
                                      geom->n_vertices * array->data.size);
    }
    if (!newdata)
        SCEE_LogSrc ();
    else {
        if (!(new = SCE_Geometry_CreateArray ())) {
            SCEE_LogSrc ();
            SCE_free (newdata);
        } else {
            new->data.data = newdata;
            new->data.type = type;
            new->canfree_data = SCE_TRUE;
            SCE_Geometry_AddArray (geom, new);
        }
    }
    return new;
}
/**
 * \brief Removes an array from its geometry
 * \sa SCE_Geometry_AddArray(), SCE_Geometry_AddArrayDup()
 */
void SCE_Geometry_RemoveArray (SCE_SGeometryArray *array)
{
    SCE_List_Removel (&array->it);
    array->geom = NULL;
}

/**
 * \brief Sets the index array of a geometry
 * \param array the index array to set, NULL if you want to remove the current
 * one of \p geom
 * \param canfree if SCE_TRUE, SCE_Geometry_Delete() will free \p array
 * \sa SCE_Geometry_AddArray(), SCE_Geometry_SetArrayData()
 */
void SCE_Geometry_SetIndexArray (SCE_SGeometry *geom, SCE_SGeometryArray *array,
                                 int canfree)
{
    SCE_Geometry_DeleteIndexArray (geom);
    geom->index_array = array;
    geom->canfree_index = (array ? canfree : SCE_FALSE);
}
/**
 * \brief Duplicates and set an index array
 * \returns the new array, duplicated from \p array
 *
 * Duplicates \p array but not its vertex data. \p canfree indicates if
 * SCE_Geometry_Delete() will free the vertex data of \p array.
 * \sa SCE_Geometry_SetIndexArray(), SCE_Geometry_GetIndexArray()
 */
SCE_SGeometryArray* SCE_Geometry_SetIndexArrayDup (SCE_SGeometry *geom,
                                                   SCE_SGeometryArray *array,
                                                   int canfree)
{
    SCE_SGeometryArray *new = NULL;
    if (!(new = SCE_Geometry_CreateArray ()))
        SCEE_LogSrc ();
    else {
        SCE_Geometry_CopyArray (new, array);
        new->canfree_data = canfree;
        SCE_Geometry_SetIndexArray (geom, new, SCE_TRUE);
    }
    return new;
}
/**
 * \brief Duplicates an index array and its data and set it to a geometry
 * \param array the index array to duplicate
 * \param keep keep data type as in \p array, otherwise they are converted to
 * an optimized data type (SCE_INDICES_TYPE)
 * \note This function requires that the number of indices have been yet
 * specified to \p geom (see SCE_Geometry_SetNumIndices()).
 * \sa SCE_Geomtry_AddArrayDupDup(), SCE_Geometry_SetIndexArrayDup(),
 * SCE_Geometry_SetIndexArray(), SCE_Geometry_SetArraDataDup()
 */
SCE_SGeometryArray* SCE_Geometry_SetIndexArrayDupDup (SCE_SGeometry *geom,
                                                      SCE_SGeometryArray *array,
                                                      int keep)
{
    SCE_SGeometryArray *new = NULL;
    void *newdata = NULL;
    SCEenum type = SCE_INDICES_TYPE;
    if (keep) {
        type = array->data.type;
        newdata = SCE_Mem_Dup (array->data.data, geom->n_indices *
                               SCE_CSizeof (array->data.type));
    } else {
        /* TODO: not according to definition of SCEindices, may be greater */
        newdata = SCE_Mem_ConvertDup (type, array->data.type, array->data.data,
                                      geom->n_indices);
    }
    if (!newdata)
        SCEE_LogSrc ();
    else {
        if (!(new = SCE_Geometry_CreateArray ())) {
            SCEE_LogSrc ();
            SCE_free (newdata);
        } else {
            new->data.data = newdata;
            new->data.type = type;
            new->canfree_data = SCE_TRUE;
            SCE_Geometry_SetIndexArray (geom, new, SCE_TRUE);
        }
    }
    return new;
}
/**
 * \brief Gets the index array of a geometry
 * \sa SCE_Geometry_SetIndexArray()
 */
SCE_SGeometryArray* SCE_Geometry_GetIndexArray (SCE_SGeometry *geom)
{
    return geom->index_array;
}


/**
 * \brief User-friendly function to quickly defined data of a geometry
 * \param pos,nor,tex vertex data, can be NULL if non-defined, but \p pos
 * must be given. all these pointer will be freed by SCE_Geometry_Delete().
 * \param index if specified, set as indices of the geometry
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * \sa SCE_Geometry_AddArray(), SCE_Geometry_SetIndexArray(),
 * SCE_Geometry_SetArrayData()
 */
int SCE_Geometry_SetData (SCE_SGeometry *geom, SCEvertices *pos,
                          SCEvertices *nor, SCEvertices *tex, SCEindices *index,
                          SCEuint n_vertices, SCEuint n_indices)
{
    int i;
    SCE_SGeometryArray array, *arrays[3];

    for (i = 0; i < 3; i++)
        arrays[i] = NULL;
    i = 0;
    SCE_Geometry_SetNumVertices (geom, n_vertices);
    SCE_Geometry_SetNumIndices (geom, n_indices);

    SCE_Geometry_InitArray (&array);
    SCE_Geometry_SetArrayPosition (&array, 3, pos, SCE_TRUE);
    if (!(arrays[i] = SCE_Geometry_AddArrayDup (geom, &array, SCE_TRUE)))
        goto fail;
    i++;
    if (nor) {
        SCE_Geometry_InitArray (&array);
        SCE_Geometry_SetArrayNormal (&array, nor, SCE_TRUE);
        if (!(arrays[i] = SCE_Geometry_AddArrayDup (geom, &array, SCE_TRUE)))
            goto fail;
        i++;
    }
    if (tex) {
        SCE_Geometry_InitArray (&array);
        SCE_Geometry_SetArrayTexCoord (&array, 0, 2, tex, SCE_TRUE);
        if (!(arrays[i] = SCE_Geometry_AddArrayDup (geom, &array, SCE_TRUE)))
            goto fail;
        i++;
    }
    if (index) {
        SCE_Geometry_InitArray (&array);
        SCE_Geometry_SetArrayData (&array, 0, SCE_INDICES_TYPE, 0, index,
                                   SCE_TRUE);
        if (!(arrays[i] = SCE_Geometry_SetIndexArrayDup (geom, &array,
                                                         SCE_TRUE)))
            goto fail;
        i++;
    }

    return SCE_OK;
fail:
    for (; i >= 0; i--) {
        arrays[i]->canfree_data = SCE_FALSE;
        SCE_Geometry_DeleteArray (arrays[i]);
    }
    SCEE_LogSrc ();
    return SCE_ERROR;
}


/**
 * \brief Sets primitive type of a geometry
 * \sa SCE_Geometry_GetPrimitiveType()
 */
void SCE_Geometry_SetPrimitiveType (SCE_SGeometry *geom, SCEenum prim)
{
    geom->prim = prim;
}
/**
 * \brief Gets primitive type of a geometry
 * \sa SCE_Geometry_SetPrimitiveType()
 */
SCEenum SCE_Geometry_GetPrimitiveType (SCE_SGeometry *geom)
{
    return geom->prim;
}

/**
 * \brief Sets the number of vertices of a geometry
 * \sa SCE_Geometry_GetNumvertices(), SCE_Geometry_SetNumIndices()
 */
void SCE_Geometry_SetNumVertices (SCE_SGeometry *geom, unsigned int n_vertices)
{
    geom->n_vertices = n_vertices;
}
/**
 * \brief Gets the number of vertices of a geometry
 * \sa SCE_Geometry_SetNumVertices(), SCE_Geometry_GetNumIndices()
 */
unsigned int SCE_Geometry_GetNumVertices (SCE_SGeometry *geom)
{
    return geom->n_vertices;
}
/**
 * \brief Sets the number of indices of a geometry
 * \sa SCE_Geometry_GetNumIndices(), SCE_Geometry_SetNumVertices()
 */
void SCE_Geometry_SetNumIndices (SCE_SGeometry *geom, unsigned int n_indices)
{
    geom->n_indices = n_indices;
}
/**
 * \brief Gets the number of indices of a geometry
 * \sa SCE_Geometry_SetNumIndices(), SCE_Geometry_GetNumVertices()
 */
unsigned int SCE_Geometry_GetNumIndices (SCE_SGeometry *geom)
{
    return geom->n_indices;
}

/**
 * \brief Gets the arrays of a geometry (not including those who are modified)
 *
 * To get all the arrays of a geometry, use also those returned by
 * SCE_Geometry_GetModifiedArrays().
 * \sa SCE_Geometry_GetModifiedArrays()
 */
SCE_SList* SCE_Geometry_GetArrays (SCE_SGeometry *geom)
{
    return &geom->arrays;
}
/**
 * \brief Gets the modified arrays of a geometry
 *
 * To get all the arrays of a geometry, use also those returned by
 * SCE_Geometry_GetArrays().
 * \sa SCE_Geometry_GetArrays()
 */
SCE_SList* SCE_Geometry_GetModifiedArrays (SCE_SGeometry *geom)
{
    return &geom->modified;
}

/**
 * \brief Is the given geometry modified since the last update?
 */
int SCE_Geometry_IsModified (SCE_SGeometry *geom)
{
    return SCE_List_HasElement (&geom->modified);
}

/**
 * \brief Loads geometry from a file
 * \param force force a new instance of the resource to be loaded
 * \sa SCE_Geometry_GetResourceType(), SCE_Resource_Load()
 */
SCE_SGeometry* SCE_Geometry_Load (const char *fname, int force)
{
    return SCE_Resource_Load (resource_type, fname, force, NULL);
}
