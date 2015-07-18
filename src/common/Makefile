.PHONY: all clean

include ../../make.h
include make.h


$(COMMON_TARGET): $(COMMON_OBJ)
	$(AR) rc $@ $^

ifneq ($(MAKECMDGOALS), clean)
sinclude $(COMMON_DEP)
endif

$(COMMON_OBJ_DIR)/%.dep: $(COMMON_SRC_DIR)/%.c
	@set -e ; \
	$(RM) -fr $@.tmp ; \
	$(CC_NO_ECHO) -E -MM -g -w $(filter %.c, $^) > $@.tmp ; \
	sed 's,\($*\)\.o[ :]*,\1.o $@: ,g' < $@.tmp > $@ ; \
	$(RM) -fr $@.tmp

clean:
	@echo "Cleaning dhcp auto config object files"
	$(RM) -f *.o
	$(RM) -f *.dep
	$(RM) -f $(COMMON_TARGET)

print:
	@echo COMMON_INC_DIR=${COMMON_INC_DIR}
	@echo COMMON_SRC_DIR=${COMMON_SRC_DIR}
	@echo COMMON_OBJ_DIR=${COMMON_OBJ_DIR}
	@echo _COMMON_OBJ_DIR=${_COMMON_OBJ_DIR}
	@echo COMMON_DIRS=${COMMON_DIRS}
	@echo COMMON_RMS=${COMMON_RMS}
	@echo COMMON_SRC=${COMMON_SRC}
	@echo COMMON_OBJ=${COMMON_OBJ}
	@echo COMMON_DEP=${COMMON_DEP}
	@echo COMMON_INC=${COMMON_INC}
	@echo COMMON_CFLAGS=${COMMON_CFLAGS}

