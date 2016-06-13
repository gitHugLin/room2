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
	src/wdrBase.cpp \
    src/wdrProcess.cpp

LOCAL_C_INCLUDES := /opt/android-opencv-sdk/OpenCV-android-sdk/sdk/native/jni/include
LOCAL_C_INCLUDES += include
LOCAL_C_INCLUDES += ./

include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)
include /opt/android-opencv-sdk/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_MODULE := WDRbin
LOCAL_LDLIBS := -llog -landroid

LOCAL_SRC_FILES := \
	src/wdrBase.cpp \
    src/main.cpp

LOCAL_C_INCLUDES := /opt/android-opencv-sdk/OpenCV-android-sdk/sdk/native/jni/include
LOCAL_C_INCLUDES += include
LOCAL_C_INCLUDES += ./

include $(BUILD_EXECUTABLE)
