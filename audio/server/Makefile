
#CC=arm-none-linux-gnueabi-gcc
CC=gcc
CC_FLAGS = -g -c -Os -Wall
#LIBRARY = /usr/local/arm/alsa/lib/libasound.so
LIBRARY = /usr/lib/i386-linux-gnu/libasound.so
INC=-I/usr/local/arm/alsa/include

all: audios udpts mcasts

audios: audios.o $(LIBRARY)
	$(CC) -o $@ audios.o $(LIBRARY) -lm -ldl

audios.o: audios.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

udpts: udpts.o $(LIBRARY)
	$(CC) -o $@ udpts.o $(LIBRARY) -lm -ldl

udpts.o: udpts.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

mcasts: mcasts.o $(LIBRARY)
	$(CC) -o $@ mcasts.o $(LIBRARY) -lm -ldl

mcasts.o: mcasts.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

clean:
	$(RM) audios.o audios
	$(RM) udpts.o udpts
	$(RM) mcasts.o mcasts
