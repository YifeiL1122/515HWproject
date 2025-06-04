"""关键点检测模块"""
import mediapipe as mp
import numpy as np
import cv2

class HandKeypointDetector:
    def __init__(self):
        # 初始化MediaPipe Hands
        self.mp_hands = mp.solutions.hands
        self.hands = self.mp_hands.Hands(
            static_image_mode=True,
            max_num_hands=1,
            min_detection_confidence=0.5
        )
    
    def detect_keypoints(self, image):
        """检测手部关键点"""
        # 转换为RGB
        image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        
        # 获取关键点
        results = self.hands.process(image_rgb)
        
        if results.multi_hand_landmarks:
            # 获取第一只手的关键点
            hand_landmarks = results.multi_hand_landmarks[0]
            
            # 提取关键点坐标
            keypoints = []
            for landmark in hand_landmarks.landmark:
                keypoints.extend([landmark.x, landmark.y])
            
            return np.array(keypoints, dtype=np.float32)
        
        return None
    
    def preprocess_keypoints(self, keypoints):
        """预处理关键点"""
        if keypoints is None:
            return None
        
        # 归一化
        keypoints = keypoints.reshape(-1, 2)
        
        # 中心化
        center = np.mean(keypoints, axis=0)
        keypoints = keypoints - center
        
        # 缩放
        scale = np.max(np.abs(keypoints))
        keypoints = keypoints / (scale + 1e-6)
        
        return keypoints.flatten()

    def __del__(self):
        self.hands.close() 