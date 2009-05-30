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

/* updated: 01/04/2009 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include <SCE/interface/libwar.h>

static char error[256] = {0};

/*** fonctions de gestion des erreurs ***/
static void war_error (const char *e, ...)
{
    va_list args;
    memset (error, '\0', sizeof error);
    va_start (args, e);
    vsprintf (error, e, args);
    va_end (args);
}

const char* war_geterror (void)
{
    return error;
}

const char* war_getversionstring (void)
{
    return WAR_VERSION_STRING;
}

/*** fonction de gestion des allocations memoire ***/
static void* my_alloc (size_t size)
{
    void *p = malloc (size);
    if (!p)
        war_error ("can't allocate memory!");
    return p;
}

#define malloc(s) my_alloc (s)

/*** fonctions de manipulation d'une structure WarMesh ***/
static void war_init (WarMesh *m)
{
    m->pos = m->tex = m->nor = NULL;
    m->vcount = m->icount = 0;
    m->indices = NULL;
    m->canfree  = 0;
}
WarMesh* war_new (void)
{
    WarMesh *m = malloc (sizeof *m);
    if (m)
        war_init (m);
    return m;
}
void war_canfree (WarMesh *m, int c)
{
    m->canfree = c;
}
void war_clear (WarMesh *m)
{
    if (m) {
        if (m->canfree) {
            free (m->pos);
            free (m->tex);
            free (m->nor);
            free (m->indices);
        }
        m->pos = NULL;
        m->tex = NULL;
        m->nor = NULL;
        m->indices = NULL;
    }
}
void war_free (WarMesh *m)
{
    war_clear (m);
    free (m);
}


/*** fonctions outils ***/

static void jumpline (FILE *f)
{
    int c;
    while ((c = fgetc (f)) != '\n' && c != EOF);
}

static void jumpspaces (FILE *f)
{
    while (isspace (fgetc (f)));
    fseek (f, -1, SEEK_CUR);
}

static long puissance (long x, WAuint n)
{
    if (n == 0)
        return 1;
    else if (n == 1)
        return x;
    else if (n%2)
        return x * puissance (x*x, (n-1)/2);
    else
        return puissance (x*x, n/2);
}

static WAfloat4 valof (const char *str)
{
    WAuint i;              /* compteur */
    WAfloat4 res = 0.;   /* valeur de retour */
    WAfloat4 coeff = 0.;
    int val = 0;
    WAuint str_ilen = 0;   /* longueur de la valeur entiere */
    WAuint str_dlen = 0;   /* longueur de la valeur décimale */
    WAuint unit_tmp = 0;

    /* calcul de la longueur de la chaine str -->
       longueur de la partie entiere */
    while (str[str_ilen] != '\0' &&
           str[str_ilen] != '\n' &&
           str[str_ilen] != '.') {
        str_ilen++;
    }
    /* --> longueur de la partie decimale (si elle existe uniquement) */
    if (str[str_ilen] == '.') {
        while (str[str_dlen+str_ilen+1] != '\0' &&
               str[str_dlen+str_ilen] != '\n') {
            str_dlen++;
        }
    }

    if (str[0] == '-' || str[0] == '+')
        val = 1;

    coeff = puissance (10, str_ilen - val);

    /* calcul de la valeur entiere */
    for (i=val; i<str_ilen; i++) {
        unit_tmp = str[i] - '0';
        if (unit_tmp > 9) {
            fprintf (stderr, "valof() : argument %d : "
                     "'%c' is not a number\n", i, str[i]);
            /* on saute, on s'en occupe pas */
            unit_tmp = 0;
        } else
            coeff /= 10.0f;

        res += unit_tmp * coeff;
    }

    coeff = 1.0f;
    /* calcul de la valeur decimale */
    for (i=str_ilen+1; i<str_dlen+str_ilen+1; i++) {
        unit_tmp = str[i] - '0';
        if (unit_tmp > 9) {
            fprintf (stderr, "valof() : argument %d : "
                     "'%c' is not a number\n", i, str[i]);
            /* on saute, on s'en occupe pas */
            unit_tmp = 0;
        } else
            coeff /= 10.0f;

        res += unit_tmp*coeff;
    }

    if (str[0] == '-')
        res = -res;

    return res;
}

static int readnumber (FILE *fp, WAfloat4 *fres, int *ires, WAint4 *uires)
{
    int c;
    char number[16] = {'\0'}; /* nombre sous forme de chaine */
    WAuint pn = 0;              /* position dans 'number' */
    int pointread = 0;        /* pour eviter de lire deux fois un point */

    while ((c = fgetc (fp)) != EOF && (
               (c <= '9' && c >= '0') ||
               (c == '.' && !pointread) ||
               (c == '-') ) && pn < sizeof number) {
        number[pn] = c;
        pn++;
        if (c == '.')
            pointread = 1;
    }
    if (pn == 0) 
        return 1;
    else {
        /* on revient au premier caractere invalide */
        fseek (fp, -1, SEEK_CUR);
        if (fres)
            *fres = valof (number);
        if (ires)
            *ires = strtol (number, NULL, 10);
        if (uires)
            *uires = strtoul (number, NULL, 10);
        return 0;
    }
}

static int readfloat (FILE *fp, WAfloat4 *result)
{
    return readnumber (fp, result, NULL, NULL);
}
#if 0
static int readint (FILE *fp, int *result)
{
    return readnumber (fp, NULL, result, NULL);
}
#endif
static int readuint (FILE *fp, WAint4 *result)
{
    return readnumber (fp, NULL, NULL, result);
}

static WAuint strsinline (FILE *fp, const int a)
{
    int valid;
    WAuint n = 0;     /* nombre de chaines */
    int c;
    long curo;

    curo = ftell (fp);

    do {
        valid = 0;
        while (!isspace ((c = fgetc (fp))) && c != EOF && c != '\n')
            valid++;
        if (valid >= 1)
            n++;
    } while (c != EOF && c != '\n');

    if (a)
        fseek (fp, curo, SEEK_SET);

    return n;
}

static WAuint getline (FILE *fp)
{
    WAuint n = 1;
    int c;
    long end = ftell (fp);
    rewind (fp);
    while ((c = fgetc (fp)) != EOF && ftell (fp) < end) {
        if (c == '\n')
            n++;
    }
    fseek (fp, end, SEEK_SET);
    return n;
}


/*** fonctions de lecture d'un .obj ***/

static void readinfos (FILE *fp, int *n_objs,
                       size_t *v, size_t *vt, size_t *vn)
{
    int c;

    *n_objs = 0;
    *v = *vt = *vn = 0;
    do {
        c = fgetc (fp);
        switch (c) {
        case 'v':
            c = fgetc (fp);
            switch (c) {
            case ' ':
                (*v) += 3;
                break;
            case 't':
                (*vt) += 2;
                break;
            case 'n':
                (*vn) += 3;
            }
            break;

        case 'g':
        case 'o':
            (*n_objs)++;
        }
        if (c != '\n')
            jumpline (fp);
    } while (c != EOF);
    if (*n_objs == 0)
        (*n_objs) = 1;             /* prout */
}

/* lit la taille du tableau d'indices de l'objet selectionne
   (retourne le nombre de valeurs que devra stocker le tableau) */
static int readnumindices (FILE *fp)
{
    int c;
    int n = 0;

    do {
        c = fgetc (fp);
        switch (c) {
        case 'f':
            /* nombre d indices que comporte la face */
            switch (strsinline (fp, 1)) {
            case 0:
            case 1:
                war_error ("line %u : bad file format, "
                           "face with lesser than two vertices!",
                           getline (fp));
                return -1;
                break;
            case 2:
            case 3:
                n += 9;
                break;
            case 4:
                n += 18;
                break;
            default:
                war_error ("line %u : bad file format, "
                           "face with more than 4 vertices!",
                           getline (fp));
                return -1;
            }
            break;

        /* objet --> fini */
        case 'o':
            c = EOF;
        }
        if (c != '\n')
            jumpline (fp);
    } while (c != EOF);

    return n;
}

static void readdataline (FILE *fp, WAfloat4 *data, int tex)
{
    jumpspaces (fp);
    readfloat (fp, &data[0]);
    jumpspaces (fp);
    readfloat (fp, &data[1]);
    if (!tex) {
        jumpspaces (fp);
        readfloat (fp, &data[2]);
    }
}

static void readdata (FILE *fp, WAfloat4 *v, WAfloat4 *vt, WAfloat4 *vn)
{
    int c;
    WAuint i = 0, j = 0, k = 0; /* initialisations importantes */

    do {
        c = fgetc (fp);
        /* si on se trouve a une definition de sommet, lire le sommet */    
        if (c == 'v') {
            c = fgetc (fp);
            switch (c) {
            case ' ':
                if (v) {
                    readdataline (fp, &v[i], 0);
                    i += 3;
                }
                break;
            case 't':
                if (vt) {
                    readdataline (fp, &vt[j], 1);
                    j += 2;
                }
                break;
            case 'n':
                if (vn) {
                    readdataline (fp, &vn[k], 0);
                    k += 3;
                }
            }
        }
        if (c != '\n')
            jumpline (fp);
    } while (c != EOF);
}

static int readindexline (FILE *fp, WAint4 *indices)
{
    int c;
    WAuint i, j;
    WAuint p = 0; /* position dans 'indices' */
    WAint4 index[4][3] = {{0}};
    WAuint numstr;

    /* algorithme de lecture des donnees dans 'fp' */
    numstr = strsinline (fp, 1);
    for (i=0; i<numstr; i++) {
        jumpspaces (fp);
        for (j=0; j<3; j++) {
            readuint (fp, &index[i][j]);
            c = fgetc (fp);
            if (c == '/') {
                c = fgetc (fp);
                if (c == '/')
                    j++;
                else
                    fseek (fp, -1, SEEK_CUR);
            } else {
                fseek (fp, -1, SEEK_CUR);
                break;
            }
        }
    }

    /* algorithme de tri des donnees selon le type de polygone lu */
    for (i=0; i<3; i++) {
        indices[p]   = index[i][0];
        indices[p+1] = index[i][1];
        indices[p+2] = index[i][2];
        p += 3;
    }

    switch (numstr) {
    case 2:
        p -= 3;
        indices[p]   = index[0][0];
        indices[p+1] = index[0][1];
        indices[p+2] = index[0][2];
    case 3:
        numstr = 9;
        break;

    case 4:
        indices[p]   = index[2][0];
        indices[p+1] = index[2][1];
        indices[p+2] = index[2][2];
        p += 3;
        indices[p]   = index[3][0];
        indices[p+1] = index[3][1];
        indices[p+2] = index[3][2];
        p += 3;
        indices[p]   = index[0][0];
        indices[p+1] = index[0][1];
        indices[p+2] = index[0][2];
        numstr = 18;
    /* pas la peine de verifier via default si une mauvaise valeur a ete lue,
       la fonction de lecture de la taille des indices s'en charge */
    }

    /* 'numstr' vaut maintenant le nombre de "cases" de 'indices' remplies */
    return numstr;
}

static void readindices (FILE *fp, WAint4 *indices)
{
    int c;
    size_t pos = 0; /* initialisation importante */

    do {
        c = fgetc (fp);
        if (c == 'f')
            pos += readindexline (fp, &indices[pos]);
        else if (c == 'o') /* definition d'un autre objet, arret */
            break;
        else if (c != '\n')
            jumpline (fp);
    } while (c != EOF);
}


static int vectcmp (WAint4 *a, WAint4 *b)
{
    return (a[0] == b[0] && a[1] == b[1] && a[2] == b[2]);
}

/* construit des indices pour un mesh et tri ses sommets en consequence */
static int makeindices (WarMesh *me)
{
    WAint4 *indices = NULL;
    WAuint i, j, k = 0, l = 0, m = 0;
    int ok = 1;
    float prc = 0.;
    float prc_back = 0.;
    WAfloat4 *pos = NULL, *nor = NULL, *tex = NULL;
    WAuint n_triangles = me->icount / 9;
    WAint4 *index = me->indices;

    indices = malloc (n_triangles * 3 * sizeof *indices);
    if (!indices)
        return -1;

    /* taille reelle impossible a evaluer, on prend la taille maximale */
    if (!(pos = malloc (n_triangles * 9 * sizeof *pos))) {
        free (indices);
        return -1;
    }
    if (index[2])
        if (!(nor = malloc (n_triangles * 9 * sizeof *nor))) {
            free (pos);
            free (indices);
            return -1;
        }
    if (index[1])
        if (!(tex = malloc (n_triangles * 6 * sizeof *tex))) {
            free (nor);
            free (pos);
            free (indices);
            return -1;
        }

    /* pour chaque triangle */
    for (i=0; i<n_triangles; i++) {
        prc = (float)i / n_triangles;
        prc *= 100.;
        if (prc >= prc_back+0.1)
            prc_back = prc;

        /* pour chaque sommet */
        for (j=0; j<3; j++) {
            ok = 1;

            /* on verifie tous les precedents indices pour en trouver
               un identique... */
            for (m=l; m>0; m--) {
                if (vectcmp (&index[(m-1)*3], &index[l*3])) {
                    /* un indice a ete trouve, on l'utilise et ignore
                       la copie des donnees vectorielles */
                    indices[l] = indices[m-1];
                    ok = 0;
                    break;
                }
            }

            if (ok) {
                /* creation d'un nouvel indice & copie des donnees
                   pour le nouveau sommet */
                indices[l] = k / 3;

                pos[k] = me->pos [3*index[l*3]-3];
                pos[k+1] = me->pos [3*index[l*3]-2];
                pos[k+2] = me->pos [3*index[l*3]-1];

                if (index[2]) {
                    nor[k] = me->nor [3*index[l*3+2]-3];
                    nor[k+1] = me->nor [3*index[l*3+2]-2];
                    nor[k+2] = me->nor [3*index[l*3+2]-1];
                }

                if (index[1]) {
                    tex[k-(k/3)] = me->tex [2*index[l*3+1]-2];
                    tex[k-(k/3)+1] = me->tex [2*index[l*3+1]-1];
                }
                k += 3;
            }
            l++;
        }
    }

    war_clear (me);

    war_canfree (me, 1);
    me->icount = n_triangles * 3;
    me->vcount = k / 3;
    me->pos = pos;
    me->nor = nor;
    me->tex = tex;
    me->indices = indices;

    return 0;
}

/* cree une suite de sommets en fonction d'indices du type des .obj */
static int makevertices (WarMesh *me)
{
    WAuint i, j;
    WAfloat4 *pos = NULL, *nor = NULL, *tex = NULL;

    /* icount = nombre de variables dans le tableau d'indices, mais
       etant donne que les indices sont intrelaces (*3 pour
       3 types de donnees par sommet), pas besoin de multiplier par 3  */
    if (!(pos = malloc (me->icount * sizeof *pos)))
        return -1;
    if (me->indices[2])
        if (!(nor = malloc (me->icount * sizeof *nor))) {
            free (pos);
            return -1;
        }
    if (me->indices[1])
        if (!(tex = malloc ((me->icount/3)*2 * sizeof *tex))) {
            free (nor);
            free (pos);
            return -1;
        }

    for (i=0; i<me->icount; i+=3) {
        for (j=0; j<3; j++)
            pos[i+j] = me->pos[3*me->indices[i]+j-3];
    }
    if (nor) {
        for (i=2; i<me->icount; i+=3) {
            for (j=0; j<3; j++)
                nor[i-2+j] = me->nor[3*me->indices[i]+j-3];
        }
    }
    if (tex) {
        for (i=1; i<me->icount; i+=3) {
            tex[i-1-(i-1)/3]   = me->tex[2*me->indices[i]-2];
            tex[i-1-(i-1)/3+1] = me->tex[2*me->indices[i]-1];
        }
    }

    war_clear (me);

    me->vcount = me->icount / 3;
    me->icount = 0;
    me->pos = pos;
    me->nor = nor;
    me->tex = tex;

    return 0;
}

WarMesh** war_read (FILE *fp, int gen_indices, int *n_objs)
{
    int i = 0;
    WarMesh **meshs = NULL;
    size_t v, vt, vn;
    int (*make)(WarMesh*) = NULL;
    int o_first = 1;

#define WAR_ASSERT(c)\
    if ((c)) {\
        for (i=0; i<*n_objs; i++)\
            war_free (meshs[i]);\
        free (meshs);\
        return NULL;\
    }

    /* lecture des informations generales */
    readinfos (fp, n_objs, &v, &vt, &vn);
    if (v == 0 && vt == 0 && vn == 0) {
        war_error ("bad file format : can't read some valid OBJ data");
        return NULL;
    } else if (n_objs == 0)
        *n_objs = 1;
    rewind (fp);

    /* allocation du tableau d'objets */
    meshs = malloc (*n_objs * sizeof *meshs);
    if (!meshs)
        return NULL;
    /* initialisation des pointeurs a NULL pour rendre
       possible l'utilisation de WAR_ASSERT */
    for (i=0; i<*n_objs; i++)
        meshs[i] = NULL;

    /* creation du premier objet */
    WAR_ASSERT (!(meshs[0] = war_new ()))
    war_canfree (meshs[0], 1);

    /* allocation des tableaux de donnees */
    WAR_ASSERT (!(meshs[0]->pos = malloc (v * sizeof *meshs[0]->pos)))
    WAR_ASSERT (!(meshs[0]->tex = malloc (vt * sizeof *meshs[0]->tex)))
    WAR_ASSERT (!(meshs[0]->nor = malloc (vn * sizeof *meshs[0]->nor)))

    /* creation des autres objets */
    for (i=1; i<*n_objs; i++) {
        WAR_ASSERT (!(meshs[i] = war_new ()))
        meshs[i]->pos = meshs[0]->pos;
        meshs[i]->tex = meshs[0]->tex;
        meshs[i]->nor = meshs[0]->nor;
    }

    /* lecture des donnees de sommet */
    readdata (fp, meshs[0]->pos, meshs[0]->tex, meshs[0]->nor);
    rewind (fp);

    switch (readnumindices (fp)) { /* pour aller jusqu'au prochain 'o' */
    case -1: WAR_ASSERT (1)
    case 0: break;
    default:
        /* donnees detectees avant le premier 'o', on considere donc que
           le premier objet n'est pas defini par un 'o' */
        rewind (fp);
        o_first = 0;
    }
    /* lecture de la taille des tableaux d'indices et allocations */
    for (i=0; i<*n_objs; i++) {
        WAR_ASSERT ((meshs[i]->icount = readnumindices (fp)) < 0)
        if (meshs[i]->icount == 0) {
            war_error ("bad file format : 0 value read for index count");
            WAR_ASSERT (1)
        }
        meshs[i]->indices = malloc (meshs[i]->icount*sizeof *meshs[i]->indices);
        WAR_ASSERT (!meshs[i]->indices)
    }
    rewind (fp);
    if (o_first)
        WAR_ASSERT (readnumindices (fp) < 0) /* pour aller jusqu'au prochain 'o' */
    /* lecture des indices */
    for (i=0; i<*n_objs; i++)
        readindices (fp, meshs[i]->indices);

    /* construction des meshs */
    if (gen_indices)
        make = makeindices;
    else
        make = makevertices;
    for (i=1; i<*n_objs; i++)
        WAR_ASSERT (make (meshs[i]) < 0)

    /* on construit le premier en dernier car on l'a autorise
       a supprimer les donnees de sommet a sa suppression (canfree) */
    WAR_ASSERT (make (meshs[0]) < 0)

    return meshs;
}
