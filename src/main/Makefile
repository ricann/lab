.SUFFIXES : .c .cpp .o#变量"SUFFIXE"用来定义默认的后缀列表

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

CC = arm-none-linux-gnueabi-g++ 

CFLAGS = -g -c -Os -Wall -DLCD_SIZE_43 
INC = -I../common 	\
	  -I../frame 	\
	  -I../mfc		\
	  -I../raptor 	\
	  -I../../inc/alsa 	\
	  -I/usr/local/arm/4.3.2/arm-none-linux-gnueabi/include/opencv

TARGET = multimedia_test

all : common frame_extractor  mfc_api raptor multimedia_test
common : 
		cd ../common; $(MAKE)

frame_extractor : 
		cd ../frame; $(MAKE)

mfc_api : 
		cd ../mfc; $(MAKE)

raptor :
		cd ../raptor; $(MAKE)

$(TARGET) : $(OBJECTS) $(LIBRARY)
		$(CC) $(OBJECTS) $(LIBRARY) -L/usr/local/arm/4.3.2/arm-none-linux-gnueabi/lib -lcv -lcxcore -lz -lpthread -o$(TARGET)
#规则表示所有的 .o文件都是依赖与相应的.c文件的
.c.o :
		$(CC) $(INC) $(CFLAGS) $<
.cpp.o :
		$(CC) $(INC) $(CFLAGS) $<

clean :
		rm -rf $(OBJECTS) $(TARGET) core

