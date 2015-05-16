#ifndef __AUDIOS_H__
#define __AUDIOS_H__

//socket
#define IP_DEFAULT "10.103.242.187"
#define PORT_DEFAULT 5555

#define MCAST_IP "239.255.1.2"
#define MCAST_PORT 8899

//pcm
#define PCM_NAME "plughw:0,0"
#define PCM_RATE 44100
#define PCM_PERIODS 8
#define PCM_PERIOD_FRAMES 1024
#define PCM_CHANNELS 2
#define PCM_SAMPLE_SIZE 2

int init_sock();
int init_mcast_sock();
int init_alsa();
int play_audio();


#endif /* __AUDIOS_H__ */
