#!/usr/bin/python
# -*- coding:utf-8 -*-

from sklearn.ensemble import AdaBoostClassifier, AdaBoostRegressor
from sklearn.tree import DecisionTreeClassifier, DecisionTreeRegressor
from sklearn.model_selection import GridSearchCV, train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import accuracy_score, mean_squared_error
from sklearn.decomposition import PCA
from sklearn.externals import joblib
from time import time
import numpy as np
import pandas as pd
import mnist
import roc

import matplotlib.pyplot as plt

if __name__ == "__main__":
    # 读取Mnist数据集, 测试AdaBoost的分类模型
    mnistSet = mnist.loadLecunMnistSet()
    train_X, train_Y, test_X, test_Y = mnistSet[0], mnistSet[1], mnistSet[2], mnistSet[3]

    m, n = np.shape(train_X)
    print(m,n)

    for i in range(m):
        for j in range(n):
            if train_X[i][j] > 200:
                train_X[i][j] = 255
            else:
                train_X[i][j] = 0

    idx = list(range(m))
    # 数据归一化
    # for i in range(int(np.ceil(1.0*m/num))):
    #     minEnd = min((i+1)*num, m)
    #     sub_idx = idx[i*num:minEnd]
    #     train_X[sub_idx] = StandardScaler().fit_transform(train_X[sub_idx])
    # test_X = StandardScaler().fit_transform(test_X)
    np.random.shuffle(idx)

    # 使用PCA降维
    # num = 30000
    # pca = PCA(n_components=0.9, whiten=True, random_state=0)
    # for i in range(int(np.ceil(1.0 * m / num))):
    #     minEnd = min((i + 1) * num, m)
    #     sub_idx = idx[i * num:minEnd]
    #     train_pca_X = pca.fit_transform(train_X[sub_idx])
    #     print np.shape(train_pca_X)

    print("\n**********测试AdaBoostClassifier类**********")
    t = time()
    model = AdaBoostClassifier(
        base_estimator=DecisionTreeClassifier(splitter='random', max_features=90, max_depth=50, min_samples_split=6,
                                              min_samples_leaf=3), n_estimators=1200, learning_rate=0.005)
    # 拟合训练数据集
    model.fit(train_X, train_Y)
    joblib.dump(model, 'save/adaboost.pkl')
    # 预测训练集
    train_Y_hat = model.predict(train_X[idx])
    print("训练集精确度: ", accuracy_score(train_Y[idx], train_Y_hat))
    # 预测测试集
    test_Y_hat = model.predict(test_X)
    print("测试集精确度: ", accuracy_score(test_Y, test_Y_hat))
    print("总耗时:", time() - t, "秒")

    # 绘制ROC曲线
    n_class = len(np.unique(train_Y))
    roc.drawROC(n_class, test_Y, test_Y_hat)

