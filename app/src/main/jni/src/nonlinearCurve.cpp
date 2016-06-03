#include "include/global.h"
#include "include/dataType.h"
#include "include/regConfig.h"
#include "include/nonlinearCurve.h"
#include "log.h"



INT32 initNonlinearCurve(NONLINEAR_CURVE *pCurve, WDR_PARAMETER *param)
{
	UINT32 y16;
	FILE *fp = NULL;
	fp = fopen(param->sw_nonlinearlut_name, "rb");
	if(NULL == fp) {
	    LOGE("initNonlinearCurve Error : Can not open %s file!\n",param->sw_nonlinearlut_name);
	    return -1;
	}

    //NONLIN_SEG_COUNT 16
	fread(pCurve->dxBitDepth, sizeof(INT8), NONLIN_SEG_COUNT, fp);
	fread(pCurve->y, sizeof(INT32), NONLIN_SEG_COUNT + 1, fp);
	y16 = pCurve->y[16];
	if((y16&0xf00)==0xf00) 
	{
	    pCurve->y[16]=0x1000;
	 }
	fclose(fp);

	return 0;
}


INT32 nonlinearCurveLut(NONLINEAR_CURVE *pCurve, PTYPE lumiVal, INT32 bitdepth)
{
	INT32 cumXVal = 0;
	INT32 xDeltN = 0;
	INT16 interpWeight;
	INT32 cumYLeft, cumYRight;
	INT32 newLumi = 0;
	INT8 i;
	//NONLIN_SEG_COUNT 16
	for(i = 0; i < NONLIN_SEG_COUNT; i++)
	{	
		xDeltN = 1<<pCurve->dxBitDepth[i];
		cumXVal += xDeltN;
		if (lumiVal < cumXVal)
		{
			cumYLeft = pCurve->y[i];
			cumYRight = pCurve->y[i+1];
			interpWeight = cumXVal - lumiVal;
			newLumi = interpWeight*cumYLeft + (xDeltN - interpWeight)*cumYRight;
	        newLumi = FIXPOINT_REVERT(newLumi, pCurve->dxBitDepth[i]);
			newLumi = newLumi>>2;
			newLumi = newLumi<<2;//12bits in&&10bits out round gamma
			break;
		}
	}
	return (PTYPE)newLumi;
}


INT32 nonlinearCurveTransfer(PTYPE *pLumiChannelBuff, INT32 width,
                INT32 height, INT32 bitdepth, NONLINEAR_CURVE *pCurve)
{
	INT32 x, y,offset;
	INT32 lumi;


	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			offset = y*width + x;
			lumi = pLumiChannelBuff[offset];
			lumi = nonlinearCurveLut(pCurve, lumi,bitdepth);
			pLumiChannelBuff[offset] = lumi;
		}
		
	}
	return 0;
}


INT32 renameDatFile(INT8 *pDstName, INT8 *pSrcName)
{
    INT32 strLen = strlen(pSrcName);
    if(strLen < 4)
    {
        return -1;
    }
    strcpy(pDstName, pSrcName);
    pDstName[strLen-4]='.';
    pDstName[strLen-3]='d';
    pDstName[strLen-2]='a';
    pDstName[strLen-1]='t';
    return 0;
}


//INT32 writeNonlinearOutData(PTYPE *pDataBuff, INT32 width, INT32 height, INT8 *pOutFileName, INT32 bitdepth)
//{
//	FILE *fp = NULL;
//	INT32 i;
//	INT32 temp;
//	INT8 newOutputFileName[MAX_FILENAME_LEN];
//	renameDatFile(newOutputFileName, pOutFileName);
//	fp = fopen(newOutputFileName, "wb");
//	for(i = 0; i < width*height; i++)
//	{
//		temp = pDataBuff[i];
//		fprintf(fp, "%.4x\n", temp);
//	}
//	fclose(fp);
//	return 0;
//}
