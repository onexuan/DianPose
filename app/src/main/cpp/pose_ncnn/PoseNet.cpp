//
// Created by 周瑞松 on 2020/7/26.
//

//
// Created by 周瑞松 on 2020/7/22.
//

#include "PoseNet.h"
#include <opencv2/opencv.hpp>
#include "snet.id.h"
#include "snet.mem.h"
#include "net.h"

#define TAG "PoseNet"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

static ncnn::UnlockedPoolAllocator g_blob_pool_allocator;
static ncnn::PoolAllocator g_workspace_pool_allocator;

PoseNet::PoseNet(int num_thread) {
    session.load_param(snet_bin);
    session.load_model(snet_param_bin);
    threads = num_thread;
}

PoseNet::PoseNet(int num_thread, std::string &path) {
    std::vector<std::string> param_files = {
            path + "/snet.param",
            path + "/snet.bin",
    };
    threads = num_thread;
    ncnn::create_gpu_instance();
    ncnn::Option opt;
    opt.lightmode = true;
    opt.num_threads = num_thread;
    opt.blob_allocator = &g_blob_pool_allocator;
    opt.workspace_allocator = &g_workspace_pool_allocator;
    opt.use_packing_layout = true;

    // use vulkan compute
    if (ncnn::get_gpu_count() != 0)
        opt.use_vulkan_compute = true;

    session.opt = opt;
    session.load_param(param_files[0].data());
    session.load_model(param_files[1].data());
}

PoseNet::~PoseNet() {
    session.clear();
    ncnn::destroy_gpu_instance();
}


int PoseNet::detect(cv::Mat &img, PersonInfo &person) {
    if (img.empty()) {
        return 0;
    }

    int image_h = img.rows;
    int image_w = img.cols;
    float ratio_x = (float) image_w / target_width;
    float ratio_y = (float) image_h / target_height;

    ncnn::Mat ncnn_img = ncnn::Mat::from_pixels(img.data, ncnn::Mat::PIXEL_RGB,
                                                img.cols, img.rows, img.step[0]);

    ncnn_img.substract_mean_normalize(mean_vals, norm_vals);
    ncnn::Mat in;
    resize_bilinear(ncnn_img, in, target_width, target_height);

    ncnn::Extractor net = session.create_extractor();
    int gpu_num = ncnn::get_gpu_count();
    LOGD("gpu's num: %d", gpu_num);
    if (!gpu_num) {
        net.set_vulkan_compute(false);
    } else {
        net.set_vulkan_compute(true);
    }

    net.input(snet_param_id::BLOB_input_1, in);
    ncnn::Mat heatmaps;
    net.extract(snet_param_id::BLOB_640, heatmaps);

    int max_conf = decode(heatmaps, person, ratio_x, ratio_y);
    return max_conf;
}

void PoseNet::_get_max_preds(ncnn::Mat &heatmaps, std::vector<partsType> &parts, float threshold) {

    for (int id = 0; id < joints; ++id) {
        float max_value = -1;
        int index = 0;
        for (int i = 0; i < heatmap_width * heatmap_height; i++) {
            if (heatmaps.channel(0)[id * heatmap_width * heatmap_height + i] >= max_value &&
                heatmaps.channel(0)[id * heatmap_width * heatmap_height + i] >= threshold) {
                index = i;
                max_value = heatmaps.channel(0)[id * heatmap_width * heatmap_height + i];
            }
        }
        cv::Point coord{index % heatmap_width, index / heatmap_width};
        parts.emplace_back(max_value, coord);
    }
}


int PoseNet::decode(ncnn::Mat &heatmaps, PersonInfo &person, float ratio_x, float ratio_y) {
    const int channel = joints;
    std::vector<partsType> parts;
    _get_max_preds(heatmaps, parts, SCORE_THRESHOLD);
    const float scale_x = ratio_x * stride;
    const float scaly_y = ratio_y * stride;
    float max_conf = 0;
    for (auto iter = parts.begin(); iter != parts.end();) {
        (iter->second).x *= scale_x;
        (iter->second).y *= scaly_y;
        iter++;
        max_conf = std::max(max_conf, iter->first);
    }
    int index = 0;
    for (auto iter = parts.begin(); iter != parts.end();) {
        person.keypoints_score[index] = iter->first;
        person.keypoints[index * 2] = iter->second.x + person.x1;
        person.keypoints[index * 2 + 1] = iter->second.y + person.y1;
        iter++;
        index++;
    }

    return max_conf > 0 ? 1 : 0;

}