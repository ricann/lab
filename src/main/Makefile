.PHONY: all clean

include ../../make.h
include make.h


$(MAIN_TARGET): $(MAIN_OBJ) $(MAIN_LIB)
	$(CCPP_LD) $(MAIN_LIB_PATH) $(MAIN_LDFLAGS) $^ -o $@ 

#define compile rules
%.o: %.cpp
	$(CCPP) $(MAIN_CFLAGS) $(MAIN_INC) $< -o $@

ifneq ($(MAKECMDGOALS), clean)
sinclude $(MAIN_DEP)
endif

$(MAIN_OBJ_DIR)/%.dep: $(MAIN_SRC_DIR)/%.cpp
	@set -e ; \
	$(RM) -fr $@.tmp ; \
	$(CCPP_NO_ECHO) $(MAIN_CFLAGS) $(MAIN_INC) -E -MM $(filter %.cpp, $^) > $@.tmp ; \
	sed 's,\($*\)\.o[ :]*,obj/\1.o $@: ,g' < $@.tmp > $@ ; \
	$(RM) -fr $@.tmp

clean:
	@echo "Cleaning dhcp auto config object files"
	$(RM) -f *.o
	$(RM) -f *.dep
	$(RM) -f $(MAIN_TARGET)

print:
	@echo MAIN_TARGET=${MAIN_TARGET}
	@echo MAIN_INC_DIR=${MAIN_INC_DIR}
	@echo MAIN_SRC_DIR=${MAIN_SRC_DIR}
	@echo MAIN_OBJ_DIR=${MAIN_OBJ_DIR}
	@echo _MAIN_OBJ_DIR=${_MAIN_OBJ_DIR}
	@echo MAIN_DIRS=${MAIN_DIRS}
	@echo MAIN_RMS=${MAIN_RMS}
	@echo MAIN_SRC=${MAIN_SRC}
	@echo MAIN_OBJ=${MAIN_OBJ}
	@echo MAIN_DEP=${MAIN_DEP}
	@echo MAIN_INC=${MAIN_INC}
	@echo MAIN_CFLAGS=${MAIN_CFLAGS}
	@echo MAIN_LDFLAGS=${MAIN_LDFLAGS}
	@echo ARM_LIB_BASE=${ARM_LIB_BASE}

