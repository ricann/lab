/*========================================================================
#   FileName: main.cpp
#   Author: Nevermore
#   LastUpdate: 2015-04-27 21:40:34
========================================================================*/

#include "main.h"
#include <time.h>
#include <limits.h>
#include <sys/time.h>
#include <semaphore.h>
#include <unistd.h>
#define K1_MAX 5000
#define T_MAX 2048
#define INDEX_SIZE 40
#define PORT_FB 3333
#define FB_DATA_SIZE 5000

#define SERVER_FILE "server_socket"
#define SERVER_D_FILE "server_d_socket"
#define SERVER2_FILE "server2_socket"
//#define YUV_NO_MAX 50000

void clean_up(int, const char*);

/*global variable*/
static int cam_p_fp = -1;
static int cam_c_fp = -1;
static char * win0_fb_addr = NULL;
//static int pre_fb_fd = -1;
//static unsigned char delimiter_h264[4] = {0x00, 0x00, 0x00, 0x01};
//static void * dec_handle;
static char ip[80];
static int gop, qp, res, lcd_width=400, lcd_height=272, with_audio, with_video, with_preview, with_multi_description,keep_alive,
		   with_local, with_fb, with_fec, t_init , k1_max_init=3000;
static int camera_no = 1;
static void *video_handle, *video_handle1; 
//static snd_pcm_t *audio_handle;

static pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//结构常量,静态初始化互斥锁

//static int finished = 0;

//add in 2014-11-4
static int time_out = 0;
static int stream = 0;
static int move = 0;
static int yuv_frame_buf_size;
static int yuv_frame_buf_third_size;
static int yuv_gray_frame_buf_size;
static int yuv_gray_frame_buf_half_size;
static int detect_rate,send_rate;

//int lt_frame = 1; // 用于计算凑齐可以进行lt的帧个数
int LT_R =  20;
int NEW_D = 1;
int M_SLICE = 1;
int time_dely = 30;
int time_dely_save = 30;
int locally_store_264 = 0;
float lose_q = 0.3;
sem_t sem_id,sem_quene,sem_room;//semaphore

typedef struct{
	int frame_no;
	long slice_no;
	int frame_type;
	long F;
	int T;
	int K;
	int R;
	int esi;
	int camera_no;
} Frame_header;

struct aliveheader{
	int cameraNum;
};
int addr_len =sizeof(struct sockaddr_in);

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
/***************************************加字幕，g_yuv处理**************************************/
enum
{
	WIDTH = 480, 
	HEIGHT= 272, 
};

enum
{
	YUV420Size = WIDTH * HEIGHT * 3 >> 1,  
};

BYTE byBuffer[YUV420Size] = { 0 };

enum
{
	STEP = 12, 
};

/***************************************加字幕，g_yuv处理**************************************/

struct quene_node{
	int size;
	struct quene_node* next;
};

struct quene{
	int size;
	quene_node* first;
	quene_node* last;
};

struct yuv_info{
	int length;
	int frameNo;
};

typedef struct Feedback
{
	int type;
	float value;
}feedback;

struct linkmsg
{
	int flag;
	int type;
	double x;
	double y;
	double speed;
	struct timeval collect_time;
};

//udp connection with video_dispaly end
int sockfd;
struct sockaddr_in saddr;

/* keep alive package*/
int alive_sockfd;
struct sockaddr_in aliveaddr;

unsigned char time_buff[256];

//void add_to_quene ( quene *, void*, size_t);
//void get_data(quene * ,char *);

char * sendq;
int savedata, getdata;
FILE* file_v;

struct cooperation_signal{
	int cameraNo;
	int move_flag;
};

/* Main Process */
int main(int argc, char **argv){		
	pthread_t /*a_pth, */v_pth, v_s_pth,d_pth;//, m_c_pth;
	pthread_t k_l_pth;
	int ret, start, found = 0;
	int /*a_id,*/ v_id, v_s_id;//, m_c_msg; 
	int k_l_id,d_id;
	unsigned int addr = 0;
	char * rgb_for_preview = (char *)malloc(lcd_width*lcd_height*4*sizeof(char));

	struct v4l2_capability cap;
	struct v4l2_input chan;
	struct v4l2_framebuffer preview;
	struct v4l2_pix_format preview_fmt;
	struct v4l2_format codec_fmt;

	int i;
	int d_r, s_r;
	unsigned char *g_yuv;

	/*****************************

	//add in 2014-11-4

	*****************************/
	//socket AF_UNIX for video_thread
	int orig_sock;
	static struct sockaddr_un clnt_adr, serv_adr;
	static char client_file[18];
	int reuse_addr, bufsize_addr, bufsize_len;
	//unsigned char *c_p;
	
	//socket AF_UNIX for detect_thread
	int bufsize2_addr, bufsize2_len;
	
	int orig2_sock;
	static struct sockaddr_un clnt2_adr, serv2_adr;
	static char client2_file[19];	
	
	/* keep alive package*/
	aliveheader * alive_header = (aliveheader *)malloc(sizeof(aliveheader));
	struct timeval t_start,t_end;
	long cost_time_sec,cost_time_usec;


	if(argc%2 == 0){
		printf(">>>>> Wrong number of arguments!\n");
		return 1;
	}
	else{
		strcpy(ip, IP_DEFAULT);
		gop = GOP_DEFAULT;
		qp = QP_DEFAULT;
		res = RES_DEFAULT;
		with_video = WITH_VEDIO_DEFAULT;
		with_audio = WITH_AUDIO_DEFAULT;
		with_preview = WITH_PREVIEW_DEFAULT;
		with_multi_description = WITH_MULTI_DESCRIPTION;
		keep_alive = KEEP_ALIVE_DEFAULT;
		with_local = WITH_LOCAL_DEFAULT;
		detect_rate = DETECT_RATE_DEFAULT;
		send_rate = SEND_RATE_DEFAULT;
		with_fb = WITH_FB_DEFAULT;
		with_fec = WITH_FEC_DEFAULT;
		t_init = T_INIT_DEFAULT;
		k1_max_init = K1_MAX;
		
		for(i = 1; i < argc; i += 2){
			switch(argv[i][1]){
				case 'i':
					strcpy(ip, argv[i+1]);
					break;
				case 'g':
					gop = atoi(argv[i+1]);
					break;
				case 'q':
					qp = atoi(argv[i+1]);
					break;
				case 'r':
					res = atoi(argv[i+1]);
					break;
				case 'v':
					with_video = atoi(argv[i+1]);
					break;
				case 'a':
					with_audio = atoi(argv[i+1]);
					break;
				case 'p':
					with_preview = atoi(argv[i+1]);
					break;
				//case 'm':
					//with_multi_description = atoi(argv[i+1]);
				//	break;
				case 'K':
					keep_alive = atoi(argv[i+1]);
					break;
				case 'l':
					with_local = atoi(argv[i+1]);
					break;
				case 'f':
					with_fb = atoi(argv[i+1]);
					break;
				case 'z':
					with_fec = atoi(argv[i+1]);
					break;
				case 't':
					t_init = atoi(argv[i+1]);
					break;
				case 'k':
					k1_max_init = atoi(argv[i+1]);
					break;
//				case 'b':
//					lt_frame = atoi(argv[i+1]);
//					break;
				case 'R':
					LT_R = atoi(argv[i+1]);
					break;
				case 'D':
					NEW_D = atoi(argv[i+1]);
					break;
				case 's':
					M_SLICE = atoi(argv[i+1]);
					break;
				case 'S':
					locally_store_264 = atoi(argv[i+1]);
				    break;
				case 'd':
					time_dely = atoi(argv[i+1]);
					break;
				case 'Q':
					time_dely_save = atoi(argv[i+1]);
					break;
				case 'c':
				    camera_no = atoi(argv[i+1]);
				    break;

				default:
					printf(">>>>> Wrong arguments!\n");
					return 1;
			}
		}
		
		switch(res){
			case 4:
				lcd_width = 480;
				lcd_height = 272;
				break;
			case 6:
				lcd_width = 640;
				lcd_height = 480;
				break;
			case 8:
				lcd_width = 800;
				lcd_height = 600;
				break;
			default:
				printf(">>>>> Wrong resolution!\n");
				return 1;
		}
	}//else

	printf("\n");
	printf("\t***********************************\n");	
	printf("\t*                                 *\n");
	printf("\t*       IP %15s    -i  *\n", ip);
	printf("\t*                                 *\n");
	printf("\t*       GOP %14d    -g  *\n", gop);
	printf("\t*                                 *\n");
	printf("\t*       QP %15d    -q  *\n", qp);
	printf("\t*                                 *\n");
	printf("\t*       RES %10d*%3d    -r  *\n", lcd_width, lcd_height);
	printf("\t*                                 *\n");
	printf("\t*       VEDIO %12d    -v  *\n", with_video);
	printf("\t*                                 *\n");
	printf("\t*       AUDIO %12d    -a  *\n", with_audio);
	printf("\t*                                 *\n");
	printf("\t*       PREVIEW %10d    -p  *\n", with_preview);
	printf("\t*                                 *\n");
	printf("\t*       KEEP ALIVE %7d       *\n", keep_alive);
	printf("\t*                                 *\n");
	printf("\t*       LOCAL %12d    -l  *\n", with_local);
	printf("\t*                                 *\n");
	printf("\t*       FEED BACK %8d    -f  *\n", with_fb);
	printf("\t*                                 *\n");
	printf("\t*       TIME DELY %8d    -d  *\n", time_dely);
	printf("\t*                                 *\n");
	printf("\t*	      CAMERA NO %8d    -c  *\n", camera_no);
	printf("\t*                                 *\n");
	printf("\t*       FEC %14d    -z  *\n", with_fec);
	printf("\t*                                 *\n");
	printf("\t*       T_INIT %11d    -t  *\n", t_init);
	printf("\t*                                 *\n");
	printf("\t*       K1_MAX %11d    -k  *\n", k1_max_init);
	printf("\t*                                 *\n");
	printf("\t*       LT_R %13d    -R  *\n", LT_R);
	printf("\t*                                 *\n");
//	printf("\t*       LT_FRAME %9d    -b  *\n", lt_frame);
//	printf("\t*                                 *\n");
	printf("\t*       NEW_D %12d    -D  *\n", NEW_D);
	printf("\t*                                 *\n");
	printf("\t*       M_SLICE %10d    -s  *\n", M_SLICE);
	printf("\t*                                 *\n");
	printf("\t*       L_STORE %10d    -S  *\n", locally_store_264);
	printf("\t*                                 *\n");
	printf("\t*	      TIME_DELY %8d   -d  *\n",time_dely);
	printf("\t*                                 *\n");
	printf("\t***********************************\n");	
	printf("\n");

	sleep(3);

	//每隔d_r帧发送一帧至detect_thread，每隔s_r帧发送一帧至video_thread
	d_r = detect_rate;
	s_r = send_rate;
	
	yuv_frame_buf_size = (lcd_width*lcd_height)+(lcd_width*lcd_height)/2;//yuv格式大小为1.5*width*height
	yuv_frame_buf_third_size = yuv_frame_buf_size/3;
	yuv_gray_frame_buf_size = lcd_width*lcd_height;
	yuv_gray_frame_buf_half_size = yuv_gray_frame_buf_size/2;
	
	g_yuv = (unsigned char*)malloc(yuv_frame_buf_size*sizeof(unsigned char));
	printf("main_thread : g_yuv : %p\n", g_yuv);

	sendq = (char*)malloc((T_MAX+sizeof(Frame_header))*20);
	savedata = getdata = 0;
	
	//set socket for video
	bzero(&saddr,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT_V);
	if(inet_pton(AF_INET, ip, &saddr.sin_addr) <= 0){
		printf("[%s] is not a valid IP address\n", ip);
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	/*	使用了connect的UDP协议，一般使用write或send，
		不能使用带宿地址的sendto，要用必须将第5个参数设为NULL
		使用connect，可提升效率
	*/
	connect(sockfd,(struct sockaddr *)&saddr, sizeof(saddr));

	/* keep alive package socket */
	bzero(&aliveaddr,sizeof(aliveaddr));
	aliveaddr.sin_family = AF_INET;
	aliveaddr.sin_port = htons(9091);
	if(inet_pton(AF_INET, ip, &aliveaddr.sin_addr) <= 0){
		printf("[%s] is not a valid IPaddress\n", ip);
		exit(1);
	}
	alive_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	connect(alive_sockfd, (struct sockaddr *)&aliveaddr, sizeof(aliveaddr));
		
	alive_header->cameraNum = htonl(camera_no);

	sem_init(&sem_id,0,1);
	sem_init(&sem_quene,0,0);
	sem_init(&sem_room,0,20);

	//signal(SIGINT, signal_ctrl_c);
	//signal(SIGINT, exit_from_app);
	
	/****************板上预览，此功能暂时不用***********************/
	if(with_preview){
		// Camera preview initialization
		if((cam_p_fp = cam_p_init()) < 0)
		    exit_from_app();

		win0_fb_addr = (char *)addr;

	    // Get capability 
	    if((ret = ioctl(cam_p_fp , VIDIOC_QUERYCAP, &cap)) < 0){
		    printf("V4L2 : ioctl on VIDIOC_QUERYCAP failled\n");
		    exit(1);
	    }

	    // Check the type - preview(OVERLAY)
	    if(!(cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)){
		    printf("V4L2 : Can not capture(V4L2_CAP_VIDEO_OVERLAY is false)\n");
		    exit(1);
	    }

	    chan.index = 0;
	    found = 0;

	    while(1){
		    if((ret = ioctl(cam_p_fp, VIDIOC_ENUMINPUT, &chan)) < 0){//视频捕获的应用首先要通过VIDIOC_ENUMINPUT
                                                                //命令来枚举所有可用的输入
			    printf("V4L2 : ioctl on VIDIOC_ENUMINPUT failled !!!!\n");
			    fflush(stdout);
			    break;
		    }

		    // Test channel.type 
		    if(chan.type & V4L2_INPUT_TYPE_CAMERA){
			    found = 1;
			    break;
		    }
		    chan.index++;
	    }
		
	    if(!found) 
		    exit_from_app();

	    // Settings for input channel 0 which is channel of webcam 
	    chan.type = V4L2_INPUT_TYPE_CAMERA;
        //一个video设备节点可能对应多个视频源，上层调用S_INPUT ioctl在多个cvbs视频输入间切换
	    if((ret = ioctl(cam_p_fp, VIDIOC_S_INPUT, &chan)) < 0){
		    printf("V4L2 : ioctl on VIDIOC_S_INPUT failed\n");
		    fflush(stdout);
		    exit(1);
	    }

	    preview_fmt.width = lcd_width;
	    preview_fmt.height = lcd_height;
	    preview_fmt.pixelformat = LCD_BPP_V4L2;

	    preview.capability = 0;
	    preview.flags = 0;
	    preview.fmt = preview_fmt;

	    // Set up for preview 
	    if((ret = ioctl(cam_p_fp, VIDIOC_S_FBUF, &preview)) < 0){
		    printf("V4L2 : ioctl on VIDIOC_S_BUF failed\n");
		    exit(1);
	    }

	    // Preview start
	    start = 1;
	    if((ret = ioctl(cam_p_fp, VIDIOC_OVERLAY, &start)) < 0){
		    printf("V4L2 : ioctl on VIDIOC_OVERLAY failed\n");
		    exit(1);
	    }
	}
	
	/****************with_local==0为摄像头采集，with_local==1为本地视频***********************/
	if(with_local == 0){
		// Camera codec initialization 
		if((cam_c_fp = cam_c_init()) < 0)
		    exit_from_app();

	    // Get capability 
	    if((ret = ioctl(cam_c_fp , VIDIOC_QUERYCAP, &cap)) < 0){
		    printf("V4L2 : ioctl on VIDIOC_QUERYCAP failled\n");
		    exit(1);
	    }

	    // Check the type - preview(OVERLAY) 
	    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){
		    printf("V4L2 : Can not capture(V4L2_CAP_VIDEO_CAPTURE is false)\n");
		    exit(1);
	    }

	    // Set format 
	    codec_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	    codec_fmt.fmt.pix.width = lcd_width; 
	    codec_fmt.fmt.pix.height = lcd_height; 	
	    codec_fmt.fmt.pix.pixelformat= V4L2_PIX_FMT_YUV420; 
	    if((ret = ioctl(cam_c_fp , VIDIOC_S_FMT, &codec_fmt)) < 0){
		    printf("V4L2 : ioctl on VIDIOC_S_FMT failled\n");
		    exit(1);
	    }
	}
	   
	/****************************************

	socket for video_thread and detect_thread
	利用socket进行线程间通信
	
	****************************************/
	
	/* socket AF_UNIX for video_thread */
	serv_adr.sun_family = AF_UNIX;
	strcpy(serv_adr.sun_path, SERVER_FILE);
	if((orig_sock = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
	{
		printf("main_thread : socket error\n");
		exit(1);
	}

	sprintf(client_file, "%10d_socket", (int)pthread_self());
	clnt_adr.sun_family = AF_UNIX;
	strcpy(clnt_adr.sun_path, client_file);
	unlink(client_file);
	if(bind(orig_sock, (struct sockaddr *)&clnt_adr, strlen(clnt_adr.sun_path)+sizeof(clnt_adr.sun_family)) < 0 )
	{
		printf("main_thread : bind error\n");
		clean_up(orig_sock, SERVER_FILE); 
		exit(1);                            
	}
	reuse_addr = 1;
	bufsize_addr = yuv_frame_buf_size*2;
	bufsize_len = sizeof(bufsize_addr);
	printf("bufsize_addr:%d, sizeof(bufsize_addr):%d, sizeof(int):%d\n", bufsize_addr, sizeof(bufsize_addr), sizeof(int));
	if(setsockopt(orig_sock, SOL_SOCKET, SO_SNDBUF, (void *)&bufsize_addr, bufsize_len) < 0)//设置socket选项，此处为设置发送缓冲区大小
	{
		perror("setsockopt()");
		exit(1);
	}

	/* socket AF_UNIX for detect_thread */
	serv2_adr.sun_family = AF_UNIX;
	strcpy(serv2_adr.sun_path, SERVER2_FILE);

	if((orig2_sock = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
	{
		printf("main : socket error\n");
		exit(1);
	}

	sprintf(client2_file, "%10d2_socket", (int)pthread_self());
	clnt2_adr.sun_family = AF_UNIX;
	strcpy(clnt2_adr.sun_path, client2_file);
	unlink(client2_file);
	if(bind(orig2_sock, (struct sockaddr *)&clnt2_adr, strlen(clnt2_adr.sun_path)+sizeof(clnt2_adr.sun_family)) < 0 )
	{
		printf("main : bind error\n");
		clean_up(orig2_sock, SERVER2_FILE); 
		exit(1);                            
	}
	bufsize2_addr = yuv_gray_frame_buf_size*2;
	bufsize2_len = sizeof(bufsize2_addr);
	printf("bufsize2_addr:%d, sizeof(bufsize2_addr):%d, sizeof(int):%d\n", bufsize2_addr, sizeof(bufsize2_addr), sizeof(int));
	if(setsockopt(orig2_sock, SOL_SOCKET, SO_SNDBUF, (void *)&bufsize2_addr, bufsize2_len) < 0)
	{
		perror("setsockopt()");
		exit(1);
	}


	// Encoding threads creation 
	if(with_video){
		v_id = pthread_create(&v_pth, 0, video_thread, 0);
		
		if(with_local == 0 && locally_store_264 == 1){
			if(!(file_v = fopen("origin_video", "wb+"))){
			    perror("origin_video file open error");
			    exit(1);
		    }
		}
	}
	d_id = pthread_create(&d_pth, 0, &detect_thread, 0);
	v_s_id = pthread_create(&v_s_pth, 0, video_send_thread, 0);
	k_l_id = pthread_create(&k_l_pth, 0, keep_linked_thread, 0);
//	if(with_audio)
//		a_id = pthread_create(&a_pth, 0, audio_thread, 0);

	while(with_preview){ // preview 
		// Get RGB frame from camera preview 
		if(!read_data(cam_p_fp, &rgb_for_preview[0], lcd_width, lcd_height, LCD_BPP)){
			printf("V4L2 : read_data() failed\n");
			break;
		}
		// Write RGB frame to LCD frame buf 
		draw(win0_fb_addr, &rgb_for_preview[0], lcd_width, lcd_height, LCD_BPP);
	}

////////////////////////////////////////////////////////////////////////
	/**************摄像头开始采集*************/
	if(with_local == 0){
		// Codec start 
	    start = 1;
	    if((ret = ioctl(cam_c_fp, VIDIOC_STREAMON, &start)) < 0){
		    printf("V4L2 : ioctl on VIDIOC_STREAMON failed(start)\n");
		    exit(1);
	    }
	}

	gettimeofday(&t_start, NULL);
	while(1)
	{
		#if WITH_LOCAL_DEFAULT == 1
		
			if(fread(g_yuv, yuv_frame_buf_size, 1, local_video_source_fp) != yuv_frame_buf_size)
				perror("fread()");
		#else
		
			if((ret = read(cam_c_fp, g_yuv, yuv_frame_buf_size)) < 0)
				perror("read()");
		#endif
			printf("main_thread : read : ret = %d\n", ret);
		
			//每隔d_r发送一帧至detect_thread
			if(--d_r == 0)
			{
				d_r = detect_rate;
				if(sendto(orig2_sock, g_yuv, yuv_gray_frame_buf_size, 0, (struct sockaddr*)&serv2_adr, sizeof(struct sockaddr)) != yuv_gray_frame_buf_size)
				{
					perror("main : sendto() to detect_thread");
					exit(1);
				}
			}
		
			pthread_mutex_lock(&work_mutex);
			#if WITH_LOCAL_DEFAULT == 1
			//stream为视频传输开关，为1则代表传视频到线程video_thread，为0则只采不传
			stream = 1;
			#endif
			if(stream == 1)
			{
				if(--s_r == 0)
				{				
					s_r = send_rate; 
					if(sendto(orig_sock, g_yuv, yuv_frame_buf_size, 0, (struct sockaddr*)&serv_adr, sizeof(struct sockaddr)) != yuv_frame_buf_size)
					{
						perror("main : sendto() to video_thread");
						exit(1);
					}
				}			
			}
			pthread_mutex_unlock(&work_mutex);	

			//heart beat alive
			gettimeofday(&t_end, NULL);
			cost_time_sec=t_end.tv_sec-t_start.tv_sec;
			cost_time_usec=t_end.tv_usec-t_start.tv_usec;
			if(cost_time_usec < 0)
			{
				cost_time_usec += 1000000;
				cost_time_sec--;
			}
			if(cost_time_sec > keep_alive)
			{
				gettimeofday(&t_start, NULL);
				//心跳包，跟时间控制为同一个socket
				write(alive_sockfd, alive_header, sizeof(aliveheader));
			}
	}

	// Start encoding thread 
	if(with_video)
		pthread_join(v_pth, NULL);//使一个线程等待另一个线程结束,linux使用此函数对创建的线程进行资源回收
    pthread_join(d_pth, NULL);
    pthread_join(v_s_pth, NULL);
	pthread_join(k_l_pth, NULL);

//	if(with_audio)
//		pthread_join(a_pth, NULL);
	free(g_yuv);
	exit_from_app();

	return 0;
}


/***************** Vedio Thread *****************/
static void* video_thread(void*){

	/************************字幕处理**************************/
	YUVImage yuvImage = { 0 };
	MixerConfig mixerConfig;

	memset(&mixerConfig, 0, sizeof(MixerConfig));

	yuvImage.dwPitch = WIDTH;
	yuvImage.dwHeight= HEIGHT;
	yuvImage.lpYUVImage= byBuffer;

	CHAR szDate[] = " ";
	CHAR szTime[] = " ";
	USHORT dayForamt[]	= { TIME_FMT_YEAR4, '-', TIME_FMT_MONTH2, '-', TIME_FMT_DAY, ' '};
	USHORT timeForamt[]	= { TIME_FMT_HOUR24, ':', TIME_FMT_MINUTE, ':', TIME_FMT_SECOND, };

	//mixerConfig.timeConfig.bEnable = true;
	mixerConfig.timeConfig.bEnable = 1;
	mixerConfig.timeConfig.dwFontSize= FONT_SIZE_16;
	mixerConfig.timeConfig.x = 150;
	mixerConfig.timeConfig.y = 250;

	//mixerConfig.timeConfig.bAdjustFontLuma = false;
	mixerConfig.timeConfig.bAdjustFontLuma = 0;
	mixerConfig.timeConfig.byFontLuma = 0xFF;

	UINT nLength = 0;
	CopyMemory(&mixerConfig.timeConfig.tFormat[nLength], &szDate, sizeof(szDate));
	nLength += sizeof(szDate) ;
	CopyMemory(&mixerConfig.timeConfig.tFormat[nLength], dayForamt, sizeof(dayForamt));
	nLength += sizeof(dayForamt);

	CopyMemory(&mixerConfig.timeConfig.tFormat[nLength], &szTime, sizeof(szTime));
	nLength += sizeof(szTime) ;
	CopyMemory(&mixerConfig.timeConfig.tFormat[nLength], timeForamt, sizeof(timeForamt));

	nLength += sizeof(timeForamt);

	for (LONG i=0; i<MAX_MASK_COUNT; ++i) {
		mixerConfig.maskConfig[i].bEnable = true;
		mixerConfig.maskConfig[i].rtMask.top	= 250;
		mixerConfig.maskConfig[i].rtMask.left	= 150;
		mixerConfig.maskConfig[i].rtMask.bottom	= mixerConfig.maskConfig[i].rtMask.top + 16;
		mixerConfig.maskConfig[i].rtMask.right	= mixerConfig.maskConfig[i].rtMask.left + 180;
	}
	yuvImage.dwYUVFmt= YUV_FMT_YV12;
/**********************end of字幕处理************************************/

	//add in 2014-10-15
	IplImage * image = NULL;
	IplImage * bg1 = NULL;	 
	
	image  = cvCreateImageHeader(cvSize(lcd_width,lcd_height),8,1);
	CvMat* bg_mat = cvCreateMat(lcd_height, lcd_width, CV_32FC1);
	CvMat* img_mat = cvCreateMat(lcd_height, lcd_width, CV_32FC1);
	//rgbimg = cvCreateImage(cvSize(lcd_width,lcd_height),8,3);

	int arr = 0;//越线和越界检测的点个数
	CvPoint ** ptx = new CvPoint*[1];  
	
	unsigned char rect_buf[256];
	unsigned char * cur_p;
	
	char file_name[100];
	struct timeval t_start,t_end;
	long cost_time_sec,cost_time_usec;
	long yuv_no = 1;
	int start, ret;
	int yuv_frame_buf_size = (lcd_width*lcd_height)+(lcd_width*lcd_height)/2;
	unsigned char g_yuv[yuv_frame_buf_size];
	unsigned char * encoded_buf;
	long encoded_buf_size;


	int orig_sock, clnt_len;
	struct sockaddr_un serv_adr, clnt_adr;
	int reuse_addr, bufsize_addr,bufsize_len;

	uint32 T = t_init;
	
	long F = 0;
	uint32 K = 0, R = 0;
	unsigned int i;
//	int i, j, k;
	unsigned int symbol_no;

	uint8* input_buf = (uint8*)malloc(K1_MAX*T_MAX);
	uint8* output = (uint8*)malloc(K1_MAX*T_MAX);
	char * output_buf = (char *)malloc(sizeof(Frame_header)+T_MAX);  //output_buf存储生成的每一个encoding symbol
	char * sps_pps = (char *)malloc(30);
	Frame_header * frame_header = (Frame_header *)malloc(sizeof(Frame_header));
	uint8* intermediate = (uint8*)malloc(K1_MAX*T_MAX+100);
	long long loop_times;
	long slice_no = 1;


	RParam para;
	para = (RParam)malloc(sizeof(RaptorParam));

	raptor_init(k1_max_init, para);

	FILE * local_video_source_fp = NULL;
	
	if(with_local){
		if((local_video_source_fp = fopen("origin_video", "rb")) == 0){
			perror("origin_video");
			exit(1);
		}
			
		loop_times = 3076;  //特定大小
	}
	else loop_times = LONG_MAX;


	pthread_mutex_lock(&mutex);
	video_handle = mfc_encoder_init(lcd_width, lcd_height, 30, 1000, 30);
	printf(">>>>>> video_handle init\n");

	sprintf(&file_name[0], "Cam_encoding_%dx%d.264", lcd_width, lcd_height);
	fflush(stdout);
	
	/*socket AF_UNIX*/
	//socket进行线程间通信,此处为服务端,与main函数中客户端相呼应
	if((orig_sock = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
	{
		printf("socket error\n");
		exit(1);
	}
	serv_adr.sun_family = AF_UNIX;
	strcpy(serv_adr.sun_path, SERVER_FILE);
	unlink(SERVER_FILE);
	if(bind(orig_sock, (struct sockaddr *)&serv_adr, strlen(serv_adr.sun_path)+sizeof(serv_adr.sun_family)) < 0 )
	{
		printf("video_thread : bind error\n");
		clean_up(orig_sock, SERVER_FILE); 
		exit(1);                            
	}
	reuse_addr = 1;
	bufsize_addr = yuv_frame_buf_size*2;
	bufsize_len = sizeof(bufsize_addr);
	if(setsockopt(orig_sock, SOL_SOCKET, SO_RCVBUF, (void *)&bufsize_addr, bufsize_len) < 0)
	{
		perror("video_thread : setsockopt()");
		exit(1);
	}
	//setsockopt(orig_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
	printf("video_thread : bufsize_addr:%d, sizeof(bufsize_addr):%d, sizeof(int):%d\n", bufsize_addr, sizeof(bufsize_addr), sizeof(int));


	//*** set GOP & QP & slice_num***
	H264_ENC_CONF conf_type = H264_ENC_SETCONF_PARAM_CHANGE;
	int value[2];
	value[0] = H264_ENC_PARAM_GOP_NUM;
	value[1] = gop;
	SsbSipH264EncodeSetConfig(video_handle, conf_type, value);

	value[0] = H264_ENC_PARAM_INTRA_QP;
	value[1] = qp;
	SsbSipH264EncodeSetConfig(video_handle, conf_type, value);

	value[0] = H264_ENC_PARAM_SLICE_MODE;
	value[1] = 4;
	SsbSipH264EncodeSetConfig(video_handle, conf_type, value);


	while(yuv_no < loop_times ){
		signal(SIGINT, (void (*)(int))ctrl_c);

		#if RES_DEFAULT == 4
		if((ret = recvfrom(orig_sock, g_yuv, yuv_frame_buf_size, 0, (struct sockaddr*) &clnt_adr, (socklen_t*) &clnt_len)) > 0)
		{
			printf("video_thread : after recv %d byte\n", ret);
			printf("video_thread : g_yuv len = %d\n", strlen((char*)g_yuv));
			printf("video_thread : g_yuv %d\n", (int)g_yuv);
		}			
			//g_yuv = (unsigned char *)c_index;
		else
			perror("recvfrom()");
		#endif


		/*****************************字幕处理，主要处理g_yuv*********************************/
		    memcpy(yuvImage.lpYUVImage,g_yuv,YUV420Size);

		    HANDLE hMixer = YOM_Initialize();
		    YOM_MixOSD(hMixer, &mixerConfig, &yuvImage);
		    YOM_Uninitialize(hMixer);
		
		    memcpy(g_yuv,yuvImage.lpYUVImage,YUV420Size);
		
		/*****************************end of 字幕处理*********************************/	


		/*******************************************************/
		//add in 2014-10-15
		//update 2014-10-20
		//入侵检测
		
		if((ret = recvfrom(sockfd, rect_buf,sizeof(rect_buf),MSG_DONTWAIT,(struct sockaddr *)&saddr,(socklen_t *)&addr_len))> 0)
		{
			if(arr!=0)
				delete [] *ptx;
			memcpy(&arr,rect_buf,sizeof(int));
			arr = ntohl(arr);
			int x = 0;
			int y = 0;
			cur_p = rect_buf + sizeof(int);

			ptx[0] = new CvPoint[arr];  
			for(int i = 0;i<arr;i++)
			{
				memcpy(&x,cur_p,sizeof(int));
				x = ntohl(x);
				cur_p =cur_p + sizeof(int);
				memcpy(&y,cur_p,sizeof(int));
				y = ntohl(y);
				cur_p =cur_p + sizeof(int);
				ptx[0][i] = cvPoint(x,y);
			}
			
		}
		if(arr>=3)
		{
			cvSetData(image,g_yuv,lcd_width);
			DetectIncursion(image,bg1,ptx,arr);
			cvPolyLine(image,ptx,&arr,1,2,CV_RGB(255,0,255),1); 
			//cvConvert(image, img_mat);
			//cvRunningAvg(img_mat, bg_mat, 0.003, 0);
			//cvConvert(bg_mat, bg1);
		}
		if(arr == 2)
		{
			cvSetData(image,g_yuv,lcd_width);

			if(!bg1)
			{
				bg1 = cvCreateImage(cvSize(lcd_width,lcd_height),8,1);
				cvCopy(image,bg1,0);
			}
			crossLine(image,bg1,ptx);
			cvPolyLine(image,ptx,&arr,1,2,CV_RGB(255,0,255),1); 
			//cvConvert(image, img_mat);
			//cvRunningAvg(img_mat, bg_mat, 0.003, 0);
			//cvConvert(bg_mat, bg1);
		}
		/*********************end of 入侵检测*******************/

		/*********************编码传输相关**************************/
		if(yuv_no % gop == 1){

			printf("264 i\n");
			
			conf_type = H264_ENC_SETCONF_CUR_PIC_OPT;
			value[0] = H264_ENC_PIC_OPT_IDR;
			value[1] = 1;
			SsbSipH264EncodeSetConfig(video_handle, conf_type, value);
		

			encoded_buf = (unsigned char*)mfc_encoder_exe(video_handle, g_yuv, yuv_frame_buf_size, 1, &encoded_buf_size);

			if(yuv_no == 1){
				memcpy(sps_pps,encoded_buf,21);
			}

			F = encoded_buf_size+21;
			K = (uint32)ceil((double)F/T);
						
			if(with_fec){
            	R = (uint32)ceil(K/2);
		        printf("\nK = %d  R = %d  LT_R = 50%%\n",(int)K,(int)R);
            }
            else{
            	memset(input_buf, 0, K*T);
			    memcpy(input_buf,sps_pps,21);
			    memcpy(input_buf+21,encoded_buf,encoded_buf_size);
            }
		}
		else{
			printf("264 p\n");

			encoded_buf = (unsigned char*)mfc_encoder_exe(video_handle, g_yuv, yuv_frame_buf_size, 0, &encoded_buf_size);
		
			F = encoded_buf_size;
			K = (uint32)ceil((double)F/T);
			if(K<5){
				K = 5;
			}
			
			if(with_fec){
            	R = (uint32)ceil((LT_R*K)/(100-LT_R));
		        printf("\nK = %d, R = %d  LT_R = %d%%\n",(int)K,(int)R, LT_R);
            }
            else{
            	memset(input_buf, 0, K*T);
			    memcpy(input_buf, encoded_buf, encoded_buf_size);
            }
		}

		memset(output_buf,0,sizeof(Frame_header)+T_MAX);//2012
		if(with_fec)
		{
			gettimeofday(&t_start, NULL);

			raptor_reset(K,para);

			memset(input_buf, 0, para->L*T);
			if(yuv_no%gop == 1){
				memcpy(input_buf+(para->S+para->H)*T,sps_pps,21);
				memcpy(input_buf+(para->S+para->H)*T+21, encoded_buf, encoded_buf_size);
			}else{
				memcpy(input_buf+(para->S+para->H)*T, encoded_buf, encoded_buf_size);
			}


			int result = raptor_encode(para,R,input_buf,intermediate,output,T);
			if(result == 0){
				printf("encode error!\n");
				//raptor_parameterfree(para);
				if(yuv_no == INT_MAX)
					yuv_no = 0;
				else
				    yuv_no++;

				continue;
			}

			gettimeofday(&t_end, NULL);
			cost_time_sec=t_end.tv_sec-t_start.tv_sec;
			cost_time_usec=t_end.tv_usec-t_start.tv_usec;
			if(cost_time_usec<0){
				cost_time_usec+=1000000;
				cost_time_sec--;			
			}
			printf("\n frame :%ld raptor encoder cost time %ld.%06ld s\n",yuv_no, cost_time_sec, cost_time_usec);
			
			for(symbol_no = 0; symbol_no < K+R; symbol_no++){ 
				frame_header->frame_no = htonl(yuv_no);
				frame_header->slice_no = htonl(slice_no);
				if(slice_no == LONG_MAX)
					slice_no = 0;
				else
					slice_no++;
				frame_header->frame_type = htonl(1);
				frame_header->F = htonl(F);
				frame_header->T = htonl(T);
				frame_header->K = htonl(K);
				frame_header->R = htonl(R);
				frame_header->esi = htonl(symbol_no);
				frame_header->camera_no = htonl(camera_no);

				memcpy(output_buf, frame_header, sizeof(Frame_header));
				memcpy(output_buf+sizeof(Frame_header), output+symbol_no*T, T);

			
				sem_wait(&sem_room);
				sem_wait(&sem_id);
			
				memcpy(sendq+(T+sizeof(Frame_header))*savedata, output_buf, sizeof(Frame_header)+T);
				savedata = (savedata+1)%20;
				sem_post(&sem_quene);
				sem_post(&sem_id);
			}

			if(yuv_no == INT_MAX)
				yuv_no = 0;
			else
				yuv_no++;	
		}
		else{

			for(i = 0; i < K; i++){
				frame_header->frame_no = htonl(yuv_no);
				frame_header->slice_no = htonl(slice_no);
				if(slice_no == LONG_MAX)
					slice_no = 0;
				else
					slice_no++;
				frame_header->frame_type = htonl(2);
				frame_header->F = htonl(F);
				frame_header->T = htonl(T);
				frame_header->K = htonl(K);
				frame_header->R = htonl(0);
				frame_header->esi = htonl(i);
                frame_header->camera_no = htonl(camera_no);
                
				memcpy(output_buf, frame_header, sizeof(Frame_header));
				memcpy(output_buf+sizeof(Frame_header), input_buf+i*T, T);


				sem_wait(&sem_room);
				sem_wait(&sem_id);

				memcpy(sendq+(T+sizeof(Frame_header))*savedata, output_buf, sizeof(Frame_header)+T);
				savedata = (savedata+1)%20;
	
				sem_post(&sem_quene);
				sem_post(&sem_id);
			}

		    if(yuv_no == INT_MAX)
				yuv_no = 0;
			else
				yuv_no++;
		}
		/*********************end of 编码传输相关**************************/
	}
	cvReleaseMat(&img_mat);
	cvReleaseMat(&bg_mat);
	cvReleaseImageHeader(&image);
	cvReleaseImage(&bg1);
	delete ptx;
	if(with_local == 0){
		// Codec stop 
	    start = 0;
	    ioctl(cam_c_fp, VIDIOC_STREAMOFF, &start);
	}

	pthread_mutex_unlock(&mutex);

	return 0;
	
}

/****************************

图像检测线程
update in 2014-11-3

****************************/

void *detect_thread(void *arg)
{
	printf("detect_thread : detect_thread start\n");
	unsigned char *yuv_frame;
	int update_bg = 1;

	int orig_d_sock, clnt_d_len;
	struct sockaddr_un serv_d_adr, clnt_d_adr;
	int reuse_d_addr, bufsize_d_addr,bufsize_d_len;
	int ret = 0;

	IplImage* bg = cvCreateImage(cvSize(lcd_width, lcd_height), IPL_DEPTH_8U, COLOR_CHN);
	IplImage* img = cvCreateImageHeader(cvSize(lcd_width, lcd_height), IPL_DEPTH_8U, COLOR_CHN);
	IplImage* copyimg = cvCreateImage(cvSize(lcd_width, lcd_height), IPL_DEPTH_8U, COLOR_CHN);
	CvMat* bg_mat = cvCreateMat(lcd_height, lcd_width, CV_32FC1);
	CvMat* img_mat = cvCreateMat(lcd_height, lcd_width, CV_32FC1);
	int framenum = 0;

	struct timeval t_start,t_end;
	long cost_time_sec,cost_time_usec;

	int start_time = 0;
	int end_time = 0; 
	unsigned char * cur_p = NULL;

	/*set socket for AF_UNIX*/
	printf("detect_thread : before set AF_UNIX socket\n");
	if((orig_d_sock = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
	{
		printf("detect_thread : socket error\n");
		exit(1);
	}
	serv_d_adr.sun_family = AF_UNIX;
	strcpy(serv_d_adr.sun_path, SERVER2_FILE);
	unlink(SERVER2_FILE);

	if(bind(orig_d_sock, (struct sockaddr *)&serv_d_adr, strlen(serv_d_adr.sun_path)+sizeof(serv_d_adr.sun_family)) < 0 )
	{
		printf("detect_thread : bind error\n");
		clean_up(orig_d_sock, SERVER2_FILE); 
		exit(1);                            
	}

	reuse_d_addr = 1;
	bufsize_d_addr = yuv_gray_frame_buf_size*2;
	bufsize_d_len = sizeof(bufsize_d_addr);

	if(setsockopt(orig_d_sock, SOL_SOCKET, SO_RCVBUF, (void *)&bufsize_d_addr, bufsize_d_len) < 0)
	{
		perror("detect_thread : setsockopt()");
		clean_up(orig_d_sock, SERVER2_FILE); 
		exit(1);
	}
	printf("detect_thread : bufsize_addr:%d, sizeof(bufsize_addr):%d, sizeof(int):%d\n", bufsize_d_addr, sizeof(bufsize_d_addr), sizeof(int));
	//setsockopt(orig_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

	yuv_frame = (unsigned char*)malloc(lcd_width*lcd_height*sizeof(unsigned char));

	printf("detect_thread : after initialization\n");

	bool time_flag = false; 
	int findnums = 0;
	while(1)
	{
		#if RES_DEFAULT == 4	
		if((ret = recvfrom(orig_d_sock, yuv_frame, yuv_gray_frame_buf_size, 0, (struct sockaddr*) &clnt_d_adr, (socklen_t *)&clnt_d_len)) <= 0)
				perror("recvfrom()");		
		#endif
				
		#if RES_DEFAULT == 6
		
		if((ret = recvfrom(orig_d_sock, yuv_frame, yuv_gray_frame_buf_half_size, 0, (struct sockaddr*) &clnt_d_adr, (socklen_t *)&clnt_d_len)) <= 0)
				perror("recvfrom()");
		if((ret = recvfrom(orig_d_sock, yuv_frame+yuv_gray_frame_buf_half_size, yuv_gray_frame_buf_half_size, 0, (struct sockaddr*) &clnt_d_adr, (socklen_t *)&clnt_d_len)) <= 0)
				perror("recvfrom()");
		#endif
		printf("detect_thread : after recvform() : %d byte\n", ret);
		//IplImage *img = cvCreateImageHeader(cvSize(lcd_width, lcd_height), IPL_DEPTH_8U, COLOR_CHN);
		cvSetData(img, yuv_frame, img->widthStep);


		/*after get (IplImage)img. detecting process start*/
		
		time_t curTime;
		time(&curTime);
		struct tm *tm_p;
		tm_p = localtime(&curTime);
		int cur_minute = 60*(tm_p->tm_hour)+tm_p->tm_min;

		time_flag = false;//如果接收到时间控制命令，则为true
		//add in 2014-11-18
		if((ret = recvfrom(alive_sockfd, time_buff,sizeof(time_buff),MSG_DONTWAIT,(struct sockaddr *)&aliveaddr,(socklen_t *)&addr_len))> 0)
		{
			time_flag = true;
			memcpy(&start_time,time_buff,sizeof(int));
			start_time = ntohl(start_time);
			cur_p = time_buff + sizeof(start_time);
			memcpy(&end_time,cur_p,sizeof(int));
			end_time = ntohl(end_time);
		}
		if(framenum > 40)
		{	
			gettimeofday(&t_start,NULL);
			cvCopy(img, copyimg, 0);
						
			if((cur_minute>=start_time&&cur_minute<=end_time))
			{
				findnums = findmotion(img, copyimg, bg) ;
				if(findnums || time_flag)
				{
					pthread_mutex_lock(&work_mutex);
					if(findnums)
						time_out = TIME_OUT_FRAME;
					else//如果收敛的情况下接收到时间命令，则将time_out设置为5帧，并计数，接着将stream置为1，传视频
						time_out = TIME_OUT_FRAME_EIGHTH;
					stream = 1;
					move = 1;
					pthread_mutex_unlock(&work_mutex);
				}
				else
				{
					pthread_mutex_lock(&work_mutex);
					move = 0;
					if(time_out > 0)
						time_out--;
					else if(time_out == 0)
						stream = 0;	
					else
						time_out = 0;
					pthread_mutex_unlock(&work_mutex);
				}
			}
			else
			{
				pthread_mutex_lock(&work_mutex);
				stream = 0;
				pthread_mutex_unlock(&work_mutex);
			}	
			if(update_bg)
			{
				cvConvert(img, img_mat);
				cvRunningAvg(img_mat, bg_mat, 0.003, 0);
				cvConvert(bg_mat, bg);
			}
				
			gettimeofday(&t_end, NULL);
		    	cost_time_sec=t_end.tv_sec-t_start.tv_sec;
		   	cost_time_usec=t_end.tv_usec-t_start.tv_usec;
			if(cost_time_usec < 0)
			{
				cost_time_usec+=1000000;
				cost_time_sec--;
			}
		    	printf("move_detect cost time %ld.%06ld s\n",cost_time_sec,cost_time_usec);
	    		
		}
		else if(framenum < 40)
		{
			++framenum;
			continue;
		}
		else if(framenum == 40)
		{
			cvCopy(img, bg, 0);
			cvConvert(bg, bg_mat);
			++framenum; 
			printf("******************** detect start! *********************\n");
		}
	}

	cvReleaseMat(&img_mat);
	cvReleaseMat(&bg_mat);
	cvReleaseImageHeader(&img);
	cvReleaseImage(&copyimg);
	cvReleaseImage(&bg);
	clean_up(orig_d_sock, SERVER2_FILE); 
	//close(sig_sock);
}


static void* video_send_thread(void*){
	int T_INIT = t_init;
	int pLen;

	char output_buf_s[1024];
	char temp_frame[1024];

	int w_n;
	int i;
	int header_size = sizeof(Frame_header);
	unsigned int esi_temp = 0;
	unsigned int frame_no_temp = 0;
	int temp_frame_flag = 0;
	int data_len = sizeof(int);

	Frame_header *frame_header = (Frame_header *)malloc(sizeof(Frame_header));

	while(1){

		i = 0;
		pLen = 0;
		memset(output_buf_s, '\0', sizeof(output_buf_s));

		if(temp_frame_flag == 1){
        	memcpy(frame_header, temp_frame, header_size);

        	frame_no_temp = ntohl(frame_header->frame_no);
	        esi_temp = ntohl(frame_header->esi);

	        printf("frame_no : %d esi : %d size : %d\n", frame_no_temp, esi_temp, header_size+T_INIT);

        	memcpy(output_buf_s, temp_frame, header_size+T_INIT);
            
            i++;
        	getdata = (getdata+1)%20;
		    pLen = T_INIT + header_size;
		    pLen += data_len;

		    temp_frame_flag = 0;
        }

		while(pLen+T_INIT <= 1024){
			sem_wait(&sem_quene);
		    sem_wait(&sem_id);

	   
		    memcpy(frame_header,sendq+getdata*(T_INIT+header_size),header_size);

	        if(pLen == 0){
	        	
	        	frame_no_temp = ntohl(frame_header->frame_no);
	        	esi_temp = ntohl(frame_header->esi);

	        	printf("frame_no : %d esi : %d size : %d\n", frame_no_temp, esi_temp, header_size+T_INIT);

	        	memcpy(output_buf_s, sendq+getdata*(T_INIT+header_size), header_size+T_INIT);
                
                
	        	pLen = T_INIT + header_size;
	        	pLen += data_len;
	        }
	        else{
	        	if(frame_no_temp == ntohl(frame_header->frame_no)){
	        		esi_temp ++;

	        		printf("frame_no : %d esi : %d size : %d\n", frame_no_temp, esi_temp, header_size+T_INIT);

//	        		memcpy(output_buf_s+header_size+i*T_INIT+data_len, sendq+getdata*(T_INIT+header_size)+header_size, T_INIT);
	        		memcpy(output_buf_s+pLen, sendq+getdata*(T_INIT+header_size)+header_size, T_INIT);

	        		
	        	    pLen += T_INIT;
	        	}
	        	else{
	        		temp_frame_flag = 1;
	        		memcpy(temp_frame, sendq+getdata*(T_INIT+header_size), header_size+T_INIT);

                    sem_post(&sem_room);
		            sem_post(&sem_id);
         
	        		break;
	        	}
	        }
		    
		    sem_post(&sem_room);
		    sem_post(&sem_id);
            
            i++;
		    getdata = (getdata+1)%20;
		    
		}
		
		i--;
		i = htonl(i);
		
        memcpy(output_buf_s+T_INIT+header_size, &i, sizeof(i));

		w_n = write(sockfd, output_buf_s, pLen);
		printf("send %d bytes plen = %d i = %d\n", w_n, pLen, ntohl(i));
	
		usleep(time_dely*1000);
	}

	return 0;
}

//心跳包，跟视频传输同一个socket，端口8888
static void* keep_linked_thread(void*){
	char msg[] = "###";

	while(1){
		write(sockfd, msg, sizeof(msg));

		sleep(keep_alive);
	}

	return 0;
}


/*
void audio_thread(void){
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
		signal(SIGINT, (void (*)(int))ctrl_c);
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
		printf("Audio | frameNo=%d, count=%d, fragmentLen=%d, fragmentNo=%d\n", ntohl(frame_info.frameNo), ntohl(frame_info.count), ntohl(frame_info.fragmentLen), ntohl(frame_info.fragmentNo));
		sendto(sockfd, frame, frame_len, 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
	}
	//snd_pcm_close(audio_handle);
}

*/
/***************** Camera driver function *****************/
static int cam_p_init(void){
	int dev_fp = -1;

	if((dev_fp = open(PREVIEW_NODE, O_RDWR)) < 0){
		perror(PREVIEW_NODE);
		return -1;
	}
	return dev_fp;
}

static int cam_c_init(void){
	int dev_fp = -1;

	if((dev_fp = open(CODEC_NODE, O_RDWR)) < 0){
		perror(CODEC_NODE);
		printf("CODEC : Open Failed \n");
		return -1;
	}
	return dev_fp;
}

static int read_data(int fp, char *buf, int width, int height, int bpp){
	int ret;
	if(bpp == 16){
		if((ret = read(fp, buf, width * height * 2)) != width * height * 2){
			return 0;
		}
	} 
	else{
		if((ret = read(fp, buf, width * height * 4)) != width * height * 4){
			return 0;
		}
	}
	return ret;
}



/***************** Display driver function *****************/
/*
static int fb_init(int win_num, int bpp, int x, int y, int width, int height, unsigned int *addr){
	int 			dev_fp = -1;
	int 			fb_size;
	s3c_win_info_t	fb_info_to_driver;

	switch(win_num){
		case 0:
			dev_fp = open(FB_DEV_NAME, O_RDWR);
			break;
		case 1:
			dev_fp = open(FB_DEV_NAME1, O_RDWR);
			break;
		case 2:
			dev_fp = open(FB_DEV_NAME2, O_RDWR);
			break;
		case 3:
			dev_fp = open(FB_DEV_NAME3, O_RDWR);
			break;
		case 4:
			dev_fp = open(FB_DEV_NAME4, O_RDWR);
			break;
		default:
			printf("Window number is wrong\n");
			return -1;
	}

	if(dev_fp < 0){
		perror(FB_DEV_NAME);
		return -1;
	}

	switch(bpp){
		case 16:
			fb_size = width * height * 2;	
			break;
		case 24:
			fb_size = width * height * 4;
			break;
		default:
			printf("16 and 24 bpp support");
			return -1;
	}
		
	if((*addr = (unsigned int) mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fp, 0)) < 0){
		printf("mmap() error in fb_init()");
		return -1;
	}

	fb_info_to_driver.Bpp 		= bpp;
	fb_info_to_driver.LeftTop_x	= x;
	fb_info_to_driver.LeftTop_y	= y;
	fb_info_to_driver.Width 	= width;
	fb_info_to_driver.Height 	= height;

	if(ioctl(dev_fp, SET_OSD_INFO, &fb_info_to_driver)){
		printf("Some problem with the ioctl SET_VS_INFO!!!\n");
		return -1;
	}

	if(ioctl(dev_fp, SET_OSD_START)){
		printf("Some problem with the ioctl START_OSD!!!\n");
		return -1;
	}

	return dev_fp;
}
*/

static void draw(char *dest, char *src, int width, int height, int bpp){
	int x, y;
	unsigned long *rgb32;
	unsigned short *rgb16;

	int end_y = height;
	int end_x = MIN(lcd_width, width);

	if(bpp == 16){

#if !defined(LCD_24BIT)
		for(y = 0; y < end_y; y++){
			memcpy(dest + y * lcd_width * 2, src + y * width * 2, end_x * 2);
		}
#else
		for(y = 0; y < end_y; y++){
			rgb16 = (unsigned short *) (src + (y * width * 2));
			rgb32 = (unsigned long *) (dest + (y * lcd_width * 2));

			// TO DO : 16 bit RGB data -> 24 bit RGB data
			for(x = 0; x < end_x; x++){
				*rgb32 = ( ((*rgb16) & 0xF800) << 16 ) | ( ((*rgb16) & 0x07E0) << 8 ) |
					( (*rgb16) & 0x001F );
				rgb32++;
				rgb16++;
			}
		}

#endif
	}
	else if(bpp == 24){
#if !defined(LCD_24BIT)
		for(y = 0; y < end_y; y++){
			rgb32 = (unsigned long *) (src + (y * width * 4));
			rgb16 = (unsigned short *) (dest + (y * lcd_width * 2));

			// 24 bit RGB data -> 16 bit RGB data 
			for(x = 0; x < end_x; x++){
				*rgb16 = ( (*rgb32 >> 8) & 0xF800 ) | ( (*rgb32 >> 5) & 0x07E0 ) | ( (*rgb32 >> 3) & 0x001F );
				rgb32++;
				rgb16++;
			}
		}
#else
		for(y = 0; y < end_y; y++){
			memcpy(dest + y * lcd_width * 4, src + y * width * 4, end_x * 4);
		}
#endif
	}
}


/***************** MFC driver function *****************/
void *mfc_encoder_init(int width, int height, int frame_rate, int bitrate, int gop_num){
	int				frame_size;
	void			*handle;
	int				ret;

	frame_size	= (width * height * 3) >> 1;

	handle = SsbSipH264EncodeInit(width, height, frame_rate, bitrate, gop_num);
	if(handle == NULL){
		LOG_MSG(LOG_ERROR, "Test_Encoder", "SsbSipH264EncodeInit Failed\n");
		return NULL;
	}

	ret = SsbSipH264EncodeExe(handle);

	return handle;
}

void *mfc_encoder_exe(void *handle, unsigned char *yuv_buf, int frame_size, int first_frame, long *size){
	unsigned char	*p_inbuf, *p_outbuf;
	int				hdr_size;
	int				ret;

	p_inbuf =(unsigned char*)SsbSipH264EncodeGetInBuf(handle, 0);

	memcpy(p_inbuf, yuv_buf, frame_size);


	ret = SsbSipH264EncodeExe(handle);
	if(first_frame){
		SsbSipH264EncodeGetConfig(handle, H264_ENC_GETCONF_HEADER_SIZE, &hdr_size);
	}

	p_outbuf = (unsigned char*)SsbSipH264EncodeGetOutBuf(handle, size);

	return p_outbuf;
}

void mfc_encoder_free(void *handle){
	SsbSipH264EncodeDeInit(handle);
}
/*
int extract_nal(char * buf, int length, void * dec_handle){
	void * pStrmBuf;
	int nFrameLeng = 0;
	FRAMEX_CTX * pFrameExCtx;
	FRAMEX_STRM_PTR file_strm;
	pFrameExCtx = FrameExtractorInit(FRAMEX_IN_TYPE_MEM, delimiter_h264, sizeof(delimiter_h264), 1);
	file_strm.p_start = file_strm.p_cur = (unsigned char *)buf;
	file_strm.p_end = (unsigned char *)(buf + length);
	FrameExtractorFirst(pFrameExCtx, &file_strm);

	if(dec_handle == NULL){
		printf("H264_Dec_Init_Failed.\n");
		return 1;
	}
	pStrmBuf = SsbSipH264DecodeGetInBuf(dec_handle, nFrameLeng);
	if(pStrmBuf == NULL){
		printf("SsbSipH264DecodeGetInBuf Failed.\n");
		return 1;
	}

	unsigned char frame_type[10];
	int nFrameSize;
	FrameExtractorPeek(pFrameExCtx, &file_strm, frame_type, sizeof(frame_type), (int *)&nFrameSize);
	int nal_type = frame_type[5];
	return nal_type;
}
*/
/*static void open_alsa_device(){
	snd_pcm_open(&audio_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
}*/
/*
static void set_alsa_params(){
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
*/
static void ctrl_c(int no){
	if(video_handle != NULL){
		mfc_encoder_free(video_handle);
		snd_pcm_close((snd_pcm_t*)video_handle);
		
		//close file
		fclose(file_v);
		printf("********Interrupt and free the handle********\n");
	}
	if(video_handle1 != NULL){
		mfc_encoder_free(video_handle1);
		snd_pcm_close((snd_pcm_t*)video_handle1);
		printf("********Interrupt and free the handle********\n");
	}
	//if(audio_handle != NULL)
	//	snd_pcm_close(audio_handle);
}

static void exit_from_app(){
	int start;
	int fb_size = lcd_width * lcd_height * 4;
	int ret;
	
	if(with_preview){
		// Stop previewing 
	    start = 0;
	    ret = ioctl(cam_p_fp, VIDIOC_OVERLAY, &start);
	    if(ret < 0){
		    printf("V4L2 : ioctl on VIDIOC_OVERLAY failed\n");
		    exit(1);
	    }

	    close(cam_p_fp);
	    munmap(win0_fb_addr, fb_size);
	}
	

	switch(LCD_BPP){
		case 16:
			fb_size = lcd_width * lcd_height * 2;
			break;
		case 24:
			fb_size = lcd_width * lcd_height * 4;
			break;
		default:
			fb_size = lcd_width * lcd_height * 4;
			printf("LCD supports 16 or 24 bpp\n");
			break;
	}

	if(cam_c_fp){
		close(cam_c_fp);
	}
	    
    if(video_handle){
    	mfc_encoder_free(video_handle);
    }
}

void clean_up(int sd, const char* the_file)
{
close(sd);
unlink(the_file);
}

/********************************
add in 2014-10-15
判断点是否在多边形内
*********************************/
bool ptInPolygon(CvPoint** ptx,int t,CvPoint pt)
{
	int nCross = 0; 
	for (int i = 0; i < t; i++) 
	{ 
		CvPoint p1 = ptx[0][i]; 
		CvPoint p2 = ptx[0][(i + 1) % t]; 
		// 求解 y=p.y 与 p1p2 的交点 
		if ( p1.y == p2.y ) // p1p2 与 y=p0.y平行 
			continue; 
		if ( (pt.y < p1.y) && (pt.y < p2.y) ) // 交点在p1p2延长线上 
			continue; 
		if ( (pt.y >= p1.y) && (pt.y >= p2.y) ) // 交点在p1p2延长线上 
			continue; 
		// 求交点的 X 坐标 
		double x = (double)(pt.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x; 
		if ( x > pt.x ) 
			nCross++; // 只统计单边交点 
	} 
	// 单边交点为偶数，点在多边形之外
	return (nCross % 2 == 1); 
}

/********************************
add in 2014-10-15
edit in 2014-10-21
入侵检测
*********************************/
void DetectIncursion(IplImage* img,IplImage* bg, CvPoint** ptx, int t)
{
	
	IplImage* img_contours = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage* img_contours1 = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage* img_contours2 = cvCreateImage(cvSize(img->width,img->height),8,1);

	cvSub(img, bg, img_contours1);
	cvSub(bg, img, img_contours2);
	cvAdd(img_contours1, img_contours2, img_contours);

	cvZero(img_contours2);
	CvPoint pt,p1,p2,p3,p4;
	cvThreshold(img_contours , img_contours , 40, 255,  CV_THRESH_BINARY);
	cvErode(img_contours , img_contours , NULL, 1);
	cvDilate(img_contours , img_contours , NULL, 1);//cvErode()腐蚀后cvDilate()膨胀，叫作开操作，那些离散点或游丝线、毛刺就被过滤
    cvSmooth(img_contours , img_contours , CV_BLUR, 5, 5, 0, 0);


	CvMemStorage * storage=cvCreateMemStorage(0);
	CvSeq * contours = 0;
	cvFindContours(img_contours , storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	CvSeq    * current;
	current=contours;  
	while(current)
	{
		if(abs(cvContourArea(current, CV_WHOLE_SEQ)) >= 256 )
		{	
			CvRect r = cvBoundingRect(current,0); 
			//cvRectangle(img, cvPoint(r.x, r.y), cvPoint(r.x + r.width, r.y + r.height),CV_RGB(255,255, 255), 1, 8, 0); 
			pt = cvPoint((r.x+ r.x + r.width)/2,(r.y + r.y + r.height)/2);
			p1 = cvPoint(r.x,r.y);
			p2 = cvPoint(r.x+r.width, r.y);
			p3 = cvPoint(r.x, r.y+r.height);
			p4 = cvPoint(r.x+r.width, r.y+r.height);
			if(ptInPolygon(ptx,t,pt) || ptInPolygon(ptx,t,p1)  || ptInPolygon(ptx,t,p2) || ptInPolygon(ptx,t,p3) || ptInPolygon(ptx,t,p4) )
			{
				CvFont font;
				cvInitFont(&font, 5, 1, 1, 0, 2);
				cvPutText(img, "Alarm, restricted zone!", cvPoint(5,20), &font, cvScalar( 255, 0, 255 ));
			}
		}
		current = current->h_next;
	}
    
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&img_contours1);
	cvReleaseImage(&img_contours2);
	cvReleaseImage(&img_contours);

}


//计算向量pp1和向量p2p1的叉积  
int direction( CvPoint p1,CvPoint p2,CvPoint p )
{
    	return ( p1.x -p.x )*( p2.y-p.y) -  ( p2.x -p.x )*( p1.y-p.y)   ;
}

//判断点p是否在线段p1 p2上  
int on_segment(CvPoint p1,CvPoint p2,CvPoint p) 
{ 

	int max=p1.x > p2.x ? p1.x : p2.x ;
	int min =p1.x < p2.x ? p1.x : p2.x ;
	int max1=p1.y > p2.y ? p1.y : p2.y ;
	int min1=p1.y < p2.y ? p1.y : p2.y ;
	if( p.x >=min && p.x <=max && p.y >=min1 && p.y <=max1 )
	    return 1;
	else
	   	return 0;
}

//判断线段p1p2和p3p4是否相交
bool judge(CvPoint p1,CvPoint p2,CvPoint p3,CvPoint p4)   
{ 
	int d1 = direction(p3,p4,p1); 
	int d2 = direction(p3,p4,p2); 
	int d3 = direction(p1,p2,p3); 
	int d4 = direction(p1,p2,p4); 
	if(d1*d2<0&&d3*d4<0) 
	    	return true; 
	if(d1==0&&on_segment(p3,p4,p1)) 
	    	return true; 
	if(d2==0&&on_segment(p3,p4,p2)) 
	    	return true; 
	if(d3==0&&on_segment(p1,p2,p3)) 
	    	return true; 
	if(d4==0&&on_segment(p1,p2,p4)) 
	    	return true; 
	return false; 
}
/********************************
add in 2014-10-22
edit in 2014-10-22
判断线段和四边形相交
*********************************/
bool rectandLine(CvPoint** ptx, CvPoint q1,CvPoint q2,CvPoint q3,CvPoint q4)
{
	CvPoint p1 = ptx[0][0];
	CvPoint p2 = ptx[0][1];
	
	if(judge(p1,p2,q1,q2) || judge(p1,p2,q2,q4)  || judge(p1,p2,q3,q4)  || judge(p1,p2,q1,q3) )
		return true;
	else
		return false;
}


/********************************
add in 2014-10-22
edit in 2014-10-22
绊线检测
*********************************/
void crossLine(IplImage* img,IplImage* bg, CvPoint** ptx)
{
	
	IplImage* img_contours = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage* img_contours1 = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage* img_contours2 = cvCreateImage(cvSize(img->width,img->height),8,1);

	cvSub(img, bg, img_contours1);
	cvSub(bg, img, img_contours2);
	cvAdd(img_contours1, img_contours2, img_contours);

	cvZero(img_contours2);
	CvPoint p1,p2,p3,p4;
	cvThreshold(img_contours , img_contours , 40, 255,  CV_THRESH_BINARY);
	cvErode(img_contours , img_contours , NULL, 1);
	cvDilate(img_contours , img_contours , NULL, 1);//cvErode()腐蚀后cvDilate()膨胀，叫作开操作，那些离散点或游丝线、毛刺就被过滤
    	cvSmooth(img_contours , img_contours , CV_BLUR, 5, 5, 0, 0);


	CvMemStorage * storage=cvCreateMemStorage(0);
	CvSeq * contours = 0;
	cvFindContours(img_contours , storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	CvSeq    * current;
	current=contours;  
	while(current)
	{
		if(abs(cvContourArea(current, CV_WHOLE_SEQ)) >= 256 )
		{	
			CvRect r = cvBoundingRect(current,0); 
			//cvRectangle(img, cvPoint(r.x, r.y), cvPoint(r.x + r.width, r.y + r.height),CV_RGB(255,255, 255), 1, 8, 0); 
			p1 = cvPoint(r.x,r.y);
			p2 = cvPoint(r.x+r.width, r.y);
			p3 = cvPoint(r.x, r.y+r.height);
			p4 = cvPoint(r.x+r.width, r.y+r.height);
			if(rectandLine(ptx,p1,p2,p3,p4) )
			{
				CvFont font;
				cvInitFont(&font, 5, 1, 1, 0, 2);
				cvPutText(img, "Alarm, cross the line!", cvPoint(5,20), &font, cvScalar( 255, 0, 255 ));
			}
		}
		current = current->h_next;
	}
    
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&img_contours1);
	cvReleaseImage(&img_contours2);
	cvReleaseImage(&img_contours);

}


int findmotion(IplImage* img, IplImage* copyimg, IplImage* bg)
{
	int find_flag = 0;
	IplImage* img_contours = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, COLOR_CHN);
	IplImage* img_contours1 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, COLOR_CHN);
	IplImage* img_contours2 = cvCreateImage(cvSize(img->width,img->height),IPL_DEPTH_8U,COLOR_CHN);	
	IplImage* show1 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, COLOR_CHN);   
    
	cvSub(img, bg, img_contours1);
	cvSub(bg, img, img_contours2);
	cvAdd(img_contours1, img_contours2, img_contours);
	cvCopy(img_contours, img_contours1, 0);//contours1 used for find avg
	cvZero(img_contours2);//contours2 used for merge rects, avoid dividing a person to several parts
	cvThreshold(img_contours, img_contours, 40, 255,  CV_THRESH_BINARY);
	
	cvErode(img_contours, img_contours, NULL, 1);
	cvDilate(img_contours, img_contours, NULL, 1);
	//cvDilate(img_contours,img_contours,NULL,1);
	//rmShadow(img_contours);
    cvSmooth(img_contours, img_contours, CV_BLUR, 5, 5, 0, 0);
	CvMemStorage * storage=cvCreateMemStorage(0);
	CvSeq * contours = 0;
	cvFindContours(img_contours, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	CvSeq    * current;
	current=contours;
	
	cvZero(show1);

	while(current)
	{
		if(abs(cvContourArea(current, CV_WHOLE_SEQ)) >= 256 )
 			find_flag++;						
		current = current->h_next;
	}
    
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&img_contours1);
	cvReleaseImage(&img_contours2);
	cvReleaseImage(&img_contours);
	cvReleaseImage(&show1);

	return find_flag;
}
