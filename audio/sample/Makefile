
#CC=arm-none-linux-gnueabi-gcc
CC=gcc
CC_FLAGS = -g -c -Os -Wall
#LIBRARY = /usr/local/arm/alsa/lib/libasound.so
LIBRARY = /usr/lib/i386-linux-gnu/libasound.so
INC=-I/usr/local/arm/alsa/include

all:cap play param pcminfo test

cap: cap.o $(LIBRARY)
	$(CC) -o $@ cap.o $(LIBRARY) -lm -ldl -lpthread

cap.o: cap.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

play: play.o $(LIBRARY)
	$(CC) -o $@ play.o $(LIBRARY) -lm -ldl -lpthread

play.o: play.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

param: param.o $(LIBRARY)
	$(CC) -o $@ param.o $(LIBRARY) -lm -ldl -lpthread

param.o: param.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

pcminfo: pcminfo.o $(LIBRARY)
	$(CC) -o $@ pcminfo.o $(LIBRARY) -lm -ldl -lpthread

pcminfo.o: pcminfo.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

test: test.o $(LIBRARY)
	$(CC) -o $@ test.o $(LIBRARY) -lm -ldl -lpthread

test.o: test.c
	$(CC) $(INC) $(CC_FLAGS) -o $@ $<

clean:
	$(RM) cap.o audio
	$(RM) play.o play
	$(RM) param.o param
	$(RM) pcminfo.o pcminfo
	$(RM) test.o test
