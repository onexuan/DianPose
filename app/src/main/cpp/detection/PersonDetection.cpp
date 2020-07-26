//
//  DianFaceDetection.cpp
//
//  Created by 周瑞松 on 2020/7/24.
//  Copyright © 2020 周瑞松. All rights reserved.
//
#include "PersonDetection.h"
#include "ultrapose_160.h"

#define clip(x, y) (x < 0 ? 0 : (x > y ? y : x))
#define num_featuremap 4
#define HARD_NMS 1
#define BLEND_NMS 2 /* mix nms was been proposaled in paper blaze face*/

bool cmpScore(BBoxInfo lsh, BBoxInfo rsh) {
    if (lsh.score < rsh.score)
        return true;
    else
        return false;
}

PersonDetectionNet::PersonDetectionNet(int num_thread) {
    /* generate prior anchors */
    for (int index = 0; index < num_featuremap; index++) {
        float scale_w = target_width / shrinkage_size[0][index];
        float scale_h = target_height / shrinkage_size[1][index];
        for (int j = 0; j < featuremap_size[1][index]; j++) {
            for (int i = 0; i < featuremap_size[0][index]; i++) {
                float x_center = (float) (i + 0.5) / scale_w;
                float y_center = (float) (j + 0.5) / scale_h;

                for (int k = 0; k < min_boxes[index].size(); k++) {
                    float w = min_boxes[index][k] / target_width;
                    float h = min_boxes[index][k] / target_height;
                    priors.push_back(
                            {clip(x_center, 1), clip(y_center, 1), clip(w, 1), clip(h, 1)});
                }
            }
        }
    }
    /* MNN Config */
    detector = MNN::Interpreter::createFromBuffer(ultrapose_mnn, ultrapose_mnn_len);

    MNN::ScheduleConfig config;
    config.type = MNN_FORWARD_AUTO;
    config.numThread = num_thread;

    MNN::BackendConfig backendConfig;
    backendConfig.precision = MNN::BackendConfig::Precision_High;
    backendConfig.power = MNN::BackendConfig::Power_High;
    config.backendConfig = &backendConfig;

    session = detector->createSession(config);
    input_tensor = detector->getSessionInput(session, NULL);

    output_tensor.resize(output_tensor_names.size());
    for (int i = 0; i < output_tensor_names.size(); i++) {
        output_tensor[i] = detector->getSessionOutput(session, output_tensor_names[i].c_str());
    }

    detector->releaseModel(); /* release model */

    std::memcpy(image_config.mean, mean_vals_detection, sizeof(mean_vals_detection));
    std::memcpy(image_config.normal, norm_vals_detection, sizeof(norm_vals_detection));

    image_config.sourceFormat = MNN::CV::ImageFormat::RGBA;
    image_config.destFormat = MNN::CV::ImageFormat::RGB;

    image_config.filterType = MNN::CV::Filter::BILINEAR;
    process = MNN::CV::ImageProcess::create(image_config);
}

PersonDetectionNet::~PersonDetectionNet() {
    detector->releaseSession(session);
    delete detector;
    delete process;
    session = NULL;
    detector = NULL;
    process = NULL;
}

int PersonDetectionNet::detect(cv::Mat &img, std::vector<PersonInfo> &face_rect) {

    source_width = img.cols;
    source_height = img.rows;

    /* Image Resize */
    MNN::CV::Matrix transform;
    transform.postScale((float) source_width / target_width, (float) source_height / target_height);
    process->setMatrix(transform);
    process->convert(img.data, source_width, source_height, img.step[0], input_tensor);

    /* Model Inference */
    int status = detector->runSession(session);
    if (status != 0) return -1;

    /* Extract Outputs */
    MNN::Tensor *bboxes = new MNN::Tensor(output_tensor[0], MNN::Tensor::CAFFE);
    output_tensor[0]->copyToHostTensor(bboxes);
    MNN::Tensor *scores = new MNN::Tensor(output_tensor[1], MNN::Tensor::CAFFE);
    output_tensor[1]->copyToHostTensor(scores);

    /* Post Process */
    std::vector<BBoxInfo> temp;
    decoder(temp, scores, bboxes);
    nms(temp, face_rect);
    return 0;
}

void PersonDetectionNet::decoder(std::vector<BBoxInfo> &outputs, MNN::Tensor *scores,
                                 MNN::Tensor *bboxes) {
    for (int i = 0; i < num_anchors; i++) {
        if (scores->host<float>()[i * 2 + 1] > score_threshold) {
            BBoxInfo rects;

            rects.x1 = clip(bboxes->host<float>()[i * 4] - 0.05, 1) * source_width;
            rects.y1 = clip(bboxes->host<float>()[i * 4 + 1] - 0.05, 1) * source_height;
            rects.x2 = clip(bboxes->host<float>()[i * 4 + 2] + 0.05, 1) * source_width;
            rects.y2 = clip(bboxes->host<float>()[i * 4 + 3] + 0.05, 1) * source_height;
            rects.score = clip(scores->host<float>()[i * 2 + 1], 1);
            rects.area = (rects.x2 - rects.x1) * (rects.y2 - rects.y1);
            outputs.push_back(rects);
        }
    }
}

void PersonDetectionNet::nms(std::vector<BBoxInfo> &inputs, std::vector<PersonInfo> &outputs) {
    if (inputs.empty()) {
        return;
    }
    std::sort(inputs.begin(), inputs.end(), cmpScore);
    float IOU = 0;
    float maxX = 0;
    float maxY = 0;
    float minX = 0;
    float minY = 0;
    std::vector<int> vPick;
    int nPick = 0;
    std::multimap<float, int> vScores;
    const int num_boxes = inputs.size();
    vPick.resize(num_boxes);
    for (int i = 0; i < num_boxes; ++i) {
        vScores.insert(std::pair<float, int>(inputs[i].score, i));
    }
    while (!vScores.empty()) {
        int last = vScores.rbegin()->second;
        vPick[nPick] = last;
        nPick += 1;
        for (auto it = vScores.begin(); it != vScores.end();) {
            int it_idx = it->second;
            maxX = std::max(inputs.at(it_idx).x1, inputs.at(last).x1);
            maxY = std::max(inputs.at(it_idx).y1, inputs.at(last).y1);
            minX = std::min(inputs.at(it_idx).x2, inputs.at(last).x2);
            minY = std::min(inputs.at(it_idx).y2, inputs.at(last).y2);
            //maxX1 and maxY1 reuse
            maxX = ((minX - maxX + 1) > 0) ? (minX - maxX + 1) : 0;
            maxY = ((minY - maxY + 1) > 0) ? (minY - maxY + 1) : 0;
            //IOU reuse for the area of two bbox
            IOU = maxX * maxY;
            IOU = IOU / (inputs.at(it_idx).area + inputs.at(last).area - IOU);

            if (IOU > iou_threshold) {
                it = vScores.erase(it);
            } else {
                it++;
            }
        }
    }
    vPick.resize(nPick);
    outputs.resize(nPick);
    for (int i = 0; i < nPick; i++) {
        outputs[i].x1 = inputs[vPick[i]].x1;
        outputs[i].y1 = inputs[vPick[i]].y1;
        outputs[i].x2 = inputs[vPick[i]].x2;
        outputs[i].y2 = inputs[vPick[i]].y2;
        outputs[i].score = inputs[vPick[i]].score;
    }

}

void
PersonDetectionNet::nms(std::vector<BBoxInfo> &inputs, std::vector<PersonInfo> &outputs, int type) {
    std::sort(inputs.begin(), inputs.end(),
              [](const BBoxInfo &a, const BBoxInfo &b) {
                  return a.score > b.score;
              });

    int box_num = inputs.size();

    std::vector<int> merged(box_num, 0);

    for (int i = 0; i < box_num; i++) {
        if (merged[i])
            continue;
        std::vector<BBoxInfo> buf;

        buf.push_back(inputs[i]);
        merged[i] = 1;

        float h0 = inputs[i].y2 - inputs[i].y1 + 1;
        float w0 = inputs[i].x2 - inputs[i].x1 + 1;

        float area0 = h0 * w0;


        for (int j = i + 1; j < box_num; j++) {
            if (merged[j])
                continue;

            float inner_x0 = inputs[i].x1 > inputs[j].x1 ? inputs[i].x1 : inputs[j].x1;
            float inner_y0 = inputs[i].y1 > inputs[j].y1 ? inputs[i].y1 : inputs[j].y1;

            float inner_x1 = inputs[i].x2 < inputs[j].x2 ? inputs[i].x2 : inputs[j].x2;
            float inner_y1 = inputs[i].y2 < inputs[j].y2 ? inputs[i].y2 : inputs[j].y2;

            float inner_h = inner_y1 - inner_y0 + 1;
            float inner_w = inner_x1 - inner_x0 + 1;


            if (inner_h <= 0 || inner_w <= 0)
                continue;

            float inner_area = inner_h * inner_w;

            float h1 = inputs[j].y2 - inputs[j].y1 + 1;
            float w1 = inputs[j].x2 - inputs[j].x1 + 1;

            float area1 = h1 * w1;

            float score;

            score = inner_area / (area0 + area1 - inner_area);

            if (score > iou_threshold) {
                merged[j] = 1;
                buf.push_back(inputs[j]);
            }
        }
        switch (type) {
            case HARD_NMS: {
                PersonInfo rects;
                rects.x1 = buf[0].x1;
                rects.y1 = buf[0].y1;
                rects.x2 = buf[0].x2;
                rects.y2 = buf[0].y2;
                rects.score = buf[0].score;
                outputs.push_back(rects);
                break;
            }
            case BLEND_NMS: {
                float total = 0;
                for (auto &item : buf) {
                    total += exp(item.score);
                }
                PersonInfo rects;
                for (auto &item : buf) {
                    float rate = exp(item.score) / total;
                    rects.x1 += item.x1 * rate;
                    rects.y1 += item.y1 * rate;
                    rects.x2 += item.x2 * rate;
                    rects.y2 += item.y2 * rate;
                    rects.score += item.score * rate;
                }
                outputs.push_back(rects);
                break;
            }
            default: {
//                printf("wrong type of nms.");
                exit(-1);
            }
        }
    }
}
