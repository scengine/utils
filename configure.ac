dnl Process this file with autoconf to produce a configure script.

dnl prelude
AC_PREREQ([2.60])
AC_INIT([SCEngine utility library],
        [0.1.0],
        [martin.antony@yahoo.fr],
        [sceutils])
AC_CONFIG_SRCDIR([src/SCEUtils.c])
AC_CONFIG_AUX_DIR([build/aux])
AC_CONFIG_MACRO_DIR([build/m4])
AM_INIT_AUTOMAKE([1.10 -Wall -Werror foreign])
AM_PROG_AR

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl check for programs
LT_PREREQ([2.2.0])
LT_INIT
AC_PROG_CC

SCE_CHECK_DOXYGEN

dnl basic setup

dnl
dnl revision of the library
dnl
dnl CURRENT:REVISION:AGE
dnl
dnl Please remeber to bump library version before releases
dnl   Remeberer:
dnl =====================================================================
dnl |   If vou have...                              |   Bump version to |
dnl =====================================================================
dnl | Not changed the interface (bug fixes)         | CURRENT:REV+1:AGE |
dnl ---------------------------------------------------------------------
dnl | Augmented the interface (new functions)       | CURRENT+1:0:AGE+1 |
dnl ---------------------------------------------------------------------
dnl | Broken old interface (e.g. removed functions) | CURRENT+1:0:0     |
dnl ---------------------------------------------------------------------
SCE_UTILS_LTVERSION="0:0:0"
AC_SUBST([SCE_UTILS_LTVERSION])

pkgconfigdir="$libdir/pkgconfig"
sce_includedir="$includedir/SCE"
sce_include_utilsdir="$sce_includedir/utils"
AC_SUBST([pkgconfigdir])
AC_SUBST([sce_includedir])
AC_SUBST([sce_include_utilsdir])

dnl legacy version names
AC_DEFINE([SCE_UTILS_VERSION_STRING], [VERSION], [Version of the SCEngine])

dnl check for libs & headers

SCE_CHECK_PTHREAD
SCE_CHECK_ZLIB
dnl we need math.h and -lm
SCE_REQUIRE_HEADER([math.h])
SCE_REQUIRE_LIB([m], [acos])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h ctype.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

dnl Checks for library functions.
dnl required functions
SCE_REQUIRE_FUNCS([vsnprintf memset pow sqrt strerror strstr memcpy])
dnl Conditional functions (we use them if possible)
AC_CHECK_FUNCS([fabsf cosf sinf tanf powf sqrtf atanf atan2f])

SCE_CHECK_DEBUG

dnl output files
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile
                 Doxyfile
                 doc/Makefile
                 src/Makefile
                 include/Makefile
                 include/SCE/Makefile
                 include/SCE/utils/Makefile
                 sceutils.pc
                 ])
AC_OUTPUT

echo "------------------------------------------"
echo "SCEUtils version               : $VERSION ($SCE_UTILS_LTVERSION)"
echo "Debugging enabled              : $enable_debug"
echo "Base installation directory    : $prefix"
echo ""
echo "Configuration succeed."
echo ""
