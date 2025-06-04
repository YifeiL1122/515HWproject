"""
Model quantization script for gesture recognition model.
Supports both dynamic range quantization and full integer quantization,
by first exporting a SavedModel to avoid H5->TFLite compatibility issues.
"""
import tensorflow as tf
import numpy as np
from pathlib import Path
import config

def export_saved_model(h5_path, saved_model_dir):
    """
    Loads a Keras H5 model and exports it as a SavedModel using the Keras 3 API.
    """
    # Load the H5 model without compiling to skip optimizer restoration
    model = tf.keras.models.load_model(h5_path, compile=False)
    # Export as SavedModel using the new Keras 3 `export` method
    model.export(str(saved_model_dir))
    print(f"✅ SavedModel 导出完成到: {saved_model_dir}")
    return saved_model_dir


def dynamic_range_quantization_from_saved(saved_model_dir, output_path):
    """
    Performs dynamic range quantization (weights-only) from a SavedModel.
    """
    converter = tf.lite.TFLiteConverter.from_saved_model(str(saved_model_dir))
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    tflite_quant = converter.convert()
    with open(output_path, 'wb') as f:
        f.write(tflite_quant)
    print(f"✅ Dynamic range quantized model saved to: {output_path}")


def representative_data_gen():
    """
    Generates representative data for calibration.
    Replace this with your real preprocessing pipeline, using actual samples.
    """
    for _ in range(100):
        inp = np.random.normal(
            loc=0.0,
            scale=1.0,
            size=(1, config.INPUT_SIZE)
        ).astype(np.float32)
        yield [inp]


def full_integer_quantization_from_saved(saved_model_dir, output_path):
    """
    Performs full integer quantization (weights and activations) from a SavedModel.
    """
    converter = tf.lite.TFLiteConverter.from_saved_model(str(saved_model_dir))
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    converter.inference_input_type = tf.int8
    converter.inference_output_type = tf.int8
    converter.representative_dataset = representative_data_gen

    tflite_quant = converter.convert()
    with open(output_path, 'wb') as f:
        f.write(tflite_quant)
    print(f"✅ Full integer quantized model saved to: {output_path}")


if __name__ == "__main__":
    # Ensure the models directory exists
    models_dir = Path(config.MODEL_DIR)
    models_dir.mkdir(exist_ok=True)

    # Paths for H5 model and SavedModel export
    h5_path = models_dir / "best_model.h5"
    saved_model_dir = models_dir / "saved_model"

    # Export H5 -> SavedModel
    print("Exporting H5 model to SavedModel…")
    export_saved_model(str(h5_path), str(saved_model_dir))

    # Output paths for quantized models
    dyn_out = models_dir / "gesture_dynamic_quant.tflite"
    int_out = models_dir / "gesture_int8_quant.tflite"

    # Perform dynamic range quantization
    print("Performing dynamic range quantization…")
    dynamic_range_quantization_from_saved(saved_model_dir, str(dyn_out))

    # Perform full integer quantization
    print("Performing full integer quantization…")
    full_integer_quantization_from_saved(saved_model_dir, str(int_out))
