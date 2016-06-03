#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__


typedef int            INT32;
typedef unsigned int   UINT32;
typedef short          INT16;
typedef unsigned short UINT16;
typedef char           INT8;
typedef unsigned char  UINT8;
typedef void           VOID;
typedef unsigned long long  LONG;

typedef unsigned short PTYPE;  //single color channel(R/G/B/Y/U/V) size

typedef struct _ARGB{
    PTYPE  b;//cb
    PTYPE  g;//y
    PTYPE  r;//cr
    PTYPE  a;
}ARGB;

typedef struct _RECT {
    INT32  left;
    INT32  top;
    INT32  right;
    INT32  down;
}RECT;

typedef enum _IMG_FORMAT {
    IMG_FORMAT_ARGB888 = 0,
    IMG_FORMAT_RGB888,
    IMG_FORMAT_RGB565,
    IMG_FORMAT_RGB666,
    IMG_FORMAT_1bpp,
    IMG_FORMAT_2bpp,
    IMG_FORMAT_4bpp,
    IMG_FORMAT_8bpp,
    IMG_FORMAT_RGB101010,
    IMG_FORMAT_RGB121212,
    IMG_FORMAT_RGB_END,
    IMG_FORMAT_YCbCr420_888,
    IMG_FORMAT_YCbCr422_888,
    IMG_FORMAT_YCbCr444_888,
	IMG_FORMAT_YVYU422,
	IMG_FORMAT_YVYU420,
	IMG_FORMAT_VYUY422,
	IMG_FORMAT_VYUY420,
    IMG_FORMAT_YCbCr888_END,
    IMG_FORMAT_YCbCr420_101010,  //saved in 16bit, with HBits=0
    IMG_FORMAT_YCbCr422_101010,
    IMG_FORMAT_YCbCr444_101010,
    IMG_FORMAT_COUNT
}IMG_FORMAT;

typedef enum _COLOR_CHANNEL_TYPE {
    CHANNEL_TYPE_Y_RGB = 0,
    CHANNEL_TYPE_CBCR,
    CHANNEL_TYPE_COUNT
}COLOR_CHANNEL_TYPE;

typedef enum _CHANNEL_INDEX_TYPE {
    CHANNEL_RY = 0,
    CHANNEL_GCb,
    CHANNEL_BCr,
    CHANNEL_A,
    CHANNEL_INDEX_COUNT
}CHANNEL_INDEX_TYPE;

//for 8bpp palette
typedef enum _ENDIAN_MODE {
    ENDIAN_BIG = 0,
    ENDIAN_LITTLE
}ENDIAN_MODE;

//Color space conversion
typedef enum _CSC_MODE {
    CSC_YUV2RGB_MPEG = 0,  //win 0 1
    CSC_YUV2RGB_BT2020,
    CSC_YUV2RGB_JPEG,
    CSC_YUV2RGB_HD
}CSC_MODE;

typedef enum _IO_FILE_FORMAT
{
    IO_FILE_FORMAT_ASCII_48BIT_PER_LINE,
    IO_FILE_FORMAT_RAW_DATA,
    IO_FILE_FORMAT_BMP
}IO_FILE_FORMAT;

typedef enum _DUMP_PIX_FORMAT
{
    DUMP_NORMAL      = 0,
    DUMP_ALPHA_EN    = 1,
    DUMP_CCIR656_444 = 2,
    DUMP_HDMI_420    = 3,
    DUMP_DP_422      = 4,
    DUMP_DP_420      = 5
}DUMP_PIX_FORMAT;

#endif
