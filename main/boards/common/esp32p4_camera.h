#ifndef ESP32P4_CAMERA_H
#define ESP32P4_CAMERA_H

#include "camera.h"
#include <string>

/**
 * ESP32-P4专用摄像头类，基于V4L2接口
 * 兼容DVP摄像头接口
 */
class Esp32P4Camera : public Camera {
private:
    int fd_;
    bool initialized_;
    uint32_t width_;
    uint32_t height_;
    uint8_t* frame_buffer_;
    size_t frame_size_;

public:
    Esp32P4Camera();
    virtual ~Esp32P4Camera();

    /**
     * 初始化摄像头
     * @return true 成功，false 失败
     */
    bool Initialize();

    /**
     * 释放摄像头资源
     */
    void Deinitialize();

    // Camera接口实现
    virtual void SetExplainUrl(const std::string& url, const std::string& token) override;
    virtual bool Capture() override;
    virtual bool SetHMirror(bool enabled) override;
    virtual bool SetVFlip(bool enabled) override;
    virtual std::string Explain(const std::string& question) override;

    /**
     * 获取摄像头分辨率
     */
    bool GetResolution(uint32_t& width, uint32_t& height);

    /**
     * 获取当前帧缓冲区
     */
    const uint8_t* GetFrameBuffer() const { return frame_buffer_; }
    
    /**
     * 获取当前帧大小
     */
    size_t GetFrameSize() const { return frame_size_; }
};

#endif // ESP32P4_CAMERA_H