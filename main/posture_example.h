#ifndef POSTURE_EXAMPLE_H
#define POSTURE_EXAMPLE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化坐姿检测系统
 * @return true 成功, false 失败
 */
bool InitializePostureDetectionSystem(void);

/**
 * @brief 运行坐姿检测功能示例
 */
void RunPostureDetectionExamples(void);

/**
 * @brief 创建坐姿检测示例任务
 */
void CreatePostureExampleTask(void);

#ifdef __cplusplus
}
#endif

#endif // POSTURE_EXAMPLE_H