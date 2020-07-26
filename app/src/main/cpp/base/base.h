//
// Created by 周瑞松 on 2020/7/24.
//

#ifndef POSE_MAC_BASE_H
#define POSE_MAC_BASE_H
#pragma once
typedef struct {
    // detection results for one person
    float x1;
    float y1;
    float x2;
    float y2;
    float score;
    // pose estimation results
    float keypoints_score[17];
    float keypoints[34];

    //track info
    cv::Mat frame_person_prev;
} PersonInfo;

#endif //POSE_MAC_BASE_H
