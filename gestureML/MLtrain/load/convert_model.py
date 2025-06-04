import tensorflow as tf
import numpy as np

def representative_dataset():
    for _ in range(100):
        data = np.random.rand(1, 63).astype(np.float32)
        yield [data]

def convert_h5_to_int8_tflite(h5_model_path, output_tflite_path):
    # 加载模型
    model = tf.keras.models.load_model(h5_model_path)
    
    # 确保模型已构建
    model.build((None, 63))
    
    # 创建转换器
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    
    # 设置量化参数
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    converter.representative_dataset = representative_dataset
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    converter.inference_input_type = tf.int8
    converter.inference_output_type = tf.int8
    
    # 转换模型
    tflite_model = converter.convert()
    
    # 保存量化后的模型
    with open(output_tflite_path, 'wb') as f:
        f.write(tflite_model)
    
    print(f"✅ Quantized model saved to {output_tflite_path}")

if __name__ == "__main__":
    convert_h5_to_int8_tflite("../models/best_model.h5", "../models/model_quantized_int8.tflite")
