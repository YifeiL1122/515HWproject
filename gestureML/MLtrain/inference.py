"""推理脚本"""
import cv2
import numpy as np
import tensorflow as tf
from keypoint_detector import HandKeypointDetector
import config

class GestureRecognizer:
    def __init__(self, model_path):
        # 加载TFLite模型
        self.interpreter = tf.lite.Interpreter(model_path=model_path)
        self.interpreter.allocate_tensors()
        
        # 获取输入输出细节
        self.input_details = self.interpreter.get_input_details()
        self.output_details = self.interpreter.get_output_details()
        
        # 初始化关键点检测器
        self.keypoint_detector = HandKeypointDetector()
    
    def predict(self, image):
        """预测手势"""
        # 检测关键点
        keypoints = self.keypoint_detector.detect_keypoints(image)
        if keypoints is None:
            return None
        
        # 预处理关键点
        keypoints = self.keypoint_detector.preprocess_keypoints(keypoints)
        
        # 准备输入数据
        input_data = np.array([keypoints], dtype=np.float32)
        
        # 设置输入张量
        self.interpreter.set_tensor(self.input_details[0]['index'], input_data)
        
        # 运行推理
        self.interpreter.invoke()
        
        # 获取输出
        output_data = self.interpreter.get_tensor(self.output_details[0]['index'])
        
        return output_data[0]

def main():
    # 初始化识别器
    recognizer = GestureRecognizer('models/model.tflite')
    
    # 打开摄像头
    cap = cv2.VideoCapture(0)
    
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        
        # 预测手势
        prediction = recognizer.predict(frame)
        
        if prediction is not None:
            # 获取最可能的手势
            gesture_id = np.argmax(prediction)
            confidence = prediction[gesture_id]
            
            # 显示结果
            cv2.putText(frame, f"Gesture: {gesture_id}, Conf: {confidence:.2f}",
                       (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        
        cv2.imshow('Gesture Recognition', frame)
        
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main() 