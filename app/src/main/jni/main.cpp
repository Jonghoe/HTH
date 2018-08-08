#include<jni.h>
#include"com_hth_autoselfdrivingdrone_MainActivity.h"
#include<opencv2/opencv.hpp>
#include<opencv2/dnn.hpp>

using namespace cv;

extern "C"{

JNIEXPORT jdoubleArray JNICALL
Java_com_hth_autoselfdrivingdrone_MainActivity_detectFromDarknet(
        JNIEnv *env,
        jobject  instance,
        jlong matAddrInput){
/*
 *  YOLO 설정 값 들
 */
    std::string cfgFilePath="cfgFileName";
    std::string modelFilePath = "modelName";
    std::string classFilePath = "classFile";
    const float minConfidence =0.24;
//---------------------------------------------------------------------------------------------------------------------------------
    jdoubleArray returnValue = env->NewDoubleArray(4);
    if(returnValue == NULL)
        return NULL;

    jdouble control[4];
    Mat &matInput = *(Mat *)matAddrInput;

    dnn::Net net = dnn::readNetFromDarknet(cfgFilePath,modelFilePath);
    if(net.empty()){
        double retV[4]={-1,-1,-1,-1};
        env->SetDoubleArrayRegion(returnValue,0,4,retV);
        return returnValue;
    }

    // setting dnn

    //  get class name
    std::vector<String> classNamesVec;
    std::ifstream classNamesFile(classFilePath.c_str());
    if (classNamesFile.is_open())
    {
        std::string className = "";
        while (std::getline(classNamesFile, className))
            classNamesVec.push_back(className);
    }

    // get Input Image
    Mat& frame = *(Mat *)matAddrInput;
    if (frame.empty())
    {
        double retV[4]={-1,-1,-1,-1};
        env->SetDoubleArrayRegion(returnValue,0,4,retV);
        return returnValue;
    }
    if (frame.channels() == 4)
        cvtColor(frame, frame, COLOR_BGRA2BGR);

    //  make input blob;
    Mat inputBlob = dnn::blobFromImage(frame, 1 / 255.F, Size(416, 416), Scalar(), true, false); //Convert Mat to batch of images
    net.setInput(inputBlob, "data");                   //set the network input

    // get detection mat
    Mat detectionMat = net.forward("detection_out");   //compute output
    std::vector<double> layersTimings;
    double tick_freq = getTickFrequency();
    double time_ms = net.getPerfProfile(layersTimings) / tick_freq * 1000;
    float confidenceThreshold = minConfidence;

    //  detection models
    for (int i = 0; i < detectionMat.rows; i++)
    {
        const int probability_index = 5;
        const int probability_size = detectionMat.cols - probability_index;
        float *prob_array_ptr = &detectionMat.at<float>(i, probability_index);
        size_t objectClass = std::max_element(prob_array_ptr, prob_array_ptr + probability_size) - prob_array_ptr;
        float confidence = detectionMat.at<float>(i, (int)objectClass + probability_index);
        if (confidence > confidenceThreshold)
        {
            float x_center = detectionMat.at<float>(i, 0) * frame.cols;
            float y_center = detectionMat.at<float>(i, 1) * frame.rows;
            float width = detectionMat.at<float>(i, 2) * frame.cols;
            float height = detectionMat.at<float>(i, 3) * frame.rows;

            String className = objectClass < classNamesVec.size() ? classNamesVec[objectClass] : cv::format("unknown(%d)", objectClass);
            String label = format("%s: %.2f", className.c_str(), confidence);
            int baseLine = 0;
            Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        }
    }

    env->SetDoubleArrayRegion(returnValue,0,4,control);
    return returnValue;
}
}