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

/* created: 28/07/2009
   updated: 28/07/2009 */

#ifndef SCECOMMON_H
#define SCECOMMON_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif


/* almost stolen from GLib's gmacros.h,
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 * under the terms of the GNU LGPL v2+ (see glib/gmacros.h for more complete
 * credits) */

/* Here we provide SCE_GNUC_EXTENSION as an alias for __extension__,
 * where this is valid. This allows for warningless compilation of
 * "long long" types even in the presence of '-ansi -pedantic'. 
 */
#if     __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8)
#  define SCE_GNUC_EXTENSION __extension__
#else
#  define SCE_GNUC_EXTENSION
#endif

/* Provide macros to feature the GCC function attribute.
 */
#if    __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
#define SCE_GNUC_PURE                            \
  __attribute__((__pure__))
#define SCE_GNUC_MALLOC    			\
  __attribute__((__malloc__))
#else
#define SCE_GNUC_PURE
#define SCE_GNUC_MALLOC
#endif

#if     __GNUC__ >= 4
#define SCE_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
#else
#define SCE_GNUC_NULL_TERMINATED
#endif

#if     (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)
#define SCE_GNUC_ALLOC_SIZE(x) __attribute__((__alloc_size__(x)))
#define SCE_GNUC_ALLOC_SIZE2(x,y) __attribute__((__alloc_size__(x,y)))
#else
#define SCE_GNUC_ALLOC_SIZE(x)
#define SCE_GNUC_ALLOC_SIZE2(x,y)
#endif

#if     __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define SCE_GNUC_PRINTF( format_idx, arg_idx )    \
  __attribute__((__format__ (__printf__, format_idx, arg_idx)))
#define SCE_GNUC_SCANF( format_idx, arg_idx )     \
  __attribute__((__format__ (__scanf__, format_idx, arg_idx)))
#define SCE_GNUC_FORMAT( arg_idx )                \
  __attribute__((__format_arg__ (arg_idx)))
#define SCE_GNUC_NORETURN                         \
  __attribute__((__noreturn__))
#define SCE_GNUC_CONST                            \
  __attribute__((__const__))
#define SCE_GNUC_UNUSED                           \
  __attribute__((__unused__))
#define SCE_GNUC_NO_INSTRUMENT			\
  __attribute__((__no_instrument_function__))
#else   /* !__GNUC__ */
#define SCE_GNUC_PRINTF( format_idx, arg_idx )
#define SCE_GNUC_SCANF( format_idx, arg_idx )
#define SCE_GNUC_FORMAT( arg_idx )
#define SCE_GNUC_NORETURN
#define SCE_GNUC_CONST
#define SCE_GNUC_UNUSED
#define SCE_GNUC_NO_INSTRUMENT
#endif  /* !__GNUC__ */

#if    __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#define SCE_GNUC_DEPRECATED                            \
  __attribute__((__deprecated__))
#else
#define SCE_GNUC_DEPRECATED
#endif /* __GNUC__ */

#if     __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)
#  define SCE_GNUC_MAY_ALIAS __attribute__((may_alias))
#else
#  define SCE_GNUC_MAY_ALIAS
#endif

#if    __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#define SCE_GNUC_WARN_UNUSED_RESULT 		\
  __attribute__((warn_unused_result))
#else
#define SCE_GNUC_WARN_UNUSED_RESULT
#endif /* __GNUC__ */

/* Provide a string identifying the current function, non-concatenatable */
#if defined (__GNUC__)
#  define SCE_FUNCTION     ((const char*) (__PRETTY_FUNCTION__))
#elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 19901L
#  define SCE_FUNCTION     ((const char*) (__func__))
#else
#  define SCE_FUNCTION     ((const char*) ("???"))
#endif

/* end of stolen GLib macros */

#if   __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)
#  define SCE_GNUC_NONNULL              __attribute__((nonnull))
#  define SCE_GNUC_NONNULL1(a)          __attribute__((nonnull (a)))
#  define SCE_GNUC_NONNULL2(a, b)       __attribute__((nonnull (a, b)))
#  define SCE_GNUC_NONNULL3(a, b, c)    __attribute__((nonnull (a, b, c)))
#else
#  define SCE_GNUC_NONNULL
#  define SCE_GNUC_NONNULL1
#  define SCE_GNUC_NONNULL2
#  define SCE_GNUC_NONNULL3
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* guard */
