//
// DianPose.cpp
// Created by 周瑞松 on 2020/7/25.
// Copyright © 2020 周瑞松. All rights reserved.
//


#include <jni.h>
#include <string>
#include "DianPose.h"
#include <opencv2/opencv.hpp>
#include <math.h>


extern "C" {
JNIEXPORT void JNICALL
Java_com_dian_dianpose_PoseTracking_run(JNIEnv *env, jobject obj, jbyteArray yuv,
                                        jint height, jint width, jlong handle, jint isFlip) {
    jbyte *pBuf = (jbyte *) env->GetByteArrayElements(yuv, 0);
    cv::Mat image(height, width, CV_8UC4, (unsigned char *) pBuf);
//    cv::Mat mBgr;
//    cv::cvtColor(image, mBgr, CV_RGBA2RGB);
    DianPose *trackingSession = (DianPose *) handle;
    trackingSession->run(image);
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
    arr[1] = round(info.x2);
    arr[2] = round(info.y1);
    arr[3] = round(info.y2);
    env->ReleaseIntArrayElements(jarr, arr, 0);
    return jarr;
}


JNIEXPORT jlong JNICALL
Java_com_dian_dianpose_PoseTracking_createSession(JNIEnv *env, jobject obj, jint thread_num) {

    DianPose *model = new DianPose(thread_num);
    return (jlong) model;

}


JNIEXPORT void JNICALL
Java_com_dian_dianpose_PoseTracking_releaseSession(JNIEnv *env, jobject obj, jlong handle) {
    DianPose *trackingSession = (DianPose *) handle;
    delete trackingSession;
}

}
