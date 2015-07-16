#---------------compile related--------
CC = echo $(notdir $<);arm-none-linux-gnueabi-gcc
CCPP = echo $(notdir $<);arm-none-linux-gnueabi-g++
AS = as
AR = echo $(notdir $<);ar
LD = echo $(notdir $@);ld
RM = rm
MAKE = make
MKDIR = mkdir

CFLAGS = -c -g -O2 -Wall -DLCD_SIZE_43
LIB_A_FLAGS = -r
LIB_SO_FLAGS = -shared -fPic
LD_FLAGS =
RM_FLAGS = -f
MKDIR_FLAGS =

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

