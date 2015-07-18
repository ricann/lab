#---------------compile related--------
CC = @echo $(notdir $<);arm-none-linux-gnueabi-gcc
CC_NO_ECHO = arm-none-linux-gnueabi-gcc
CCPP = @echo $(notdir $<);arm-none-linux-gnueabi-g++
AR = @echo $@;arm-none-linux-gnueabi-ar
AS = @echo $(notdir $@);arm-none-linux-gnueabi-as
LD = @echo $(notdir $@);arm-none-linux-gnueabi-ld
RM = rm
MAKE = make
MKDIR = mkdir

#CFLAGS = -g -c -O2 -Wall -DLCD_SIZE_43
CFLAGS = -g -c -O2 -Wall
LIB_A_FLAGS = -r
LIB_SO_FLAGS = -shared -fPic
LD_FLAGS =
RM_FLAGS = -f
MKDIR_FLAGS =

#--------system dir-------------------
ARM_INC_BASE = /usr/local/arm/4.3.2/arm-none-linux-gnueabi/include
ARM_OPENCV_BASE = $(ARM_INC_BASE)/opencv

#--------1st level dir----------------
LIB_BASE = ${VIDEO_BASE}/lib
INC_BASE = ${VIDEO_BASE}/inc
SRC_BASE = ${VIDEO_BASE}/src

#--------2st level dir----------------
COMMON_BASE = ${SRC_BASE}/common
FRAME_BASE = ${SRC_BASE}/frame
MFC_BASE = ${SRC_BASE}/mfc
RAPTOR_BASE = ${SRC_BASE}/raptor
MAIN_BASE = ${SRC_BASE}/main

#--------all targets------------------
COMMON_TARGET = $(LIB_BASE)/libcommon.a
FRAME_TARGET = $(LIB_BASE)/libframe.a
MFC_TARGET = $(LIB_BASE)/libmfc.a
RAPTOR_TARGET = $(LIB_BASE)/libraptor.a

MAIN_TARGET = $(VIDEO_BASE)/video_demo

