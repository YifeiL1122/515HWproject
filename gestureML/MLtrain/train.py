"""训练脚本：42维归一化（x, y）手势模型"""
import tensorflow as tf
import numpy as np
from pathlib import Path
from model import create_skeleton_classifier
from data_augmentation import augment_keypoints
import config
from sklearn.model_selection import train_test_split

def load_data():
    """加载所有处理好的关键点数据"""
    data_dir = Path('ML2/processed_data')
    X = []
    y = []
    gesture_mapping = {}
    
    print("Loading data from:", data_dir)
    for idx, npy_file in enumerate(sorted(data_dir.glob('*_keypoints.npy'))):
        gesture_name = npy_file.stem.replace('_keypoints', '')
        gesture_mapping[idx] = gesture_name
        print(f"Loading {gesture_name} data...")
        
        keypoints = np.load(npy_file)
        print(f"Data shape for {gesture_name}: {keypoints.shape}")  # 添加这行来查看数据形状
        labels = np.full(len(keypoints), idx)
        
        X.append(keypoints)
        y.append(labels)
        print(f"Loaded {len(keypoints)} samples for {gesture_name}")
    
    X = np.concatenate(X, axis=0)
    y = np.concatenate(y, axis=0)
    
    print(f"\nFinal data shape: {X.shape}")  # 添加这行来查看最终数据形状
    return X, y, gesture_mapping

def prepare_data(X, y):
    """划分训练集/验证集"""
    return train_test_split(X, y, test_size=0.2, random_state=42, stratify=y)

def train_model():
    print("Starting training process...")

    X, y, gesture_mapping = load_data()
    X_train, X_val, y_train, y_val = prepare_data(X, y)

    print("\nCreating model...")
    model = create_skeleton_classifier()
    model.summary()

    optimizer = tf.keras.optimizers.Adam(learning_rate=config.LEARNING_RATE)
    model.compile(
        optimizer=optimizer,
        loss='sparse_categorical_crossentropy',
        metrics=['accuracy']
    )

    callbacks = [
        tf.keras.callbacks.ModelCheckpoint(
            'ML2/models/best_model.h5',
            monitor='val_accuracy',
            save_best_only=True,
            verbose=1
        ),
        tf.keras.callbacks.EarlyStopping(
            monitor='val_accuracy',
            patience=config.EARLY_STOPPING_PATIENCE,
            restore_best_weights=True,
            verbose=1
        ),
        tf.keras.callbacks.ReduceLROnPlateau(
            monitor='val_accuracy',
            factor=0.5,
            patience=5,
            min_lr=1e-6,
            verbose=1
        )
    ]

    print("\nStarting training...")
    history = model.fit(
        X_train, y_train,
        batch_size=config.BATCH_SIZE,
        epochs=config.EPOCHS,
        validation_data=(X_val, y_val),
        callbacks=callbacks,
        verbose=1
    )

    print("\nEvaluating best model...")
    test_loss, test_acc = model.evaluate(X_val, y_val, verbose=0)
    print(f"Test accuracy: {test_acc:.4f}")

    print("\nConverting to TFLite...")
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]  # ✅ 加入量化优化
    tflite_model = converter.convert()

    Path("ML2/models").mkdir(parents=True, exist_ok=True)
    Path("ML2/models/model.tflite").write_bytes(tflite_model)
    print("Training completed!")

if __name__ == "__main__":
    train_model()
