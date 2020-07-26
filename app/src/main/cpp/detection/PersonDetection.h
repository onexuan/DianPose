//
//  DianFaceDetection.h
//
//  Created by 周瑞松 on 2020/7/24.
//  Copyright © 2020 周瑞松. All rights reserved.
//

#ifndef DianFaceDetection_hpp
#define DianFaceDetection_hpp

#include <stdio.h>

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Interpreter.hpp"
#include "MNNDefine.h"
#include "Tensor.hpp"
#include "ImageProcess.hpp"
#include "base.h"

typedef struct {
    // detection results for one person
    float x1;
    float y1;
    float x2;
    float y2;
    float score;
    float area;

} BBoxInfo;

class PersonDetectionNet {
public:
    explicit PersonDetectionNet(int num_thread);

    ~PersonDetectionNet();

    int detect(cv::Mat& img,  std::vector<PersonInfo> &persons);

private:
    void decoder(std::vector<BBoxInfo> &bbox_collection, MNN::Tensor *scores, MNN::Tensor *bboxes);

    void nms(std::vector<BBoxInfo> &inputs, std::vector<PersonInfo> &outputs, int type);
    void nms(std::vector<BBoxInfo> &inputs, std::vector<PersonInfo> &outputs);

private:
    /* MNN MODEL CONFIG */
    MNN::Interpreter *detector = NULL;
    MNN::Session *session = NULL;

    MNN::Tensor *input_tensor = NULL;
    std::vector<MNN::Tensor *> output_tensor;
    MNN::CV::ImageProcess::Config image_config;
    MNN::CV::ImageProcess *process = NULL;

    int source_width, source_height;

    std::vector<std::vector<float>> priors = {};

    /* CONSTANT VALUE FOR DETECTION MODEL */
    const int target_width = 160;
    const int target_height = 120;
    const int num_anchors = 1118;
    const float score_threshold = 0.8;
    const float iou_threshold = 0.1;
    const float mean_vals_detection[3] = {127, 127, 127};
    const float norm_vals_detection[3] = {0.0078125, 0.0078125, 0.0078125};
    const float center_variance = 0.1;
    const float size_variance = 0.2;
    const std::vector<std::vector<float>> min_boxes = {{10.0f,  16.0f,  24.0f},
                                                       {32.0f,  48.0f},
                                                       {64.0f,  96.0f},
                                                       {128.0f, 192.0f, 256.0f}};
    const std::vector<std::vector<float>> featuremap_size = {{20, 10, 5, 3},
                                                             {15, 8,  4, 2}};
    const std::vector<std::vector<float>> shrinkage_size = {{8, 16, 32, 64},
                                                            {8, 16, 32, 48}};
    const std::vector<std::string> output_tensor_names = {"boxes", "scores"};

};

#endif /* DianFaceDetection_hpp */
