
CC=arm-none-linux-gnueabi-gcc
#CC=gcc
CC_FLAGS = -g -c -O2 -Wall
LIBRARY = /usr/local/arm/alsa/lib/libasound.so
#LIBRARY = /usr/lib/i386-linux-gnu/libasound.so
INC=-I/usr/local/arm/alsa/include

all: audioc udptc

audioc: audioc.o $(LIBRARY)
	$(CC) -o $@ audioc.o $(LIBRARY) -lm -ldl

audioc.o: audioc.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

udptc: udptc.o $(LIBRARY)
	$(CC) -o $@ udptc.o $(LIBRARY) -lm -ldl

udptc.o: udptc.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

clean:
	$(RM) audioc.o audioc
	$(RM) udptc.o udptc
