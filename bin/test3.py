# 导入所需的库
#!/usr/bin/python3
import platform
import time

print(platform.python_version())
import numpy as np
import sklearn

print(sklearn.__version__)
from sklearn.datasets import fetch_openml
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score
from sklearn.model_selection import train_test_split

start_time = time.time()

# 加载MNIST数据集,
# mnist = fetch_openml('mnist_784', version=1, as_frame=False)
#60000个训练样本和10000个测试样本组成，每个样本都是一张28 * 28像素的灰度手写数字图片
mnist = fetch_openml('mnist_784', version=1, as_frame=False, parser='auto')


# 提取特征和标签
X = mnist.data
y = mnist.target.astype(int)

# 将数据集划分为训练集和测试集
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# 创建随机森林分类器
rf_classifier = RandomForestClassifier(n_estimators=100, random_state=42)

# 训练分类器
rf_classifier.fit(X_train, y_train)

# 预测测试集
y_pred = rf_classifier.predict(X_test)

# 计算准确率
accuracy = accuracy_score(y_test, y_pred)
print("随机森林模型的准确率:", accuracy)

end_time = time.time()
total_time = end_time - start_time
print("脚本运行时间（秒）:", total_time)
