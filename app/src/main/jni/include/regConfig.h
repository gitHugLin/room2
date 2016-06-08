#ifndef __REGCONFIG_H__
#define __REGCONFIG_H__
#include "include/dataType.h"
#include "string.h"
#include "log.h"

#define     MAX_FILENAME_LEN              128
#define     PYRAMID_LEVELS                4
#define     PYRAMID_LEVEL0_MAXSCALE       5
#define     PYRAMID_LEVEL1_MAXSCALE       8
#define     PYRAMID_LEVEL2_MAXSCALE       13
#define     PYRAMID_LEVEL3_MAXSCALE       20
#define     PYRAMID_LEVELS_MAXROWS        5
#define     TONE_CURVE_SEG_COUNT          32
#define     TONE_MAP_BLK_SIZEBITS         8
#define     TONE_MAP_BLK_SIZE             256
#define     IMAGE_FILE_OUTPUT_PATH        "./"
#define     IMAGE_FILE_INPUT_PATH         "./"
#define     LUT_FILE_INPUT_PATH           "./lut/"
#define     NONLINEAR_FILE_INPUT_PATH     "./lut/"
#define     IMAGE_FILE_INPUT_NAME         "/sdcard/wdr_raw"
#define     IMAGE_FILE_OUTPUT_NAME        "/sdcard/wdr_cmodel_out"
#define     IMAGE_FILE_LUT_NAME0          "/sdcard/toneCurveLutG1.0.dat"
#define     IMAGE_FILE_LUT_NAME1          "/sdcard/toneCurveLutS1.0.dat"
#define     IMAGE_FILE_LUT_NAME2          "/sdcard/toneCurveLutS2.0.dat"
#define     IMAGE_FILE_LUT_NAME3          "/sdcard/toneCurveLutS3.0.dat"
#define     IMAGE_FILE_NONLINEAR_NAME0    "/sdcard/nonlinearLut0.dat"
#define     IMAGE_FILE_NONLINEAR_NAME1    "/sdcard/nonlinearLut1.dat"


typedef struct _WDR_PARAMETER
{
    INT8   sw_outfile_path[MAX_FILENAME_LEN];//reserved for multi test case
    INT8   sw_outfile_name[MAX_FILENAME_LEN];
    INT8   sw_infile_name[MAX_FILENAME_LEN];
    INT32  sw_img_width;
    INT32  sw_img_height;
    INT32  sw_img_depth;

    UINT32  sw_pyramid_kernel_scales[PYRAMID_LEVELS];
    UINT8   sw_pyramid_kernel_rows;
    //one quarter of gause filter factor:
    UINT8   sw_pyramid_factors_l0[(PYRAMID_LEVELS_MAXROWS-1)/2][PYRAMID_LEVEL0_MAXSCALE+1];
    UINT8   sw_pyramid_factors_l1[(PYRAMID_LEVELS_MAXROWS-1)/2][PYRAMID_LEVEL1_MAXSCALE+1];
    UINT8   sw_pyramid_factors_l2[(PYRAMID_LEVELS_MAXROWS-1)/2][PYRAMID_LEVEL2_MAXSCALE+1];
    UINT8   sw_pyramid_factors_l3[(PYRAMID_LEVELS_MAXROWS-1)/2][PYRAMID_LEVEL3_MAXSCALE+1];

    //rkwdr ctrl
    UINT8   sw_rkwdr_ctrl ; //default=0x1f; //{sw_wdr_lvl_en[3:0],sw_wdr_pym_mode,sw_wdr_csc_sel,sw_wdr_h_size_sel,sw_rkwdr_en}
    UINT8   sw_tonemap_lut; //0:toneCurveLutG1.0.dat used; 1:toneCurveLutS1.0.dat used; 2:toneCurveLutS2.0.dat used; 3:toneCurveLutS3.0.dat used;
    //diff between levels, default 0.05-0.1
    UINT8   sw_pyramid_epsilon;

    //noise/maxLumiRange, default 0.2
    UINT8   sw_noiseRatio;
    UINT16  sw_gainOffset; //siliconImg for noise reduction //zty fix bits width, 20160224
    UINT16  sw_rgbOffset ; //siliconImg, r/g/b share the same offset? TO BE CHECKED
    UINT16  sw_pym_cc    ; //hpz add for gaussian pyramid level compare 
    
    //tone curve
    INT8    sw_lutfile_name[MAX_FILENAME_LEN];
    UINT8   sw_tonecurve_deltxbit[TONE_CURVE_SEG_COUNT];
    UINT32  sw_tonecurve_y[TONE_CURVE_SEG_COUNT];
    UINT32  sw_tonecurve_xBits; //bitdepth for sum(dx)
    UINT32  sw_tonecurve_dxBits;    //bitdepth for min(dx)

	//RGB to Y (2nd version on 20160305)
	UINT8  sw_wdr_csc_sel; //1:rgb_avg; 0: rgb_max
	UINT32 sw_wdr_coe_00; //channel R
	UINT32 sw_wdr_coe_01; //channel G
	UINT32 sw_wdr_coe_02; //channel B
	UINT32 sw_wdr_coe_off; //offset

	//block tone map
	//UINT8 sw_block_width; // 1920/256
	//UINT8 sw_block_height;// 1080/256
	UINT16 sw_wdr_gain_off1;
	UINT16 sw_wdr_gain_off2;

	//linear to nonlinear
	UINT8 sw_wdr_nonl_mode0; //block wdr gaussian lpL mode 0: nonlinear 1: linear
	UINT8 sw_wdr_nonl_mode1; //block wdr meanY mode 0:nonlinear  1: lineaer
	UINT8 sw_wdr_nonl_segm; //0: divided equally(interval 2^8)  1: divided unequally
	UINT8 sw_wdr_nonl_open; //0: linear to nonlinear off  1: linear to nonlinear on
	//UINT8 sw_nonlinearlut_name[MAX_FILENAME_LEN];
	INT8 sw_nonlinearlut_name[MAX_FILENAME_LEN];

	//wdr mode option
	UINT8 sw_wdr_blk_sel; //0:rk-global wdr  1: rk-block wdr

	//gaussian pyramid for lpL enable
	UINT8 sw_wdr_flt_sel; //1: pyramid enable 
	UINT8 sw_wdr_lvl_en;
	UINT8 sw_wdr_lvl_i_en[4];
	
	//confine block mean luminance value
	UINT8  sw_wdr_bavg_clip;
	UINT32 sw_wdr_bestlight;
	UINT32 sw_wdr_noiseratio;
	//
	UINT32 sw_wdr_gain_max;
	UINT8  sw_wdr_gain_max_en;

}WDR_PARAMETER;


void initWdrPara(WDR_PARAMETER &G_wdr_para);


#endif
