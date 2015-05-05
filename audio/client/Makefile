
CC=arm-none-linux-gnueabi-gcc
#CC=gcc
CC_FLAGS = -g -c -Os -Wall
LIBRARY = /usr/local/arm/alsa/lib/libasound.so
#LIBRARY = /usr/lib/i386-linux-gnu/libasound.so
INC=-I/usr/local/arm/alsa/include



udptc: udptc.o $(LIBRARY)
	$(CC) -o $@ udptc.o $(LIBRARY) -lm -ldl -lpthread

udptc.o: udptc.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

clean:
	$(RM) udptc.o udptc
