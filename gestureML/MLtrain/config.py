"""配置文件"""
import os
from pathlib import Path

# 路径配置
BASE_DIR = Path(__file__).parent
DATA_DIR = BASE_DIR / 'processed_data'
MODEL_DIR = BASE_DIR / 'models'

# 模型配置
NUM_KEYPOINTS = 21  # MediaPipe Hands 输出的关键点数量
NUM_COORDS = 3      # 每个关键点的坐标维度 (x,y,z)
INPUT_SIZE = NUM_KEYPOINTS * NUM_COORDS  # 输入特征维度: 21 * 3 = 63
NUM_CLASSES = 18    # 实际的手势类别数量（修正）

# 模型架构配置
HIDDEN_UNITS = [256, 128, 64]  # MLP隐藏层大小
DROPOUT_RATE = 0.2        # Dropout比率

# 训练配置
BATCH_SIZE = 64
EPOCHS = 50
LEARNING_RATE = 0.001
EARLY_STOPPING_PATIENCE = 10

# 数据增强配置
NOISE_SCALE = 0.01        # 坐标噪声比例
DROPOUT_PROB = 0.1        # 关键点随机丢失概率 