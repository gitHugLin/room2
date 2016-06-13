//
// Created by linqi on 16-6-12.
//
#include "include/wdrBase.h"
using namespace wdr;



int main(int argc, char * argv[])
{
    wdrBase wdr;
    wdr.process();
    LOGE("end of frame processing ! \n");
    //LOGE("wdrProcess : wdrProcess is running!\n");
	//G_wdr_para.sw_wdr_lvl_en      = 0xf  ; //initial gaussian levelx4 enable
    //G_wdr_para.sw_wdr_lvl_i_en[0] = (G_wdr_para.sw_wdr_lvl_en&0x1)>>0;
    //G_wdr_para.sw_wdr_lvl_i_en[1] = (G_wdr_para.sw_wdr_lvl_en&0x2)>>1;
    //G_wdr_para.sw_wdr_lvl_i_en[2] = (G_wdr_para.sw_wdr_lvl_en&0x4)>>2;
    //G_wdr_para.sw_wdr_lvl_i_en[3] = (G_wdr_para.sw_wdr_lvl_en&0x8)>>3;
    //G_wdr_para.sw_wdr_flt_sel     = 1    ; // 1: gaussian pyramid on  0: gaussian pyramid off
    //initWdrPara(G_wdr_para);
    //frameProc();

    return 0;
}
