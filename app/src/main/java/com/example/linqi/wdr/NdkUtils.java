package com.example.linqi.wdr;



/**
 * Created by linqi on 16-1-18.
 */
public class NdkUtils {

    double time;
    NdkUtils() {
        time = 0;
    }
    public native long processing();

    static {
        System.loadLibrary("WDRLib");   //defaultConfig.ndk.moduleName
    }
}
