LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := WDRLib
LOCAL_LDFLAGS := -Wl,--build-id
LOCAL_LDLIBS := -llog -landroid -lGLESv2 -lEGL -lcutils -lui -lutils
LOCAL_CFLAGS :=  -DEGL_EGLEXT_PROTOTYPES -DGL_GLEXT_PROTOTYPES

LOCAL_SRC_FILES := \
	myjni.cpp \
	src/config0.cpp \
	src/config_public.cpp \
	src/fileIO.cpp \
	src/gauseFilter.cpp \
	src/wdrProcess.cpp \
	src/nonlinearCurve.cpp \
	src/toneMap.cpp \
	src/fileParser.cpp \
	src/regConfig.cpp \

LOCAL_C_INCLUDES += include
LOCAL_C_INCLUDES += ./

include $(BUILD_SHARED_LIBRARY)
