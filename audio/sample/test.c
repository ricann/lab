
#include <alsa/asoundlib.h>

int main()
{
  /* Handle for the PCM device */
  snd_pcm_t  *pcm_handle;

  /*Playback stream*/
  snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

  /***************
  Thins structure sontaions informatin about
  the hardware and can be used to specify the
  configuration to be used for the PCM stream.
  ******************/
  snd_pcm_hw_params_t *hwparams;

  /* Name of the PCM device, like plughw:0,0
  The first number is the number of the soundcard,
  the second number is the number of the device.*/
  char *pcm_name;

  /* For this example, we assume that the soundcard can be configured
  for stereo playback of 16 Bit Little Endian data, sampled at
  Hz. Accordingly, we restrict the configuration space to 
  match this configuration */
  unsigned int rate = 44100; /* hz */
  unsigned int exact_rate ;/*Sample rate returned by snd_pcm_hw_params_set_rate_near*/

  //buffer, period, frame, sameple setting
  //period_frames must > periods, or set buffer size will error
  unsigned int periods = 8;
  snd_pcm_uframes_t period_frames = 1024;
  int channels = 2;
  int sample_size = 2; /* bytes */
  snd_pcm_uframes_t period_size = period_frames * channels * sample_size;  /* bytes */
  int buf_frames = period_frames * periods;

  /*
  exact_rate == rate, dir = 0
  exact_rate <  rate, dir = -1
  exact_rate >  rate, dir = 1 */
  int dir;
  
  /*Init pcm_name. Of course, later you 
  will make this configurable ;-)*/
  pcm_name = strdup("plughw:0,0");

  /*Open PCM. The last parameter of this function is the mode.
  If this is set to 0, the standard mode is used. Possible
  other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.
  If SND_PCM_NONBLOCK is used, read/write access to the PCM device
  will return immediately. If SND_PCM_ASYNC is specified, SIGIO 
  will be emitted whenever a period has been completely processed 
  by the soundcard.*/
  if (snd_pcm_open(&pcm_handle, pcm_name, stream, 0) < 0 )
  {
    fprintf(stderr, "Error opening PCM device %s\n", pcm_name);
    return(-1);
  }


  /* Allocate the snd_pcm_hw_params_t structure on the stack*/
  snd_pcm_hw_params_alloca(&hwparams);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(pcm_handle, hwparams);

  /*Set acccess type. */
  if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
  {
    fprintf(stderr, "Error setting access.\n");
    return(-1);
  }

  /*Set sample format */
  if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) <0)
  {
    fprintf(stderr, "Error setting format. \n");
    return(-1);
  }

  /* Set sample rate. If the exact rate is not supported
  by the hardware, use nearest possible rate.*/
  exact_rate = rate;
  if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0)
  {
    fprintf(stderr, "Error setting rate. \n");
    return(-1);
  }

  if (rate != exact_rate)
  {
    fprintf(stderr, "The rate %d Hz is not supported by your hardware. \n"
      "==> Using %d Hz instead. \n", rate, exact_rate);
  }

  /*Set number of channels*/
  if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, channels) < 0) 
  {
    fprintf(stderr, "Error setting channels \n");
    return(-1);
  }

  /*Set number of periods. Periods used to be called fragments*/
  if (snd_pcm_hw_params_set_periods_near(pcm_handle, hwparams, &periods, &dir) < 0) 
  {
    fprintf(stderr, "Error setting periods. \n");
    return(-1);
  }

  /* Set buffer size (in frames). buffersize = period_size * periods * framesize =  bytes
  The resulting latency is given by latency = period_size * periods / (rate * bytes_per_frame)
  */
  if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, buf_frames) < 0) 
  {
    fprintf(stderr, "Error setting buffersize. \n");
    return(-1);
  }

  /* Apply HW parameter settings to
  PCM device and prepare device*/
  if (snd_pcm_hw_params(pcm_handle, hwparams) < 0)
  {
    fprintf(stderr, "Error setting HW params. \n");
    return(-1);
  }

  //-----------Playback start here------------------------------
  /* We have to make sure that our application sends enough data
  to the soundcard buffer. Otherwise, a buffer underrun will
  occur. After such an underrun has occured, snd_pcm_prepare 
  should be called.*/

  unsigned char *data;
  int pcmreturn, l1, l2;
  short s1, s2;   //The PCM data

  data = (unsigned char *)malloc(period_size);
  if(!data)
  {
    fprintf(stderr, "memory allocated failed!\n");
    exit(1);
  }
  memset(data, 0, period_size);

  for(l1 = 0; l1 < 100; l1++)
  {                            //creates PCM data for 100 cycles
    for(l2 = 0; l2 < period_frames; l2++) 
    {
      s1 = (l2 % 128) * 100 - 5000;           //creates -5000 < s1 < 7800
      s2 = (l2 % 256) * 100 - 5000;           //creates -5000 < s2 < 20600
      data[4*12] = (unsigned char)s1;         //data format ::= F1(s1[0..7]s1[8..15]s2[1..7]s2[8..15])F2(...)...F2048(...)
      data[4*12+1] = s1 >> 8;
      data[4*12+2] = (unsigned char)s2;
      data[4*12+3] = s2 >> 8;
    }

    while ((pcmreturn = snd_pcm_writei(pcm_handle, data, period_frames)) < 0)
    {
      snd_pcm_prepare(pcm_handle);
      fprintf(stderr, "<<<<<<<<<Buffer Underrun >>>>>>>>>>");
    }
  }

  return 1;
}