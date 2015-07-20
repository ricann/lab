.PHONY: all clean

include ../../make.h
include make.h


$(RAPTOR_TARGET): $(RAPTOR_OBJ)
	$(AR) rc $@ $^

#define compile rules
%.o: %.c $(RAPTOR_DEP)
	$(CC) $(RAPTOR_INC) $(RAPTOR_CFLAGS) $< -o $@

clean:
	@echo "Cleaning dhcp auto config object files"
	$(RM) -f *.o
	$(RM) -f *.dep
	$(RM) -f $(RAPTOR_TARGET)

print:
	@echo RAPTOR_INC_DIR=${RAPTOR_INC_DIR}
	@echo RAPTOR_SRC_DIR=${RAPTOR_SRC_DIR}
	@echo RAPTOR_OBJ_DIR=${RAPTOR_OBJ_DIR}
	@echo RAPTOR_DIRS=${RAPTOR_DIRS}
	@echo RAPTOR_RMS=${RAPTOR_RMS}
	@echo RAPTOR_SRC=${RAPTOR_SRC}
	@echo RAPTOR_OBJ=${RAPTOR_OBJ}
	@echo RAPTOR_DEP=${RAPTOR_DEP}
	@echo RAPTOR_INC=${RAPTOR_INC}
	@echo RAPTOR_CFLAGS=${RAPTOR_CFLAGS}

