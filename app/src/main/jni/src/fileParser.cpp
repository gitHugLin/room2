#include "include/global.h"
#include "include/dataType.h"
#include "include/fileParser.h"
#include "include/fileIO.h"
#include "log.h"



#define FSEEK64 fseek

INT32 widthBytes(INT32 width, INT32 bitCount)//BitCount
{
    INT32 WBytes;
    WBytes = (width*bitCount+31)/8;
    WBytes = WBytes/4*4;
    return WBytes;
}

INT32 readHeaderBmp(INT8 *pInFileName, INT32 *pWidth, INT32 *pHeight, INT32 *nChannel)
{
    INT32 height,width;

    BITMAP_FILE_HEADER bfh;
    BITMAP_INFO_HEADER bih;

    FILE *fp = fopen(pInFileName, "rb");

    if(fp == NULL)
    {
        LOGE("Fail open file %s in readImageBmp. \n", pInFileName);
        return -1;
    }

    fseek(fp, 0, SEEK_SET);
    fread(&bfh, sizeof(BITMAP_FILE_HEADER), 1, fp);

    if (bfh.bfType != 'M'*256 + 'B')
    {
        LOGE("This is not a bmp file.\n");
        fclose(fp);
        return -1;
    }

    fseek(fp, 14, SEEK_SET);
    fread(&bih, sizeof(BITMAP_INFO_HEADER), 1, fp);

    height    = bih.biHeight;
    width     = bih.biWidth;
    *pWidth   = width;
    *pHeight  = height;
    *nChannel = bih.biBitCount/8;
    
    fclose(fp);
    
    return 0;
}

INT32 readImageBmp(INT8 *pInFileName, UINT8 *pdat, INT32 width, INT32 height,
                INT32 heightClip, INT32 nChannel, INT32 dstWBytes)
{
    INT32   i,j;
    INT32   srcWBytes;
    INT32   colors = 256;
    RGBTAB  tab[256];
    INT32   ret = 0;

    FILE *fp = fopen(pInFileName, "rb");

    if(fp == NULL)
    {
        LOGE("Fail open file %s in readImageBmp. \n", pInFileName);
        return -1;
    }

    srcWBytes = widthBytes(width, nChannel * 8);

    fseek(fp, 54, SEEK_SET);
    fread(tab, sizeof(RGBQUAD), 256, fp);

    if (nChannel == 1)
    {
        UINT8 srcData;
        //256 colors
        for (i=height-1; i>=(height-heightClip); i--)
        {
            for (j=0; j<width; j++)
            {
                UINT8 *pDst = pdat + (height-1-i) * dstWBytes + j * 4;
                fseek(fp, 54+colors*sizeof(RGBQUAD)+i*srcWBytes+j, SEEK_SET);
                fread(&srcData, 1, 1, fp);
				//pdat[(height-1-i) * width + j]=tab[srcData].val;
                *pDst++ = tab[srcData].quad.rgbBlue;
                *pDst++ = tab[srcData].quad.rgbGreen;
                *pDst++ = tab[srcData].quad.rgbRed;
                *pDst++ = 255;//tab[srcData].quad.rgbReserved;
            }
        }
    }
    else if (nChannel == 3 || nChannel == 4)
    {
        //highAddr RGBRG lowAddr, width 4 aligned
        for (i=height-1;i>=(height-heightClip);i--)
        {
            /*for(j=0;j<width;j++)
            {
            	UINT8 *pDst = pdat + (height-1-i)*width*nChannel + j*nChannel; //dst RGB packed
            	fseek(fp, 54+i*srcWBytes+j*nChannel, SEEK_SET);//no color table
            	fread(pDst, 1, BytePerPixel, fp);
            }*/
            UINT8 *pDst = pdat + (height-1-i) * dstWBytes;
            fseek(fp, 54+i*srcWBytes, SEEK_SET);//no color table
            fread(pDst, 1, srcWBytes, fp);
        }
    }
    else
    {
        LOGE("bmp nChannel=%d unsupport!\n", nChannel);
        ret = -1;
    }

    fclose(fp);
    return ret;
}


INT32 writeImageBmp_RGB24(UINT8 *pRGB, INT32 width, INT32 height,
            INT8 *pOutFileName, INT32 srcMarginW, INT32 srcMarginH)
{
    INT32 x = 0;
    INT32 y = 0;
    INT32 i = 0;
    INT32 j = 0;

    UINT8 *pBmpFileBuf = NULL;
    UINT8 *pBmpDataBuf = NULL;
    INT32 bytePerLine = 0;

    INT32 realWidth  = width - srcMarginW;
    INT32 realHeight = height - srcMarginH;
    INT32 remainW    = realWidth*3%4;

    FILE *fp = fopen(pOutFileName, "wb");

    if(fp == NULL)
    {
        LOGE("Fail open file %s in writeImageBmp_RGB24. \n", pOutFileName);
        return -1;
    }

    remainW     = (remainW != 0) ? 4 - remainW : remainW;
    bytePerLine = realWidth * 3 + remainW;

    MALLOC(pBmpFileBuf, UINT8, bytePerLine*realHeight + 54);

    //BMPͷ
    pBmpFileBuf[0] = 'B';
    pBmpFileBuf[1] = 'M';
    *((UINT32*)(pBmpFileBuf+2))  = bytePerLine*realHeight + 54;
    *((UINT32*)(pBmpFileBuf+10)) = 54;
    *((UINT32*)(pBmpFileBuf+14)) = 40;
    *((UINT32*)(pBmpFileBuf+18)) = realWidth;
    *((UINT32*)(pBmpFileBuf+22)) = realHeight;
    *((UINT16*)(pBmpFileBuf+26)) = 1;
    *((UINT16*)(pBmpFileBuf+28)) = 24;
    *((UINT32*)(pBmpFileBuf+34)) = bytePerLine*realHeight;

    pBmpDataBuf = pBmpFileBuf + 54;

    for (y = realHeight-1, j = 0; y >= 0; y--,j++)
    {
        for (x = 0,i = 0; x < realWidth*3; x++)
        {
            pBmpDataBuf[y*bytePerLine + i++] = pRGB[j*width*3 + x];
        }
    }

    fwrite(pBmpFileBuf, 1, bytePerLine*realHeight + 54, fp);
    FREE(pBmpFileBuf);

    fclose(fp);
    return 0;
}

/* PGM files allow a comment starting with '#' to end-of-line.  
 * Skip white space including any comments. */
void skipComments(FILE *fp)
{
    int ch;

    fscanf(fp," ");      /* Skip white space. */
    while ((ch = fgetc(fp)) == '#') {
      while ((ch = fgetc(fp)) != '\n'  &&  ch != EOF)
	;
      fscanf(fp," ");
    }
    ungetc(ch, fp);      /* Replace last character read. */
}


INT32 readRgbFile(PTYPE *pDataBuffB,PTYPE *pDataBuffG,PTYPE *pDataBuffR,
                    INT32 pWidth, INT32 pHeight, INT32 depth, INT8 *pInFileName)
{
    LOGE("readRgbFile:rgb picture width = %d, height = %d \n", pWidth, pHeight);
    Mat rgbImage,bayer;
    bayer = imread(pInFileName,0);
    cvtColor(bayer, rgbImage, CV_BayerBG2BGR);
    vector<Mat> bgr;
    //分离后各通道
    rgbImage.convertTo(rgbImage,CV_16UC1);
    split(rgbImage,bgr);
    memcpy(pDataBuffB,bgr[0].data,pHeight*pWidth*sizeof(PTYPE));
    memcpy(pDataBuffG,bgr[1].data,pHeight*pWidth*sizeof(PTYPE));
    memcpy(pDataBuffR,bgr[2].data,pHeight*pWidth*sizeof(PTYPE));
    //Mat SingleImage(pHeight,pWidth,CV_16UC1,pDataBuffR);
    //imwrite("/sdcard/SingleImage.jpg",SingleImage);
    LOGE("readRgbFile has been finished!");
    return 0;
}

INT32 readPgmFile(PTYPE *pDataBuffB,PTYPE *pDataBuffG,PTYPE *pDataBuffR,
                INT32 *pWidth, INT32 *pHeight, INT32 depth, INT8 *pInFileName)
{
    FILE      *fp           = NULL;
    INT32 char1, char2, w, h, max, c1, c2, c3, x, y;
    PTYPE val;
    fp = fopen(pInFileName, "rb");

    if (fp  == NULL)
    {
        LOGE("Fail open file %s in readPgmFile. \n", pInFileName);
        return -1;
    }
    
    char1 = fgetc(fp);
    char2 = fgetc(fp);
    skipComments(fp);
    c1 = fscanf(fp, "%d", &w);
    skipComments(fp);
    c2 = fscanf(fp, "%d", &h);
    skipComments(fp);
    c3 = fscanf(fp, "%d", &max);

    if (char1 != 'P' || char2 != '5' || 
        c1 != 1 || c2 != 1 || c3 != 1 || max!= 65535) {
            LOGE("Input is not a standard raw16 PGM file.\n");
            return -1;
    }
    
    *pWidth = w;
    *pHeight = h;
    //LOGE("readPgmFile:pgm picture width = %d, height = %d \n",w,h);

    fgetc(fp);  /* Discard exactly one byte after header. */

    //BGGR->rgb
    for (y =0; y < h; y++) {
        for (x = 0; x < w; x++) {
            val = (PTYPE) fgetc(fp)<<8;
            val |= (PTYPE) fgetc(fp);
            val = val>>(16-depth);

            if ((x&0x1)&&(y&0x01))
                *(pDataBuffR + (y/2) * w/2 + (x/2)) = val;
            else if((x&0x1)&&!(y&0x01))
                 *(pDataBuffG + (y/2) * w/2 + (x/2)) = val; //B"G"GR
            else if(!(x&0x1)&&!(y&0x01))
                 *(pDataBuffB + (y/2) * w/2 + (x/2)) = val;
        }
    }

    if(fp != NULL)
    {
        fclose(fp);
    }
    return 0;
}


INT32 renamePgmFile(INT8 *pDstName, INT8 *pSrcName)
{
    INT32 strLen = strlen(pSrcName);
    if(strLen < 4)
    {
        return -1;
    }
    strcpy(pDstName, pSrcName);
    pDstName[strLen-4]='.';
    pDstName[strLen-3]='p';
    pDstName[strLen-2]='g';
    pDstName[strLen-1]='m';
    return 0;
}

INT32 writeSingleChannelToFile(  PTYPE *pDataBuff, INT32      width, INT32 height, INT8 *pOutFileName, INT32 bitdepth)
{
    FILE      *fp           = NULL;
    INT32     i;
    
    FWRITE_MODE fwriteMode = FWRITE_W;
    INT8      newOutputFileName[MAX_FILENAME_LEN];

    renamePgmFile(newOutputFileName, pOutFileName);
    if(fwriteMode == FWRITE_W)
    {
        fp = fopen(newOutputFileName, "wb");
    }
    else
    {
        fp = fopen(newOutputFileName, "ab");
    }

    if (fp  == NULL)
    {
        LOGE("Fail open file %s in writeAlignedBufferToFile. \n", pOutFileName);
        return -1;
    }

    switch(bitdepth)
    {
    case 8:
        fprintf(fp, "P5\n %d %d\n%d\n", width, height, BIT_MASK(bitdepth));

        for(i = 0; i < width*height; i++)
        {
            UINT8 tmpVal = (UINT8)(pDataBuff[i]);
            fwrite(&tmpVal, sizeof(UINT8), 1, fp);
        }
        break;
    case 12:
    case 14:
    case 16:
        fprintf(fp, "P5\n %d %d\n%d\n", width, height, BIT_MASK(16));

        for(i = 0; i < width*height; i++)
        {
            UINT16 tmpVal = pDataBuff[i]<<(16-bitdepth);
            tmpVal = ((tmpVal & 0xff)<<8) | ((tmpVal & 0xff00)>>8); //[low-bits high-bits]
            fwrite(&tmpVal, sizeof(UINT16), 1, fp);
        }
        break;

    default:
        LOGE("Unsupport File Format in writeAlignedBufferToFile !\n ");
        break;
    }

    if(fp != NULL)
    {
        fclose(fp);
    }
    return 0;
}

//TODO: pChannelWStrideBuf unused
//only support IMG_FORMAT_YCbCr420_888 raw data, used as YCbCr420 or 444(UV copy) in CModel
int readRawYUVSeq(INT32 *pChannelWidthBuf,
                            INT32 *pChannelHeightBuf,
                            INT32 *pChannelWStrideBuf,
                            PTYPE **ppChannelBuff,
                            INT8  *pInFileName,
                            IMG_FORMAT format,
                            int seekFrame,
                            int bSemiPlanar)
{
    FILE *fp;
    int ret = -1;
    int i, j;
    int framesize;

    RECT clipRect; //reserved
    int clipWidth; //reserved

    INT32     YrgbWidth   = pChannelWidthBuf[CHANNEL_RY];  //=pixWidth
    INT32     YrgbHeight  = pChannelHeightBuf[CHANNEL_RY];
    //INT32     YrgbWStride = pChannelWStrideBuf[CHANNEL_RY];

    PTYPE   *pChannelBuffRY  = ppChannelBuff[CHANNEL_RY];
    PTYPE   *pChannelBuffGCb = ppChannelBuff[CHANNEL_GCb];
    PTYPE   *pChannelBuffBCr = ppChannelBuff[CHANNEL_BCr];

    int retNum;
    int bUVcopy = (format == IMG_FORMAT_YCbCr444_888);

    clipRect.left = 0;
    clipRect.top  = 0;
    clipRect.right = YrgbWidth;
    clipRect.down  = YrgbHeight;

    clipWidth = clipRect.right - clipRect.left;

    if (format != IMG_FORMAT_YCbCr420_888 && format != IMG_FORMAT_YCbCr444_888)
    {
        LOGE("YUV Format unsupported file: %s.\n", pInFileName );
        return -1;
    }

    if (!(fp=fopen( pInFileName, "rb" ))) //yuv420 Input File
    {
        LOGE("Cannot open file: %s.\n", pInFileName );
        return -1;
    }

    framesize = YrgbWidth*YrgbHeight;

    for (i = clipRect.top; i < clipRect.down; i++)
    {
        FSEEK64(fp, (mINT64)seekFrame*framesize*3/2 + i*YrgbWidth + clipRect.left, SEEK_SET);
        for (j = clipRect.left; j < clipRect.right; j++)
        {
            retNum = fread(pChannelBuffRY++, sizeof(unsigned char), 1, fp);
            //assert(retNum == 1);
        }
    }

    if(!bSemiPlanar)
    {
        for (i = clipRect.top/2; i < clipRect.down/2; i++)
        {
            FSEEK64(fp, (mINT64)seekFrame*framesize*3/2 + framesize + (i*YrgbWidth + clipRect.left)/2, SEEK_SET);
            for (j = clipRect.left/2; j < clipRect.right/2; j++)
            {
                unsigned char pixval;
                retNum = fread(&pixval, sizeof(unsigned char), 1, fp);                
                //assert(retNum == 1);
                if(!bUVcopy)
                {
                    *pChannelBuffGCb = pixval;
                    pChannelBuffGCb++;
                }
                else
                {
                    *pChannelBuffGCb = *(pChannelBuffGCb+1) = pixval;
                    *(pChannelBuffGCb+clipWidth) = *(pChannelBuffGCb+clipWidth+1) = pixval;
                    pChannelBuffGCb += 2;
                }
            }
            if(bUVcopy)
                pChannelBuffGCb += clipWidth;

        }
        for (i = clipRect.top/2; i < clipRect.down/2; i++)
        {
            FSEEK64(fp, (mINT64)seekFrame*framesize*3/2 + framesize*5/4 + (i*YrgbWidth + clipRect.left)/2, SEEK_SET);
            for (j = clipRect.left/2; j < clipRect.right/2; j++)
            {
                unsigned char pixval;
                retNum = fread(&pixval, sizeof(unsigned char), 1, fp);
                //assert(retNum == 1);
                if(!bUVcopy)
                {
                    *pChannelBuffBCr = pixval;
                    pChannelBuffBCr++;
                }
                else
                {
                    *pChannelBuffBCr = *(pChannelBuffBCr+1) = pixval;
                    *(pChannelBuffBCr+clipWidth) = *(pChannelBuffBCr+clipWidth+1) = pixval;
                    pChannelBuffBCr += 2;
                }
            }
            if(bUVcopy)
                pChannelBuffBCr += clipWidth;
        }        
    }
    else
    {
        for (i = clipRect.top/2; i < clipRect.down/2; i++)
        {
            FSEEK64(fp, (mINT64)seekFrame*framesize*3/2 + framesize + (i*YrgbWidth + clipRect.left), SEEK_SET);
            for (j = clipRect.left/2; j < clipRect.right/2; j++)
            {
                char pixvalCb, pixValCr;
                retNum = fread(&pixvalCb, sizeof(char), 1, fp);
                //assert(retNum == 1);
                retNum = fread(&pixValCr, sizeof(char), 1, fp);
                //assert(retNum == 1);
                if(!bUVcopy)
                {
                    *pChannelBuffGCb = pixvalCb;
                    pChannelBuffGCb++;
                    *pChannelBuffBCr = pixValCr;
                    pChannelBuffBCr++;
                }
                else
                {
                    *pChannelBuffGCb = *(pChannelBuffGCb+1) = pixvalCb;
                    *(pChannelBuffGCb+clipWidth) = *(pChannelBuffGCb+clipWidth+1) = pixvalCb;
                    pChannelBuffGCb+=2;
                    *pChannelBuffBCr = *(pChannelBuffBCr+1) = pixValCr;
                    *(pChannelBuffBCr+clipWidth) = *(pChannelBuffBCr+clipWidth+1) = pixValCr;
                    pChannelBuffBCr+=2;
                }
            }
            if(bUVcopy)
            {
                pChannelBuffGCb += clipWidth;
                pChannelBuffBCr += clipWidth;
            }
        }
    }

    fclose(fp);
    return 0;
}

int writeRawYUVSeq(INT32 *pChannelWidthBuf,
                    INT32 *pChannelHeightBuf,
                    INT32 *pChannelWStrideBuf,
                    PTYPE **ppChannelBuff,
                    INT8  *pInFileName,
                    IMG_FORMAT format,
                    int seekFrame)
{
    FILE *fp;
    int ret = -1;
    int i, j;
    int framesize;

    RECT clipRect; //reserved
    int clipWidth; //reserved

    INT32     YrgbWidth   = pChannelWidthBuf[CHANNEL_RY];  //=pixWidth
    INT32     YrgbHeight  = pChannelHeightBuf[CHANNEL_RY];
    //INT32     YrgbWStride = pChannelWStrideBuf[CHANNEL_RY];

    PTYPE   *pChannelBuffRY  = ppChannelBuff[CHANNEL_RY];
    PTYPE   *pChannelBuffGCb = ppChannelBuff[CHANNEL_GCb];
    PTYPE   *pChannelBuffBCr = ppChannelBuff[CHANNEL_BCr];

    int retNum;

    clipRect.left = 0;
    clipRect.top  = 0;
    clipRect.right = YrgbWidth;
    clipRect.down  = YrgbHeight;

    clipWidth = clipRect.right - clipRect.left;

    if (format != IMG_FORMAT_YCbCr444_888 && format != IMG_FORMAT_YCbCr420_888)
    {
        LOGE("YUV Format unsupported file: %s.\n", pInFileName );
        return -1;
    }

    if (!(fp=fopen( pInFileName, "ab" )))
    {
        LOGE("Cannot open file: %s.\n", pInFileName );
        return -1;
    }

    framesize = YrgbWidth*YrgbHeight;

    if(format == IMG_FORMAT_YCbCr420_888) //YUV420p
    {
        for (i = clipRect.top; i < clipRect.down; i++)
        {
            FSEEK64(fp, (mINT64)seekFrame*framesize*3/2 + i*YrgbWidth + clipRect.left, SEEK_SET);
            for (j = clipRect.left; j < clipRect.right; j++)
            {
                retNum = fwrite(pChannelBuffRY++, sizeof(char), 1, fp);
                //assert(retNum == 1);
            }
        }

        for (i = clipRect.top/2; i < clipRect.down/2; i++)
        {
            FSEEK64(fp, (mINT64)seekFrame*framesize*3/2 + framesize + (i*YrgbWidth + clipRect.left)/2, SEEK_SET);
            for (j = clipRect.left/2; j < clipRect.right/2; j++)
            {
                retNum = fwrite(pChannelBuffGCb++, sizeof(char), 1, fp);
                //assert(retNum == 1);
            }
        }
        for (i = clipRect.top/2; i < clipRect.down/2; i++)
        {
            FSEEK64(fp, (mINT64)seekFrame*framesize*3/2 + framesize*5/4 + (i*YrgbWidth + clipRect.left)/2, SEEK_SET);
            for (j = clipRect.left/2; j < clipRect.right/2; j++)
            {
                retNum = fwrite(pChannelBuffBCr++, sizeof(char), 1, fp);
                //assert(retNum == 1);
            }
        }        
    }
    else //YUV444
    {
        for (i = clipRect.top; i < clipRect.down; i++)
        {
            FSEEK64(fp, (mINT64)seekFrame*framesize*3 + i*YrgbWidth + clipRect.left, SEEK_SET);
            for (j = clipRect.left; j < clipRect.right; j++)
            {
                retNum = fwrite(pChannelBuffRY++, sizeof(char), 1, fp);
                //assert(retNum == 1);
            }
        }

        for (i = clipRect.top; i < clipRect.down; i++)
        {
            FSEEK64(fp, (mINT64)seekFrame*framesize*3 + framesize + (i*YrgbWidth + clipRect.left), SEEK_SET);
            for (j = clipRect.left; j < clipRect.right; j++)
            {
                retNum = fwrite(pChannelBuffGCb++, sizeof(char), 1, fp);
                //assert(retNum == 1);
            }
        }
        for (i = clipRect.top; i < clipRect.down; i++)
        {
            FSEEK64(fp, (mINT64)seekFrame*framesize*3 + framesize*2 + (i*YrgbWidth + clipRect.left), SEEK_SET);
            for (j = clipRect.left; j < clipRect.right; j++)
            {
                retNum = fwrite(pChannelBuffBCr++, sizeof(char), 1, fp);
                //assert(retNum == 1);
            }
        } 
    }

    fclose(fp);
    return 0;
}


