#include<jni.h>
#include"com_hth_autoselfdrivingdrone_MainActivity.h"
#include<opencv2/opencv.hpp>

using namespace cv;

extern "C"{

JNIEXPORT jdoubleArray JNICALL
Java_com_hth_autoselfdrivingdrone_MainActivity_detectFromDarknet(
        JNIEnv *env,
        jobject  instance,
        jlong matAddrInput){

    jdoubleArray returnValue = env->NewDoubleArray(4);
    if(returnValue == NULL)
        return NULL;

    jdouble control[4];
    Mat &matInput = *(Mat *)matAddrInput;




    for(int i=0;i<4;++i){
        env->SetDoubleArrayRegion(returnValue,0,4,control);
    }
    return returnValue;
}
}