# Project: dcto8d

objects = object/dcto8dmain.o object/dc6809emul.o \
	object/dcto8dinterface.o object/dcto8ddevices.o object/dcto8demulation.o \
	object/dcto8dkeyb.o object/dcto8doptions.o object/dcto8dvideo.o \
	object/dc6809dass.o object/dcto8ddesass.o

dcto8d : $(objects)
	cc -o dcto8d -lSDL $(objects)

object/dcto8dmain.o : source/dcto8dmain.c
	cc -c source/dcto8dmain.c -o object/dcto8dmain.o -O2

object/dc6809emul.o : source/dc6809emul.c
	cc -c source/dc6809emul.c -o object/dc6809emul.o -O2

object/dcto8dinterface.o : source/dcto8dinterface.c
	cc -c source/dcto8dinterface.c -o object/dcto8dinterface.o -O2

object/dcto8ddevices.o : source/dcto8ddevices.c
	cc -c source/dcto8ddevices.c -o object/dcto8ddevices.o -O2

object/dcto8demulation.o : source/dcto8demulation.c
	cc -c source/dcto8demulation.c -o object/dcto8demulation.o -O2

object/dcto8dkeyb.o : source/dcto8dkeyb.c source/dcto8dkeyb.h
	cc -c source/dcto8dkeyb.c -o object/dcto8dkeyb.o -O2

object/dcto8doptions.o : source/dcto8doptions.c
	cc -c source/dcto8doptions.c -o object/dcto8doptions.o -O2

object/dcto8dvideo.o : source/dcto8dvideo.c
	cc -c source/dcto8dvideo.c -o object/dcto8dvideo.o -O2

object/dc6809dass.o: source/dc6809dass.c source/dc6809dass.h
	cc -c source/dc6809dass.c -o object/dc6809dass.o $(CFLAGS)

object/dcto8ddesass.o: source/dcto8ddesass.c
	cc -c source/dcto8ddesass.c -o object/dcto8ddesass.o -O2

clean :
	rm dcto8d $(objects)