#include<jni.h>
#include"com_hth_autoselfdrivingdrone_MainActivity.h"
#include<opencv2/opencv.hpp>

using namespace cv;

extern "C"{

JNIEXPORT void JNICALL
Java_com_hth_autoselfdrivingdrone_MainActivity_ConvertRGBtoGray(
        JNIEnv *env,
        jobject  instance,
        jlong matAddrInput,
        jlong matAddrResult){


    Mat &matInput = *(Mat *)matAddrInput;
    Mat &matResult = *(Mat *)matAddrResult;

    cvtColor(matInput, matResult, CV_RGBA2GRAY);


}
}