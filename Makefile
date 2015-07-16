.PHONY: all clean
.SILENT:

include make.h

all:
	$(MAKE) -w -C $(COMMON_BASE)
	$(MAKE) -w -C $(MFC_BASE)
	$(MAKE) -w -C $(FRAME_BASE)
	$(MAKE) -w -C $(RAPTOR_BASE)
	$(MAKE) -w -C $(MAIN_BASE)

clean:
	$(MAKE) -w -C $(COMMON_BASE) clean
	$(MAKE) -w -C $(MFC_BASE) clean
	$(MAKE) -w -C $(FRAME_BASE) clean
	$(MAKE) -w -C $(RAPTOR_BASE) clean
	$(MAKE) -w -C $(MAIN_BASE) clean
