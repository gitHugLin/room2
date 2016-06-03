#ifndef __TONE_MAP_H__
#define __TONE_MAP_H__

#define FIXPOINT_REVERT(fixval, fixbits)   ( ((fixval) + (1<<((fixbits)-1))) >> (fixbits) )
#define FIXPOINT_CLIP(fixval, fixbits)	(fixval >> fixbits)

#define TONE_GAIN_FIXPOINT_BITS 14
#define TONE_GAIN_FIXPOINT_FACTOR(x)    ((INT32)((x)*(1<<TONE_GAIN_FIXPOINT_BITS)))
#define TONE_GAIN_FIXPOINT_REVERT(fixval)   ( ((fixval) + (1<<(TONE_GAIN_FIXPOINT_BITS-1))) >> TONE_GAIN_FIXPOINT_BITS )

#define RATIO_FIXPOINT_BITS  6
#define RATIO_FIXPOINT_FACTOR(x)    ((INT32)((x)*(1<<RATIO_FIXPOINT_BITS)))
#define RATIO_FIXPOINT_REVERT(fixval)   ( ((fixval) + (1<<(RATIO_FIXPOINT_BITS-1))) >> RATIO_FIXPOINT_BITS )

#define USING_GAIN_LUT    0   // //1: Tone     0:Tone

//#define Y_CHANNEL_BITS 10   //Y Channel bitdepth default:10 //not used

#define R2Y_COE_FIXPOINT_BITS 8
//2nd version on 20160308
#define TONE_RADIUS_FIXPOINT_BITS 1
#define TONE_RADIUS_FIXPOINT_FACTOR(x)     ((INT32)((x)*(1<<TONE_RADIUS_FIXPOINT_BITS)))
#define TONE_RADIUS_FIXPOINT_REVERT(fixval)    ( ((fixval) + (1<<(TONE_RADIUS_FIXPOINT_BITS-1))) >> TONE_RADIUS_FIXPOINT_BITS)

#define TONE_WEIGHT_FIXPOINT_BITS 9    // TONE_MAP_BLK_SIZEBITS + TONE_RADIUS_FIXPOINT_BITS = 8+1
#define TONE_WEIGHT_FIXPOINT_FACTOR(x)    ((INT32)((x)*(1<<TONE_WEIGHT_FIXPOINT_BITS)))
#define TONE_WEIGHT_FIXPOINT_REVERT(fixval)  ( ((fixval) + (1<<(TONE_WEIGHT_FIXPOINT_BITS-1))) >> TONE_WEIGHT_FIXPOINT_BITS )

#define TONE_BLK_FIXPOINT_BITS 12
#define TONE_BLK_FIXPOINT_FACTOR(x)    ((INT32)((x)*(1<<TONE_BLK_FIXPOINT_BITS)))
#define TONE_BLK_FIXPOIINT_REVERT(fixval)    ( ((fixval) + (1<<(TONE_BLK_FIXPOINT_BITS-1))) >> TONE_BLK_FIXPOINT_BITS )

typedef struct _TONE_CURVE
{
    INT32  xBitDepth;
    INT32  dxBitDepth;
    INT32  bDec; //0: increase 1:decrease 
    INT8   xDeltN[TONE_CURVE_SEG_COUNT];
    INT32  y[TONE_CURVE_SEG_COUNT+1];    //TONE_GAIN_FIXPOINT_BITS
    INT32  noiseRatio;
    INT32  gainOffset;
    INT32  rgbOffset;
}TONE_CURVE;


PTYPE getLumiChannel(PTYPE **ppChannelBuff, PTYPE *pRetLumiBuff, INT32 width, INT32 height);
INT32 initToneCurve(TONE_CURVE *pCurve,  WDR_PARAMETER *param);
INT32 toneCurveTransfer(PTYPE **ppChannelBuff, PTYPE *pLumiBuff, INT32 width, INT32 height, INT32 bitdepth, TONE_CURVE *pCurve, INT32 inFixBits);

//2nd version on 20160311
PTYPE lumiFixpoints(PTYPE *pLumiBuff, PTYPE *pLmLpBuff, INT32 width, INT32 height, INT32 outFixbits);
PTYPE getAvgLumiChannel(PTYPE **ppChannelBuff, PTYPE *pAvgLumiBuff, WDR_PARAMETER *param, INT32 width, INT32 height);
INT32 getBlockLumi(PTYPE *pAvgLumiBuff, INT32 *blkLumiBuff, INT32 width, INT32 height, INT32 blkWidth, INT32 blkHeight, WDR_PARAMETER *param);
INT32 blkMeansGain(INT32 blkLumi, PTYPE lpLumi, INT32 gainOffset1, INT32 gainOffset2);
INT32 blockCenterIndexUL(INT32 x, INT32 blkCenter, INT32 blkRadius);
INT32 toneMapping(PTYPE **ppChannelBuff, INT32 *blkLumiMeanBuff, PTYPE *pLmLpBuff, INT32 width, INT32 height, INT32 bitdepth, WDR_PARAMETER *param, INT8 blkWidth, INT8 blkHeight);

#endif
