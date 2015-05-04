#ifndef __AUDIO_H__
#define __AUDIO_H__

#define IP_DEFAULT "10.0.0.87"

#define PORT_A 5555

struct frame_info{
	int type;
	int frameNo;
	int count;
	int fragmentLen;
	int fragmentNo;
	int offset;
	int iCnt;
	int pCnt;
};

void audio_thread(void);
static void open_alsa_device();
static void set_alsa_params();

#endif /* __AUDIO_H__ */