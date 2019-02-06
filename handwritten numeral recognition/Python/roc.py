#!/usr/bin/python
# -*- coding:utf-8 -*-

from sklearn import metrics
from sklearn.preprocessing import label_binarize
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

def drawROC(n_class, test_Y, test_Y_hat):
    '''
    绘制ROC曲线
    :param n_class:
    :param test_Y:
    :param test_Y_hat:
    :return:
    '''
    if n_class > 2:
        # label_binarize([1, 6], classes=[1, 2, 4, 6]) ==> array([[1, 0, 0, 0], [0, 0, 0, 1]])
        # label_binarize([1, 6], classes=[1, 2, 4, 6]) ==> array([[1, 0, 0, 0], [0, 1, 0, 0]])
        test_Y_one_hot = label_binarize(test_Y, classes=np.arange(n_class))
        # 预测输入样本的属于各个类别的概率[n_samples, n_classes]
        test_Y_one_hot_hat = label_binarize(test_Y_hat, classes=np.arange(n_class))
        # fpr: 假阳率 = FP/(FP+TN)
        # tpr: 真阳率 = TP/(TP+FN)
        # thresholds: 阈值
        fpr, tpr, thresholds = metrics.roc_curve(test_Y_one_hot.ravel(), test_Y_one_hot_hat.ravel())
        # 计算AUC值, 其中x,y分别为数组形式, 根据(xi, yi)在坐标上的点, 生成曲线, 然后计算AUC值
        print('Micro AUC:\t', metrics.auc(fpr, tpr))
        # 直接根据真实值(必须是二值), 预测值(可以是0/1, 也可以是proba值)计算出auc值, 中间过程的roc计算省略
        auc = metrics.roc_auc_score(test_Y_one_hot, test_Y_one_hot_hat, average='micro')
        print('Micro AUC(System):\t', auc)
        auc = metrics.roc_auc_score(test_Y_one_hot, test_Y_one_hot_hat, average='macro')
        print('Macro AUC:\t', auc)
    else:
        fpr, tpr, thresholds = metrics.roc_curve(test_Y.ravel(), test_Y_hat.ravel())
        print('AUC:\t', metrics.auc(fpr, tpr))
        auc = metrics.roc_auc_score(test_Y, test_Y_hat)
        print('AUC(System):\t', auc)

    mpl.rcParams['font.sans-serif'] = [u'SimHei'] # 指定显示字体
    mpl.rcParams['axes.unicode_minus'] = False # 解决保存图像中负号'-'显示为方块的问题
    plt.figure(figsize=(8, 7), dpi=80, facecolor='w') # 创建一幅图
    # 绘制AUC曲线
    plt.plot(fpr, tpr, 'r-', lw=2, label='AUC=%.4f'%auc)
    plt.legend(loc='lower right')
    plt.xlim((-0.01, 1.02))
    plt.ylim((-0.01, 1.02))
    plt.xticks(np.arange(0, 1.1, 0.1))
    plt.yticks(np.arange(0, 1.1, 0.1))
    plt.xlabel(u'False Positive Rate(假阳率)', fontsize=14)
    plt.ylabel(u'True Positive Rate(真阳率)', fontsize=14)
    plt.grid(b=True, ls=":")
    plt.title(u'ROC曲线和AUC', fontsize=18)
    plt.show()
