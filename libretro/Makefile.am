AM_CFLAGS = -D__LIBRETRO__ -shared -pedantic -Wall -Wno-overflow -Werror-implicit-function-declaration -std=c99 -O2 -fsigned-char -Wformat -Wformat-security -Werror=format-security -D_FORTIFY_SOURCE=2 $(STACKPROTECT_FLAG) -fPIC
AM_LDFLAGS = $(LINKER_HARDENING_FLAGS)
theodore_libretro_la_CFLAGS = $(SDL2_CFLAGS) -Ilibretro-common/include $(AM_CFLAGS)
theodore_libretro_la_LDFLAGS = $(SDL2_LIBS) $(AM_LDFLAGS) -module -avoid-version -shared -no-undefined -version-script=link.T

lib_LTLIBRARIES = theodore_libretro.la
theodore_libretro_la_SOURCES = \
				 ../src/6809emulator.c \
				 ../src/6809emulator.h \
				 ../src/devices.c \
				 ../src/devices.h \
				 ../src/global.h \
				 ../src/options.c \
				 ../src/options.h \
				 ../src/to8dbasic.h \
				 ../src/to8demulator.c \
				 ../src/to8demulator.h \
				 ../src/to8dmoniteur.h \
				 ../src/video.c \
				 ../src/video.h \
				 libretro-common/include/libretro.h \
				 libretro.c
