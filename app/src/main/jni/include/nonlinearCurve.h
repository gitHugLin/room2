#ifndef __NONLINEAR_CURVE_H__
#define __NONLINEAR_CURVE_H__

#define NONLIN_SEG_COUNT 16
#define FIXPOINT_REVERT(fixval, fixbits)   ( ((fixval) + (1<<((fixbits)-1))) >> (fixbits) )
#define FIXPOINT_CLIP(fixval, fixbits)	(fixval >> fixbits)

typedef struct _NONLINEAR_CURVE
{
	//INT32 xBitDepth;
	INT8 dxBitDepth[NONLIN_SEG_COUNT]; // interval bitdepth
	INT32 y[NONLIN_SEG_COUNT+1];
}NONLINEAR_CURVE;

INT32 initNonlinearCurve(NONLINEAR_CURVE *pCurve, WDR_PARAMETER *param);
INT32 nonlinearCurveLut(NONLINEAR_CURVE *pCurve, PTYPE lumiVal, INT32 bitdepth);
INT32 nonlinearCurveTransfer(PTYPE *pLumiChannelBuff, INT32 width, INT32 height, INT32 bitdepth, NONLINEAR_CURVE *pCurve);
INT32 renameDatFile(INT8 *pDstName, INT8 *pSrcName);
//INT32 writeNonlinearOutData(PTYPE *pDataBuff, INT32 width, INT32 height, INT8 *pOutFileName, INT32 bitdepth);
#endif
