
CC=arm-none-linux-gnueabi-gcc
CC_FLAGS = -g -c -Os -Wall
LIBRARY = /usr/local/arm/alsa/lib/libasound.so
INC=-I/usr/local/arm/alsa/include



audio: audio.o $(LIBRARY)
	$(CC) -o $@ audio.o $(LIBRARY) -lm -ldl -lpthread

audio.o: audio.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

clean:
	$(RM) audio.o audio
