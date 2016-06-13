#include "myjni.h"
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "include/wdrProcess.h"


/*
JNIEXPORT jlong JNICALL processing(JNIEnv *env, jobject obj)
{
    jfieldID  nameFieldId ;
    jclass cls = env->GetObjectClass(obj);  //获得Java层该对象实例的类引用，即HelloJNI类引用
    nameFieldId = env->GetFieldID(cls ,"time", "D"); //获得属性句柄
    if(nameFieldId == NULL)
    {
        LOGE("LOGE: 没有得到 TIME 的句柄ID \n");
    }

    //wdrProcess();
    double gTime = 0;
    long imgData = 0;
    env->SetDoubleField(obj,nameFieldId ,gTime); // 设置该字段的值
    return (jlong)imgData;
}
*/


static const char *className = "com/example/linqi/wdr/NdkUtils";

//定义方法隐射关系
static JNINativeMethod methods[] = {
      //  {"processing","()J",(void*)processing},
};

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
//声明变量
    jint result = JNI_ERR;
    JNIEnv* env = NULL;
    jclass clazz;
    int methodsLenght;

//获取JNI环境对象
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed\n");
        return JNI_ERR;
    }
    assert(env != NULL);

//注册本地方法.Load 目标类
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGE("Native registration unable to find class '%s'", className);
        return JNI_ERR;
    }

//建立方法隐射关系
//取得方法长度
    methodsLenght = sizeof(methods) / sizeof(methods[0]);
    if (env->RegisterNatives(clazz, methods, methodsLenght) < 0) {
        LOGE("RegisterNatives failed for '%s'", className);
        return JNI_ERR;
    }

    result = JNI_VERSION_1_4;
    return result;
}

jint JNI_Unload(JavaVM* vm,void* reserved)
{
    jint result = JNI_ERR;
    JNIEnv* env = NULL;

    LOGI("JNI_OnUnload!");
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed");
        return JNI_ERR;
    }
    result = JNI_VERSION_1_4;
    return result;
}


