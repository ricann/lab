.PHONY: all clean
.SILENT:

include make.h

all: 
	$(MAKE) -w -C $(AUDIO_BASE)/server
	$(MAKE) -w -C $(AUDIO_BASE)/client
	$(MAKE) -w -C $(AUDIO_BASE)/sample

clean:
	$(MAKE) -w -C $(AUDIO_BASE)/server clean
	$(MAKE) -w -C $(AUDIO_BASE)/client clean
	$(MAKE) -w -C $(AUDIO_BASE)/sample clean
