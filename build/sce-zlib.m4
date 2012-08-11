dnl SCE_CHECK_ZLIB
dnl -----------------
dnl Checks for zlib and fails if not found
AC_DEFUN([SCE_CHECK_ZLIB],
[
    SCE_REQUIRE_HEADER([zlib.h])
    SCE_REQUIRE_LIB([z], [inflate])
    dnl those are fake flags in case they will become useful, but currently
    dnl AC_CHECK_LIB (called through SCE_REQUIRE_LIB) fills LIBS as needed
    AC_SUBST([ZLIB_CFLAGS], [])
    AC_SUBST([ZLIB_LIBS], [])
])
