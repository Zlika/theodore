#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([dcto8d], [2009.05])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([src/dc6809dass.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([SDL2], [sdl2 >= 2.0.0])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
# -fstack-protector-strong only exists since GCC 4.9, use -fstack-protector as fallback
AC_MSG_CHECKING(whether compiler understands -fstack-protector-strong)
old_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -fstack-protector-strong"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],[])],
	AC_MSG_RESULT(yes); STACKPROTECT_FLAG="-fstack-protector-strong",
	AC_MSG_RESULT(no); STACKPROTECT_FLAG="-fstack-protector")
CFLAGS="$old_CFLAGS"
AC_SUBST(STACKPROTECT_FLAG)

# Checks for library functions.
AC_CHECK_FUNCS([atexit getcwd memset strcasecmp strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
