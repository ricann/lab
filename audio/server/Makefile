
#CC=arm-none-linux-gnueabi-gcc
CC=gcc
CC_FLAGS = -g -c -Os -Wall
#LIBRARY = /usr/local/arm/alsa/lib/libasound.so
LIBRARY = /usr/lib/i386-linux-gnu/libasound.so
INC=-I/usr/local/arm/alsa/include



udpts: udpts.o $(LIBRARY)
	$(CC) -o $@ udpts.o $(LIBRARY) -lm -ldl -lpthread

udpts.o: udpts.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

clean:
	$(RM) udpts.o udpts
