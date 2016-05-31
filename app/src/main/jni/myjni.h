#include <jni.h>
#include "string.h"
#include "assert.h"
#include "log.h"

#ifndef MY_JNI_MYJNI_H
#define MY_JNI_MYJNI_H
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL processing(JNIEnv *env, jobject obj);


#ifdef __cplusplus
}
#endif

#endif //MY_JNI_MYJNI_H
