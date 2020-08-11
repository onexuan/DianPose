//
// DianPose.cpp
// Created by 周瑞松 on 2020/7/25.
// Copyright © 2020 周瑞松. All rights reserved.
//


#include <jni.h>
#include <string>
#include "DianPose.h"
#include <opencv2/opencv.hpp>
#include <cmath>

#include <android/log.h>
#define TAG "DianPose"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)


std::string jstring2str(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    std::string stemp(rtn);
    free(rtn);
    return stemp;
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_dian_dianpose_PoseTracking_run(JNIEnv *env, jobject obj, jbyteArray rgba,
                                        jint height, jint width, jlong handle, jint isFlip) {
    jbyte *pBuf = (jbyte *) env->GetByteArrayElements(rgba, 0);
    cv::Mat image(height, width, CV_8UC4, (unsigned char *) pBuf);
    cv::Mat mBgr;
    cv::cvtColor(image, mBgr, CV_RGBA2RGB);
    DianPose *trackingSession = (DianPose *) handle;
    trackingSession->run(mBgr);
}


JNIEXPORT jint JNICALL
Java_com_dian_dianpose_PoseTracking_getTrackingNum(JNIEnv *env, jobject obj, jlong handle) {
    DianPose *trackingSession = (DianPose *) handle;
    return (jint) trackingSession->trackPersons.size();

}


JNIEXPORT jintArray JNICALL
Java_com_dian_dianpose_PoseTracking_getTrackingPoseKeypointsByIndex(JNIEnv *env, jobject obj,
                                                                    jint target, jlong handle) {
    DianPose *trackingSession = (DianPose *) handle;
    jintArray jarr = env->NewIntArray(17 * 2);
    jint *arr = env->GetIntArrayElements(jarr, NULL);
    const PersonInfo &info = trackingSession->trackPersons[target];
    int i = 0;
    for (; i < 34; i++) {
        arr[i] = round(info.keypoints[i]);
    }
    env->ReleaseIntArrayElements(jarr, arr, 0);
    return jarr;
}

JNIEXPORT jfloatArray JNICALL
Java_com_dian_dianpose_PoseTracking_getTrackingPoseConfidenceByIndex(JNIEnv *env, jobject obj,
                                                                     jint target, jlong handle) {
    DianPose *trackingSession = (DianPose *) handle;
    jfloatArray jarr = env->NewFloatArray(17);
    jfloat *arr = env->GetFloatArrayElements(jarr, NULL);
    const PersonInfo &info = trackingSession->trackPersons[target];
    int i = 0;
    for (; i < 17; i++) {
        arr[i] = round(info.keypoints_score[i]);
    }
    env->ReleaseFloatArrayElements(jarr, arr, 0);
    return jarr;
}

JNIEXPORT jintArray JNICALL
Java_com_dian_dianpose_PoseTracking_getTrackingLocationByIndex(JNIEnv *env, jobject obj,
                                                               jint target, jlong handle) {
    DianPose *trackingSession = (DianPose *) handle;
    jintArray jarr = env->NewIntArray(4);
    jint *arr = env->GetIntArrayElements(jarr, NULL);
    const PersonInfo &info = trackingSession->trackPersons[target];

    arr[0] = round(info.x1);
    arr[1] = round(info.y1);
    arr[2] = round(info.x2);
    arr[3] = round(info.y2);
    env->ReleaseIntArrayElements(jarr, arr, 0);
    return jarr;
}


JNIEXPORT jlong JNICALL
Java_com_dian_dianpose_PoseTracking_createSession(JNIEnv *env, jobject obj, jint thread_num, jstring folder) {
    std::string detector_path = jstring2str(env, folder);
    DianPose *model = new DianPose(thread_num);
    return (jlong) model;

}


JNIEXPORT void JNICALL
Java_com_dian_dianpose_PoseTracking_releaseSession(JNIEnv *env, jobject obj, jlong handle) {
    DianPose *trackingSession = (DianPose *) handle;
    delete trackingSession;
}

}
