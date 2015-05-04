#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "alsa/asoundlib.h"

#include "audio.h"


static char ip[32];
static int finished = 0;
static snd_pcm_t *audio_handle;

int main()
{
	
	audio_thread();

	return 0;
}

void audio_thread(void)
{
	int sockfd;
	struct sockaddr_in saddr;
	snd_pcm_uframes_t period_size = 80;
	snd_pcm_uframes_t buf_size = period_size*2;

	int frame_len = sizeof(struct frame_info)+buf_size;
	unsigned char * frame = (unsigned char *)malloc(frame_len);
	unsigned char * buf = (unsigned char *)malloc(buf_size);

	long frame_cnt = 1;

	//record init
	open_alsa_device();
	set_alsa_params();

	//socket init
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0){
		perror("Socket error");
		exit(1);
	}
	bzero(&saddr, sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT_A);
	if(inet_aton(ip, &saddr.sin_addr) < 0){
		perror("IP error");
		exit(1);
	}

	while(!finished){
		//signal(SIGINT, (void (*)(int))ctrl_c);
		//record
		while(snd_pcm_readi(audio_handle, buf, period_size) != period_size) 
			snd_pcm_prepare(audio_handle);
		
		//send by udp-socket
		//write frame_info
		struct frame_info frame_info;
		frame_info.type = 1;
		frame_info.frameNo = htonl(frame_cnt++);
		frame_info.count = htonl(buf_size);
		frame_info.fragmentLen = htonl(buf_size);
		frame_info.fragmentNo = htonl(1);

		//fix frame_info & frameData to frame
		memset(frame, 0, frame_len);
		memcpy(frame, &frame_info, sizeof(frame_info));
		memcpy(frame+sizeof(frame_info), buf, buf_size);
		printf("Audio | frameNo=%d, count=%d, fragmentLen=%d, fragmentNo=%d\n", 
			ntohl(frame_info.frameNo), ntohl(frame_info.count), ntohl(frame_info.fragmentLen), 
			ntohl(frame_info.fragmentNo));
		sendto(sockfd, frame, frame_len, 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
	}
	//snd_pcm_close(audio_handle);
}


static void open_alsa_device()
{
	snd_pcm_open(&audio_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
}

static void set_alsa_params()
{
	snd_pcm_hw_params_t * audio_params;
	unsigned int audio_sample_rate = 8000;

	snd_pcm_hw_params_malloc(&audio_params);
	snd_pcm_hw_params_any(audio_handle, audio_params);
	snd_pcm_hw_params_set_access(audio_handle, audio_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(audio_handle, audio_params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_rate_near(audio_handle, audio_params, &audio_sample_rate, 0);
	snd_pcm_hw_params_set_channels(audio_handle, audio_params, 1);
	snd_pcm_hw_params(audio_handle, audio_params);
	snd_pcm_hw_params_free(audio_params);
	snd_pcm_prepare(audio_handle);
}