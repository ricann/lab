.SUFFIXES : .c .cpp .o#变量"SUFFIXE"用来定义默认的后缀列表

OBJECTS = ./main.o						\
		  ./Common/performance.o		\
		  ./Common/LogMsg.o				\
		  ./FrameExtractor/FileRead.o 		\
		  ./FrameExtractor/FrameExtractor.o 	\
		  ./FrameExtractor/H263Frames.o		\
		  ./FrameExtractor/H264Frames.o		\
		  ./FrameExtractor/MPEG4Frames.o	\
		  ./FrameExtractor/VC1Frames.o		\
		  ./MFC_API/SsbSipH264Decode.o	\
		  ./MFC_API/SsbSipH264Encode.o	\
		  ./MFC_API/SsbSipMfcDecode.o	\
		  ./MFC_API/SsbSipMpeg4Decode.o	\
		  ./MFC_API/SsbSipMpeg4Encode.o	\
		  ./MFC_API/SsbSipVC1Decode.o \
		  ./CharSet.o            \
		  ./YUVOSDMixer.o        \
		  ./Raptor/raptorcode.o		\
		  ./Raptor/matrix.o     


LIBRARY = ../lib/libasound.so

CC = arm-none-linux-gnueabi-g++ 

CFLAGS = -g -c -Os -Wall -DLCD_SIZE_43 
INC = -I./Common -I./FrameExtractor -I./MFC_API  -I/usr/share/arm-alsa/include  -I./Raptor -I/usr/local/arm/4.3.2/arm-none-linux-gnueabi/include/opencv

TARGET = multimedia_test

all : common frame_extractor  mfc_api raptor multimedia_test
common : 
		cd Common; $(MAKE)

frame_extractor : 
		cd FrameExtractor; $(MAKE)

mfc_api : 
		cd MFC_API; $(MAKE)

raptor :
		cd Raptor; $(MAKE)

$(TARGET) : $(OBJECTS) $(LIBRARY)
		$(CC) $(OBJECTS) $(LIBRARY) -L/usr/local/arm/4.3.2/arm-none-linux-gnueabi/lib -lcv -lcxcore -lz -lpthread -o$(TARGET)
#规则表示所有的 .o文件都是依赖与相应的.c文件的
.c.o :
		$(CC) $(INC) $(CFLAGS) $<
.cpp.o :
		$(CC) $(INC) $(CFLAGS) $<

clean :
		rm -rf $(OBJECTS) $(TARGET) core

