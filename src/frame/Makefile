.PHONY: all clean

include ../../make.h
include make.h


$(FRAME_TARGET): $(FRAME_OBJ)
	$(AR) rc $@ $^

ifneq ($(MAKECMDGOALS), clean)
sinclude $(FRAME_DEP)
endif

$(FRAME_OBJ_DIR)/%.dep: $(FRAME_SRC_DIR)/%.c
	@set -e ; \
	$(RM) -fr $@.tmp ; \
	$(CC_NO_ECHO) -E -MM -g -w $(filter %.c, $^) > $@.tmp ; \
	sed 's,\($*\)\.o[ :]*,obj/\1.o $@: ,g' < $@.tmp > $@ ; \
	$(RM) -fr $@.tmp

clean:
	@echo "Cleaning dhcp auto config object files"
	$(RM) -f *.o
	$(RM) -f *.dep
	$(RM) -f $(FRAME_TARGET)

print:
	@echo FRAME_INC_DIR=${FRAME_INC_DIR}
	@echo FRAME_SRC_DIR=${FRAME_SRC_DIR}
	@echo FRAME_OBJ_DIR=${FRAME_OBJ_DIR}
	@echo _FRAME_OBJ_DIR=${_FRAME_OBJ_DIR}
	@echo FRAME_DIRS=${FRAME_DIRS}
	@echo FRAME_RMS=${FRAME_RMS}
	@echo FRAME_SRC=${FRAME_SRC}
	@echo FRAME_OBJ=${FRAME_OBJ}
	@echo FRAME_DEP=${FRAME_DEP}
	@echo FRAME_INC=${FRAME_INC}
	@echo FRAME_CFLAGS=${FRAME_CFLAGS}

