/*

//G_wdr_para.sw_img_width  = 0x40; //2104/2;
//G_wdr_para.sw_img_height = 0x20; //1560/2;
//G_wdr_para.sw_rgbOffset  = 0x0 ;
//G_wdr_para.sw_gainOffset = 0x1 ;
//G_wdr_para.sw_pyramid_epsilon = 0xc; //EPSILON_FIXPOINT_FACTOR(0.05)=12;
//G_wdr_para.sw_rkwdr_ctrl = 0x1f; //not used //default=0x1f; //{sw_wdr_lvl_en[3:0],sw_wdr_pym_mode,sw_wdr_csc_sel,sw_wdr_h_size_sel,sw_rkwdr_en}                  
//G_wdr_para.sw_tonemap_lut= 0x1 ; //0:toneCurveLutG1.0.dat used; 1:toneCurveLutS1.0.dat used; 2:toneCurveLutS2.0.dat used; 3:toneCurveLutS3.0.dat used;
  
 ///////////////////////////// Public config ////////////////////////////////////
  #ifdef WIN32
  #define IMAGE_FILE_OUTPUT_PATH  "..\\..\\datOutput\\"
  #define IMAGE_FILE_INPUT_PATH   "..\\..\\datInput\\"
  #define LUT_FILE_INPUT_PATH     "..\\..\\datInput\\"
  #define NONLINEAR_FILE_INPUT_PATH  "..\\..\\datInput\\"   //2nd version on 20160315
  #else
  #define IMAGE_FILE_OUTPUT_PATH  "./"
  #define IMAGE_FILE_INPUT_PATH   "./"    
  #define LUT_FILE_INPUT_PATH     "./lut/"
  #define NONLINEAR_FILE_INPUT_PATH  "./lut/"
  #endif

//#define IMAGE_FILE_INPUT_NAME    IMAGE_FILE_INPUT_PATH"g11.9070_t0.0333_num_478_2104x1560_raw16_BGGRblc.pgm" //"g11.9070_t0.0333_num_578_linear.bmp"
//#define IMAGE_FILE_INPUT_NAME    IMAGE_FILE_INPUT_PATH"g11.9070_t0.0333_num_478_1052x780_raw16_BGR.dat"
//#define IMAGE_FILE_OUTPUT_NAME   IMAGE_FILE_OUTPUT_PATH"g11.9070_t0.0333_num_478_out2.dat"
 #define IMAGE_FILE_INPUT_NAME    IMAGE_FILE_INPUT_PATH"wdr_dat_in"   //"wdr_in001.dat"
 #define IMAGE_FILE_OUTPUT_NAME   IMAGE_FILE_OUTPUT_PATH"wdr_cmodel_out" //"wdr_out001.dat"
 #define IMAGE_FILE_LUT_NAME0     LUT_FILE_INPUT_PATH"toneCurveLutG1.0.dat"
 #define IMAGE_FILE_LUT_NAME1     LUT_FILE_INPUT_PATH"toneCurveLutS1.0.dat"
 #define IMAGE_FILE_LUT_NAME2     LUT_FILE_INPUT_PATH"toneCurveLutS2.0.dat"
 #define IMAGE_FILE_LUT_NAME3     LUT_FILE_INPUT_PATH"toneCurveLutS3.0.dat"

 #define IMAGE_FILE_NONLINEAR_NAME0    NONLINEAR_FILE_INPUT_PATH"nonlinearLut0.dat"
 #define IMAGE_FILE_NONLINEAR_NAME1    NONLINEAR_FILE_INPUT_PATH"nonlinearLut1.dat"

  strcpy(G_wdr_para.sw_outfile_path, IMAGE_FILE_OUTPUT_PATH);
  strcpy(G_wdr_para.sw_outfile_name, IMAGE_FILE_OUTPUT_NAME);
  strcpy(G_wdr_para.sw_infile_name,  IMAGE_FILE_INPUT_NAME );
  
  strcat(G_wdr_para.sw_outfile_name, frmnumStr0);
  strcat(G_wdr_para.sw_infile_name,  frmnumStr0);
  strcat(G_wdr_para.sw_outfile_name, ".dat");
  strcat(G_wdr_para.sw_infile_name,  ".dat");
  
//G_wdr_para.sw_img_width  = 64; //2104/2;
//G_wdr_para.sw_img_height = 32; //1560/2;
  G_wdr_para.sw_img_depth  = 12;

  //gause filter factor:
  G_wdr_para.sw_pyramid_kernel_scales[0] = PYRAMID_LEVEL0_MAXSCALE;
  G_wdr_para.sw_pyramid_kernel_scales[1] = PYRAMID_LEVEL1_MAXSCALE;
  G_wdr_para.sw_pyramid_kernel_scales[2] = PYRAMID_LEVEL2_MAXSCALE;
  G_wdr_para.sw_pyramid_kernel_scales[3] = PYRAMID_LEVEL3_MAXSCALE;
  G_wdr_para.sw_pyramid_kernel_rows = 5;
  
  /*
  G_wdr_para.sw_pyramid_factors_l0[5][PYRAMID_LEVEL0_MAXSCALE*2+1];
  G_wdr_para.sw_pyramid_factors_l1[5][PYRAMID_LEVEL1_MAXSCALE*2+1];
  G_wdr_para.sw_pyramid_factors_l2[5][PYRAMID_LEVEL2_MAXSCALE*2+1];
  G_wdr_para.sw_pyramid_factors_l3[5][PYRAMID_LEVEL3_MAXSCALE*2+1];
  */
/*
  //diff between levels, default 0.05-0.1
  //strcpy(G_wdr_para.sw_lutfile_name, IMAGE_FILE_LUT_NAME);
  switch(G_wdr_para.sw_tonemap_lut) { 
    case 0: strcpy(G_wdr_para.sw_lutfile_name, IMAGE_FILE_LUT_NAME0); break;
    case 1: strcpy(G_wdr_para.sw_lutfile_name, IMAGE_FILE_LUT_NAME1); break;
    case 2: strcpy(G_wdr_para.sw_lutfile_name, IMAGE_FILE_LUT_NAME2); break;
    case 3: strcpy(G_wdr_para.sw_lutfile_name, IMAGE_FILE_LUT_NAME3); break;
    default: LOGD("->sw_tonemap_lut=%d out of range, be careful!\n",G_wdr_para.sw_tonemap_lut);
  }
  
  switch(G_wdr_para.sw_wdr_nonl_segm){
	case 0: strcpy(G_wdr_para.sw_nonlinearlut_name, IMAGE_FILE_NONLINEAR_NAME1);break;
	case 1: strcpy(G_wdr_para.sw_nonlinearlut_name, IMAGE_FILE_NONLINEAR_NAME0);break;
	default: LOGD("->sw_nonlinear_lut=%d out of range, be careful!\n",G_wdr_para.sw_wdr_nonl_segm);
  }
*/
/*

  //sw_tonecurve_deltxbit[TONE_CURVE_SEG_COUNT];
  //sw_tonecurve_y[TONE_CURVE_SEG_COUNT];
  G_wdr_para.sw_tonecurve_xBits  = 12;
  G_wdr_para.sw_tonecurve_dxBits = 3;
//G_wdr_para.sw_pyramid_epsilon = EPSILON_FIXPOINT_FACTOR(0.05);
//G_wdr_para.sw_noiseRatio = RATIO_FIXPOINT_FACTOR(0.2); //not used
//G_wdr_para.sw_rgbOffset  = 0;
//G_wdr_para.sw_gainOffset = 1;
  
*/