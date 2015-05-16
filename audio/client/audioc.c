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

#include "audioc.h"

//socket variables
char ip[32];
int port;
int sockfd;
struct sockaddr_in daddr;

//pcm variables
snd_pcm_t *handle;
snd_pcm_hw_params_t *params;
snd_pcm_uframes_t period_frames = PCM_PERIOD_FRAMES;
snd_pcm_uframes_t period_size = \
	PCM_PERIOD_FRAMES * PCM_CHANNELS * PCM_SAMPLE_SIZE;
int buf_frames = PCM_PERIODS * PCM_PERIOD_FRAMES;
char data[PCM_PERIOD_FRAMES * PCM_CHANNELS * PCM_SAMPLE_SIZE];

int main(int argc, char *argv[])
{
	//init ip and port
	if(argc == 3)
	{
		strncpy(ip, argv[1], strlen(argv[1]));
		port = atoi(argv[2]);
	}
	else
	{
		strncpy(ip, IP_DEFAULT, strlen(IP_DEFAULT));
		port = PORT_DEFAULT;
	}

	//init alsa, open device and set parameters
	if(init_alsa() != 0)
	{
		return -1;
	}

	//init socket
	if(init_sock() != 0)
	{
		return -1;
	}

	//capture and transfer audio
	if(cap_audio() != 0)
	{
		return -1;
	}

	return 0;
}

int init_sock()
{
	int ret;

	//socket init
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		perror("Socket error");
		return -1;
	}
	bzero(&daddr, sizeof(struct sockaddr_in));
	daddr.sin_family = AF_INET;
	daddr.sin_port = htons(port);
	if(inet_aton(ip, &daddr.sin_addr) < 0)
	{
		perror("IP error");
		return -1;
	}

	ret = connect(sockfd, (struct sockaddr *)&daddr, sizeof(struct sockaddr_in));
	if(ret == -1)
	{
	    perror("connect");
	    return -1;
	}

	return 0;
}

int init_alsa()
{
	int rc, dir = 0;
	unsigned int rate = PCM_RATE;


	/* Open PCM device for recording (capture). */
	rc = snd_pcm_open(&handle, PCM_NAME, SND_PCM_STREAM_CAPTURE, 0);
	if (rc < 0) 
	{
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		return -1;
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	/* Set the desired hardware parameters. */
	/* Interleaved mode */
	rc = snd_pcm_hw_params_set_access(handle, params, 
		SND_PCM_ACCESS_RW_INTERLEAVED);
	if (rc < 0) 
	{
		fprintf(stderr, "Error setting access.\n");
		return -1;
	}

	/* Signed 16-bit little-endian format */
	rc = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
	if (rc < 0) 
	{
		fprintf(stderr, "Error setting format. \n");
		return -1;
	}

	/* Two channels (stereo) */
	rc = snd_pcm_hw_params_set_channels(handle, params, PCM_CHANNELS);
	if (rc < 0) 
	{
		fprintf(stderr, "Error setting channels \n");
		return -1;
	}

	/* sampling rate */
	dir = 0;
	rc = snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);
	if (rc < 0) 
	{
		fprintf(stderr, "Error setting rate. \n");
		return -1;
	}
	if (rate != PCM_RATE)
	{
		fprintf(stderr, "The rate %d Hz is not supported by your hardware. \n"
		  "==> Using %d Hz instead. \n", PCM_RATE, rate);
		return -1;
	}

	/*Set number of periods. Periods used to be called fragments*/
	dir = 0;
	rc = snd_pcm_hw_params_set_period_size_near(handle, 
		params, &period_frames, &dir);
	if (rc < 0) 
	{
		fprintf(stderr, "Error setting periods. \n");
		return -1;
	}
	if (period_frames != PCM_PERIOD_FRAMES)
	{
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
	rc = snd_pcm_hw_params_set_buffer_size(handle, params, buf_frames);
	if (rc < 0) 
	{
		fprintf(stderr, "Error setting buffersize. \n");
		return -1;
	}

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) 
	{
		fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
		return -1;
	}

	return 0;
}

int cap_audio()
{
	int rc;

	while(1)
	{
		rc = snd_pcm_readi(handle, data, period_frames);
		if (rc == -EPIPE) 
		{
			/* EPIPE means overrun */
			fprintf(stderr, "overrun occurred\n");
			snd_pcm_prepare(handle);
		}

		/*
		rc = sendto(sockfd, data, sizeof(data), 0, 
			(struct sockaddr *)&daddr, sizeof(struct sockaddr_in));
		*/
		rc = write(sockfd, data, sizeof(data));
		if(rc == -1)
		{
			perror("write error!");
			//return -1;
		}
	}

 	snd_pcm_drain(handle);
	snd_pcm_close(handle);

 	return 0;
}
