AM_CFLAGS = -D__LIBRETRO__ -shared -pedantic -Wall -Wno-overflow -Werror-implicit-function-declaration -std=c99 -O2 -fsigned-char -Wformat -Wformat-security -Werror=format-security -D_FORTIFY_SOURCE=2 $(STACKPROTECT_FLAG) -fPIC
AM_LDFLAGS = $(LINKER_HARDENING_FLAGS)
dcto8d_libretro_la_CFLAGS = $(SDL2_CFLAGS) -Ilibretro-common/include $(AM_CFLAGS)
dcto8d_libretro_la_LDFLAGS = $(SDL2_LIBS) $(AM_LDFLAGS) -module -avoid-version -shared -no-undefined -version-script=link.T

lib_LTLIBRARIES = dcto8d_libretro.la
dcto8d_libretro_la_SOURCES = \
				 ../src/dc6809emul.c \
				 ../src/dc6809emul.h \
				 ../src/dcto8ddevices.c \
				 ../src/dcto8ddevices.h \
				 ../src/dcto8demulation.c \
				 ../src/dcto8demulation.h \
				 ../src/dcto8dglobal.h \
				 ../src/dcto8doptions.c \
				 ../src/dcto8doptions.h \
				 ../src/dcto8dvideo.c \
				 ../src/dcto8dvideo.h \
				 ../src/to8dbasic.h \
				 ../src/to8dmoniteur.h \
				 libretro-common/include/libretro.h \
				 libretro.c
