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

/* created: 06/04/2009
   updated: 13/05/2009 */

#include <SCE/SCEMinimal.h>

#include <SCE/utils/SCEResources.h>
#include <SCE/interface/SCEMD5Loader.h>
#include <SCE/interface/SCEAnimatedMesh.h>

/**
 * \defgroup animmesh Animated mesh
 * \ingroup interface
 * \internal
 * \brief Skeletal animated meshs
 */

/** @{ */

#define SCE_ANIMMESH_ENABLE_QUAT_TRANSFORM 0

static int is_init = SCE_FALSE;
static int media_type = 0;

/**
 * \internal
 * \brief Initializes the animated mesh manager
 */
int SCE_Init_AnimMesh (void)
{
    SCE_btstart ();
    if (!is_init)
    {
        media_type = SCE_Media_GenTypeID ();
        SCE_Media_RegisterLoader (media_type, 0, "."SCE_MD5MESH_FILE_EXTENSION,
                                  SCE_idTechMD5_LoadMesh);
    }
    SCE_btend ();
    return SCE_OK;
}

void SCE_Quit_AnimMesh (void)
{
    SCE_btstart ();
    SCE_btend ();
}


static void SCE_AnimMesh_InitVertex (SCE_SVertex *vert)
{
    vert->weight_id = 0;
    vert->weight_count = 0;
}

static void SCE_AnimMesh_InitWeight (SCE_SVertexWeight *weight)
{
    weight->weight = 0.0;
    weight->joint_id = 0;
    weight->next_vertex_id = 0;
}

static void SCE_AnimMesh_ApplySkeletonP (SCE_SAnimatedMesh*, SCE_SSkeleton*);

/**
 * \internal
 * \brief Initializes an animated mesh structure
 */
static void SCE_AnimMesh_Init (SCE_SAnimatedMesh *amesh)
{
    unsigned int i;
    amesh->mesh = NULL;
    amesh->animskel = amesh->baseskel = NULL;
    amesh->canfree_animskel = amesh->canfree_baseskel = SCE_FALSE;
    amesh->vertices = NULL;
    amesh->weights = NULL;
    amesh->n_vertices = 0;
    amesh->n_weights = 0;
    for (i = 0; i < SCE_MAX_ANIMATED_VERTEX_ATTRIBUTES; i++)
    {
        amesh->base[i] = NULL;
        amesh->local[i] = SCE_FALSE;
        amesh->output[i] = NULL;
        amesh->size[i] = 3;
    }
    amesh->applyskel = SCE_AnimMesh_ApplySkeletonP;
}

/**
 * \brief Creates a new animated mesh
 */
SCE_SAnimatedMesh* SCE_AnimMesh_Create (void)
{
    SCE_SAnimatedMesh *amesh = NULL;
    SCE_btstart ();
    if (!(amesh = SCE_malloc (sizeof *amesh)))
        Logger_LogSrc ();
    else
        SCE_AnimMesh_Init (amesh);
    SCE_btend ();
    return amesh;
}

/**
 * \brief Deletes an animated mesh
 */
void SCE_AnimMesh_Delete (SCE_SAnimatedMesh *amesh)
{
    if (amesh)
    {
        unsigned int i;
        for (i = 0; i < SCE_MAX_ANIMATED_VERTEX_ATTRIBUTES; i++)
            SCE_free (amesh->base[i]);

        if (amesh->canfree_baseskel)
            SCE_Skeleton_Delete (amesh->baseskel);
        if (amesh->canfree_animskel)
            SCE_Skeleton_Delete (amesh->animskel);

        SCE_free (amesh);
    }
}


/**
 * \brief Sets the bind pose skeleton of an animated mesh
 */
void SCE_AnimMesh_SetBaseSkeleton (SCE_SAnimatedMesh *amesh,
                                   SCE_SSkeleton *skel, int canfree)
{
    if (amesh->canfree_baseskel)
        SCE_Skeleton_Delete (amesh->baseskel);
    amesh->baseskel = skel;
    amesh->canfree_baseskel = canfree;
}
/**
 * \brief Gets the bind pose skeleton of an animated mesh
 */
SCE_SSkeleton* SCE_AnimMesh_GetBaseSkeleton (SCE_SAnimatedMesh *amesh)
{
    return amesh->baseskel;
}
/**
 * \brief Sets the animtation skeleton of an animated mesh
 */
void SCE_AnimMesh_SetAnimSkeleton (SCE_SAnimatedMesh *amesh,
                                   SCE_SSkeleton *skel, int canfree)
{
    if (amesh->canfree_animskel)
        SCE_Skeleton_Delete (amesh->animskel);
    amesh->animskel = skel;
    amesh->canfree_animskel = canfree;
}
/**
 * \brief Gets the animation skeleton of an animated mesh
 */
SCE_SSkeleton* SCE_AnimMesh_GetAnimSkeleton (SCE_SAnimatedMesh *amesh)
{
    return amesh->animskel;
}
/**
 * \brief Creates and sets a new skeleton as the animation skeleton
 * \sa SCE_AnimMesh_GetAnimSkeleton(), SCE_AnimMesh_SetAnimSkeleton()
 */
int SCE_AnimMesh_AllocateAnimSkeleton (SCE_SAnimatedMesh *amesh)
{
    int code = SCE_OK;
    unsigned int n;
    SCE_btstart ();
#ifdef SCE_DEBUG
    if (!amesh->baseskel)
    {
        Logger_Log (SCE_INVALID_OPERATION);
        Logger_LogMsg ("this animated has not bind pose skeleton: animation "
                       "skeleton can't be created");
        return SCE_ERROR;
    }
#endif
    SCE_free (amesh->animskel);
    if (!(amesh->animskel = SCE_Skeleton_Create ()))
        goto failure;
    n = SCE_Skeleton_GetNumJoints (amesh->baseskel);
    if (SCE_Skeleton_AllocateJoints (amesh->animskel, n) < 0)
        goto failure;
    goto success;
failure:
    Logger_LogSrc (), code = SCE_ERROR;
success:
    SCE_btend ();
    return code;
}


/**
 * \brief Sets vertices for an animated mesh
 * \sa SCE_AnimMesh_SetWeights()
 */
void SCE_AnimMesh_SetVertices (SCE_SAnimatedMesh *amesh, SCE_SVertex *vertices,
                               unsigned int n_vertices)
{
    amesh->vertices = vertices;
    amesh->n_vertices = n_vertices;
}

/**
 * \brief Sets vertex weights for an animated mesh
 * \sa SCE_AnimMesh_SetVertices()
 */
void SCE_AnimMesh_SetWeights (SCE_SAnimatedMesh *amesh, SCE_SVertexWeight *w,
                              unsigned int n_w)
{
    amesh->weights = w;
    amesh->n_weights = n_w;
}

/**
 * \brief Allocates memory for vertices
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * \sa SCE_AnimMesh_SetVertices(), SCE_AnimMesh_AllocateWeights()
 */
int SCE_AnimMesh_AllocateVertices (SCE_SAnimatedMesh *amesh, unsigned int n)
{
    unsigned int i;
    if (!(amesh->vertices = SCE_malloc (n * sizeof *amesh->vertices)))
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    amesh->n_vertices = n;
    for (i = 0; i < n; i++)
        SCE_AnimMesh_InitVertex (&amesh->vertices[i]);
}
/**
 * \brief Allocates memory for vertex weights
 * \returns SCE_ERROR on error, SCE_OK otherwise
 * \sa SCE_AnimMesh_SetWeights(), SCE_AnimMesh_AllocateVertices()
 */
int SCE_AnimMesh_AllocateWeights (SCE_SAnimatedMesh *amesh, unsigned int n)
{
    unsigned int i;
    if (!(amesh->weights = SCE_malloc (n * sizeof *amesh->weights)))
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    amesh->n_weights = n;
    for (i = 0; i < n; i++)
        SCE_AnimMesh_InitWeight (&amesh->weights[i]);
}

/**
 * \brief Gets the vertices pointer of an animated mesh
 * \sa SCE_AnimMesh_GetWeights()
 */
SCE_SVertex* SCE_AnimMesh_GetVertices (SCE_SAnimatedMesh *amesh)
{
    return amesh->vertices;
}
/**
 * \brief Gets the vertices pointer of an animated mesh
 * \sa SCE_AnimMesh_GetVertices()
 */
SCE_SVertexWeight* SCE_AnimMesh_GetWeights (SCE_SAnimatedMesh *amesh)
{
    return amesh->weights;
}


static unsigned int SCE_AnimMesh_GetVerticesID (unsigned int attrib)
{
    unsigned int id = 0;
    switch (attrib)
    {
    /*case SCE_POSITION: id = 0; break;*/
    case SCE_NORMAL:   id = 1; break;
    case SCE_TANGENT:  id = 2; break;
    case SCE_BINORMAL: id = 3;
    }
    return id;
}
static unsigned int SCE_AnimMesh_GetVerticesAttrib (unsigned int id)
{
    unsigned int attribs[] = {SCE_POSITION, SCE_NORMAL, SCE_TANGENT,
                              SCE_BINORMAL};
    return attribs[id];
}

static void SCE_AnimMesh_ApplySkeletonLocalP (SCE_SAnimatedMesh*,
                                              SCE_SSkeleton*);
#if SCE_ANIMMESH_ENABLE_QUAT_TRANSFORM
static void SCE_AnimMesh_ApplySkeletonLocalPQuat (SCE_SAnimatedMesh*,
                                              SCE_SSkeleton*);
#endif

/**
 * \brief Specifies the base vertices for an animated mesh
 * \param id the vertex attribute that \p data represents,
 * can be SCE_POSITION, SCE_NORMAL, SCE_TANGENT and SCE_BINORMAL
 * \param data data of the specified attribute
 * \param local does the vectors are in local position of the joints?
 * \note \p data will be freed when \p amesh is deleted.
 * \sa SCE_AnimMesh_SetOutputVertices()
 */
void SCE_AnimMesh_SetBaseVertices (SCE_SAnimatedMesh *amesh, int attrib,
                                   SCEvertices *data, int local)
{
    unsigned int id = SCE_AnimMesh_GetVerticesID (attrib);
    SCE_free (amesh->base[id]);
    amesh->base[id] = data;
    amesh->local[id] = local;
    amesh->applyskel = (local ? SCE_AnimMesh_ApplySkeletonLocalP : SCE_AnimMesh_ApplySkeletonP);
}
/**
 * \brief Specifies the base vertices data for an animated mesh
 *
 * Does like SCE_AnimMesh_SetBaseVertices() exept that \p data is duplicated
 * before adding.
 */
int SCE_AnimMesh_SetBaseVerticesDup (SCE_SAnimatedMesh *amesh, int attrib,
                                     SCEvertices *data, int local, int size,
                                     float w)
{
    SCEvertices *new = NULL;
#ifdef SCE_DEBUG
    if (amesh->n_vertices == 0)
    {
        Logger_Log (SCE_INVALID_OPERATION);
        Logger_LogMsg ("missing number of vertices in the animated mesh");
        return SCE_ERROR;
    }
#endif
    if (size == 3)
        new = SCE_malloc (amesh->n_vertices * 4 * sizeof *new);
    else
        new = SCE_Mem_Dup (data, amesh->n_vertices * 4 * sizeof *data);
    if (!new)
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    if (size == 3)
    {
        unsigned int i;
        /* create 4D vectors from the 3D vectors */
        for (i = 0; i < amesh->n_vertices; i++)
        {
            SCE_Vector3_Copy (&new[i * 4], &data[i * 3]);
            new[i * 4 + 3] = w;
        }
    }
    SCE_AnimMesh_SetBaseVertices (amesh, attrib, new, local);
}
/**
 * \brief Defines an output buffer where write the resulting vectors after
 * a transformation
 * \param attrib vertex attribute used as a target of the transformations,
 * can be SCE_POSITION, SCE_NORMAL, SCE_TANGENT and SCE_BINORMAL
 * \param data the vertex buffer where write
 * \param size number of components of the vectors in \p data
 * \sa SCE_AnimMesh_SetBaseVertices()
 */
void SCE_AnimMesh_SetOutputVertices (SCE_SAnimatedMesh *amesh, int attrib,
                                     SCEvertices *data, int size)
{
    unsigned int id = SCE_AnimMesh_GetVerticesID (attrib);
    amesh->output[id] = data;
    amesh->size[id] = size;
}


/**
 * \brief Allocates memory for the base vertices (4D vectors)
 * \param attrib vertex attribute of the vectors
 * \param local are the vectors in local position from their joints?
 * \returns SCE_ERROR on error, SCE_OK otherwise
 */
int SCE_AnimMesh_AllocateBaseVertices (SCE_SAnimatedMesh *amesh, int attrib,
                                       int local)
{
    float *data = NULL;
    unsigned int i, n;
    if (local)
        n = amesh->n_weights;
    else
        n = amesh->n_vertices;
#ifdef SCE_DEBUG
    if (n == 0)
    {
        Logger_Log (42);
        if (local)
            Logger_LogMsg ("you must call SCE_AnimMesh_AllocateWeights() or "
                      "SCE_AnimMesh_SetWeights() before calling this function");
        else
            Logger_LogMsg ("you must call SCE_AnimMesh_AllocateVertices() or "
                     "SCE_AnimMesh_SetVertices() before calling this function");
        return SCE_ERROR;
    }
#endif
    if (!(data = SCE_malloc (n * 4 * sizeof *data)))
    {
        Logger_LogSrc ();
        return SCE_ERROR;
    }
    for (i = 0; i < n * 4; i++)
        data[i] = 0.0;
    SCE_AnimMesh_SetBaseVertices (amesh, attrib, data, local);
    return SCE_OK;
}

/**
 * \brief Gets the base vertices data of an animated mesh
 * \param attrib the vertex attribute of the queried data
 */
SCEvertices* SCE_AnimMesh_GetBaseVertices (SCE_SAnimatedMesh *amesh, int attrib)
{
    unsigned int id = SCE_AnimMesh_GetVerticesID (attrib);
    return amesh->base[id];
}

/**
 * \brief Gets the output vertices data of an animated mesh
 * \param attrib the vertex attribute of the queried data
 * \param size if not NULL, the size of the returned vectors is write here,
 * can be 3 or 4
 */
SCEvertices* SCE_AnimMesh_GetOutputVertices (SCE_SAnimatedMesh *amesh,
                                             int attrib, int *size)
{
    unsigned int id = SCE_AnimMesh_GetVerticesID (attrib);
    if (size)
        *size = amesh->size[id];
    return amesh->output[id];
}


void MulMatScalar (SCE_TMatrix4x3 m, float s, SCE_TMatrix4x3 n)
{
    n[0] = s * m[0];
    n[1] = s * m[1];
    n[2] = s * m[2];
    n[3] = s * m[3];
    n[4] = s * m[4];
    n[5] = s * m[5];
    n[6] = s * m[6];
    n[7] = s * m[7];
    n[8] = s * m[8];
    n[9] = s * m[9];
    n[10] = s * m[10];
    n[11] = s * m[11];
}
void MadMatScalar (SCE_TMatrix4x3 m, float s, SCE_TMatrix4x3 n)
{
    n[0] += s * m[0];
    n[1] += s * m[1];
    n[2] += s * m[2];
    n[3] += s * m[3];
    n[4] += s * m[4];
    n[5] += s * m[5];
    n[6] += s * m[6];
    n[7] += s * m[7];
    n[8] += s * m[8];
    n[9] += s * m[9];
    n[10] += s * m[10];
    n[11] += s * m[11];
}

#if 0
static void SCE_AnimMesh_ApplySkeleton (SCE_SAnimatedMesh *amesh,
                                        unsigned int n,
                                        SCE_SSkeleton *skel)
{
    unsigned int i, j;

    for (i = 0; i < amesh->n_vertices; i++)
    {
        SCE_TMatrix4x3 mat;
        SCE_SVertexWeight *weight;
        SCE_SVertex *vert = &amesh->vertices[i];

        weight = &amesh->weights[vert->weight_id];
        MulMatScalar (&skel->mat[0][weight->joint_id * 12], weight->weight,
                      mat);
        for (j = 1; j < vert->weight_count; j++)
        {
            weight = &amesh->weights[j + vert->weight_id];
            MadMatScalar (&skel->mat[0][weight->joint_id * 12], weight->weight,
                          mat);
        }

        SCE_Matrix4x3_MulV4 (mat, &amesh->base[n][i * 4],
                             &amesh->output[n][i * amesh->size[0]]);
    }
}
#endif
static void SCE_AnimMesh_ApplySkeletonP (SCE_SAnimatedMesh *amesh,
                                         SCE_SSkeleton *skel)
{
    unsigned int i, j;

    for (i = 0; i < amesh->n_vertices; i++)
    {
        SCE_TMatrix4x3 mat;
        SCE_SVertexWeight *weight;
        SCE_SVertex *vert = &amesh->vertices[i];

        weight = &amesh->weights[vert->weight_id];
        MulMatScalar (&skel->mat[0][weight->joint_id * 12], weight->weight,
                      mat);
        for (j = 1; j < vert->weight_count; j++)
        {
            weight = &amesh->weights[j + vert->weight_id];
            MadMatScalar (&skel->mat[0][weight->joint_id * 12], weight->weight,
                          mat);
        }

        SCE_Matrix4x3_MulV4 (mat, &amesh->base[0][i * 4],
                             &amesh->output[0][i * amesh->size[0]]);
    }
}
static void SCE_AnimMesh_ApplySkeletonPN (SCE_SAnimatedMesh *amesh,
                                          SCE_SSkeleton *skel)
{
    unsigned int i, j;

    for (i = 0; i < amesh->n_vertices; i++)
    {
        SCE_TMatrix4x3 mat;
        SCE_SVertexWeight *weight;
        SCE_SVertex *vert = &amesh->vertices[i];

        weight = &amesh->weights[vert->weight_id];
        MulMatScalar (&skel->mat[0][weight->joint_id * 12], weight->weight,
                      mat);
        for (j = 1; j < vert->weight_count; j++)
        {
            weight = &amesh->weights[j + vert->weight_id];
            MadMatScalar (&skel->mat[0][weight->joint_id * 12], weight->weight,
                          mat);
        }

        SCE_Matrix4x3_MulV4 (mat, &amesh->base[0][i * 4],
                             &amesh->output[0][i * amesh->size[0]]);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[1][i * 4],
                             &amesh->output[1][i * amesh->size[1]]);
    }
}
static void SCE_AnimMesh_ApplySkeletonPNT (SCE_SAnimatedMesh *amesh,
                                           SCE_SSkeleton *skel)
{
    unsigned int i, j;

    for (i = 0; i < amesh->n_vertices; i++)
    {
        SCE_TMatrix4x3 mat;
        SCE_SVertexWeight *weight;
        SCE_SVertex *vert = &amesh->vertices[i];

        weight = &amesh->weights[vert->weight_id];
        MulMatScalar (&skel->mat[0][weight->joint_id * 12], weight->weight,
                      mat);
        for (j = 1; j < vert->weight_count; j++)
        {
            weight = &amesh->weights[j + vert->weight_id];
            MadMatScalar (&skel->mat[0][weight->joint_id * 12], weight->weight,
                          mat);
        }

        SCE_Matrix4x3_MulV4 (mat, &amesh->base[0][i * 4],
                             &amesh->output[0][i * amesh->size[0]]);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[1][i * 4],
                             &amesh->output[1][i * amesh->size[1]]);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[2][i * 4],
                             &amesh->output[2][i * amesh->size[2]]);
    }
}
static void SCE_AnimMesh_ApplySkeletonPNTB (SCE_SAnimatedMesh *amesh,
                                            SCE_SSkeleton *skel)
{
    unsigned int i, j;

    for (i = 0; i < amesh->n_vertices; i++)
    {
        SCE_TMatrix4x3 mat;
        SCE_SVertexWeight *weight;
        SCE_SVertex *vert = &amesh->vertices[i];

        weight = &amesh->weights[vert->weight_id];
        MulMatScalar (&skel->mat[0][weight->joint_id * 12], weight->weight,
                      mat);
        for (j = 1; j < vert->weight_count; j++)
        {
            weight = &amesh->weights[j + vert->weight_id];
            MadMatScalar (&skel->mat[0][weight->joint_id * 12], weight->weight,
                          mat);
        }

        SCE_Matrix4x3_MulV4 (mat, &amesh->base[0][i * 4],
                             &amesh->output[0][i * amesh->size[0]]);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[1][i * 4],
                             &amesh->output[1][i * amesh->size[1]]);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[2][i * 4],
                             &amesh->output[2][i * amesh->size[2]]);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[3][i * 4],
                             &amesh->output[3][i * amesh->size[3]]);
    }
}

static void SCE_AnimMesh_ApplySkeletonLocal (SCE_SAnimatedMesh *amesh,
                                             unsigned int n,
                                             SCE_SSkeleton *skel)
{
    unsigned int i, j, index;
    float *out = NULL;

    for (i = 0; i < amesh->n_vertices; i++)
    {
        float *mat = NULL;
        SCE_SVertexWeight *weight;
        SCE_SVertex *vert = &amesh->vertices[i];

        index = vert->weight_id;
        out = &amesh->output[n][i * amesh->size[n]];

        weight = &amesh->weights[index];
        mat = &skel->mat[0][weight->joint_id * 12];
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[n][index * 4], out);
        for (j = 1; j < vert->weight_count; j++)
        {
            index++;
            weight = &amesh->weights[index];
            mat = &skel->mat[0][weight->joint_id * 12];
            SCE_Matrix4x3_MulV4Add (mat, &amesh->base[n][index * 4], out);
        }
    }
}
static void SCE_AnimMesh_ApplySkeletonLocalP (SCE_SAnimatedMesh *amesh,
                                              SCE_SSkeleton *skel)
{
    unsigned int i, j, index;
    float *out = NULL;

    for (i = 0; i < amesh->n_vertices; i++)
    {
        float *mat = NULL;
        SCE_SVertexWeight *weight;
        SCE_SVertex *vert = &amesh->vertices[i];

        index = vert->weight_id;
        out = &amesh->output[0][i * amesh->size[0]];

        weight = &amesh->weights[index];
        mat = &skel->mat[0][weight->joint_id * 12];
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[0][index * 4], out);
        for (j = 1; j < vert->weight_count; j++)
        {
            index++;
            weight = &amesh->weights[index];
            mat = &skel->mat[0][weight->joint_id * 12];
            SCE_Matrix4x3_MulV4Add (mat, &amesh->base[0][index * 4], out);
        }
    }
}
#if SCE_ANIMMESH_ENABLE_QUAT_TRANSFORM
static void ApplyJointToVector (SCE_SJoint *j, float bias, SCE_TVector3 in,
                                SCE_TVector3 out)
{
    /* calculate transformed vertex for this weight */
    SCE_TVector3 wv;
    SCE_Quaternion_RotateV3 (j->orientation, in, wv);

    /* the sum of all weight->bias should be 1.0 */
    out[0] += (j->position[0] + wv[0]) * bias;
    out[1] += (j->position[1] + wv[1]) * bias;
    out[2] += (j->position[2] + wv[2]) * bias;
}
static void SCE_AnimMesh_ApplySkeletonLocalPQuat (SCE_SAnimatedMesh *amesh,
                                                  SCE_SSkeleton *skel)
{
    unsigned int i, j, index;
    float *out = NULL;
    SCE_SJoint *joints = NULL;

    joints = skel->joints;

    for (i = 0; i < amesh->n_vertices; i++)
    {
        float *mat = NULL;
        SCE_SVertexWeight *weight;
        SCE_SVertex *vert = &amesh->vertices[i];
        SCE_SJoint *joint = NULL;

        index = vert->weight_id;
        out = &amesh->output[0][i * amesh->size[0]];
        SCE_Vector3_Set (out, 0.0, 0.0, 0.0);
        for (j = 0; j < vert->weight_count; j++, index++)
        {
            weight = &amesh->weights[index];
            joint = &joints[weight->joint_id];
            ApplyJointToVector (joint, weight->weight, &amesh->base[0][index * 4], out);
        }
    }
}
#endif
static void SCE_AnimMesh_ApplySkeletonLocalPN (SCE_SAnimatedMesh *amesh,
                                               SCE_SSkeleton *skel)
{
    unsigned int i, j, index;
    float *out = NULL, *out2 = NULL;

    for (i = 0; i < amesh->n_vertices; i++)
    {
        float *mat = NULL;
        SCE_SVertexWeight *weight;
        SCE_SVertex *vert = &amesh->vertices[i];

        index = vert->weight_id;
        out = &amesh->output[0][i * amesh->size[0]];
        out2 = &amesh->output[1][i * amesh->size[0]];

        weight = &amesh->weights[index];
        mat = &skel->mat[0][weight->joint_id * 12];
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[0][index * 4], out);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[1][index * 4], out2);
        for (j = 1; j < vert->weight_count; j++)
        {
            index++;
            weight = &amesh->weights[index];
            mat = &skel->mat[0][weight->joint_id * 12];
            SCE_Matrix4x3_MulV4Add (mat, &amesh->base[0][index * 4], out);
            SCE_Matrix4x3_MulV4Add (mat, &amesh->base[1][index * 4], out2);
        }
    }
}
static void SCE_AnimMesh_ApplySkeletonLocalPNT (SCE_SAnimatedMesh *amesh,
                                                SCE_SSkeleton *skel)
{
    unsigned int i, j, index;
    float *out = NULL, *out2 = NULL, *out3 = NULL;

    for (i = 0; i < amesh->n_vertices; i++)
    {
        float *mat = NULL;
        SCE_SVertexWeight *weight;
        SCE_SVertex *vert = &amesh->vertices[i];

        index = vert->weight_id;
        out = &amesh->output[0][i * amesh->size[0]];
        out2 = &amesh->output[1][i * amesh->size[0]];
        out3 = &amesh->output[2][i * amesh->size[0]];

        weight = &amesh->weights[index];
        mat = &skel->mat[0][weight->joint_id * 12];
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[0][index * 4], out);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[1][index * 4], out2);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[2][index * 4], out3);
        for (j = 1; j < vert->weight_count; j++)
        {
            index++;
            weight = &amesh->weights[index];
            mat = &skel->mat[0][weight->joint_id * 12];
            SCE_Matrix4x3_MulV4Add (mat, &amesh->base[0][index * 4], out);
            SCE_Matrix4x3_MulV4Add (mat, &amesh->base[1][index * 4], out2);
            SCE_Matrix4x3_MulV4Add (mat, &amesh->base[2][index * 4], out3);
        }
    }
}
static void SCE_AnimMesh_ApplySkeletonLocalPNTB (SCE_SAnimatedMesh *amesh,
                                                 SCE_SSkeleton *skel)
{
    unsigned int i, j, index;
    float *out = NULL, *out2 = NULL, *out3 = NULL, *out4 = NULL;

    for (i = 0; i < amesh->n_vertices; i++)
    {
        float *mat = NULL;
        SCE_SVertexWeight *weight;
        SCE_SVertex *vert = &amesh->vertices[i];

        index = vert->weight_id;
        out = &amesh->output[0][i * amesh->size[0]];
        out2 = &amesh->output[1][i * amesh->size[0]];
        out3 = &amesh->output[2][i * amesh->size[0]];
        out4 = &amesh->output[3][i * amesh->size[0]];

        weight = &amesh->weights[index];
        mat = &skel->mat[0][weight->joint_id * 12];
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[0][index * 4], out);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[1][index * 4], out2);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[2][index * 4], out3);
        SCE_Matrix4x3_MulV4 (mat, &amesh->base[3][index * 4], out4);
        for (j = 1; j < vert->weight_count; j++)
        {
            index++;
            weight = &amesh->weights[index];
            mat = &skel->mat[0][weight->joint_id * 12];
            SCE_Matrix4x3_MulV4Add (mat, &amesh->base[0][index * 4], out);
            SCE_Matrix4x3_MulV4Add (mat, &amesh->base[1][index * 4], out2);
            SCE_Matrix4x3_MulV4Add (mat, &amesh->base[2][index * 4], out3);
            SCE_Matrix4x3_MulV4Add (mat, &amesh->base[3][index * 4], out4);
        }
    }
}

/**
 * \brief Applies a skeleton to an animated mesh
 *
 * This function does not update the internal SCE_SMesh mesh of \p amesh,
 * so call SCE_AnimMesh_Prout() for that.
 * \sa SCE_AnimMesh_Prout()
 */
void SCE_AnimMesh_ApplySkeleton (SCE_SAnimatedMesh *amesh, SCE_SSkeleton *skel)
{
    amesh->applyskel (amesh, skel);
}

/**
 * \brief Applies the bind pose skeleton of an animated mesh
 * \sa SCE_AnimMesh_ApplySkeleton()
 */
void SCE_AnimMesh_ApplyBaseSkeleton (SCE_SAnimatedMesh *amesh)
{
#ifdef SCE_DEBUG
    if (!amesh->baseskel)
    {
        Logger_Log (SCE_INVALID_OPERATION);
        Logger_LogMsg ("this animated mesh has not bind pose skeleton, SCE_Ani"
                       "mMesh_ApplyBaseSkeleton() aborted");
    }
    else
#endif
    SCE_AnimMesh_ApplySkeleton (amesh, amesh->baseskel);
}
/**
 * \brief Applies the bind pose skeleton of an animated mesh
 * \sa SCE_AnimMesh_ApplySkeleton()
 */
void SCE_AnimMesh_ApplyAnimSkeleton (SCE_SAnimatedMesh *amesh)
{
#ifdef SCE_DEBUG
    if (!amesh->animskel)
    {
        Logger_Log (SCE_INVALID_OPERATION);
        Logger_LogMsg ("this animated mesh has not animation skeleton, SCE_Ani"
                       "mMesh_ApplyBaseSkeleton() aborted");
    }
    else
#endif
    SCE_AnimMesh_ApplySkeleton (amesh, amesh->animskel);
}

#if 0
/**
 * \brief Sets the vertices in local position of the joints (so duplicate
 * vertices if necessary)
 * \param bpose the bind pose skeleton
 * \sa SCE_AnimMesh_SetGlobal()
 */
void SCE_AnimMesh_SetLocal (SCE_SAnimatedMesh *amesh, SCE_SSkeleton *bpose)
{
    SCEvertices *vert = NULL;
}
#endif
/**
 * \brief Set the vertices in global position
 * \param bpose the bind pose skeleton
 * \sa SCE_AnimMesh_SetGlobal()
 * \todo manage the w component
 */
int SCE_AnimMesh_SetGlobal (SCE_SAnimatedMesh *amesh)
{
    unsigned int i;
    SCEvertices *vert = NULL;

    for (i = 0; i < SCE_MAX_ANIMATED_VERTEX_ATTRIBUTES; i++)
    {
        if (amesh->local[i])
        {
            unsigned int j;
            /* alloc new base */
            if (!(vert = SCE_malloc (amesh->n_vertices * 4 * sizeof *vert)))
            {
                Logger_LogSrc ();
                return SCE_ERROR;
            }
            SCE_AnimMesh_ApplySkeletonLocal (amesh, i, amesh->baseskel);
            for (j = 0; j < amesh->n_vertices; j++)
            {
                SCE_Vector3_Copy (&vert[j * 4],
                                  &amesh->output[i][j * amesh->size[i]]);
            }
            SCE_AnimMesh_SetBaseVertices (amesh,
                                          SCE_AnimMesh_GetVerticesAttrib (i),
                                          vert, SCE_FALSE);
        }
    }
    return SCE_OK;
}

/**
 * \brief Builds the internal SCE_SMesh of an animated mesh
 * \param mode vertex buffers organization, can be
 * SCE_INDEPENDANT_VERTEX_BUFFER, SCE_GLOBAL_VERTEX_BUFFER or
 * SCE_GLOBAL_TBN_VERTEX_BUFFER
 * \param size_array an array of the vector sizes of the generated mesh vertices
 *
 * This functions builds a SCE_SMesh based on the given animated mesh. \p mode
 * is used to defines the storage mode of the data in the vertex buffers.
 * \p size_array is an array of 4 elements, each one gives the size of the
 * corresponding vertex attribute (SCE_POSITION, SCE_NORMAL, SCE_TANGENT and
 * SCE_BINORMAL, respectively).
 */
int SCE_AnimMesh_BuildMesh (SCE_SAnimatedMesh *amesh, int mode, int *size_array)
{
    int code = SCE_OK;
    unsigned int posid, norid, tanid, binid;
    int *size = NULL;
    SCE_SMesh *mesh = NULL;

    SCE_btstart ();
    if (!(mesh = SCE_Mesh_Create ()))
        goto failure;

    size = (size_array ? size_array : amesh->size);

    switch (mode)
    {
    case SCE_INDEPENDANT_VERTEX_BUFFER:
        posid = 0;
        norid = 1;
        tanid = 2;
        binid = 3;
        break;
    case SCE_GLOBAL_VERTEX_BUFFER:
        posid = norid = tanid = binid = 0;
        break;
    case SCE_GLOBAL_TBN_VERTEX_BUFFER:
        posid = 0;
        norid = tanid = binid = 1;
    }

#define SCE_Anim_AddVert(id, type, size)\
    SCE_Mesh_AddVertices (mesh, id, type, SCE_VERTICES_TYPE,\
                          size, amesh->n_vertices, NULL, 0)
    if (amesh->base[0])
    {
        if (SCE_Anim_AddVert (posid, SCE_POSITION, size[0]) < 0)
            goto failure;
        SCE_AnimMesh_SetOutputVertices
            (amesh, SCE_POSITION, SCE_Mesh_GetVerticesPositions (mesh),
             size[0]);
    }
    if (amesh->base[1])
    {
        if (SCE_Anim_AddVert (norid, SCE_NORMAL, size[1]) < 0)
            goto failure;
        SCE_AnimMesh_SetOutputVertices
            (amesh, SCE_NORMAL, SCE_Mesh_GetVerticesNormals (mesh),
             size[1]);
    }
    if (amesh->base[2])
    {
        if (SCE_Anim_AddVert (tanid, SCE_TANGENT, size[2]) < 0)
            goto failure;
        SCE_AnimMesh_SetOutputVertices
            (amesh, SCE_TANGENT, SCE_Mesh_GetVerticesTangents(amesh->mesh),
             size[2]);
    }
    if (amesh->base[3])
    {
        if (SCE_Anim_AddVert (binid, SCE_BINORMAL, size[3]) < 0)
            goto failure;
        SCE_AnimMesh_SetOutputVertices
            (amesh, SCE_BINORMAL, SCE_Mesh_GetVerticesBinormals (mesh),
             size[3]);
    }
#undef SCE_Anim_AddVert

    SCE_Mesh_SetRenderMode (mesh, SCE_TRIANGLES);
    if (SCE_Mesh_Build (mesh) < 0)
        goto failure;

    SCE_Mesh_Delete (amesh->mesh);
    amesh->mesh = mesh;

    goto success;
failure:
    SCE_Mesh_Delete (mesh);
    Logger_LogSrc ();
    code = SCE_ERROR;
success:
    SCE_btend ();
    return code;
}

/**
 * \brief Gets the internal SCE_SMesh mesh of an animated mesh
 */
SCE_SMesh* SCE_AnimMesh_GetMesh (SCE_SAnimatedMesh *amesh)
{
    return amesh->mesh;
}

/**
 * \brief Updates an animated mesh from its mesh
 *
 * Checks the vertex buffers available in \p amesh::mesh and link it as new
 * output for the transformations
 */
int SCE_AnimMesh_UpdateMesh (SCE_SAnimatedMesh *amesh)
{
    unsigned int i;
    SCE_SMeshVertexData *data = NULL;

    SCE_btstart ();
    for (i = 0; i < 4; i++)
    {
        int attrib = SCE_AnimMesh_GetVerticesAttrib (i);
        SCEvertices *vert = NULL;
        data = SCE_Mesh_LocateData (amesh->mesh, attrib, NULL);
        if (data && (vert = data->data) != amesh->output[i])
        {
            unsigned int j;
            size_t offset;
            if (SCE_AnimMesh_AllocateBaseVertices (amesh, attrib, SCE_FALSE) < 0)
            {
                Logger_LogSrc ();
                SCE_btend ();
                return SCE_ERROR;
            }
            offset = data->dec->size;
            for (j = 0; j < amesh->n_vertices; j++)
            {
                memcpy (amesh->base[i], vert, offset * sizeof *amesh->base[0]);
                vert = &vert[offset];
            }
            SCE_AnimMesh_SetOutputVertices (amesh, attrib, vert, offset);
        }
    }
    SCE_btend ();
    return SCE_OK;
}


/**
 * \brief Loads an animated mesh from a file
 * \param fmesh the file of the mesh
 * \param fskel the file of the bind pose skeleton for the mesh (can be NULL)
 * \returns a new animated mesh based on the given files or NULL on error
 * \todo check media type id
 */
SCE_SAnimatedMesh* SCE_AnimMesh_Load (const char *fmesh, const char *fskel)
{
    SCE_SAnimatedMesh *amesh = NULL;

    SCE_btstart ();
    if (!(amesh = SCE_Resource_Load (fmesh, NULL, NULL)))
    {
        Logger_LogSrc ();
        SCE_btend ();
        return NULL;
    }
    if (fskel)
    {
        SCE_SSkeleton *skel = NULL;
        if (!(skel = SCE_Resource_Load (fmesh, NULL, NULL)))
        {
            SCE_AnimMesh_Delete (amesh);
            Logger_LogSrc ();
            SCE_btend ();
            return NULL;
        }
        SCE_AnimMesh_SetBaseSkeleton (amesh, skel, SCE_TRUE);
    }
    SCE_btend ();
    return amesh;
}

/* @} */
