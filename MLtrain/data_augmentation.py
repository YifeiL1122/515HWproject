"""数据增强模块"""
import numpy as np
import config

def add_noise(keypoints, scale=config.NOISE_SCALE):
    """添加随机噪声"""
    noise = np.random.normal(0, scale, keypoints.shape)
    return keypoints + noise

def random_dropout(keypoints, prob=config.DROPOUT_PROB):
    """随机丢失关键点"""
    mask = np.random.random(keypoints.shape[0] // 2) > prob
    mask = np.repeat(mask, 2)  # 对x,y坐标使用相同的mask
    keypoints = keypoints.copy()
    keypoints[~mask] = 0
    return keypoints

def augment_keypoints(keypoints):
    """应用数据增强"""
    # 添加噪声
    keypoints = add_noise(keypoints)
    
    # 随机丢失关键点
    keypoints = random_dropout(keypoints)
    
    return keypoints 