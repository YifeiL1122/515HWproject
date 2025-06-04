"""骨架分类器模型"""
import tensorflow as tf
from tensorflow.keras import layers, models
import config

def create_skeleton_classifier():
    """创建轻量级骨架分类器"""
    model = models.Sequential([
        # 输入层
        layers.Input(shape=(config.INPUT_SIZE,)),
        
        # 第一个隐藏层
        layers.Dense(config.HIDDEN_UNITS[0], activation='relu'),
        layers.BatchNormalization(),
        layers.Dropout(config.DROPOUT_RATE),
        
        # 第二个隐藏层
        layers.Dense(config.HIDDEN_UNITS[1], activation='relu'),
        layers.BatchNormalization(),
        layers.Dropout(config.DROPOUT_RATE),
        
        # 第三个隐藏层
        layers.Dense(config.HIDDEN_UNITS[2], activation='relu'),
        layers.BatchNormalization(),
        layers.Dropout(config.DROPOUT_RATE),
        
        # 输出层
        layers.Dense(config.NUM_CLASSES, activation='softmax')
    ])
    
    return model

def create_sequential_classifier():
    """创建序列模型（用于连续手势）"""
    model = models.Sequential([
        # 输入层 (时间步, 特征)
        layers.Input(shape=(None, config.INPUT_SIZE)),
        
        # GRU层
        layers.GRU(64, return_sequences=False),
        layers.BatchNormalization(),
        layers.Dropout(config.DROPOUT_RATE),
        
        # 输出层
        layers.Dense(config.NUM_CLASSES, activation='softmax')
    ])
    
    return model 