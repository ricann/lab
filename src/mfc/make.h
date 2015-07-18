
MFC_INC_DIR		= $(MFC_BASE)
MFC_SRC_DIR		= $(MFC_BASE)
MFC_OBJ_DIR		= $(MFC_BASE)

MFC_DIRS		= $(MFC_INC_DIR)  $(MFC_SRC_DIR)  $(MFC_OBJ_DIR)
MFC_SRC		= $(wildcard $(MFC_SRC_DIR)/*.c)

MFC_OBJ		= $(MFC_SRC:.c=.o)
MFC_OBJ		:= $(filter %.o, $(MFC_OBJ))
MFC_OBJ		:= $(patsubst $(MFC_SRC_DIR)/%, $(MFC_OBJ_DIR)/%, $(MFC_OBJ)) 

MFC_DEP		= $(MFC_SRC:.c=.dep)
MFC_DEP		:= $(filter %.dep, $(MFC_DEP))
MFC_DEP		:= $(patsubst $(MFC_SRC_DIR)/%, $(MFC_OBJ_DIR)/%, $(MFC_DEP)) 

MFC_INC		= -I$(COMMON_BASE)

MFC_CFLAGS	= $(CFLAGS)


