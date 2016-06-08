LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
#OPENCV_LIB_TYPE := SHARED -lm -ljnigraphics -lui
include /opt/android-opencv-sdk/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk

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
	src/regConfig.cpp

LOCAL_C_INCLUDES := /opt/android-opencv-sdk/OpenCV-android-sdk/sdk/native/jni/include
LOCAL_C_INCLUDES += include
LOCAL_C_INCLUDES += ./

include $(BUILD_SHARED_LIBRARY)
include $(CLEAR_VARS)
include /opt/android-opencv-sdk/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk
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
	src/regConfig.cpp

LOCAL_C_INCLUDES := /opt/android-opencv-sdk/OpenCV-android-sdk/sdk/native/jni/include
LOCAL_C_INCLUDES += include
LOCAL_C_INCLUDES += ./

LOCAL_MODULE := WDRbin
LOCAL_LDLIBS := -llog -landroid
include $(BUILD_EXECUTABLE)