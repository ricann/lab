include ../../make.h

OBJECTS = ./main.o						\
		  ../common/performance.o		\
		  ../common/LogMsg.o				\
		  ../frame/FileRead.o 		\
		  ../frame/FrameExtractor.o 	\
		  ../frame/H263Frames.o		\
		  ../frame/H264Frames.o		\
		  ../frame/MPEG4Frames.o	\
		  ../frame/VC1Frames.o		\
		  ../mfc/SsbSipH264Decode.o	\
		  ../mfc/SsbSipH264Encode.o	\
		  ../mfc/SsbSipMfcDecode.o	\
		  ../mfc/SsbSipMpeg4Decode.o	\
		  ../mfc/SsbSipMpeg4Encode.o	\
		  ../mfc/SsbSipVC1Decode.o \
		  ./CharSet.o            \
		  ./YUVOSDMixer.o        \
		  ../raptor/raptorcode.o		\
		  ../raptor/matrix.o     


LIBRARY = ../../lib/libasound.so

INC = -I../common 	\
	  -I../frame 	\
	  -I../mfc		\
	  -I../raptor 	\
	  -I../../inc/alsa 	\
	  -I/usr/local/arm/4.3.2/arm-none-linux-gnueabi/include/opencv

video_demo : $(OBJECTS) $(LIBRARY)
		$(CCPP) $(OBJECTS) $(LIBRARY) -L/usr/local/arm/4.3.2/arm-none-linux-gnueabi/lib -lcv -lcxcore -lz -lpthread -ovideo_demo

.c.o :
		$(CCPP) $(INC) $(CFLAGS) $<
.cpp.o :
		$(CCPP) $(INC) $(CFLAGS) $<

clean :
		rm -rf *.o video_demo

