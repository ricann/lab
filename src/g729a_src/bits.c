/*****************************************************************************/
/* bit stream manipulation routines                                          */
/*****************************************************************************/
#include "typedef.h"
#include "ld8a.h"
#include "tab_ld8a.h"

/* prototypes for local functions */
static void bit2byte(Word16 para,int bitlen,unsigned char * bits,int bitpos); 
static Word16 byte2bit(int bitlen,unsigned char * bits,int bitpos);

/*----------------------------------------------------------------------------
 * prm2bits_ld8k -converts encoder parameter vector into vector of serial bits
 * bits2prm_ld8k - converts serial received bits to  encoder parameter vector
 *
 * The transmitted parameters are:
 *
 *     LPC:     1st codebook           7+1 bit
 *              2nd codebook           5+5 bit
 *
 *     1st subframe:
 *          pitch period                 8 bit
 *          parity check on 1st period   1 bit
 *          codebook index1 (positions) 13 bit
 *          codebook index2 (signs)      4 bit
 *          pitch and codebook gains   4+3 bit
 *
 *     2nd subframe:
 *          pitch period (relative)      5 bit
 *          codebook index1 (positions) 13 bit
 *          codebook index2 (signs)      4 bit
 *          pitch and codebook gains   4+3 bit
 *----------------------------------------------------------------------------
 */
void prm2bits_ld8k(Word16 *para,unsigned char *bits)
{
  int i;
  int bitpos = 0;
  for (i = 0;i < PRM_SIZE; i++) {
    bit2byte(*para++, bitsno[i],bits,bitpos);
    bitpos += bitsno[i];
  }
}

/*----------------------------------------------------------------------------
 *  bits2prm_ld8k - converts serial received bits to  encoder parameter vector
 *----------------------------------------------------------------------------
 */
void bits2prm_ld8k(unsigned char *bits,Word16 *para)
{
  int i;
  int bitpos = 0;
  for (i = 0;i<PRM_SIZE;i++) {
    *para++ = byte2bit(bitsno[i],bits,bitpos);
    bitpos += bitsno[i];
  }
}

void bit2byte(Word16 para,int bitlen,unsigned char * bits,int bitpos)
{
  int i;
  int bit = 0;
  unsigned char newbyte = 0;
  
  unsigned char *p = bits + (bitpos / 8);
  for (i = 0 ;i < bitlen; i++) {
    bit = (para >> (bitlen - i -1) ) &0x01;
    newbyte = (1 << (7 - bitpos % 8));
    if (bit == 1)
      *p |= newbyte;
    else
      *p &= ~newbyte;
    bitpos++;
    if (bitpos % 8 == 0)
      p++;
  }
}

Word16 byte2bit(int bitlen,unsigned char * bits,int bitpos)
{
  int i;
  int bit = 0;
  Word16 newbyte = 0;
  Word16 value = 0;
  
  unsigned char *p = bits + (bitpos / 8);
  for (i = 0 ;i < bitlen; i++) {
    bit = (*p >> (7 - bitpos % 8)) &0x01;
    if (bit == 1) {
      newbyte = (1 << (bitlen - i -1));
      value |= newbyte;
    }
    bitpos++;
    if (bitpos % 8 == 0)
      p++;
  }
  return value;
}

