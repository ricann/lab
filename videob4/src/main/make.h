
MAIN_INC_DIR		= $(MAIN_BASE)
MAIN_SRC_DIR		= $(MAIN_BASE)
MAIN_OBJ_DIR		= $(MAIN_BASE)

MAIN_DIRS		= $(MAIN_INC_DIR)  $(MAIN_SRC_DIR)  $(MAIN_OBJ_DIR)
MAIN_SRC		= $(wildcard $(MAIN_SRC_DIR)/*.cpp)

MAIN_OBJ		= $(MAIN_SRC:.cpp=.o)
MAIN_OBJ		:= $(filter %.o, $(MAIN_OBJ))
MAIN_OBJ		:= $(patsubst $(MAIN_SRC_DIR)/%, $(MAIN_OBJ_DIR)/%, $(MAIN_OBJ)) 

MAIN_DEP		= $(MAIN_SRC:.cpp=.dep)
MAIN_DEP		:= $(filter %.dep, $(MAIN_DEP))
MAIN_DEP		:= $(patsubst $(MAIN_SRC_DIR)/%, $(MAIN_OBJ_DIR)/%, $(MAIN_DEP)) 

MAIN_INC		= -I$(COMMON_BASE)	\
				-I$(FRAME_BASE)		\
				-I$(MFC_BASE)		\
				-I$(RAPTOR_BASE)	\
				-I$(INC_BASE)/alsa	\
				-I$(ARM_OPENCV_BASE)

MAIN_LIB = $(LIB_BASE)/libasound.so \
			$(LIB_BASE)/libcommon.a	\
			$(LIB_BASE)/libframe.a	\
			$(LIB_BASE)/libmfc.a	\
			$(LIB_BASE)/libraptor.a	

MAIN_LIB_PATH	= -L$(ARM_LIB_BASE)

MAIN_CFLAGS		= $(CFLAGS) 

MAIN_LDFLAGS	= -lm			\
				-lcv		\
				-lcxcore	\
				-lz			\
				-lpthread


