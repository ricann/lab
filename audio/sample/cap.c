/*
This example reads from the default PCM device
and writes to standard output for 5 seconds of data.
*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

int main()
{
  char *pcm_name = "plughw:0,0";
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_stream_t stream = SND_PCM_STREAM_CAPTURE;

  unsigned int val;
  int dir = 0;
  int rc;
  char *data;
  long loops;

  unsigned int rate = 44100; /* hz */
  unsigned int exact_rate  = rate;/*Sample rate returned by snd_pcm_hw_params_set_rate_near*/

  //data, period, frame, sameple setting
  //period_frames must > periods, or set data size will error
  unsigned int periods = 8;
  snd_pcm_uframes_t period_frames = 128;
  int channels = 2;
  int sample_size = 2; /* bytes */
  snd_pcm_uframes_t period_size = period_frames * channels * sample_size;  /* bytes */
  int buf_frames = period_frames * periods;


  /* Open PCM device for recording (capture). */
  rc = snd_pcm_open(&handle, pcm_name, stream, 0);
  if (rc < 0) 
  {
      fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
      exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */
  /* Interleaved mode */
  rc = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (rc < 0) 
  {
    fprintf(stderr, "Error setting access.\n");
    return(-1);
  }

  /* Signed 16-bit little-endian format */
  rc = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
  if (rc < 0) 
  {
    fprintf(stderr, "Error setting format. \n");
    return(-1);
  }

  /* Two channels (stereo) */
  rc = snd_pcm_hw_params_set_channels(handle, params, channels);
  if (rc < 0) 
  {
    fprintf(stderr, "Error setting channels \n");
    return(-1);
  }

  /* 44100 hz/second sampling rate (CD quality) */
  rc = snd_pcm_hw_params_set_rate_near(handle, params, &exact_rate, &dir);
  if (rc < 0) 
  {
    fprintf(stderr, "Error setting rate. \n");
    return(-1);
  }
  if (rate != exact_rate)
  {
    fprintf(stderr, "The rate %d Hz is not supported by your hardware. \n"
      "==> Using %d Hz instead. \n", rate, exact_rate);
  }

  /*Set number of periods. Periods used to be called fragments*/
  rc = snd_pcm_hw_params_set_period_size_near(handle, params, &period_frames, &dir);
  if (rc < 0) 
  {
    fprintf(stderr, "Error setting periods. \n");
    return(-1);
  }

  /* Set buffer size (in frames). buffersize = period_size * periods * framesize =  bytes
  The resulting latency is given by latency = period_size * periods / (rate * bytes_per_frame)
  */
  rc = snd_pcm_hw_params_set_buffer_size(handle, params, buf_frames);
  if (rc < 0) 
  {
    fprintf(stderr, "Error setting buffersize. \n");
    return(-1);
  }

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) 
  {
      fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
      exit(1);
  }

  /* Use a data large enough to hold one period */
  data = (char *) malloc(period_size);
  if(!data)
  {
    fprintf(stderr, "memory allocated failed!\n");
    exit(1);
  }
  memset(data, 0, period_size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params, &val, &dir);
  //printf("period time = %d us\n", val);
  loops = 5000000 / val;

  while (loops > 0) 
  {
      loops--;
      rc = snd_pcm_readi(handle, data, period_frames);
      if (rc == -EPIPE) 
      {
        /* EPIPE means overrun */
        fprintf(stderr, "overrun occurred\n");
        snd_pcm_prepare(handle);
      } 
      else if (rc < 0) 
      {
        fprintf(stderr, "error from read: %s\n", snd_strerror(rc));
      } 
      else if (rc != (int)period_frames) 
      {
        fprintf(stderr, "short read, read %d period_frames\n", rc);
      }
      rc = write(1, data, period_size);
      if (rc != period_size)
        fprintf(stderr, "short write: wrote %d bytes\n", rc);
  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(data);

  return 0;
}