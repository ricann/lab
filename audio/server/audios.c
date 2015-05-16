#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "alsa/asoundlib.h"

#include "audios.h"

//socket variables
char ip[32];
int port;
int sockfd;
socklen_t sin_size;
struct sockaddr_in my_addr;
struct sockaddr_in ac_addr;

int sockfd_mcast;
struct sockaddr_in mcast_addr;


//pcm variables
snd_pcm_t *handle;
snd_pcm_hw_params_t *params;
snd_pcm_uframes_t period_frames = PCM_PERIOD_FRAMES;
snd_pcm_uframes_t period_size = \
	PCM_PERIOD_FRAMES * PCM_CHANNELS * PCM_SAMPLE_SIZE;
int buf_frames = PCM_PERIODS * PCM_PERIOD_FRAMES;
char data[PCM_PERIOD_FRAMES * PCM_CHANNELS * PCM_SAMPLE_SIZE];

int main()
{
	//init alsa, open device and set parameters
	if (init_alsa() != 0) {
		return -1;
	}

	//init socket
	if (init_sock() != 0) {
		return -1;
	}

	//init multicast socket
	if (init_mcast_sock() != 0) {
		return -1;
	}

	//capture and transfer audio
	if (play_audio() != 0) {
		return -1;
	}

	return 0;
}

int init_sock()
{
	//init socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket error");
		return -1;
	}
	bzero(&my_addr, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT_DEFAULT);
	my_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
	    perror("bind");
	    return -1;
	}

	return 0;
}

int init_mcast_sock()
{
	//init socket
	if ((sockfd_mcast = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket error");
		exit(1);
	}
	bzero(&mcast_addr, sizeof(struct sockaddr_in));
	mcast_addr.sin_family = AF_INET;
	mcast_addr.sin_port = htons(MCAST_PORT);
	if (inet_aton(MCAST_IP, &mcast_addr.sin_addr) < 0)
	{
		perror("IP error");
		return -1;
	}

	if ((connect(sockfd_mcast, (struct sockaddr *)&mcast_addr, 
		sizeof(struct sockaddr_in))) == -1) {
	    perror("connect");
	    return -1;
	}

	return 0;
}

int init_alsa()
{
	int dir = 0;
	unsigned int rate = PCM_RATE;

	/* Open PCM device for recording (capture). */
	if ((snd_pcm_open(&handle, PCM_NAME, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		//fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		fprintf(stderr, "unable to open pcm device\n");
		return -1;
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	/* Set the desired hardware parameters. */
	/* Interleaved mode */
	if ((snd_pcm_hw_params_set_access(handle, params, 
		SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf(stderr, "Error setting access.\n");
		return -1;
	}

	/* Signed 16-bit little-endian format */
	if ((snd_pcm_hw_params_set_format(handle, params, 
		SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf(stderr, "Error setting format. \n");
		return -1;
	}

	/* Two channels (stereo) */
	if ((snd_pcm_hw_params_set_channels(handle, params, PCM_CHANNELS)) < 0) {
		fprintf(stderr, "Error setting channels \n");
		return -1;
	}

	/* sampling rate */
	dir = 0;
	if ((snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir)) < 0) {
		fprintf(stderr, "Error setting rate. \n");
		return -1;
	}
	if (rate != PCM_RATE) {
		fprintf(stderr, "The rate %d Hz is not supported by your hardware. \n"
		  "==> Using %d Hz instead. \n", PCM_RATE, rate);
		return -1;
	}

	/*Set number of periods. Periods used to be called fragments*/
	dir = 0;
	if ((snd_pcm_hw_params_set_period_size_near(handle, params, 
		&period_frames, &dir)) < 0) {
		fprintf(stderr, "Error setting periods. \n");
		return -1;
	}
	if (period_frames != PCM_PERIOD_FRAMES) {
		fprintf(stderr, "The periods %d is not supported by your hardware. \n"
		  "==> Using %d periods instead. \n", 
		  PCM_PERIOD_FRAMES, (int)period_frames);
		return -1;
	}

	/* Set buffer size (in frames). 
	buffersize = period_size * periods * framesize (bytes)
	The resulting latency is given by 
	latency = period_size * periods / (rate * bytes_per_frame)
	*/
	if ((snd_pcm_hw_params_set_buffer_size(handle, params, buf_frames)) < 0) {
		fprintf(stderr, "Error setting buffersize. \n");
		return -1;
	}

	/* Write the parameters to the driver */
	if ((snd_pcm_hw_params(handle, params)) < 0) {
		//fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
		fprintf(stderr, "unable to set hw parameters\n");
		return -1;
	}

	return 0;
}

int play_audio()
{
	int n, cnt, ret;

	//every 10 udp datagrams, display one line info
	cnt = 0;

	while(1) {
		if ((n = recvfrom(sockfd, data, sizeof(data), 0, 
			(struct sockaddr *)&ac_addr, &sin_size)) == -1) {
			perror ("recvfrom");
			//return -1;
		}

		if (++cnt == 100) {
			printf("Received datagram from %s:\n",inet_ntoa(ac_addr.sin_addr));
			cnt = 0;
		}
		
		if (write(sockfd_mcast, data, n) == -1) {
			perror("write error!");
			return -1;
		}
		
		ret = snd_pcm_writei(handle, data, period_frames);
		if (ret == -EPIPE) {
			//EPIPE means underrun
			fprintf(stderr, "underrun occurred\n");
			snd_pcm_prepare(handle);
		} else if (ret < 0) {
			fprintf(stderr, "error from writei: %s\n", snd_strerror(ret));
		} else if (ret != (int)period_frames) {
			fprintf(stderr, "short write, write %d period_frames\n", ret);
		}
		
	}

	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	return 0;
}