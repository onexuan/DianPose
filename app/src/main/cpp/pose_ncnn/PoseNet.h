//
// Created by 周瑞松 on 2020/7/26.
//

#ifndef POSE_MAC_POSENET_H
#define POSE_MAC_POSENET_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include <net.h>
#include "base.h"

typedef std::pair<float, cv::Point> partsType;
class PoseNet {
public:
    PoseNet(int num_thread);
    PoseNet(int num_thread, std::string& path);
    ~PoseNet();
    ncnn::Net session;
    int detect(cv::Mat& img, PersonInfo& person);

private:
    int threads;
    int decode(ncnn::Mat &heatmap, PersonInfo& person, float ratio_x, float ratio_y);
    void _get_max_preds(ncnn::Mat& heatmaps, std::vector<partsType> &parts, float threshold);
    const int joints=17;
    const int stride = 4;
    const int target_width = 192;
    const int target_height = 256;
    const int heatmap_width = target_width / stride;
    const int heatmap_height= target_height / stride;
    const float mean_vals[3] = {123.68f, 116.78f,103.94f};
    const float norm_vals[3] = {0.017f,0.017f,0.017f};
    const std::vector<std::string> output_tensor_names = {"581"};
    const float SCORE_THRESHOLD= 0.3;

};

#endif //POSE_MAC_POSENET_H
