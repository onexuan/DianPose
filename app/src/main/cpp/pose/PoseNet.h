//
// Created by 周瑞松 on 2020/7/22.
//

#ifndef POSE_MAC_POSENET_H
#define POSE_MAC_POSENET_H


#include <Interpreter.hpp>
#include <MNNDefine.h>
#include <Tensor.hpp>
#include <ImageProcess.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "base.h"
#pragma once
class PoseNet {
public:
    explicit PoseNet(int num_thread);
    ~PoseNet();

    int detect(cv::Mat& img, PersonInfo& person);

private:

    int decode(MNN::Tensor *heatmap, PersonInfo& person, float ratio_x, float ratio_y);
    std::shared_ptr<MNN::Interpreter> net ;
    MNN::Session *session = NULL;
    MNN::Tensor *input_tensor = NULL;
    MNN::CV::ImageProcess *process = NULL;
    std::vector<MNN::Tensor *> output_tensor;
    MNN::CV::ImageProcess::Config img_config;

    const int joints=17;
    const int stride = 4;
    const int target_width = 96;
    const int target_height = 128;
    const int heatmap_width = target_width / stride;
    const int heatmap_height= target_height / stride;
    const float mean_vals[3] = {123.68f, 116.78f,103.94f};
    const float norm_vals[3] = {0.017f,0.017f,0.017f};
    const std::vector<std::string> output_tensor_names = {"heatmap"};
    const float SCORE_THRESHOLD= 0.3;
};

#endif //POSE_MAC_POSENET_H
