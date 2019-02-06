import numpy as np
from sklearn import datasets
from PIL import Image
import PIL.ImageOps
import mnist
import os
from sklearn.externals import joblib
import csv

model = joblib.load('save/adaboost.pkl')

ddir='E:/vs workspace/MeanShiftSegmentation/MeanShiftSegmentation/result/number'
ddirList=os.listdir(ddir)
for k in range(len(ddirList)):

    datasetDir = ddir+'/'+ddirList[k]+'/'
    imageList = os.listdir(datasetDir)
    dirNum = len(imageList)
    file = open('predict.txt', 'a')
    testList = []
    tempList = []


    def func(x):
        a = len(x)
        return int(x[:a - 4])


    for i in range(dirNum):
        imageDir = datasetDir + str(i) + '/'
        imageList = os.listdir(imageDir)
        testList.clear()
        imageList.sort(key=lambda x: func(x))
        for j in range(len(imageList)):
            img = Image.open(imageDir + imageList[j])
            img = img.resize((28, 28))
            img = img.convert("L")
            test = np.array(img).reshape(1, -1)
            testList.append(test[0])

        # 预测训练集
        predict_Y = model.predict(testList)
        print(imageDir)
        tempStr = [str(x) for x in predict_Y]
        temp = ''.join(tempStr)
        print(temp)
        tempList.append(temp)

    with open('nuber.csv', 'a') as csvfile:
        csvwriter = csv.writer(csvfile, dialect=("excel"))
        csvwriter.writerow(tempList)

import matplotlib.pyplot as plt
# 导入字体管理器，用于提供中文支持
import matplotlib.font_manager as fm
font_set= fm.FontProperties(fname='C:/Windows/Fonts/msyh.ttc', size=14)

# 将图像和目标标签合并到一个列表中
images_and_labels = list(zip(testList, predict_Y))

# 打印数据集的前8个图像
plt.figure(figsize=(16, 10))
for index, (image, label) in enumerate(images_and_labels[:32]):
    plt.subplot(4, 8, index + 1)
    plt.axis('off')
    image1=Image.open(imageDir+imageList[index])
    plt.imshow(image1, cmap=plt.cm.gray_r,interpolation='nearest')
    plt.title(u'预测结果：' + str(label), fontproperties=font_set)

plt.show()
