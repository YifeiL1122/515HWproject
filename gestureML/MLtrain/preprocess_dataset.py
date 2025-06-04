import os
import cv2
import numpy as np
import mediapipe as mp
import zipfile
from tqdm import tqdm
import shutil
from pathlib import Path

class DatasetPreprocessor:
    def __init__(self, dataset_path, output_path):
        self.dataset_path = Path(dataset_path)
        self.output_path = Path(output_path)
        self.temp_path = self.output_path / 'temp'
        
        # 初始化 MediaPipe
        self.mp_hands = mp.solutions.hands
        self.hands = self.mp_hands.Hands(
            static_image_mode=True,
            max_num_hands=1,
            min_detection_confidence=0.5,
            min_tracking_confidence=0.5
        )

        # 创建输出目录
        self.output_path.mkdir(parents=True, exist_ok=True)
        self.temp_path.mkdir(parents=True, exist_ok=True)

    def extract_keypoints(self, image_path):
        """从单个图像中提取手部关键点"""
        image = cv2.imread(str(image_path))
        if image is None:
            return None
            
        # 获取图像尺寸
        height, width, _ = image.shape
            
        # 转换为 RGB
        image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        
        # 处理图像
        results = self.hands.process(image_rgb)
        
        if not results.multi_hand_landmarks:
            return None
            
        # 获取第一只手的关键点
        hand_landmarks = results.multi_hand_landmarks[0]
        
        # 提取关键点坐标并归一化
        keypoints = []
        for landmark in hand_landmarks.landmark:
            # 将坐标转换为相对于图像尺寸的比例
            x = landmark.x
            y = landmark.y
            z = landmark.z
            keypoints.extend([x, y, z])
            
        return np.array(keypoints)

    def process_gesture_class(self, zip_path, gesture_name):
        """处理单个手势类别的数据"""
        print(f"Processing {gesture_name}...")
        
        # 创建临时目录
        temp_gesture_path = self.temp_path / gesture_name
        temp_gesture_path.mkdir(exist_ok=True)
        
        # 解压文件
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            zip_ref.extractall(temp_gesture_path)
            
        # 收集所有图像文件
        image_files = list(temp_gesture_path.rglob('*.jpg')) + \
                     list(temp_gesture_path.rglob('*.png'))
        
        # 提取关键点
        keypoints_list = []
        valid_count = 0
        
        for img_path in tqdm(image_files, desc=f"Extracting keypoints for {gesture_name}"):
            keypoints = self.extract_keypoints(img_path)
            if keypoints is not None:
                keypoints_list.append(keypoints)
                valid_count += 1
                
        # 保存关键点数据
        if keypoints_list:
            keypoints_array = np.array(keypoints_list)
            save_path = self.output_path / f"{gesture_name}_keypoints.npy"
            np.save(save_path, keypoints_array)
            print(f"Saved {valid_count} samples for {gesture_name}")
            
        # 清理临时文件
        shutil.rmtree(temp_gesture_path)
        
        return valid_count

    def process_dataset(self):
        """处理整个数据集"""
        # 获取所有zip文件
        zip_files = list(self.dataset_path.glob('*.zip'))
        
        total_samples = 0
        gesture_counts = {}
        
        for zip_path in zip_files:
            gesture_name = zip_path.stem
            valid_count = self.process_gesture_class(zip_path, gesture_name)
            
            gesture_counts[gesture_name] = valid_count
            total_samples += valid_count
            
        # 打印统计信息
        print("\nProcessing completed!")
        print(f"Total samples processed: {total_samples}")
        print("\nSamples per gesture:")
        for gesture, count in gesture_counts.items():
            print(f"{gesture}: {count}")
            
        # 清理临时目录
        if self.temp_path.exists():
            shutil.rmtree(self.temp_path)

def main():
    dataset_path = Path("Dataset/trainval")
    output_path = Path("ML2/processed_data")
    
    preprocessor = DatasetPreprocessor(dataset_path, output_path)
    preprocessor.process_dataset()

if __name__ == "__main__":
    main() 