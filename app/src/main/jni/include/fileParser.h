#ifndef __FILEPARSER_H__
#define __FILEPARSER_H__
#include "include/dataType.h"



typedef struct _BITMAP_FILE_HEADER
{
    UINT16  bfType;
    UINT32  bfSize;
    UINT16  bfReserved1;
    UINT16  bfReserved2;
    UINT32  bfOffBits;
}BITMAP_FILE_HEADER;

typedef struct _BITMAP_INFO_HEADER
{
    UINT32 biSize;
    INT32  biWidth;
    INT32  biHeight;
    UINT16 biPlanes;
    UINT16 biBitCount;
    UINT32 biCompression;
    UINT32 biSizeImage;
    INT32  biXPelsPerMeter;
    INT32  biYPelsPerMeter;
    UINT32 biClrUsed;
    UINT32 biClrImportant;
}BITMAP_INFO_HEADER;

typedef struct _RGBQUAD
{
    UINT8 rgbBlue;
    UINT8 rgbGreen;
    UINT8 rgbRed;
    UINT8 rgbReserved;
}RGBQUAD;

typedef union _RGBTAB
{
    RGBQUAD quad;
    UINT32  val;
}RGBTAB;

INT32 readHeaderBmp(INT8 *pInFileName, INT32 *pWidth, INT32 *pHeight, INT32 *nChannel);

INT32 readImageBmp(INT8 *pInFileName, UINT8 *pdat, INT32 width,
                    INT32 height, INT32 heightClip, INT32 nChannel, INT32 dstwidth);

INT32 writeImageBmp_RGB24(UINT8 *pRGB, INT32 width, INT32 height,
                    INT8 *pOutFileName, INT32 srcMarginW, INT32 srcMarginH);

INT32 readPgmFile(PTYPE *pDataBuffB,PTYPE *pDataBuffG,PTYPE *pDataBuffR,
                    INT32 *pWidth, INT32 *pHeight, INT32 depth, INT8 *pInFileName);

INT32 writeSingleChannelToFile(  PTYPE *pDataBuff, INT32 width, INT32 height,
                    INT8 *pOutFileName, INT32 bitdepth);

int readRawYUVSeq(INT32 *pChannelWidthBuf,
                            INT32 *pChannelHeightBuf,
                            INT32 *pChannelWStrideBuf,
                            PTYPE **ppChannelBuff,
                            INT8  *pInFileName,
                            IMG_FORMAT format,
                            int seekFrame,
                            int bSemiPlanar);

int writeRawYUVSeq(INT32 *pChannelWidthBuf,
                    INT32 *pChannelHeightBuf,
                    INT32 *pChannelWStrideBuf,
                    PTYPE **ppChannelBuff,
                    INT8  *pInFileName,
                    IMG_FORMAT format,
                    int seekFrame);

#endif
