#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([theodore], m4_esyscmd([git describe --dirty --always --tags | tr -d '\n']))
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([src/6809disasm.h])
AC_CONFIG_HEADERS([config.h])

# Libtool to generate the libretro shared library.
LT_PREREQ([2.4])
LT_INIT([disable-static])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([SDL2], [sdl2 >= 2.0.0])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stddef.h stdint.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
# -fstack-protector-strong only exists since GCC 4.9, use -fstack-protector as fallback
AC_MSG_CHECKING(whether compiler understands -fstack-protector-strong)
old_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -fstack-protector-strong"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],[])],
	AC_MSG_RESULT(yes); STACKPROTECT_FLAG="-fstack-protector-strong",
	AC_MSG_RESULT(no); STACKPROTECT_FLAG="-fstack-protector")
CFLAGS="$old_CFLAGS"
AC_SUBST(STACKPROTECT_FLAG)
# Some hardening flags does not exist in old versions of Clang
AC_MSG_CHECKING(whether linker understands hardening flags)
old_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS -pie -z relro -z now"
AC_LINK_IFELSE([AC_LANG_PROGRAM([],[])],
	AC_MSG_RESULT(yes); LINKER_HARDENING_FLAGS="-pie -z relro -z now",
	AC_MSG_RESULT(no); LINKER_HARDENING_FLAGS="")
LDFLAGS="$old_LDFLAGS"
AC_SUBST(LINKER_HARDENING_FLAGS)

# Checks for library functions.
AC_CHECK_FUNCS([atexit getcwd memset strcasecmp strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
