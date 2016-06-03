#ifndef __GAUSE_FILTER_H__
#define __GAUSE_FILTER_H__
#include "include/dataType.h"



#define GAUSE_FACTOR_FIXPOINT_BITS       5 
#define GAUSE_FACTOR_FIXPOINT_FACTOR(x)        ( (INT32)((x)*(1<<GAUSE_FACTOR_FIXPOINT_BITS)) )
//zty: #define GAUSE_NORM_FIXPOINT_BITS       17
//zty: #define GAUSE_NORM_FIXPOINT_FACTOR(x)        ( (INT32)((x)*(1<<GAUSE_NORM_FIXPOINT_BITS)) )
#define GAUSE_NORM_FIXPOINT_BITS0       17 
#define GAUSE_NORM_FIXPOINT_BITS1       18 
#define GAUSE_NORM_FIXPOINT_BITS2       19 
#define GAUSE_NORM_FIXPOINT_BITS3       20 
#define GAUSE_NORM_FIXPOINT_FACTOR0(x)        ( (INT32)((x)*(1<<GAUSE_NORM_FIXPOINT_BITS0)) )
#define GAUSE_NORM_FIXPOINT_FACTOR1(x)        ( (INT32)((x)*(1<<GAUSE_NORM_FIXPOINT_BITS1)) )
#define GAUSE_NORM_FIXPOINT_FACTOR2(x)        ( (INT32)((x)*(1<<GAUSE_NORM_FIXPOINT_BITS2)) )
#define GAUSE_NORM_FIXPOINT_FACTOR3(x)        ( (INT32)((x)*(1<<GAUSE_NORM_FIXPOINT_BITS3)) )
#define GAUSE_FIXPOINT_REVERT_S(fixval, shft)  ( ((fixval) + (1<<((shft)-1))) >> (shft) )


#define EPSILON_FIXPOINT_BITS  8
#define EPSILON_FIXPOINT_POINTBITS  2 //reduce hardware-bits when comparison with epsilon
#define EPSILON_FIXPOINT_FACTOR(x)    ((INT32)((x)*(1<<EPSILON_FIXPOINT_BITS)))
//zty: #define EPSILON_FIXPOINT_REVERT(fixval)   ( ((fixval) + (1<<(EPSILON_FIXPOINT_BITS-1))) >> EPSILON_FIXPOINT_BITS )

#define LUMI_FIXPOINT_BITS    2

typedef struct _GAUSE_PYRAMID_KERNELS
{
    INT32   width [PYRAMID_LEVELS];
    INT32   heightClipped;
    INT32   *factors[PYRAMID_LEVELS];
    INT32   normFactor[PYRAMID_LEVELS];
    INT32   epsilon;   //EPSILON_FIXPOINT_BITS
    INT32   pym_cc;
}GAUSE_PYRAMID_KERNELS;

INT32 genHwGauseKernel(INT32 level, INT32 *pFilterFactor, INT32 rows, INT32 cols, INT32 scale);
INT32 initGausePyramidKernels(GAUSE_PYRAMID_KERNELS *pKernels, WDR_PARAMETER *param);
INT32 destroyGausePyramidKernels(GAUSE_PYRAMID_KERNELS *pKernels);
INT32 gausePyramidFilter(PTYPE *pSrcBuf, PTYPE *pDstBuf, INT32 width, INT32 height, GAUSE_PYRAMID_KERNELS *pKernels, WDR_PARAMETER *param, INT32 outFixBits);


#endif
