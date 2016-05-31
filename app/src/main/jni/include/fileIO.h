#ifndef __FILEIO_H__
#define __FILEIO_H__

//yuv10bit input file format:
//  DEFAULT:  10bit compact
//If your input format is 16bit per pixel(zero padding high bits), uncomment the following
//#define IMPORT_EXPANDED_10BIT

#include <sys/stat.h>
#include "include/dataType.h"
#ifndef WIN32
#define STAT64 stat64
#define FSEEK64 fseeko64
#define FILEPATH_SLASH '/'
#else
#define STAT64 _stat64
#define FSEEK64 _fseeki64
#define FILEPATH_SLASH '\\'



//Following Macro is ONLY for C Model Debug
//direct load one YUV420p bin file, instead of two ASCII files(Y + UV)
//direct load ARGB .bmp file
#define LOAD_RAW_DATA   

#endif

typedef enum _FWRITE_MODE
{
    FWRITE_W = 0,   //from the file head
    FWRITE_ADD      //follow the existing file
}FWRITE_MODE;

INT32 writeAlignedBufferToFile(UINT16 *pDataBuff, INT32 sizeInHlfWord, INT8 *pOutFileName, IO_FILE_FORMAT outFileFormat,
                            FWRITE_MODE fwriteMode);

INT32 loadChannelData(IMG_FORMAT format,
                            INT32 *pChannelWidthBuf,
                            INT32 *pChannelHeightBuf,
                            INT32 *pChannelWStrideBuf,
                            PTYPE **ppChannelBuff,
                            INT8  *pInFileName, 
                            IO_FILE_FORMAT inFileFormat);


INT32 dumpChannelData(IMG_FORMAT format,
                            INT32 *pChannelWidthBuf,
                            INT32 *pChannelHeightBuf,
                            INT32 *pChannelWStrideBuf,
                            PTYPE **ppChannelBuff,
                            INT8  *pOutFileName,
                            IO_FILE_FORMAT outFileFormat);

INT32 dumpPixelsDataToBmpFile(IMG_FORMAT format,
                            INT32 *pChannelWidthBuf,
                            INT32 *pChannelHeightBuf,
                            INT32 *pChannelWStrideBuf,
                            PTYPE **ppChannelBuff,
                            INT8  *pOutFileName,
                            RECT  *pClipRect,
                            INT8 gammaCorrect);

#endif
