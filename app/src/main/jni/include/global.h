#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

//#define WDR_LOAD_DATA_EN //zty add for debug
//#define WDR_NONlINEAER_OUT_EN //added for debugging
//#define WDR_R2Y_OUT_EN 
//#define WDR_BLKLUMI_OUT_EN
//#define WDR_BLKGAIN_OUT_EN

//#define WIN32
#define MAX_FILENAME_LEN                 128

#define MAX_UINT10                       ((1 << 10)-1)
#define MAX_UINT8                        ((1 << 8)-1)

#define CLIP(a, min, max)			    (((a) < (min)) ? (min) : (((a) > (max)) ? (max) : (a)))
#define ABS(A)                          ( (A) > 0 ? (A) : -(A) )
#define MIN(a,b)                        ((a) <= (b) ? (a):(b))
#define MAX(a,b)                        ((a) >= (b) ? (a):(b))
#define SWAP(_T_,A,B)                   { _T_ tmp = (A); (A) = (B); (B) = tmp; }

#define DIV_ROUND(x, n)                 (((x) + (n)/2)/(n))
#define ROUND_F(x)                      (int)((x)+((x) > 0 ? 0.5 : -0.5))
#define ROUND_I(_T_, x, scale_f)        (int)(((_T_)(x) + (1<<((scale_f)-1)))>>(scale_f))
#define ADD1(x)                         ((x)+1)
#define BIT_SET(val, i, bin)	        ((val) = (val) | ((bin) << (i)))
#define BIT_TEST(val, i)		        (((val) >> (i))&0x1)

#define BIT_MASK(i)               ((1 << (i)) - 1)
#define BITS_EXTRACT(src, offset, count)  (((src) >> (offset)) & (BIT_MASK(count)))  //little endian
#define BITS_SET(dst, val, offset, count) ((dst)|(((val) & (BIT_MASK(count))) << (offset)))
#define BITS_REVERT(src, mask)            ((src) = (~(src))&(mask))                    //extract some bits and revert, clear other bits


#define MALLOC(ptr, type, size)               do{(ptr) = (type *)malloc((sizeof(type))*(size)); \
                                                if((ptr) == NULL){ printf("Malloc Fail!\n"); exit(1);} \
                                                memset(ptr, 0, (sizeof(type))*(size));\
                                              }while(0);
#define FREE(ptr)                           do{\
                                                if((ptr) != NULL){ free(ptr); (ptr) = NULL;}\
                                            }while(0);
#ifdef WIN32
#define UINT64 unsigned __int64
#define INT64  __int64
#define EXIT(ret)   system("pause")//exit(ret)
#else
#define mUINT64 unsigned long long
#define mINT64 long long
#define EXIT(ret)  exit(ret)
#endif

#define MAX_ALPHA_VAL   255

#define RGB565_B_SHIFT  0
#define RGB565_G_SHIFT  5
#define RGB565_R_SHIFT  11

#define RGB666_B_SHIFT  0
#define RGB666_G_SHIFT  6
#define RGB666_R_SHIFT  12

#define RGB888_B_SHIFT  0
#define RGB888_G_SHIFT  8
#define RGB888_R_SHIFT  16
#define ARGB888_A_SHIFT  24

#define RGB101010_B_SHIFT  0
#define RGB101010_G_SHIFT  10
#define RGB101010_R_SHIFT  20


#define RGB565_GET_B5(RGB565Val)  (((RGB565Val) >> RGB565_B_SHIFT) & 0x1F)
#define RGB565_GET_G6(RGB565Val)  (((RGB565Val) >> RGB565_G_SHIFT) & 0x3F)
#define RGB565_GET_R5(RGB565Val)  (((RGB565Val) >> RGB565_R_SHIFT) & 0x1F)

#define RGB888_GET_B8(RGB888Val)  (((RGB888Val) >> RGB888_B_SHIFT) & 0xFF)
#define RGB888_GET_G8(RGB888Val)  (((RGB888Val) >> RGB888_G_SHIFT) & 0xFF)
#define RGB888_GET_R8(RGB888Val)  (((RGB888Val) >> RGB888_R_SHIFT) & 0xFF)


#define GET_RGB565(R5, G6, B5)  ((((R5)&0x1F)<<RGB565_R_SHIFT)|(((G6)&0x3F)<<RGB565_G_SHIFT)|(((B5)&0x1F)<<RGB565_B_SHIFT))
#define GET_RGB666(R6, G6, B6)  ((((R6)&0x3F)<<RGB666_R_SHIFT)|(((G6)&0x3F)<<RGB666_G_SHIFT)|(((B6)&0x3F)<<RGB666_B_SHIFT))
#define GET_RGB888(R8, G8, B8)  ((((R8)&0xFF)<<RGB888_R_SHIFT)|(((G8)&0xFF)<<RGB888_G_SHIFT)|(((B8)&0xFF)<<RGB888_B_SHIFT))
#define GET_RGB101010(R10, G10, B10)  ((((R10)&0x3FF)<<RGB101010_R_SHIFT)|(((G10)&0x3FF)<<RGB101010_G_SHIFT)|(((B10)&0x3FF)<<RGB101010_B_SHIFT))
#define GET_ARGB888(A8, R8, G8, B8)  ((((A8)&0xFF)<<ARGB888_A_SHIFT)|(((R8)&0xFF)<<RGB888_R_SHIFT)|(((G8)&0xFF)<<RGB888_G_SHIFT)|(((B8)&0xFF)<<RGB888_B_SHIFT))

/****************************************************************/
//version list:

#define VERSION_CURRENT 0


#endif
