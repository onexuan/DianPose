//
// DianPose.cpp
// Created by 周瑞松 on 2020/7/25.
// Copyright © 2020 周瑞松. All rights reserved.
//
#include "DianPose.h"

#include <android/log.h>
#define TAG "DianPose"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)


DianPose::DianPose(int num_thread) {
    this->personModel = new PersonDetectionNet(num_thread);
    this->poseModel = new PoseNet(num_thread);
    detection_Time = -1;
    detection_Interval = 350; //detect faces every 200 ms
    first_run = true;
}

DianPose::~DianPose() {
    delete this->personModel;
    delete this->poseModel;
    trackPersons.clear();
}

void DianPose::detecting(cv::Mat &image) {
    detecting_lock = true;
    personModel->detect(image, trackPersons);
    for (auto &person:trackPersons) {
        LOGD("detected person %f %f %f %f",person.x1, person.y1, person.x2 - person.x1, person.y2 - person.y1);
        cv::Rect roi(person.x1, person.y1, person.x2 - person.x1, person.y2 - person.y1);
        image(roi).copyTo(person.frame_person_prev);
    }
    detecting_lock = false;
}

void DianPose::tracking_corrfilter(const cv::Mat &frame, const cv::Mat &model, cv::Rect &trackBox,
                                   float scale) {
    trackBox.x /= scale;
    trackBox.y /= scale;
    trackBox.height /= scale;
    trackBox.width /= scale;
    cv::Mat frame_;
    cv::Mat model_;
    cv::resize(frame, frame_, cv::Size(), 1 / scale, 1 / scale);
    cv::resize(model, model_, cv::Size(), 1 / scale, 1 / scale);
    cv::Mat gray;
    cvtColor(frame_, gray, CV_RGB2GRAY);
    cv::Mat gray_model;
    cvtColor(model_, gray_model, CV_RGB2GRAY);
    cv::Rect searchWindow;
    searchWindow.width = trackBox.width * 3;
    searchWindow.height = trackBox.height * 3;
    searchWindow.x = trackBox.x + trackBox.width * 0.5 - searchWindow.width * 0.5;
    searchWindow.y = trackBox.y + trackBox.height * 0.5 - searchWindow.height * 0.5;
    searchWindow &= cv::Rect(0, 0, frame_.cols, frame_.rows);
    cv::Mat similarity;
    matchTemplate(gray(searchWindow), gray_model, similarity, CV_TM_CCOEFF_NORMED);
    double mag_r;
    cv::Point point;
    minMaxLoc(similarity, 0, &mag_r, 0, &point);
    trackBox.x = point.x + searchWindow.x;
    trackBox.y = point.y + searchWindow.y;
    trackBox.x *= scale;
    trackBox.y *= scale;
    trackBox.height *= scale;
    trackBox.width *= scale;
}

bool DianPose::tracking(cv::Mat &image, PersonInfo &person, bool use_track) {

    cv::Rect roi(person.x1, person.y1, person.x2 - person.x1, person.y2 - person.y1);
    cv::Mat roiImage;
    clock_t start_time = clock();
    if (use_track) {
        tracking_corrfilter(image, person.frame_person_prev, roi, 2);
        person.x1 = roi.x;
        person.y1 = roi.y;
        person.x2 = roi.x + roi.width;
        person.y2 = roi.y + roi.height;
        image(roi).copyTo(person.frame_person_prev);
    }

    image(roi).copyTo(roiImage);
    int track = poseModel->detect(roiImage, person);
    clock_t finish_time = clock();
    double total_time = (double) (finish_time - start_time) / CLOCKS_PER_SEC;
    LOGD("Finish tracking and detecting pose. time is %f ms\n", total_time * 1000);
    return track != 0;

}

void DianPose::update(cv::Mat &image) {

    bool use_track = true;
    if (detection_Time < 0) {
        detection_Time = (double) cvGetTickCount();
    } else {
        double diff = (double) cvGetTickCount() - detection_Time;
        diff /= (cvGetTickFrequency() * 1000);
        if (diff > detection_Interval) {
            std::cout << "Redetecting..." << std::endl;
            detection_Time = (double) cvGetTickCount();
            // do detection in thread
            trackPersons.clear();
            detecting(image);
            use_track = false;
        }
    }

    for (auto iter = trackPersons.begin(); iter != trackPersons.end();) {
        if (!tracking(image, *iter, use_track)) {
            iter = trackPersons.erase(iter); //追踪失败 则删除此人
        } else {
            iter++;
        }
    }
}

void DianPose::run(cv::Mat &image){
    if(first_run){
        detecting(image);
        detection_Time = (double) cvGetTickCount();
        update(image);
        first_run = false;
    } else {
        update(image);
    }
}