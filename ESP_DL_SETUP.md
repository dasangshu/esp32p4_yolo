# ESP-DL 组件设置说明

## 概述
由于ESP-DL组件可能没有预安装或网络问题，当前代码配置为在没有ESP-DL的情况下也能正常编译。

## 当前状态
- ✅ **基础坐姿检测功能**: 使用传统算法进行坐姿分析
- ❌ **AI模型推理**: 需要ESP-DL组件支持（当前已禁用）

## 启用ESP-DL支持的步骤

### 1. 安装ESP-DL组件
确保`main/idf_component.yml`中包含以下依赖：

```yaml
espressif/esp-dl:
  version: "^2.0.0"
  rules:
  - if: target in [esp32p4]

espressif/coco_pose:
  version: "^0.1.0" 
  rules:
  - if: target in [esp32p4]
```

### 2. 下载依赖
运行以下命令更新组件：
```bash
idf.py reconfigure
```

### 3. 启用ESP-DL支持
在以下文件中修改宏定义：

**main/coco_pose_model.h** (第12行):
```cpp
#define HAS_COCO_POSE 1  // 改为1
```

**main/posture_camera_adapter.h** (第11行):
```cpp
#define HAS_ESP_DL 1  // 改为1
```

### 4. 重新编译
```bash
idf.py build
```

## 功能对比

### 当前状态 (无ESP-DL)
- ✅ 基于关键点的坐姿分析算法
- ✅ 6种坐姿类型检测
- ✅ 实时状态监控和提醒
- ✅ 统计数据收集
- ✅ 可视化绘制功能
- ❌ 无法进行实际的人体关键点检测
- ❌ 需要手动提供关键点数据

### 启用ESP-DL后
- ✅ 所有基础功能
- ✅ **AI模型推理**: 自动从摄像头图像提取关键点
- ✅ **端到端检测**: 从图像直接到坐姿分析结果
- ✅ **高准确度**: 基于COCO训练的YOLO11模型

## 测试建议

### 当前状态下的测试
可以使用`posture_example.cc`中的示例代码来测试坐姿检测算法：

```cpp
// 模拟关键点数据进行测试
std::vector<int> keypoints(34, 0);
// 设置一些模拟的关键点...
PostureResult result = detector.AnalyzePosture(keypoints, 0.8f);
```

### ESP-DL启用后的测试
```cpp
// 直接从摄像头图像进行检测
auto model = COCOPoseModelManager::GetInstance().GetModel();
PoseDetectionResult result = model->Predict(image_data, width, height, 3);
```

## 故障排除

### 1. 组件下载失败
```bash
# 清理并重新下载
idf.py clean
rm -rf managed_components/espressif__*
idf.py reconfigure
```

### 2. 编译错误
如果启用ESP-DL后出现编译错误：
1. 检查ESP-IDF版本是否为5.4.0+
2. 确保目标芯片设置为ESP32P4
3. 检查managed_components目录是否包含esp-dl和coco_pose

### 3. 运行时错误
1. 确保模型文件已正确嵌入到固件中
2. 检查flash分区大小是否足够
3. 查看日志中的错误信息

## 模型文件
- **模型位置**: `main/models/coco_pose_yolo11n_pose_224_p4.espdl`
- **模型大小**: ~3MB
- **输入尺寸**: 224x224x3
- **输出**: 17个人体关键点坐标

## 注意事项
1. ESP-DL功能仅在ESP32P4芯片上可用
2. 模型文件会自动嵌入到固件中，确保flash空间充足
3. 推理性能取决于具体的ESP32P4配置和时钟频率
4. 建议在良好的光照条件下使用摄像头