#include "include/global.h"
#include "include/dataType.h"
#include "include/regConfig.h"
#include "include/gauseFilter.h"
#include "include/fileParser.h"
#include "log.h"

#define GAUSE_APPROXIMATE 1       //save shift bits in gaussian filter kernel, bad precise
#define GAUSE_KERNEL_CALCULATE 1  //1:calculate gaussian filter factor in CModel; 0: get from regConfig

INT32 genHwGauseKernel( INT32 level, INT32 *pFilterFactor, INT32 rows, INT32 cols, INT32 scale)
{
    INT32 x,y;
    INT32 ry = (INT32)(rows/2);
    INT32 rx = (INT32)(cols/2);
    INT32 val, normFactor;
    INT32 cumVal = 0;

    LOGI("GauseKernel L%d, shift bits: ", scale);
    for (y = -ry; y <= ry; y++) {
        for (x = -rx; x <= rx; x++) {
            val = 4*(abs(x)+abs(y))/scale;
            //right shift bits:
            *(pFilterFactor + (y+ry)*cols + x+rx) = val;
            cumVal += GAUSE_FACTOR_FIXPOINT_FACTOR(1)>>val;
            //zty: LOGI("  %d, ", val);
        }
        //zty: LOGI("\n");
    }
    
    //normilize scale factor, to make sum(gause_kernel)=1
    //zty: normFactor = GAUSE_NORM_FIXPOINT_FACTOR(GAUSE_FACTOR_FIXPOINT_FACTOR(1))/cumVal;
    if(level ==0) {
        normFactor = GAUSE_NORM_FIXPOINT_FACTOR0(GAUSE_FACTOR_FIXPOINT_FACTOR(1))/cumVal;
    } //1//(1<<17<<5) div sum_coe0
    else if(level ==1) {
        normFactor = GAUSE_NORM_FIXPOINT_FACTOR1(GAUSE_FACTOR_FIXPOINT_FACTOR(1))/cumVal;
    } //1//(1<<18<<5) div sum_coe1
    else if(level ==2) {
        normFactor = GAUSE_NORM_FIXPOINT_FACTOR2(GAUSE_FACTOR_FIXPOINT_FACTOR(1))/cumVal;
    } //1//(1<<19<<5) div sum_coe2
    else if(level ==3) {
        normFactor = GAUSE_NORM_FIXPOINT_FACTOR3(GAUSE_FACTOR_FIXPOINT_FACTOR(1))/cumVal;
    } //1//(1<<20<<5) div sum_coe3
    else {
        LOGE("normFactor, Level range out of 0~3, Error!!!");
    }
    LOGI(" normFactor_fixed = %d \n", normFactor);
    return normFactor;
}

INT32 initGausePyramidKernels(GAUSE_PYRAMID_KERNELS *pKernels, WDR_PARAMETER *param)
{
    INT32 level;
    INT32 cols;
    INT32 rows = param->sw_pyramid_kernel_rows;    //G_wdr_para.sw_pyramid_kernel_rows = 5;
    pKernels->heightClipped = rows;
	//PYRAMID_LEVELS = 4
	//G_wdr_para.sw_pyramid_kernel_scales[0] = 5;
    //G_wdr_para.sw_pyramid_kernel_scales[1] = 8;
    //G_wdr_para.sw_pyramid_kernel_scales[2] = 13;
    //G_wdr_para.sw_pyramid_kernel_scales[3] = 20;

    for(level = 0; level < PYRAMID_LEVELS; level++)
    {
        cols = param->sw_pyramid_kernel_scales[level]*2+1;
        pKernels->width[level] = cols;

        MALLOC(pKernels->factors[level], INT32, rows*cols);

#if GAUSE_KERNEL_CALCULATE == 1       
        pKernels->normFactor[level] = genHwGauseKernel(level, pKernels->factors[level], rows, cols, 
            param->sw_pyramid_kernel_scales[level]);
#else
        //or get from sw_pyramid_factors_lx:
        LOGI("initGausePyramidKernels TODO...");
#endif
    }

    pKernels->epsilon  = param->sw_pyramid_epsilon;   //25
    pKernels->pym_cc   = param->sw_pym_cc;            //3
    return 0;
}

INT32 destroyGausePyramidKernels(GAUSE_PYRAMID_KERNELS *pKernels)
{
    INT32 level;
    for(level = 0; level < PYRAMID_LEVELS; level++)
    {
        FREE(pKernels->factors[level]);
    }
    return 0;
}

//pSrcBuf[x-dx:x+dx][y-dy:y+dy] * FltKernel, outputDepth = inputDepth + outFixBits
PTYPE gauseFilter(INT32 level, PTYPE *pSrcBuf, INT32 width,
        INT32 height, INT32 x, INT32 y, INT32 *pFltKernelTab, INT32 kerWidth,
                INT32 kerHeight, INT32 kerNorm, INT32 outFixBits)
{
    INT32 m, n, cnt,fac,shiftBits;
    INT32 row, col;
    UINT32 sum= 0;
    UINT32 levLum= 0;

    for (n = 0, cnt = 0; n < kerHeight; n++)
    {
        row = y - (kerHeight>>1) + n;
        row = CLIP(row, 0, height-1);

        for (m = 0; m < kerWidth; m++)
        {
            col  = x - (kerWidth>>1) + m;
            col  = CLIP(col, 0, width-1);			
            
			fac = GAUSE_FACTOR_FIXPOINT_FACTOR(1) >> pFltKernelTab[cnt];
			sum  += fac * pSrcBuf[row*width+col];

            cnt++;
        }
    }
    sum = GAUSE_FIXPOINT_REVERT_S(sum, GAUSE_FACTOR_FIXPOINT_BITS); 
    //NOTE: max(sum):0x4e1b1 for 12bit input; max(kerNorm):0x346b; max(sum*kerNorm)=0xFFE248FB!! 

    //zty: sum = (GAUSE_NORM_FIXPOINT_BITS-outFixBits) ? GAUSE_FIXPOINT_REVERT_S(sum*kerNorm, GAUSE_NORM_FIXPOINT_BITS-outFixBits) : sum*kerNorm;
    if(level ==0)  {shiftBits =  (GAUSE_NORM_FIXPOINT_BITS0-outFixBits);}
    else if(level ==1)  {shiftBits =  (GAUSE_NORM_FIXPOINT_BITS1-outFixBits);}
    else if(level ==2)  {shiftBits =  (GAUSE_NORM_FIXPOINT_BITS2-outFixBits);}
    else if(level ==3)  {shiftBits =  (GAUSE_NORM_FIXPOINT_BITS3-outFixBits);}
    else  { LOGE("shiftBits, Level range out of 0~3, Error!!!");}
    levLum = shiftBits ? GAUSE_FIXPOINT_REVERT_S(sum*kerNorm, shiftBits) : sum*kerNorm; //{sum div sum_coe , 2'b0}
    return levLum;
}

//outputDepth = inputDepth + outFixBits
INT32 gausePyramidFilter(PTYPE *pSrcBuf, PTYPE *pDstBuf, INT32 width,
                INT32 height, GAUSE_PYRAMID_KERNELS *pKernels,WDR_PARAMETER *param, INT32 outFixBits)
{
    INT32 x, y, lev,cmpVaule1,cmpValue2;
	INT8 levelDisEn, i;
    PTYPE fltVal, fltVal2;
    INT32 dstScale;
    INT32 epsilon = pKernels->epsilon;
	INT32 pym_cc  = pKernels->pym_cc ;
#ifdef WIN32
    PTYPE *pIndexMap = NULL;
    MALLOC(pIndexMap, PTYPE, width*height);
#endif
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            dstScale = PYRAMID_LEVELS-1;

            fltVal = gauseFilter(0, pSrcBuf,  width, height, x, y, pKernels->factors[0],
                    pKernels->width[0], pKernels->heightClipped, pKernels->normFactor[0], outFixBits);

            #ifdef  WDR_LOAD_DATA_EN  
                LOGD("offset=%d(x=%d,y=%d): fltVal0=0x%x,",(y*width+x),x,y,fltVal); 
            #endif
			
            for(lev = 1; lev <= PYRAMID_LEVELS; lev++)//lev:\u4e0b\u4e00\u5c42\u6807\u53f7
            {
                if(lev != PYRAMID_LEVELS)
                    fltVal2 = gauseFilter(lev, pSrcBuf,  width, height, x, y,
                        pKernels->factors[lev], pKernels->width[lev],
                            pKernels->heightClipped, pKernels->normFactor[lev], outFixBits);
                            //\u4e0b\u4e00\u5c42\u7684\u503c
                #ifdef  WDR_LOAD_DATA_EN  
                    LOGD("fltVal=0x%x,",fltVal2); 
                #endif

                //zty: if(abs(fltVal2 - fltVal) > EPSILON_FIXPOINT_REVERT(epsilon*fltVal)  
              //cmpVaule1 = abs(fltVal2 - fltVal) >> EPSILON_FIXPOINT_POINTBITS;
              //cmpValue2 = (epsilon*fltVal) >> (EPSILON_FIXPOINT_BITS + EPSILON_FIXPOINT_POINTBITS);
                cmpVaule1 = abs((fltVal2>>EPSILON_FIXPOINT_POINTBITS) - (fltVal>>EPSILON_FIXPOINT_POINTBITS));
                cmpValue2 = (epsilon*((fltVal>>EPSILON_FIXPOINT_POINTBITS)+pym_cc)) >> EPSILON_FIXPOINT_BITS;

/////////////////////////////////////////////
				levelDisEn = 1;
				for(i = PYRAMID_LEVELS-1; i > lev-1; i--)
					levelDisEn = levelDisEn && (!param->sw_wdr_lvl_i_en[i]);

				if((param->sw_wdr_lvl_i_en[lev-1]&&(cmpVaule1 > cmpValue2 ||levelDisEn))
					||lev == PYRAMID_LEVELS) //when all levels closed, default use level4
					{
						pDstBuf[y*width + x] = fltVal;
	#ifdef WIN32
						pIndexMap[y*width + x] = (lev-1) *255/(PYRAMID_LEVELS-1) ;//lev-1:\u4f7f\u7528\u7684\u5c42 \u5bf9\u5e94\u7684\u6807\u53f7
	#endif
						#ifdef  WDR_LOAD_DATA_EN
						 LOGD("fltVal_out=0x%x, lev=%d\n", fltVal,lev); 
						#endif
						break;
					}
///////////////////////////////////////////
                fltVal = fltVal2;
            }

        }
    }
#ifdef WIN32
    writeSingleChannelToFile(pIndexMap, width, height, "tempIndexMapOut.pgm", 8);
    FREE(pIndexMap);
#endif    
    return 0;
}

