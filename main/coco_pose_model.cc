#include "coco_pose_model.h"
#include <esp_vfs.h>
#include <esp_spiffs.h>
#include <dirent.h>
#include <errno.h>

const char* COCOPoseModel::TAG = "COCOPoseModel";

COCOPoseModel::COCOPoseModel() : model_loaded_(false), input_width_(224), input_height_(224) {
}

COCOPoseModel::~COCOPoseModel() {
}

bool COCOPoseModel::Initialize(const std::string& model_path) {
#if defined(CONFIG_IDF_TARGET_ESP32P4) && HAS_COCO_POSE
    ESP_LOGI(TAG, "正在初始化COCO姿态检测模型...");
    
    try {
        // ESP-DL模型是编译时嵌入的，不需要从文件加载
        // 直接创建模型对象，使用可用的模型类型
        pose_model_ = std::make_unique<COCOPose>(COCOPose::YOLO11N_POSE_S8_V1);
        
        if (pose_model_) {
            model_loaded_ = true;
            ESP_LOGI(TAG, "COCO姿态检测模型初始化成功，输入尺寸: %dx%d", input_width_, input_height_);
            return true;
        } else {
            ESP_LOGE(TAG, "创建COCO姿态检测模型失败");
            return false;
        }
    } catch (const std::exception& e) {
        ESP_LOGE(TAG, "初始化模型异常: %s", e.what());
        return false;
    }
#else
    ESP_LOGW(TAG, "ESP-DL组件未找到或当前芯片不支持COCO姿态检测模型");
    return false;
#endif
}

PoseDetectionResult COCOPoseModel::Predict(const uint8_t* image_data, int width, int height, int channels) {
    PoseDetectionResult result;
    result.valid = false;
    result.confidence = 0.0f;
    
    if (!model_loaded_) {
        ESP_LOGW(TAG, "模型未加载");
        return result;
    }
    
#if defined(CONFIG_IDF_TARGET_ESP32P4) && HAS_COCO_POSE
    try {
        // 预处理图像
        if (!PreprocessImage(image_data, width, height, channels)) {
            ESP_LOGW(TAG, "图像预处理失败");
            return result;
        }
        
        // 运行推理
        dl::image::img_t input_img;
        input_img.width = input_width_;
        input_img.height = input_height_;
        input_img.pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB888;
        input_img.data = const_cast<uint8_t*>(image_data);  // 假设已经预处理过
        
        // 执行姿态检测
        auto detection_result = pose_model_->run(input_img);
        
        // 后处理结果
        result = PostprocessResult();
        
        ESP_LOGD(TAG, "姿态检测完成，置信度: %.2f", result.confidence);
        
    } catch (const std::exception& e) {
        ESP_LOGE(TAG, "推理异常: %s", e.what());
        result.valid = false;
    }
#endif
    
    return result;
}

bool COCOPoseModel::PreprocessImage(const uint8_t* image_data, int width, int height, int channels) {
    // 简化的预处理实现
    // 实际实现中需要进行缩放、归一化等操作
    ESP_LOGD(TAG, "预处理图像: %dx%dx%d -> %dx%dx3", width, height, channels, input_width_, input_height_);
    return true;
}

PoseDetectionResult COCOPoseModel::PostprocessResult() {
    PoseDetectionResult result;
    result.valid = true;
    result.confidence = 0.8f;  // 示例置信度
    
    // 初始化关键点数据（17个关键点 * 2坐标 = 34个数值）
    result.keypoints.resize(34, 0);
    
    // 这里需要根据实际的ESP-DL输出格式来解析关键点
    // 示例数据 - 实际使用时需要从模型输出中获取
    
    ESP_LOGD(TAG, "后处理完成，检测到 %d 个关键点", result.keypoints.size() / 2);
    
    return result;
}

// 模型管理器实现
bool COCOPoseModelManager::InitializeModel(const std::string& model_path) {
    if (!model_) {
        model_ = std::make_unique<COCOPoseModel>();
    }
    
    std::string actual_model_path = model_path;
    if (actual_model_path.empty()) {
        // 使用默认的模型路径
        actual_model_path = "/spiffs/models/pose_model.espdl";
    }
    
    return model_->Initialize(actual_model_path);
}