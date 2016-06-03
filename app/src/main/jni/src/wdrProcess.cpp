#include "include/wdrProcess.h"

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


WDR_PARAMETER G_wdr_para;

INT32 frameFlag = 0;

INT32 clearGlobalConfig()
{
    memset(&G_wdr_para, 0, sizeof(WDR_PARAMETER));
    return 0;
}

INT32 frameProc()
{
    //load image config
    INT8 *pInFileName   = G_wdr_para.sw_infile_name;
    INT8 *pOutFileName  = G_wdr_para.sw_outfile_name;
    INT32 width         = G_wdr_para.sw_img_width;
    INT32 height        = G_wdr_para.sw_img_height;
    INT32 bitdepth      = G_wdr_para.sw_img_depth;
	INT8 blkHeight;
	INT8 blkWidth;

    //CHANNEL_INDEX_COUNT = 4
    INT32 widthBuf[CHANNEL_INDEX_COUNT];        //reserved for yuv420
    INT32 heightBuf[CHANNEL_INDEX_COUNT];       //reserved for yuv420
    PTYPE *ppChannelBuff[CHANNEL_INDEX_COUNT];  //Image data with RGB channels
    PTYPE *pLumiBuff = NULL;                    //Image Lumi data for lpL
	INT32 *lineLumiBuff = NULL;
	//2nd version on 20160311				
	PTYPE *pAvgLumiBuff = NULL;					//Y channel lumi: coe_r*R + coe_g*G + coe_b*B 
	//
    PTYPE *pLmLpBuff = NULL;                    //Low pass filter output
	INT32 *blkLumiBuff = NULL;					//Image block luminance means data(2nd version on 20160308)
    PTYPE L;
	PTYPE avgL;
    GAUSE_PYRAMID_KERNELS gKernels;
    TONE_CURVE tmoCurve;
	FILE *fp = NULL;
	FILE *fp1 = NULL;
	INT8 newOutputFileName[MAX_FILENAME_LEN];

	//2nd version on 20160315
	NONLINEAR_CURVE nonlCurve;

    IMG_FORMAT format = (bitdepth == 10)?IMG_FORMAT_RGB101010:IMG_FORMAT_RGB121212;

    //memory init
    INT8 channelIndex;
    for(channelIndex = 0; channelIndex < CHANNEL_INDEX_COUNT; channelIndex++)
    {
        MALLOC(ppChannelBuff[channelIndex], PTYPE, width * height);
        widthBuf[channelIndex] = width;
        heightBuf[channelIndex] = height;
    }

    MALLOC(pLumiBuff, PTYPE, width * height);    //maxlumiMode buffer
	//2nd version on 20160311
	MALLOC(pAvgLumiBuff, PTYPE, width * height); //avglumi buffer

    //TONE_MAP_BLK_SIZEBITS = 8
	blkWidth = width >> TONE_MAP_BLK_SIZEBITS;
	blkHeight = height >> TONE_MAP_BLK_SIZEBITS;
	if (width - (blkWidth << TONE_MAP_BLK_SIZEBITS) > 0)
		blkWidth += 1;
	if (height - (blkHeight << TONE_MAP_BLK_SIZEBITS) > 0)
		blkHeight += 1;

	MALLOC(blkLumiBuff, INT32, blkWidth * blkHeight);//block means lumi buffer(2nd version on 20160308)
	MALLOC(lineLumiBuff, INT32, height * blkWidth);

    MALLOC(pLmLpBuff, PTYPE, width * height);      

    //load image file
    loadChannelData(format,
                    widthBuf,
                    heightBuf,
                    widthBuf,
                    ppChannelBuff,
                    pInFileName,
                    IO_FILE_FORMAT_ASCII_48BIT_PER_LINE);


    dumpPixelsDataToBmpFile(format,
        widthBuf,
        heightBuf,
        widthBuf,
        ppChannelBuff,
        pInFileName,
        NULL,
        0);
workBegin();
	/************************************Luminance linear to nonlinear*************************************/
	initNonlinearCurve(&nonlCurve, &G_wdr_para);

    //get Y chanel
	avgL = getAvgLumiChannel(ppChannelBuff, pAvgLumiBuff, &G_wdr_para, width, height); 

  	if (((G_wdr_para.sw_wdr_nonl_mode1)&0x1)==0x0 && ((G_wdr_para.sw_wdr_nonl_open)&0x1)==0x1)
	{
		nonlinearCurveTransfer(pAvgLumiBuff, width, height, bitdepth, &nonlCurve);
	}
	else
		LOGE("Error in opening nonlinear transfer!\n");

	LOGE("frameProc : frameProc is running! 4444444444444444444444444\n");
	if (G_wdr_para.sw_wdr_csc_sel == 0)
	{
	    //get the max chanel from RGB channel
		L = getLumiChannel(ppChannelBuff, pLumiBuff, width, height); //lumiMode0(Max mode)

	}
	else if (G_wdr_para.sw_wdr_csc_sel == 0x1)
	{
	// 2nd version on 20160311
		L = getAvgLumiChannel(ppChannelBuff, pLumiBuff, &G_wdr_para, width, height); //lumiMode1(average mode)
	}
	else
		LOGE("Error in low pass luminance!\n");

/***************************************** Gaussian Pyramid *************************************/
    //get low-pass filter output, LUMI_FIXPOINT_BITS
	if(G_wdr_para.sw_wdr_flt_sel == 0x0) // gaussian pyramid
	{
//workBegin();
	    //run here
		memset(&gKernels, 0, sizeof(gKernels));
		initGausePyramidKernels(&gKernels, &G_wdr_para);
		gausePyramidFilter(pLumiBuff, pLmLpBuff, width, height, &gKernels, &G_wdr_para, LUMI_FIXPOINT_BITS);
		destroyGausePyramidKernels(&gKernels);
//workEnd("WDR TIME COUNT");
	}
	else if (G_wdr_para.sw_wdr_flt_sel == 0x1)
	{
		lumiFixpoints(pLumiBuff, pLmLpBuff, width, height, LUMI_FIXPOINT_BITS); //bitdepth 12 bit + 2
	}
	else
		LOGE("Error in Gaussian pyramid building!\n");

	bitdepth += LUMI_FIXPOINT_BITS;//bitdepth 14 bit

	/************************************** WDR tone mapping ********************************/
	if(G_wdr_para.sw_wdr_blk_sel == 0)  //global method
	{   //for next frame with sw_wdr_blk_sel=1

 	    getBlockLumi(pAvgLumiBuff, blkLumiBuff, width, height,blkWidth, blkHeight, &G_wdr_para );//get 5x8 block mean
		renameDatFile(newOutputFileName, "lastBlockLumi1Data");
		fp1 = fopen(newOutputFileName, "wb");
		fwrite(blkLumiBuff, sizeof(INT32),blkHeight*blkWidth, fp1);
		fclose(fp1);

		//tone curve mapping
		initToneCurve(&tmoCurve, &G_wdr_para);
		toneCurveTransfer(ppChannelBuff, pLmLpBuff, width, height, bitdepth, &tmoCurve, LUMI_FIXPOINT_BITS);

	}
	else if(G_wdr_para.sw_wdr_blk_sel == 0x1) //local method (in block)
	{
		//tone mapping in block means
		if (frameFlag == 0)
		{
		    //run here
			getBlockLumi(pAvgLumiBuff, blkLumiBuff, width, height,blkWidth, blkHeight, &G_wdr_para );//get 5x8 block mean
#ifdef LAST_FRAME_BASED
			//if based on lumi of last frame
			renameDatFile(newOutputFileName, "lastBlockLumi1Data");
			fp1 = fopen(newOutputFileName, "wb");
			fwrite(blkLumiBuff, sizeof(INT32),blkHeight*blkWidth, fp1);
			fclose(fp1);
#endif
		}
  		
  		else //if(frameFlag == 0x1)
		{
			fp1 = fopen("lastBlockLumi1.dat", "rb");
			fread(blkLumiBuff, sizeof(INT32), blkHeight*blkWidth, fp1);//block lumi of last frame
			fclose(fp1);
		}

        //run here
		bitdepth = toneMapping(ppChannelBuff, blkLumiBuff, pLmLpBuff, width, height, bitdepth, &G_wdr_para, blkWidth, blkHeight);
#ifdef LAST_FRAME_BASED
		if(frameFlag == 0x1) //frame num=2 used 
		{
			getBlockLumi(pAvgLumiBuff, blkLumiBuff, width, height,blkWidth, blkHeight, &G_wdr_para );//get 5x8 block mean of current frame
			fp1 = fopen("lastBlockLumi1.dat","wb");
			fwrite(blkLumiBuff, sizeof(INT32),blkHeight*blkWidth, fp1);
			fclose(fp1);
		}
#endif
	}
	else
    LOGE("Error in tone mapping\n");

	bitdepth -= LUMI_FIXPOINT_BITS; //14bit to 12bit

workEnd("WDR TIME COUNT");
  dumpPixelsDataToBmpFile(format,
      widthBuf,
      heightBuf,
      widthBuf,
      ppChannelBuff,
      pOutFileName,
      NULL,
      0);

  for(channelIndex = 0; channelIndex < CHANNEL_INDEX_COUNT; channelIndex++)
      FREE(ppChannelBuff[channelIndex]);

  FREE(pLumiBuff);
  FREE(pLmLpBuff);
	//2nd version on 20160311
  FREE(pAvgLumiBuff);
  FREE(blkLumiBuff);
  LOGE("end of frame processing ! \n");
  return 0;
}

/****************************************************************/
#ifdef WIN32
#define CONFIG_PATH 
#else
#define CONFIG_PATH // "./c_lib/"
#endif

#define WDR_LOOP_EN
//#define LAST_FRAME_BASED
#define FAMENUM  0x1
char  frmnumStr0[2];

#ifdef WDR_LOOP_EN
    INT32 wdrProcess()
    {
        LOGE("wdrProcess : wdrProcess is running!\n");
        //DISP_DEVICE *dispDev = NULL;
        INT32 frameCount;

    	for(frameCount = 0; frameCount < FAMENUM; frameCount++)
    	{

          LOGE("-------------------------------------------------------------\n");
          LOGE("--------------------------Frame_%d---------------------------\n",frameCount);
          LOGE("-------------------------------------------------------------\n");
        //itoa(frameCount, frmnumStr0, 10); //<10
          sprintf(frmnumStr0,"%02d",frameCount); //<100
		  G_wdr_para.sw_wdr_lvl_en      = 0xf  ; //initial gaussian levelx4 enable
          G_wdr_para.sw_wdr_lvl_i_en[0] = (G_wdr_para.sw_wdr_lvl_en&0x1)>>0;
          G_wdr_para.sw_wdr_lvl_i_en[1] = (G_wdr_para.sw_wdr_lvl_en&0x2)>>1;
          G_wdr_para.sw_wdr_lvl_i_en[2] = (G_wdr_para.sw_wdr_lvl_en&0x4)>>2;
          G_wdr_para.sw_wdr_lvl_i_en[3] = (G_wdr_para.sw_wdr_lvl_en&0x8)>>3; 
          G_wdr_para.sw_wdr_flt_sel     = 1    ; // 1: gaussian pyramid on  0: gaussian pyramid off
          initWdrPara(G_wdr_para,frmnumStr0);
			switch(frameCount) { 
              case 0: 
              	    #if (FAMENUM > 0x0)

                        break;
                    #endif
              case 1: 
              	    #if (FAMENUM > 0x1)
#ifdef LAST_FRAME_BASED
                        frameFlag ++;
#endif
                        #include CONFIG_PATH"config1.c"
                        break;
                    #endif
              case 2: 
              	    #if (FAMENUM > 0x2)
                        frameFlag ++;
                        #include CONFIG_PATH"config2.c"
                        break;
                    #endif
              case 3: 
              	    #if (FAMENUM > 0x3)
                        #include CONFIG_PATH"config3.c"
                        break;
                    #endif
              case 4: 
              	    #if (FAMENUM > 0x4)
                        #include CONFIG_PATH"config4.c"
                        break;
                    #endif
              case 5: 
              	    #if (FAMENUM > 0x5)
                        #include CONFIG_PATH"config5.c"
                        break;
                    #endif

              default: LOGD("->Frame_%d out of range, be careful!\n",frameCount);
            }

              //#include CONFIG_PATH"config_public.c"
            frameProc();
        }

        return 0;
    }

#else

    INT32 simpleProcess()
    {
        //DISP_DEVICE *dispDev = NULL;
        INT32 frameCount;
        
        //MALLOC(dispDev, DISP_DEVICE, 1);
    
        for(frameCount = 0; frameCount < 1; frameCount++)
        {
            LOGD("------Frame_%d_f0---------\n",frameCount);
    
            clearGlobalConfig();
    
            //#include CONFIG_PATH"regConfig.c"
            
            frameProc();
    
        }

        return 0;
    }
#endif
