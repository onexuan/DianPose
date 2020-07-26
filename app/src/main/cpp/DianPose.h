//
//  DianPose/h
//  Created by 周瑞松 on 2020/7/25.
//  Copyright © 2020 周瑞松. All rights reserved.
//

#ifndef POSE_MAC_DIANPOSE_H
#define POSE_MAC_DIANPOSE_H

#include <vector>
#include <opencv2/opencv.hpp>
#include "PoseNet.h"
#include "PersonDetection.h"
#include "base.h"

class DianPose{
public:
    explicit DianPose(int num_thread);
    ~DianPose();
    void run(cv::Mat &image);
    std::vector <PersonInfo> trackPersons;
private:
    void detecting(cv::Mat &image);
    bool tracking(cv::Mat &image, PersonInfo& person,  bool use_track);
    void update(cv::Mat &image);
    void tracking_corrfilter(const cv::Mat &frame, const cv::Mat &model, cv::Rect &trackBox,
                             float scale);

    PoseNet *poseModel = NULL;
    PersonDetectionNet *personModel = NULL;
    bool detecting_lock;
    double detection_Time;
    double detection_Interval;
    bool first_run;


};
#endif //POSE_MAC_DIANPOSE_H
