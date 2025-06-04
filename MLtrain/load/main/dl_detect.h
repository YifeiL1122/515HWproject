#ifndef DL_DETECT_H
#define DL_DETECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_camera.h"

// 检测结果结构体
typedef struct {
    int x;          // 检测框左上角x坐标
    int y;          // 检测框左上角y坐标
    int width;      // 检测框宽度
    int height;     // 检测框高度
    float score;    // 置信度
    int class_id;   // 类别ID
} dl_detect_result_t;

// 初始化检测模型
esp_err_t dl_detect_init(void);

// 运行检测
// @param fb: 摄像头帧缓冲区
// @param results: 检测结果数组
// @param max_results: 最大结果数量
// @param num_results: 实际检测到的结果数量
esp_err_t dl_detect_run(camera_fb_t *fb, dl_detect_result_t *results, int max_results, int *num_results);

// 释放资源
void dl_detect_deinit(void);

#ifdef __cplusplus
}
#endif

#endif // DL_DETECT_H 