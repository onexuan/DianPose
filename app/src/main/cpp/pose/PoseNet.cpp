//
// Created by 周瑞松 on 2020/7/22.
//

#include "PoseNet.h"
#include "pose_snet.h"

#include <android/log.h>
#define TAG "DianPose"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

typedef std::pair<float, MNN::CV::Point> partsType;

float sign(float x) {
    return x > 0 ? 1 : -1;
}

PoseNet::PoseNet(int num_thread) {
    net = std::shared_ptr<MNN::Interpreter>(MNN::Interpreter::createFromBuffer(snet_ap34_mnn,snet_ap34_mnn_len));
    MNN::ScheduleConfig config;
    config.type = MNN_FORWARD_CPU;
    config.numThread = num_thread;
    MNN::BackendConfig backendConfig;
    backendConfig.precision = MNN::BackendConfig::Precision_High;
    backendConfig.power = MNN::BackendConfig::Power_High;
    config.backendConfig = &backendConfig;

    session = net->createSession(config);
    input_tensor = net->getSessionInput(session, NULL);
    output_tensor.resize(output_tensor_names.size());
    for (auto i = 0; i < output_tensor_names.size(); i++) {
        output_tensor[i] = net->getSessionOutput(session, output_tensor_names[i].c_str());
    }

    ::memcpy(img_config.mean, mean_vals, sizeof(mean_vals));
    ::memcpy(img_config.normal, norm_vals, sizeof(norm_vals));

    img_config.sourceFormat = MNN::CV::ImageFormat::RGB;
    img_config.destFormat = MNN::CV::ImageFormat::RGB;

    img_config.filterType = MNN::CV::Filter::BILINEAR;
    img_config.wrap = MNN::CV::Wrap::REPEAT;

    process = MNN::CV::ImageProcess::create(img_config);

}

PoseNet::~PoseNet() {
    net->releaseSession(session);
    net->releaseModel();
    delete process;
}


int PoseNet::detect(cv::Mat &img, PersonInfo &person) {
    if (img.empty()) {
        return 0;
    }

    int image_h = img.rows;
    int image_w = img.cols;
    float ratio_x = (float) image_w / target_width;
    float ratio_y = (float) image_h / target_height;

    MNN::CV::Matrix trans;
    trans.postScale(ratio_x, ratio_y);
    process->setMatrix(trans);
    process->convert(img.data, image_w, image_h, img.step[0], input_tensor);
    int status = net->runSession(session);
    if (status != 0) {
        LOGD("Model Forward Error!!");
        LOGD("Error Code is: %d", status);
    }
    MNN::Tensor *heatmaps = new MNN::Tensor(output_tensor[0], MNN::Tensor::CAFFE);
    output_tensor[0]->copyToHostTensor(heatmaps);

    int max_conf = decode(heatmaps, person, ratio_x, ratio_y);
    return max_conf;
}

void _get_max_preds(MNN::Tensor *heatmaps, std::vector<partsType> &parts, float threshold) {
    const int channel = heatmaps->length(1);
    const int height = heatmaps->length(2);
    const int width = heatmaps->length(3);
    auto scoresPtr = heatmaps->host<float>();
    for (int id = 0; id < channel; ++id) {
        auto idScoresPtr = scoresPtr + id * width * height;
        float max_value = -1;
        int max_index = 0;
        int second_index = 0;
        float second_value = -1;
        for (int i = 0; i < width * height; i++) {
            if (idScoresPtr[i] < threshold) {
                continue;
            }
            if (idScoresPtr[i] >= max_value) {
                second_index = max_index;
                second_value = max_value;
                max_index = i;
                max_value = idScoresPtr[i];
            } else if (idScoresPtr[i] > second_value && idScoresPtr[i] < max_value) {
                second_index = i;
                second_value = idScoresPtr[i];
            }
        }
        float offset_x = max_index % width + sign(second_index % width - max_index % width) * 0.25;
        float offset_y = max_index / width + sign(second_index / width - max_index / width) * 0.25;

        MNN::CV::Point coord{static_cast<float>(offset_x), static_cast<float>(offset_y)};
        parts.emplace_back(max_value, coord);
    }
}


int PoseNet::decode(MNN::Tensor *heatmaps, PersonInfo &person, float ratio_x, float ratio_y) {
    const int channel = heatmaps->length(1);
    std::vector<partsType> parts;
    _get_max_preds(heatmaps, parts, SCORE_THRESHOLD);
    const float scale_x = ratio_x * stride;
    const float scaly_y = ratio_y * stride;
    float max_conf = 0;
    for (auto iter = parts.begin(); iter != parts.end();) {
        (iter->second).fX *= scale_x;
        (iter->second).fY *= scaly_y;
        iter++;
        max_conf = std::max(max_conf, iter->first);
    }
    int index = 0;
    for (auto iter = parts.begin(); iter != parts.end();) {
        person.keypoints_score[index] = iter->first;
        person.keypoints[index * 2] = iter->second.fX + person.x1;
        person.keypoints[index * 2 + 1] = iter->second.fY + person.y1;
        iter++;
        index++;
    }

    return max_conf > 0 ? 1 : 0;

}