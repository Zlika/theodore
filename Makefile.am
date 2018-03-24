AM_CFLAGS = --pedantic -Wall -Wno-overflow -std=c99 -O2 -fsigned-char -Wformat -Wformat-security -Werror=format-security -D_FORTIFY_SOURCE=2 $(STACKPROTECT_FLAG) -fPIE -pie
AM_LDFLAGS = -z relro -z now
dcto8d_CFLAGS = $(SDL2_CFLAGS) $(AM_CFLAGS)
dcto8d_LDFLAGS = $(SDL2_LIBS) $(AM_LDFLAGS)

bin_PROGRAMS = dcto8d
dcto8d_SOURCES = src/dc6809dass.c \
				 src/dc6809dass.h \
				 src/dc6809emul.c \
				 src/dc6809emul.h \
				 src/dcto8ddesass.c \
				 src/dcto8ddesass.h \
				 src/dcto8ddevices.c \
				 src/dcto8ddevices.h \
				 src/dcto8demulation.c \
				 src/dcto8demulation.h \
				 src/dcto8dfont.h \
				 src/dcto8dglobal.h \
				 src/dcto8dicon.h \
				 src/dcto8dinterface.c \
				 src/dcto8dinterface.h \
				 src/dcto8dkeyb.c \
				 src/dcto8dkeyb.h \
				 src/dcto8dmain.c \
				 src/dcto8dmsg.c \
				 src/dcto8dmsg.h \
				 src/dcto8doptions.c \
				 src/dcto8doptions.h \
				 src/dcto8dvideo.c \
				 src/dcto8dvideo.h \
				 src/to8dbasic.h \
				 src/to8dmoniteur.h

