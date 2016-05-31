#include "include/global.h"
#include "include/dataType.h"
#include "include/fileIO.h"
#include "include/fileParser.h"
#include "log.h"
#include <math.h>


INT8 imgFormatNameBuf[IMG_FORMAT_COUNT][20] =
{
    "RGB888",
    "ARGB888",
    "RGB565",
    "RGB666",
    "1bpp",
    "2bpp",
    "4bpp",
    "8bpp",
    "RGB101010",
    "RGB_END",
    "YCbCr420_888",
    "YCbCr422_888",
    "YCbCr444_888",
    "YVYU422",
    "YVYU420",
    "VYUY422",
    "VYUY420",
    "YCbCr888_END",
    "YCbCr420_101010",
    "YCbCr422_101010",
    "YCbCr444_101010"
};

/******************************************************************************/

INT32 readAlignedFileToBuffer(UINT16 *pDataBuff, INT32 sizeInHlfWord, INT8 *pInFileName, IO_FILE_FORMAT inFileFormat)
{
    FILE      *fp       = NULL;
    UINT32    temp;
    INT32     i, ret;

    fp = fopen(pInFileName, "r");

    if (fp == NULL)
    {
        LOGE("Fail open file %s in readAlignedFileToBuffer. \n", pInFileName);
        return -1;
    }
    
    switch(inFileFormat)
    {
        case IO_FILE_FORMAT_ASCII_48BIT_PER_LINE:
            for (i = 0; i < sizeInHlfWord; i++)
            {
                ret = fscanf(fp,"%4x", &temp);

                if(ret > 0)
                {
                    pDataBuff[i] = (UINT16)temp;
                }
                else
                {
                    LOGE("End of file in readAlignedFileToBuffer \n");
                    fclose(fp);
                    EXIT(-1);
                }
            }
            break;

        default:
            LOGE("Unsupport File Format in writeAlignedBufferToFile !\n ");
            break;
    }
    
    fclose(fp);
    return 0;
}

INT32 writeAlignedBufferToFile(UINT16 *pDataBuff, INT32 sizeInHlfWord, INT8 *pOutFileName, IO_FILE_FORMAT outFileFormat,
                                      FWRITE_MODE fwriteMode)
{
    FILE      *fp           = NULL;
    INT32     lineIndex;
    UINT32    temp;
    INT32     i;

    switch(outFileFormat)
    {
        case IO_FILE_FORMAT_ASCII_48BIT_PER_LINE:

            if(fwriteMode == FWRITE_W)
            {
                fp = fopen(pOutFileName, "w");
            }
            else
            {
                fp = fopen(pOutFileName, "a");
            }

            if (fp  == NULL)
            {
                LOGE("Fail open file %s in writeAlignedBufferToFile. \n", pOutFileName);
                return -1;
            }

            for (lineIndex = 0; lineIndex < sizeInHlfWord/3; lineIndex++)
            {
                for(i = 0; i < 3; i++)
                {
                    temp = pDataBuff[3*lineIndex + i];                    
                    fprintf(fp,"%.4x",temp);
                }
                fprintf(fp,"\n");
            }
            break;

        case IO_FILE_FORMAT_RAW_DATA:
            if(fwriteMode == FWRITE_W)
            {
                fp = fopen(pOutFileName, "wb");
            }
            else
            {
                fp = fopen(pOutFileName, "ab");
            }

            if (fp  == NULL)
            {
                LOGE("Fail open file %s in writeAlignedBufferToFile. \n", pOutFileName);
                return -1;
            }

            fwrite(pDataBuff, sizeof(INT16), sizeInHlfWord, fp);
            break;

        default:
            LOGE("Unsupport File Format in writeAlignedBufferToFile !\n ");
            break;
    }

    fclose(fp);
    return 0;
}

// INPUT FILE: 6Byte(one pixel) PerLine, ASCII
//
// B=0x89a, G=0x9ab, R=0xabc
// txt file:  089a09ab0abc

INT32 loadChannelData(IMG_FORMAT format,
                            INT32 *pChannelWidthBuf,
                            INT32 *pChannelHeightBuf,
                            INT32 *pChannelWStrideBuf,
                            PTYPE **ppChannelBuff,
                            INT8  *pInFileName, 
                            IO_FILE_FORMAT inFileFormat)
{
    UINT16    *pYrgbBuf     = NULL;

    INT32     YrgbWidth   = pChannelWidthBuf[CHANNEL_RY];  //=pixWidth
    INT32     YrgbHeight  = pChannelHeightBuf[CHANNEL_RY];
    //INT32     YrgbWStride = pChannelWStrideBuf[CHANNEL_RY];

    PTYPE     *pChannelBuffRY  = ppChannelBuff[CHANNEL_RY];
    PTYPE     *pChannelBuffGCb = ppChannelBuff[CHANNEL_GCb];
    PTYPE     *pChannelBuffBCr = ppChannelBuff[CHANNEL_BCr];

    INT32     pixSize, pixOffset;

#ifdef LOAD_RAW_DATA    
    static int frameNum = 0;
#endif

#ifdef LOAD_RAW_DATA
    if((format == IMG_FORMAT_RGB101010 || format == IMG_FORMAT_RGB121212 ) && (strstr(pInFileName, ".pgm") != NULL))
    {
        INT32 w, h; //reserved for file check
        INT32 depth = (format == IMG_FORMAT_RGB101010)?10:12;
        return readPgmFile(ppChannelBuff[CHANNEL_BCr],ppChannelBuff[CHANNEL_GCb],ppChannelBuff[CHANNEL_RY], 
            &w, &h, depth, pInFileName);
    }
#endif

    /************************************************************/
    pixSize = YrgbWidth*YrgbHeight;
    MALLOC(pYrgbBuf, UINT16, pixSize*3);
    readAlignedFileToBuffer(pYrgbBuf, pixSize*3, pInFileName, inFileFormat);

    /************************************************************/
    switch (format)
    {
        case IMG_FORMAT_RGB121212:
        case IMG_FORMAT_RGB101010:
            for (pixOffset = 0; pixOffset < pixSize; pixOffset++)
            {
                pChannelBuffRY [pixOffset] = pYrgbBuf[pixOffset*3+2];
                pChannelBuffGCb[pixOffset] = pYrgbBuf[pixOffset*3+1];
                pChannelBuffBCr[pixOffset] = pYrgbBuf[pixOffset*3+0];
            }
            break;

        default:
            LOGE("Unsupport format %d in readChannelDatat \n", format);
            break;
    }

    FREE(pYrgbBuf);
    return 0;
}

INT32 dumpChannelData(IMG_FORMAT format,
                            INT32 *pChannelWidthBuf,
                            INT32 *pChannelHeightBuf,
                            INT32 *pChannelWStrideBuf,
                            PTYPE **ppChannelBuff,
                            INT8  *pOutFileName,
                            IO_FILE_FORMAT outFileFormat)
{
    UINT16    *pYrgbBuf     = NULL;

    INT32     YrgbWidth   = pChannelWidthBuf[CHANNEL_RY];  //=src PixWidth
    INT32     YrgbHeight  = pChannelHeightBuf[CHANNEL_RY];
    INT32     YrgbWStride = pChannelWStrideBuf[CHANNEL_RY];

    PTYPE     *pChannelBuffRY  = ppChannelBuff[CHANNEL_RY];
    PTYPE     *pChannelBuffGCb = ppChannelBuff[CHANNEL_GCb];
    PTYPE     *pChannelBuffBCr = ppChannelBuff[CHANNEL_BCr];

    INT32     pixSize,pixOffset;

#ifdef DUMP_RAW_DATA
    static int frameNum = 0;
#endif

#ifdef DUMP_RAW_DATA
    //dump raw binary .yuv444p file
    if ((format == IMG_FORMAT_YCbCr444_888) && (strstr(pOutFileName, ".yuv") != NULL))
    {        
        return writeRawYUVSeq(pChannelWidthBuf,
            pChannelHeightBuf,
            pChannelWStrideBuf,
            ppChannelBuff,
            pOutFileName,
            format,
            frameNum++//0
            );
    }
#endif

    /************************************************************/
    pixSize = YrgbWidth*YrgbHeight;
    MALLOC(pYrgbBuf, UINT16, pixSize*3);
    
    /************************************************************/

    switch (format)
    {
        case IMG_FORMAT_RGB121212:
        case IMG_FORMAT_RGB101010:
            for (pixOffset = 0; pixOffset < pixSize; pixOffset++)
            {
                pYrgbBuf[pixOffset*3+2] = pChannelBuffRY [pixOffset];
                pYrgbBuf[pixOffset*3+1] = pChannelBuffGCb[pixOffset];
                pYrgbBuf[pixOffset*3+0] = pChannelBuffBCr[pixOffset];
            }
            break;

        default:
            LOGE("Unsupport format %d in dumpChannelData \n", format);
            break;
    }

    /************************************************************/
    writeAlignedBufferToFile(pYrgbBuf, pixSize*3, pOutFileName, outFileFormat, FWRITE_W);
    
    FREE(pYrgbBuf);

    return 0;
}

INT32 renameBmpFile(INT8 *pDstName, INT8 *pSrcName)
{
    INT32 strLen = strlen(pSrcName);
    if(strLen < 4)
    {
        return -1;
    }
    strcpy(pDstName, pSrcName);
    pDstName[strLen-4]='.';
    pDstName[strLen-3]='b';
    pDstName[strLen-2]='m';
    pDstName[strLen-1]='p';
    return 0;
}

PTYPE getNonlinearRGB(PTYPE imVal, INT32 bitdepth)
{
    double val = (double)imVal/BIT_MASK(bitdepth);
    double ret;
    if(val < 0.018)
        ret = val*4.5;
    else
        ret = pow(val, 0.45) * 1.099 - 0.099;
    return ROUND_F(ret*BIT_MASK(bitdepth));
}

// OUTPUT FILE:
//      RGB888 bmp, with no alpha channel
// pClipRect = NULL : no clip
//gammaCorrect: linearRGB->NonlinearRGB
INT32 dumpPixelsDataToBmpFile(IMG_FORMAT format,
                            INT32 *pChannelWidthBuf,
                            INT32 *pChannelHeightBuf,
                            INT32 *pChannelWStrideBuf,
                            PTYPE **ppChannelBuff,
                            INT8  *pOutFileName,
                            RECT  *pClipRect,
                            INT8 gammaCorrect)
{
    INT32     YrgbWidth   = pChannelWidthBuf[CHANNEL_RY];  //=pixWidth
    INT32     YrgbHeight  = pChannelHeightBuf[CHANNEL_RY];
    INT32     YrgbWStride = pChannelWStrideBuf[CHANNEL_RY];

    PTYPE     *pChannelBuffA   = ppChannelBuff[CHANNEL_A];
    PTYPE     *pChannelBuffRY  = ppChannelBuff[CHANNEL_RY];
    PTYPE     *pChannelBuffGCb = ppChannelBuff[CHANNEL_GCb];
    PTYPE     *pChannelBuffBCr = ppChannelBuff[CHANNEL_BCr];

    INT32     x, y;
    INT32     pixOffset;
    PTYPE     r, g, b;
    INT32     stx, endx, sty, endy, size;
    UINT8     *pRGB888Buf = NULL;
    UINT8     *pCurRGBBuf = NULL;
    INT8      newOutputFileName[MAX_FILENAME_LEN];

    if(pClipRect == NULL)
    {
        stx = 0;
        sty = 0;
        endx = YrgbWidth;
        endy = YrgbHeight;
    }
    else
    {
        stx  = MAX(pClipRect->left, 0);
        sty  = MAX(pClipRect->top, 0);
        endx = MIN(pClipRect->right, YrgbWidth);
        endy = MIN(pClipRect->down, YrgbHeight);
    }

    size = (endx - stx)*(endy - sty)*3;
    MALLOC(pRGB888Buf, UINT8, size);
    pCurRGBBuf = pRGB888Buf;

    for (y = sty; y < endy; y++)
    {
        for (x = stx; x < endx; x++)
        {
            pixOffset = y * YrgbWStride + x;

            switch (format)
            {
                case IMG_FORMAT_ARGB888:
                case IMG_FORMAT_RGB888:
                    r = pChannelBuffRY[pixOffset];
                    g = pChannelBuffGCb[pixOffset];
                    b = pChannelBuffBCr[pixOffset];
                    break;

                case IMG_FORMAT_RGB101010:
                    if(gammaCorrect)
                    {
                        r = getNonlinearRGB(pChannelBuffRY[pixOffset], 10);
                        g = getNonlinearRGB(pChannelBuffGCb[pixOffset], 10);
                        b = getNonlinearRGB(pChannelBuffBCr[pixOffset], 10);
                    }
                    else
                    {
                        r = pChannelBuffRY[pixOffset];
                        g = pChannelBuffGCb[pixOffset];
                        b = pChannelBuffBCr[pixOffset];                    
                    }
                    r = (r>>2);
                    g = (g>>2);
                    b = (b>>2);
                    break;
                case IMG_FORMAT_RGB121212:
                    if(gammaCorrect)
                    {
                        r = getNonlinearRGB(pChannelBuffRY[pixOffset], 12);
                        g = getNonlinearRGB(pChannelBuffGCb[pixOffset], 12);
                        b = getNonlinearRGB(pChannelBuffBCr[pixOffset], 12);
                    }
                    else
                    {
                        r = pChannelBuffRY[pixOffset];
                        g = pChannelBuffGCb[pixOffset];
                        b = pChannelBuffBCr[pixOffset];                    
                    }
                    r = (r>>4);
                    g = (g>>4);
                    b = (b>>4);
                    break;
                default:
                    LOGE("Unsupport format %d in dumpPixelsDataToBmpFile \n", format);
                    FREE(pRGB888Buf);
                    return -1;
            }

            *pCurRGBBuf++ = (UINT8)b;
            *pCurRGBBuf++ = (UINT8)g;
            *pCurRGBBuf++ = (UINT8)r;
        }
    }

    renameBmpFile(newOutputFileName, pOutFileName);
    writeImageBmp_RGB24(pRGB888Buf, (endx - stx), (endy - sty), newOutputFileName, 0, 0);

    FREE(pRGB888Buf);
    return 0;
}

