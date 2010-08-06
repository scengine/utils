# Process this file with autoconf to produce a configure script.

# prelude
AC_PREREQ([2.60])
AC_INIT([SCEngine utility library],
        [0.1.0],
        [martin.antony@yahoo.fr],
        [sceutils])
AC_CONFIG_SRCDIR([src/SCEUtils.c])
AC_CONFIG_AUX_DIR([build/aux])
AC_CONFIG_MACRO_DIR([build/m4])
AM_INIT_AUTOMAKE([1.10 -Wall -Werror foreign])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# check for programs
LT_PREREQ([2.2.0])
LT_INIT
AC_PROG_CC

# basic setup

#
# revision of the library
# 
# CURRENT:REVISION:AGE
#
# Please remeber to bump library version before releases
#   Remeberer:
# =====================================================================
# |   If vou have...                              |   Bump version to |
# =====================================================================
# | Not changed the interface (bug fixes)         | CURRENT:REV+1:AGE |
# ---------------------------------------------------------------------
# | Augmented the interface (new functions)       | CURRENT+1:0:AGE+1 |
# ---------------------------------------------------------------------
# | Broken old interface (e.g. removed functions) | CURRENT+1:0:0     |
# ---------------------------------------------------------------------
SCE_UTILS_LTVERSION="0:0:0"
AC_SUBST([SCE_UTILS_LTVERSION])

pkgconfigdir="$libdir/pkgconfig"
sce_includedir="$includedir/SCE"
sce_include_utilsdir="$sce_includedir/utils"
AC_SUBST(pkgconfigdir)
AC_SUBST(sce_includedir)
AC_SUBST(sce_include_utilsdir)

# legacy version names
AC_DEFINE([SCE_UTILS_VERSION_STRING], [VERSION], [Version of the SCEngine])

# check for libs & headers

# AX_REQUIRE_HEADER(HEADER, IF-PRESENT)
# ----------------------------
# Check presence of HEADER and exit if not present, or execute IF-PRESENT if
# prsent
AC_DEFUN([AX_REQUIRE_HEADER],
         [AC_CHECK_HEADERS([$1],
                           [$2],
                           [AC_MSG_ERROR([[$1 not found]])])])

# AX_REQUIRE_LIB(LIBRARY, FUNC, IF-PRESENT)
# ----------------------------
# Check presence of HEADER and exit if not present, or execute IF-PRESENT if
# prsent
AC_DEFUN([AX_REQUIRE_LIB],
         [AC_CHECK_LIB([$1], [$2],
                       [$3],
                       [AC_MSG_ERROR([[$1 not found]])])])

AX_REQUIRE_HEADER([pthread.h])
AX_REQUIRE_LIB([pthread], [pthread_create])
AX_REQUIRE_HEADER([math.h])
AX_REQUIRE_LIB([m], [acos])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h ctype.h math.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_VPRINTF
# FIXME: these ones are not conditional, we need them
AC_CHECK_FUNCS([memset pow sqrt strerror strstr strtol strtoul memcpy])
# Conditional functions (we use them if possible)
AC_CHECK_FUNCS([fabsf cosf sinf tanf powf sqrtf atanf atan2f])

# paranoiac compilation
AC_ARG_ENABLE(
  [paranoia],
  AC_HELP_STRING([--enable-paranoia], [enable paranioac compiler options [[default=no]]]),
  [enable_paranoia="$enableval"],
  [enable_paranoia="no"]
)
AC_MSG_CHECKING([[whether to enable paranoiac compiler options]])
if test "x$enable_paranoia" = "xyes"; then
  CFLAGS="$CFLAGS -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int \
          -Werror-implicit-function-declaration -Wmain \
          -Wparentheses -Wsequence-point -Wreturn-type -Wswitch \
          -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas \
          -Wfloat-equal -Wundef -Wshadow -Wpointer-arith \
          -Wbad-function-cast -Wwrite-strings \
          -Wconversion \
          -Wsign-compare -Waggregate-return -Wstrict-prototypes \
          -Wmissing-prototypes -Wmissing-declarations \
          -Wmissing-noreturn -Wformat -Wmissing-format-attribute \
          -Wpacked -Wredundant-decls -Wnested-externs \
          -Winline -Wlong-long -Wunreachable-code"
  AC_MSG_RESULT([[yes]])
else
  AC_MSG_RESULT([[no]])
fi

# Debugging flags
AC_ARG_ENABLE(
  [debug_all],
  AC_HELP_STRING([--enable-debug-all], [enable all paranoiac and commonly useless debugging [[default=no]]]),
  [enable_debug_all="$enableval"
   enable_optimisation="no"
   enable_debug_stack="yes"
   enable_debug="yes"],
  [enable_debug_all="no"]
)
AC_ARG_ENABLE(
  [optimisation],
  AC_HELP_STRING([--disable-optimisation], [disable any compiler optimisation [[default=no]]]),
  [enable_optimisation="$enableval"],
  [enable_optimisation="yes"]
)
AC_ARG_ENABLE(
  [debug_stack],
  AC_HELP_STRING([--enable-debug-stack], [enable stack debugging [[default=no]]]),
  [enable_debug_stack="$enableval"],
  [enable_debug_stack="no"]
)
AC_ARG_ENABLE(
  [debug],
  AC_HELP_STRING([--enable-debug], [enable debugging [[default=yes]]]),
  [enable_debug="$enableval"],
  [enable_debug="yes"]
)
# normal debugging
AC_MSG_CHECKING([[whether to enable debugging]])
if test "x$enable_debug" = "xyes"; then
  AC_DEFINE([SCE_DEBUG], [1], [is debugging enabled])
  DEBUG_CFLAGS="-DSCE_DEBUG"
  CFLAGS="$CFLAGS $DEBUG_CFLAGS"
  AC_MSG_RESULT([[yes]])
else
  DEBUG_CFLAGS=
  AC_MSG_RESULT([[no]])
fi
AC_SUBST([DEBUG_CFLAGS])
# stack debugging
AC_MSG_CHECKING([[whether to enable stack debugging]])
if test "x$enable_debug_stack" = "xyes"; then
  CFLAGS="$CFLAGS -fstack-protector-all"
  AC_MSG_RESULT([[yes]])
else
  AC_MSG_RESULT([[no]])
fi
# optimisation debugging (must be the last change to CFLAGS)
AC_MSG_CHECKING([[whether to disable any compiler optimisation]])
if test "x$enable_optimisation" = "xno"; then
  CFLAGS="$CFLAGS -O0"
  AC_MSG_RESULT([[yes]])
else
  AC_MSG_RESULT([[no]])
fi

# check to Doxygen and generate right target for doc according to its presence
AC_CHECK_PROGS([DOXYGEN], [doxygen], [:])
if test "x$DOXYGEN" = "x:"; then
  AC_MSG_WARN([[Doxygen not found, you will not able to generate documentation]])
  DOC_TARGET='@echo "no compatible doc generator found, please install one then re-run configure" >&2; exit 1'
else
  DOC_TARGET='$(DOXYGEN) Doxyfile'
fi
AC_SUBST([DOC_TARGET])

# output files
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
echo "Paranoiac compiler options     : $enable_paranoia"
echo "Base installation directory    : $prefix"
echo ""
echo "Configuration succeed."
echo ""
