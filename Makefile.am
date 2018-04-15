AM_CFLAGS = --pedantic -Wall -Wno-overflow -Werror-implicit-function-declaration -std=c99 -O2 -fsigned-char -Wformat -Wformat-security -Werror=format-security -D_FORTIFY_SOURCE=2 $(STACKPROTECT_FLAG) -fPIE
AM_LDFLAGS = $(LINKER_HARDENING_FLAGS)
theodore_CFLAGS = $(SDL2_CFLAGS) $(AM_CFLAGS)
theodore_LDFLAGS = $(SDL2_LIBS) $(AM_LDFLAGS)

bin_PROGRAMS = theodore
theodore_SOURCES = \
                 src/6809disasm.c \
				 src/6809disasm.h \
				 src/6809emulator.c \
				 src/6809emulator.h \
				 src/devices.c \
				 src/devices.h \
				 src/disasmui.c \
				 src/disasmui.h \
				 src/font.h \
				 src/global.h \
				 src/icon.h \
				 src/interface.c \
				 src/interface.h \
				 src/keyboard.c \
				 src/keyboard.h \
				 src/main.c \
				 src/msg.c \
				 src/msg.h \
				 src/options.c \
				 src/options.h \
				 src/to8dbasic.h \
				 src/to8demulator.c \
				 src/to8demulator.h \
				 src/to8dmoniteur.h \
				 src/video.c \
				 src/video.h

