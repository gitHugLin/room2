#include "include/regConfig.h"
//
// Created by linqi on 16-5-30.
//
void initWdrPara(WDR_PARAMETER &G_wdr_para)
{

    G_wdr_para.sw_img_width                = 2104; //1920;
    G_wdr_para.sw_img_height               = 1560; //1080;
    G_wdr_para.sw_rgbOffset                = 0;
    G_wdr_para.sw_gainOffset               = 1;
    G_wdr_para.sw_pyramid_epsilon          = 25;
    G_wdr_para.sw_rkwdr_ctrl               = 1;
    G_wdr_para.sw_tonemap_lut              = 1;
    G_wdr_para.sw_pym_cc                   = 0x3    ;

    //RGB to Y (2nd version on 20160305)
    G_wdr_para.sw_wdr_csc_sel              = 0      ; // gaussian pyramid Y: 1: rgb_avg; 0: rgb_max
    G_wdr_para.sw_wdr_coe_00               = 54     ; //channel R  0.2126*256
    G_wdr_para.sw_wdr_coe_01               = 183    ; //channel G	 0.7152*256
    G_wdr_para.sw_wdr_coe_02               = 18     ; //channel B	 0.0722*256
    G_wdr_para.sw_wdr_coe_off              = 0      ; //offset
    G_wdr_para.sw_wdr_gain_off1            = 0      ;
    G_wdr_para.sw_wdr_gain_off2            = 410    ;

    G_wdr_para.sw_wdr_blk_sel              = 1;
    G_wdr_para.sw_wdr_flt_sel              = 1      ; // 1: gaussian pyramid on  0: gaussian pyramid off
    //G_wdr_para.sw_wdr_nonl_mode0           = 1      ; // lpL for gaussian pyramid 1: linear 0:nonlinear //delete already
    G_wdr_para.sw_wdr_nonl_mode1           = 0      ; // Y for tonemap 1:linear  0:nonlinear
    G_wdr_para.sw_wdr_nonl_open            = 1      ; // Y nonlinear  1: on  0:off
    G_wdr_para.sw_wdr_nonl_segm            = 0      ; //linear to nonlinear curve 1: equal interval 0:unequal interval
    G_wdr_para.sw_wdr_gain_max             = 0xffff ; // wdr max gain value[7:0], {4:14}={gain_max[7:4],gain_max[3:0],10'b0}
    G_wdr_para.sw_wdr_gain_max_en          = 0      ; // wdr max gain clip enable


    G_wdr_para.sw_wdr_lvl_en               = 0xf    ;
    G_wdr_para.sw_wdr_lvl_i_en[0]          = (G_wdr_para.sw_wdr_lvl_en&0x1)>>0;
    G_wdr_para.sw_wdr_lvl_i_en[1]          = (G_wdr_para.sw_wdr_lvl_en&0x2)>>1;
    G_wdr_para.sw_wdr_lvl_i_en[2]          = (G_wdr_para.sw_wdr_lvl_en&0x4)>>2;
    G_wdr_para.sw_wdr_lvl_i_en[3]          = (G_wdr_para.sw_wdr_lvl_en&0x8)>>3;

    G_wdr_para.sw_wdr_bavg_clip            = 0      ;
    G_wdr_para.sw_wdr_bestlight            = 0x0cf0 ;
    G_wdr_para.sw_wdr_noiseratio           = 0x00c0 ;

    strcpy(G_wdr_para.sw_outfile_path, IMAGE_FILE_OUTPUT_PATH);
    strcpy(G_wdr_para.sw_outfile_name, IMAGE_FILE_OUTPUT_NAME);
    strcpy(G_wdr_para.sw_infile_name,  IMAGE_FILE_INPUT_NAME );

    //strcat(G_wdr_para.sw_outfile_name, (char *)frmnumStr0);
    //strcat(G_wdr_para.sw_infile_name,  (char *)frmnumStr0);
    strcat(G_wdr_para.sw_outfile_name, ".dat");
    strcat(G_wdr_para.sw_infile_name,  ".pgm");

    //G_wdr_para.sw_img_width  = 64; //2104/2;
    //G_wdr_para.sw_img_height = 32; //1560/2;
    G_wdr_para.sw_img_depth  = 12;

    //gause filter factor:
    G_wdr_para.sw_pyramid_kernel_scales[0] = PYRAMID_LEVEL0_MAXSCALE;
    G_wdr_para.sw_pyramid_kernel_scales[1] = PYRAMID_LEVEL1_MAXSCALE;
    G_wdr_para.sw_pyramid_kernel_scales[2] = PYRAMID_LEVEL2_MAXSCALE;
    G_wdr_para.sw_pyramid_kernel_scales[3] = PYRAMID_LEVEL3_MAXSCALE;
    G_wdr_para.sw_pyramid_kernel_rows = 5;


    //LUT（Look-up table）颜色查找表
    //diff between levels, default 0.05-0.1
    //strcpy(G_wdr_para.sw_lutfile_name, IMAGE_FILE_LUT_NAME);
    switch(G_wdr_para.sw_tonemap_lut) {
        case 0: strcpy(G_wdr_para.sw_lutfile_name, IMAGE_FILE_LUT_NAME0); break;
        case 1: strcpy(G_wdr_para.sw_lutfile_name, IMAGE_FILE_LUT_NAME1); break;
        case 2: strcpy(G_wdr_para.sw_lutfile_name, IMAGE_FILE_LUT_NAME2); break;
        case 3: strcpy(G_wdr_para.sw_lutfile_name, IMAGE_FILE_LUT_NAME3); break;
        default: LOGD("->sw_tonemap_lut=%d out of range, be careful!\n",G_wdr_para.sw_tonemap_lut);
    }

    switch(G_wdr_para.sw_wdr_nonl_segm) {
        case 0: strcpy(G_wdr_para.sw_nonlinearlut_name, IMAGE_FILE_NONLINEAR_NAME1);break;
	    case 1: strcpy(G_wdr_para.sw_nonlinearlut_name, IMAGE_FILE_NONLINEAR_NAME0);break;
	    default: LOGD("->sw_nonlinear_lut=%d out of range, be careful!\n",G_wdr_para.sw_wdr_nonl_segm);
    }
    //sw_tonecurve_deltxbit[TONE_CURVE_SEG_COUNT];
    //sw_tonecurve_y[TONE_CURVE_SEG_COUNT];
    G_wdr_para.sw_tonecurve_xBits  = 12;
    G_wdr_para.sw_tonecurve_dxBits = 3;
}



