.PHONY: all clean

include ../../make.h
include make.h


$(MFC_TARGET): $(MFC_OBJ)
	$(AR) rc $@ $^

#define compile rules
%.o: %.c
	$(CC) $(MFC_CFLAGS) $(MFC_INC) $< -o $@

ifneq ($(MAKECMDGOALS), clean)
sinclude $(MFC_DEP)
endif

$(MFC_OBJ_DIR)/%.dep: $(MFC_SRC_DIR)/%.c
	@set -e ; \
	$(RM) -fr $@.tmp ; \
	$(CC_NO_ECHO) $(MFC_CFLAGS) $(MFC_INC) -E -MM $(filter %.c, $^) > $@.tmp ; \
	sed 's,\($*\)\.o[ :]*,obj/\1.o $@: ,g' < $@.tmp > $@ ; \
	$(RM) -fr $@.tmp

clean:
	@echo "Cleaning dhcp auto config object files"
	$(RM) -f *.o
	$(RM) -f *.dep
	$(RM) -f $(MFC_TARGET)

print:
	@echo MFC_INC_DIR=${MFC_INC_DIR}
	@echo MFC_SRC_DIR=${MFC_SRC_DIR}
	@echo MFC_OBJ_DIR=${MFC_OBJ_DIR}
	@echo _MFC_OBJ_DIR=${_MFC_OBJ_DIR}
	@echo MFC_DIRS=${MFC_DIRS}
	@echo MFC_RMS=${MFC_RMS}
	@echo MFC_SRC=${MFC_SRC}
	@echo MFC_OBJ=${MFC_OBJ}
	@echo MFC_DEP=${MFC_DEP}
	@echo MFC_INC=${MFC_INC}
	@echo MFC_CFLAGS=${MFC_CFLAGS}

