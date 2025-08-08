#ifndef COCO_POSE_MODEL_H
#define COCO_POSE_MODEL_H

#include <vector>
#include <string>
#include <memory>
#include <esp_log.h>

#ifdef CONFIG_IDF_TARGET_ESP32P4
// 预定义ESP-DL是否可用的宏，避免编译时检查
// 如果ESP-DL组件未安装，可以将此设为0来禁用相关功能
#define HAS_COCO_POSE 1

#if HAS_COCO_POSE
#include "dl_image.hpp"
#include "coco_pose.hpp"
#endif
#else
#define HAS_COCO_POSE 0
#endif

struct PoseDetectionResult {
    std::vector<int> keypoints;  // 17个关键点的x,y坐标 (34个数值)
    float confidence;
    int box_x, box_y, box_w, box_h;
    bool valid;
};

class COCOPoseModel {
public:
    COCOPoseModel();
    ~COCOPoseModel();
    
    // 初始化模型
    bool Initialize(const std::string& model_path = "/spiffs/coco_pose_yolo11n_pose_224_p4.espdl");
    
    // 运行推理
    PoseDetectionResult Predict(const uint8_t* image_data, int width, int height, int channels);
    
    // 检查模型是否已加载
    bool IsLoaded() const { return model_loaded_; }
    
    // 获取模型输入尺寸
    void GetInputSize(int& width, int& height) const { width = input_width_; height = input_height_; }

private:
    bool model_loaded_;
    int input_width_;
    int input_height_;
    
#if defined(CONFIG_IDF_TARGET_ESP32P4) && HAS_COCO_POSE
    std::unique_ptr<COCOPose> pose_model_;
#endif
    
    // 预处理图像
    bool PreprocessImage(const uint8_t* image_data, int width, int height, int channels);
    
    // 后处理结果
    PoseDetectionResult PostprocessResult();
    
    static const char* TAG;
};

// 全局模型管理器
class COCOPoseModelManager {
public:
    static COCOPoseModelManager& GetInstance() {
        static COCOPoseModelManager instance;
        return instance;
    }
    
    bool InitializeModel(const std::string& model_path = "");
    COCOPoseModel* GetModel() { return model_.get(); }
    
    COCOPoseModelManager(const COCOPoseModelManager&) = delete;
    COCOPoseModelManager& operator=(const COCOPoseModelManager&) = delete;

private:
    COCOPoseModelManager() = default;
    std::unique_ptr<COCOPoseModel> model_;
};

#endif // COCO_POSE_MODEL_H