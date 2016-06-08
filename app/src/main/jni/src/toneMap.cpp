#include "include/global.h"
#include "include/dataType.h"
#include "include/regConfig.h"
#include "log.h"
#include "include/toneMap.h"
#include "include/nonlinearCurve.h"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


static double work_begin = 0;
static double work_end = 0;
static double gTime = 0;
//开始计时
static void workEnd(char *tag = "TimeCounts" );
static void workBegin();
static void workBegin()
{
    work_begin = getTickCount();
}
//结束计时
static void workEnd(char *tag)
{
    work_end = getTickCount() - work_begin;
    gTime = work_end /((double)getTickFrequency() )* 1000.0;
    LOGE("[TAG: %s ]:TIME = %lf ms \n",tag,gTime);
}

// turning lpLumi into 14 bits

PTYPE lumiFixpoints(PTYPE *pLumiBuff, PTYPE *pLmLpBuff, INT32 width, INT32 height, INT32 outFixbits)
{
	INT32 x, y, offset;
	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			offset = y*width + x;
			pLmLpBuff[offset] = pLumiBuff[offset] << outFixbits;
		}
	}
	return 0;
}

PTYPE getAvgLumiChannel(PTYPE **ppChannelBuff,PTYPE *pAvgLumiBuff,
                WDR_PARAMETER *param, INT32 width, INT32 height)
 {
	INT32 x, y, offset;
	PTYPE channelR, channelG, channelB;
	INT16 coeChannelR, coeChannelG, coeChannelB;
	INT32 coeOffset;
	INT32 lumi;
	UINT32 avgL = 0;
	coeChannelR = param->sw_wdr_coe_00;
	coeChannelG = param->sw_wdr_coe_01;
	coeChannelB = param->sw_wdr_coe_02;
	coeOffset   = param->sw_wdr_coe_off;
	
	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x ++)
		{
			offset = y * width + x;
			channelR = ppChannelBuff[CHANNEL_RY][offset];
			channelG = ppChannelBuff[CHANNEL_GCb][offset];
			channelB = ppChannelBuff[CHANNEL_BCr][offset];

			lumi = coeChannelR * channelR + coeChannelG * channelG + coeChannelB * channelB + coeOffset;
			//lumi >> 8 (lumi/256)
			lumi = FIXPOINT_CLIP(lumi, R2Y_COE_FIXPOINT_BITS);
			lumi = MIN(lumi, 0xfff);

			pAvgLumiBuff[offset] = lumi;
			avgL += pAvgLumiBuff[offset];
		}
	}
	avgL = avgL/(width*height);
	return (PTYPE)avgL;
}

//INT32 initBlockLumi(PTYPE *pAvgLumiBuff, PTYPE *blkLumiBuff, INT32 width, INT32 height, INT32 blkWidth, INT32 blkHeight)
INT32 getBlockLumi(PTYPE *pAvgLumiBuff, INT32 *blkLumiBuff,
                INT32 width, INT32 height, INT32 blkWidth, INT32 blkHeight, WDR_PARAMETER *param)
{
	INT32 x, y, offset;
	INT8 blkX, blkY, blkOffset;
	INT32 xRest, yRest,curAvgLumiVal;
	// G_wdr_para.sw_wdr_bestlight   = 0x0cf0//3321
    // G_wdr_para.sw_wdr_noiseratio  = 0x00c0//192
	INT32 noiseRatio = ((param->sw_wdr_noiseratio)>>4)<<4; //clip low 4bits, 20160330
	INT32 bestLight  = ((param->sw_wdr_bestlight )>>4)<<4; //clip low 4bits, 20160330

	//blkRow = height >> TONE_MAP_BLK_SIZEBITS;
	//blkCol = width >> TONE_MAP_BLK_SIZEBITS;
	xRest = (blkWidth << TONE_MAP_BLK_SIZEBITS) - width;
	yRest = (blkHeight << TONE_MAP_BLK_SIZEBITS) - height;

	for( y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {

			offset = y * width + x;
			blkX = x >> TONE_MAP_BLK_SIZEBITS;
			blkY = y >> TONE_MAP_BLK_SIZEBITS;
			blkOffset = blkY * blkWidth + blkX;

	        curAvgLumiVal = (pAvgLumiBuff[offset]>>2)<<2; //pAvgLumiBuff[offset], update for nonl_mode1=1(closed), 20160420
		    blkLumiBuff[blkOffset] += curAvgLumiVal;

	        if(x == width-1) {
	            blkLumiBuff[blkOffset] += xRest * curAvgLumiVal;
		    }
		    if(y == height-1) {
			    blkLumiBuff[blkOffset] += yRest * curAvgLumiVal;
	        }
	        if(x == width-1 && y == height-1) {
		        blkLumiBuff[blkWidth*blkHeight-1] += xRest*yRest * curAvgLumiVal;
		    }
		}
	}

	for(blkY = 0; blkY < blkHeight; blkY++)
	{
		for(blkX = 0; blkX < blkWidth; blkX ++)
		{
			blkOffset = blkY * blkWidth + blkX;
			blkLumiBuff[blkOffset] = (blkLumiBuff[blkOffset]>> TONE_MAP_BLK_SIZEBITS) >> TONE_MAP_BLK_SIZEBITS; //blockSumLumi/blkSize/blkSize
			blkLumiBuff[blkOffset] = (blkLumiBuff[blkOffset]>>2)<<2; //gain clip into 10bits, 20160326
			if((param->sw_wdr_bavg_clip)&0x1)
				blkLumiBuff[blkOffset] = CLIP(blkLumiBuff[blkOffset], noiseRatio,0xffffffff);
            if(((param->sw_wdr_bavg_clip)&0x2)>>1)
				blkLumiBuff[blkOffset] = CLIP(blkLumiBuff[blkOffset], 0x0, bestLight);
			LOGD("%d ",blkLumiBuff[blkOffset]);
		}
		LOGD("\n");
	}
	
	return 0;
}


INT32 blkMeansGain(INT32 blkLumi, PTYPE lpLumi, INT32 gainOffset1, INT32 gainOffset2)
{
	INT32 lumiGain;
	INT32 newLumi;
	gainOffset1 = ((gainOffset1)>>4)<<4;
	gainOffset2 = ((gainOffset2)>>4)<<4;
	newLumi =  (TONE_BLK_FIXPOINT_FACTOR(1) + (blkLumi*lpLumi >> TONE_BLK_FIXPOINT_BITS)  + gainOffset1);
	lumiGain = TONE_GAIN_FIXPOINT_FACTOR(newLumi)/(blkLumi + lpLumi + gainOffset2);

	lumiGain = MIN(lumiGain, 0x3ffff); //gain 18bit
	return lumiGain;
}

INT32 blockCenterIndexUL(INT32 x, INT32 blkCenter, INT32 blkRadius)
{

	INT32 blkSizeOffset;
	INT32 blkIndex;

	//x = TONE_RADIUS_FIXPOINT_FACTOR(x);
	blkSizeOffset = x - blkCenter;//blkCenter = 255

	if (blkSizeOffset < 0)
		blkIndex = -(blkRadius+1);
	else
	{
	    //TONE_WEIGHT_FIXPOINT_BITS = 9
		blkSizeOffset = blkSizeOffset>>TONE_WEIGHT_FIXPOINT_BITS;
		blkIndex = (blkSizeOffset<<TONE_WEIGHT_FIXPOINT_BITS) + blkCenter;
	}

	//blkIndex = blkIndex + TONE_RADIUS_FIXPOINT_FACTOR(TONE_MAP_BLK_SIZE);
	
	return blkIndex;
}

INT32 toneMapping(PTYPE **ppChannelBuff, INT32 *blkLumiMeanBuff,
            PTYPE *pLmLpBuff, INT32 width, INT32 height, INT32 bitdepth,
                                WDR_PARAMETER *param, INT8 blkWidth, INT8 blkHeight)
{
	INT32 x, y, offset;
	INT32 row, col;
	INT32 channelR, channelG, channelB;
	INT32 channelOR, channelOG, channelOB;
	INT32 gainOffset1, gainOffset2;
	INT32 gain;
	LONG  oldGain;
	INT32 rgbOffset = param->sw_rgbOffset;//0
	//INT32 bitdepth = param->sw_img_depth;
	FILE *fp = NULL;

	INT32 blkRadius, blkCenter;
	INT32 blkIndexUY, blkIndexLX, blkIndexDY, blkIndexRX;
	INT32 blkDWeightX, blkDWeightY;
	LONG  blkDWeightUL, blkDWeightUR, blkDWeightDL, blkDWeightDR;//64bit
	INT32 blkLumiUL, blkLumiUR, blkLumiDL, blkLumiDR;
	INT32 blkMapUL, blkMapUR, blkMapDL, blkMapDR;
	INT32 gainMax = ((param->sw_wdr_gain_max)>>10)<<10; //sw_wdr_gain_max = 0xffff; gainMax = 60
	PTYPE lpLumi;
	INT8  inFixBits;
	//blkHeight = param->sw_block_height;
	//blkWidth = param->sw_block_width;
	inFixBits   = 2;
	gainOffset1 = param->sw_wdr_gain_off1;//0
	gainOffset2 = param->sw_wdr_gain_off2;//410

	blkRadius = TONE_MAP_BLK_SIZE; //256
	blkCenter = (TONE_MAP_BLK_SIZE>>1)-1 + (TONE_MAP_BLK_SIZE>>1); //xCenter,yCenter = TONE_RADIUS_FIXPOINT_FACTOR( (blkRadius-1 + blkRadius)/2 )

    int round = 0;
    bool r = true;
	//get coordinates of four adjacent blocks
	workBegin();
	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
		if( y == 0)
		workBegin();
			offset = y * width + x;
			channelR = ppChannelBuff[CHANNEL_RY][offset];
            channelG = ppChannelBuff[CHANNEL_GCb][offset];
            channelB = ppChannelBuff[CHANNEL_BCr][offset];
			lpLumi = pLmLpBuff[offset] >> inFixBits; //from 14bit in gaussian pyramid to 12bit

			row = TONE_RADIUS_FIXPOINT_FACTOR(y);   //y*2
			col = TONE_RADIUS_FIXPOINT_FACTOR(x);   //x*2
			
			// block center coordinates
			blkIndexUY = blockCenterIndexUL(row, blkCenter, blkRadius);
			blkIndexLX = blockCenterIndexUL(col, blkCenter, blkRadius);
			blkIndexDY = blkIndexUY + TONE_RADIUS_FIXPOINT_FACTOR(TONE_MAP_BLK_SIZE);
			blkIndexRX = blkIndexLX + TONE_RADIUS_FIXPOINT_FACTOR(TONE_MAP_BLK_SIZE);

			//find out distance between current pixel and each block;
			blkDWeightY = row - blkIndexUY; //distance from upleft = y(or x) - blockCenter_upleft
			blkDWeightX = col - blkIndexLX;

		    //calculate weight factors of these four blocks (8bit * 8bit)
		    blkDWeightUL = (TONE_WEIGHT_FIXPOINT_FACTOR(1) - blkDWeightY)*(TONE_WEIGHT_FIXPOINT_FACTOR(1) - blkDWeightX);
		    blkDWeightUR = (TONE_WEIGHT_FIXPOINT_FACTOR(1) - blkDWeightY)*(blkDWeightX);
		    blkDWeightDL = (blkDWeightY)*(TONE_WEIGHT_FIXPOINT_FACTOR(1) - blkDWeightX);
	        blkDWeightDR = (blkDWeightY)*(blkDWeightX);

			// block index for finding out corresponding block lumi average
			blkIndexUY = MAX(blkCenter, blkIndexUY) >> TONE_WEIGHT_FIXPOINT_BITS; //boundary clip to get average lumi of each block
			blkIndexLX = MAX(blkCenter, blkIndexLX) >> TONE_WEIGHT_FIXPOINT_BITS;
			blkIndexDY = MIN(((blkHeight<<TONE_WEIGHT_FIXPOINT_BITS) - blkCenter), blkIndexDY) >> TONE_WEIGHT_FIXPOINT_BITS;
			blkIndexRX = MIN(((blkWidth<<TONE_WEIGHT_FIXPOINT_BITS) - blkCenter), blkIndexRX) >> TONE_WEIGHT_FIXPOINT_BITS;
			
			//get average luminance of each block( or use data from last frame)
//workBegin();
			blkLumiUL = blkLumiMeanBuff[blkIndexUY*blkWidth + blkIndexLX];
			blkLumiUR = blkLumiMeanBuff[blkIndexUY*blkWidth + blkIndexRX];
			blkLumiDL = blkLumiMeanBuff[blkIndexDY*blkWidth + blkIndexLX];
			blkLumiDR = blkLumiMeanBuff[blkIndexDY*blkWidth + blkIndexRX];
//workEnd("blkLumiMeanBuff TIME COUNT");
			//figure out gain = (1 + blkLumi*lpLumi + gainOffset1)/(blkLumi + lpLumi + gainOffset2)18bit
//workBegin();
			blkMapUL = blkMeansGain(blkLumiUL, lpLumi, gainOffset1, gainOffset2);
			blkMapUR = blkMeansGain(blkLumiUR, lpLumi, gainOffset1, gainOffset2);
			blkMapDL = blkMeansGain(blkLumiDL, lpLumi, gainOffset1, gainOffset2);
			blkMapDR = blkMeansGain(blkLumiDR, lpLumi, gainOffset1, gainOffset2);
//workEnd("blkMeansGain TIME COUNT");
		    oldGain = ((blkDWeightUL*blkMapUL)>>18) + ((blkDWeightUR*blkMapUR)>>18) + ((blkDWeightDL*blkMapDL)>>18) + ((blkDWeightDR*blkMapDR)>>18);
			gain = (INT32)oldGain;
			//if(param->sw_wdr_gain_max_en)//sw_wdr_gain_max_en = 0
				//gain = MIN(gain, gainMax);

			//get current pixel's luminance after tone mapping
			channelOR = MIN(FIXPOINT_REVERT((channelR+rgbOffset)*gain, TONE_GAIN_FIXPOINT_BITS), BIT_MASK(bitdepth-2));
            channelOG = MIN(FIXPOINT_REVERT((channelG+rgbOffset)*gain, TONE_GAIN_FIXPOINT_BITS), BIT_MASK(bitdepth-2));
            channelOB = MIN(FIXPOINT_REVERT((channelB+rgbOffset)*gain, TONE_GAIN_FIXPOINT_BITS), BIT_MASK(bitdepth-2));
            ppChannelBuff[CHANNEL_RY][offset]  = channelOR;
            ppChannelBuff[CHANNEL_GCb][offset] = channelOG;
            ppChannelBuff[CHANNEL_BCr][offset] = channelOB;
if( y == 0)
workEnd("toneMapping TIME COUNT");
		}

	}


	return bitdepth;
}

//return average Lumi
//get the max chanel from RGB channel
PTYPE getLumiChannel(PTYPE **ppChannelBuff, PTYPE *pRetLumiBuff, INT32 width, INT32 height)
{
    INT32 x, y, offset;
    PTYPE channelR, channelG, channelB;
    UINT32 avgL = 0;
    //UINT32 maxL = 0;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            offset = y * width + x;
            channelR = ppChannelBuff[CHANNEL_RY][offset];
            channelG = ppChannelBuff[CHANNEL_GCb][offset];
            channelB = ppChannelBuff[CHANNEL_BCr][offset];

            pRetLumiBuff[offset] = MAX(MAX(channelR, channelG), channelB);
            avgL += pRetLumiBuff[offset];
            //if(maxL < pRetLumiBuff[offset])
                //maxL = pRetLumiBuff[offset];
        }
    }
    avgL = avgL/(width*height);
    
    return (PTYPE)avgL;
}

//see curvePlot.m
INT32 initToneCurve(TONE_CURVE *pCurve,  WDR_PARAMETER *param)
{
    //method1:get from file
    FILE *fp = fopen((char *)param->sw_lutfile_name, "rb");
    fread(pCurve->xDeltN, sizeof(INT8), TONE_CURVE_SEG_COUNT, fp);
    fread(pCurve->y, sizeof(INT32), TONE_CURVE_SEG_COUNT+1, fp);
    fclose(fp);
    /*
    //method2:get from reg config
    INT32 i;
    for(i = 0; i <TONE_CURVE_SEG_COUNT; i++)
    {
        pCurve->xDeltN[i] = param->sw_tonecurve_deltxbit[i];
        pCurve->y[i] = param->sw_tonecurve_y[i];
    }
    //method3: calculate directly from avgLumi[last frame]
    //TODO
    */    
    pCurve->xBitDepth = param->sw_tonecurve_xBits;
    pCurve->dxBitDepth = param->sw_tonecurve_dxBits;
    pCurve->bDec = (pCurve->y[TONE_CURVE_SEG_COUNT-1] < pCurve->y[0]);
    pCurve->noiseRatio = param->sw_noiseRatio;
    pCurve->gainOffset = param->sw_gainOffset;
    pCurve->rgbOffset = param->sw_rgbOffset;
    return 0;
}

//get gain or newLumi from Lut
INT32 toneCurveLut(TONE_CURVE *pCurve, PTYPE lumiVal, INT32 bitdepth)
{
    INT32 i, ret = -1;
    INT32 cumsumX = 0;      //end of current seg 
    INT32 cumsumXlast = 0;  //start of current seg
    INT32 xBitshift = (bitdepth - pCurve->xBitDepth);

    //convert to x axis(0~1 fixedPointShift)
    if(pCurve->xBitDepth > bitdepth)
    {
        LOGE("Error in toneCurveLut");
        return -1;
    }

    for(i = 0; i <TONE_CURVE_SEG_COUNT; i++)
    {
        cumsumX += (1<< (pCurve->xDeltN[i] + pCurve->dxBitDepth +  xBitshift));
        if(cumsumX >= lumiVal)
        {
            ret = (lumiVal - cumsumXlast)* pCurve->y[i+1] + (cumsumX -lumiVal)* pCurve->y[i]; 
            ret = FIXPOINT_REVERT(ret, (pCurve->xDeltN[i] + pCurve->dxBitDepth + xBitshift)) ;
            break;
        }
        cumsumXlast = cumsumX;
    }
    if(ret == -1)
        ret = pCurve->y[TONE_CURVE_SEG_COUNT];

    return ret;
}


INT32 toneCurveTransfer(PTYPE **ppChannelBuff, PTYPE *pLumiBuff, INT32 width, INT32 height, INT32 bitdepth, TONE_CURVE *pCurve, INT32 inFixBits)
{
    INT32 x, y, offset;
    INT32 channelR , channelG , channelB ;
    INT32 channelOR, channelOG, channelOB;
    INT32 gain;
  //INT32 rgbOffset = pCurve->rgbOffset << inFixBits;
	INT32 rgbOffset = pCurve->rgbOffset;  //channelR/G/B 12bits, zty fix 20160215
#if USING_GAIN_LUT==0
    INT32 lumi, newLumi;
    INT32 gainOffset = pCurve->gainOffset << inFixBits; //(channelR/G/B 12bits)*4
#endif    
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            offset = y * width + x;
            channelR = ppChannelBuff[CHANNEL_RY][offset];
            channelG = ppChannelBuff[CHANNEL_GCb][offset];
            channelB = ppChannelBuff[CHANNEL_BCr][offset];

            //\u9632\u6b62\u4f4e\u5149\u88ab\u8fc7\u5ea6\u589e\u5f3a
            //pLumiBuff[offset] = MAX(pLumiBuff[offset], NOISERATIO_FIXPOINT_REVERT(pCurve->noiseRatio * BIT_MASK(bitdepth)));

#if USING_GAIN_LUT==1
            gain = toneCurveLut(pCurve, pLumiBuff[offset], bitdepth);
#else
            lumi = pLumiBuff[offset]; //(channelR/G/B 12bits)*4 already
            newLumi = toneCurveLut(pCurve, lumi, bitdepth);
            //zty: gain = (TONE_GAIN_FIXPOINT_FACTOR(newLumi)+lumi/2)/(lumi+0.000001f);//(gainOffset<<inFixBits)
            gain = TONE_GAIN_FIXPOINT_FACTOR(newLumi)/(lumi+gainOffset); //not need round, and +gainOffset
            gain = MIN(gain,0x3ffff); //gain[17:14]<=0xf;  zty fix 20160130
#endif
            channelOR = MIN(FIXPOINT_REVERT((channelR+rgbOffset)*gain, TONE_GAIN_FIXPOINT_BITS), BIT_MASK(bitdepth-inFixBits));
            channelOG = MIN(FIXPOINT_REVERT((channelG+rgbOffset)*gain, TONE_GAIN_FIXPOINT_BITS), BIT_MASK(bitdepth-inFixBits));
            channelOB = MIN(FIXPOINT_REVERT((channelB+rgbOffset)*gain, TONE_GAIN_FIXPOINT_BITS), BIT_MASK(bitdepth-inFixBits));
            ppChannelBuff[CHANNEL_RY][offset]  = channelOR;
            ppChannelBuff[CHANNEL_GCb][offset] = channelOG;
            ppChannelBuff[CHANNEL_BCr][offset] = channelOB;
            
            #ifdef  WDR_LOAD_DATA_EN
                LOGD("offset=%d(x=%d,y=%d): iR=0x%x,iG=0x%x,iB=0x%x;   newLumi=0x%x/(lumi+off)=0x%x->gain=0x%x;   oR=0x%x,oG=0x%x,oB=0x%x;\n",
                offset,x,y,channelR,channelG,channelB,newLumi,(lumi+gainOffset),gain,channelOR,channelOG,channelOB); 
            #endif
        }
    }

    return 0;
}



