#!/usr/bin/python
# -*- coding:utf-8 -*-

import numpy as np
import struct
import gzip
from time import time

# http://blog.csdn.net/u010165147/article/details/50599490
def loadLecunMnistImageSet(which=0):
    '''
    读取Lecun Mnist(http://yann.lecun.com/exdb/mnist/)数据集样本的解压缩文件
    :param which:
    :return:
    '''
    # print "load image set"
    binfile = None
    if which==0:
        binfile = open("data/train-images.idx3-ubyte", 'rb')
    else:
        binfile = open("data/t10k-images.idx3-ubyte", 'rb')
    buffers = binfile.read()

    head = struct.unpack_from('>IIII', buffers, 0)
    # print "head = ", head

    offset = struct.calcsize('>IIII')
    imgNum = head[1]
    width = head[2]
    height = head[3]
    # [60000]*28*28
    bits = imgNum * width * height
    bitsString = '>' + str(bits) + 'B' # like '>47040000B'

    imgs = struct.unpack_from(bitsString, buffers, offset)

    binfile.close()
    imgs = np.reshape(imgs, [imgNum, width*height])
    # print "load imgs finished"
    return imgs

def loadLecunMnistLabelSet(which=0):
    '''
    读取Lecun Mnist(http://yann.lecun.com/exdb/mnist/)数据集类别标签的解压缩文件
    :param which:
    :return:
    '''
    # print "load label set"
    binfile = None
    if which==0:
        binfile = open("data/train-labels.idx1-ubyte", 'rb')
    else:
        binfile = open("data/t10k-labels.idx1-ubyte", 'rb')
    buffers = binfile.read()

    head = struct.unpack_from('>II', buffers, 0)
    # print "head = ", head
    imgNum = head[1]

    offset = struct.calcsize('>II')
    numString = '>' + str(imgNum) + "B"
    labels = struct.unpack_from(numString, buffers, offset)
    binfile.close()
    labels = np.reshape(labels, [imgNum, 1])

    # print 'load label finished'
    return labels

def readTheanoMnistFile(dataset):
    '''
    读取Theano Mnist(http://deeplearning.net/tutorial/gettingstarted.html#index-1)数据集的压缩文件
    :param dataset:
    :return:
    '''
    f = gzip.open(dataset, 'rb')
    train_set, valid_set, test_set = pickle.load(f)
    f.close()
    return [train_set, valid_set, test_set]

def loadTheanoMnistSet():
    '''
    读取Theano Mnist(http://deeplearning.net/tutorial/gettingstarted.html#index-1)数据集
    :return:
    '''
    # print "load dataset"
    sets = readTheanoMnistFile("data/mnist.pkl.gz")
    train_X, train_Y = sets[0]
    valid_X, valid_Y = sets[1]
    test_X, test_Y = sets[2]
    # print "load dataset end"
    return [train_X, train_Y, valid_X, valid_Y, test_X, test_Y]

def readLecunMnistFile(image, label):
    '''
    读取Lecun Mnist(http://yann.lecun.com/exdb/mnist/)数据集的压缩文件
    :param image:
    :param label:
    :return:
    '''
    minist_dir = ''
    with gzip.open(minist_dir + label) as flbl:
        magic, num = struct.unpack(">II", flbl.read(8))
        label = np.fromstring(flbl.read(), dtype=np.int8)
    with gzip.open(minist_dir + image, 'rb') as fimg:
        magic, num, rows, cols = struct.unpack(">IIII", fimg.read(16))
        image = np.fromstring(fimg.read(), dtype=np.uint8).reshape(len(label), rows*cols)
    return image, label

def loadLecunMnistSet():
    '''
    读取Lecun Mnist(http://yann.lecun.com/exdb/mnist/)数据集
    :return:
    '''
    train_X, train_Y = readLecunMnistFile(
            'data/train-images-idx3-ubyte.gz',
            'data/train-labels-idx1-ubyte.gz')
    test_X, test_Y = readLecunMnistFile(
            'data/t10k-images-idx3-ubyte.gz',
            'data/t10k-labels-idx1-ubyte.gz')
    return [train_X, train_Y, test_X, test_Y]

if __name__=="__main__":
    np.set_printoptions(linewidth=1000)

    # print u'**********读取Lecun Mnist数据集的解压缩文件**********'
    # t = time()
    # train_X = loadLecunMnistImageSet()
    # train_Y = loadLecunMnistLabelSet()
    # test_X = loadLecunMnistImageSet(which=1)
    # test_Y = loadLecunMnistLabelSet(which=1)
    # print "Total time:", time() - t
    # print 'train_X:', np.shape(train_X), '\ttrain_Y:', np.shape(train_Y)
    # print 'test_X:', np.shape(test_X), '\ttest_Y:', np.shape(test_Y)
    # print np.reshape(train_X[0], (28, 28))
    # print train_Y[0]

    print(u'\n**********读取Theano Mnist数据集的压缩文件**********')
    t = time()
    mnistSet = loadTheanoMnistSet()
    print("Total time:", time() - t)
    train_X, train_Y, valid_X, valid_Y, test_X, test_Y = mnistSet[0], mnistSet[1], mnistSet[2], mnistSet[3], mnistSet[4], mnistSet[5]
    print('train_X:', np.shape(train_X), '\ttrain_Y:', np.shape(train_Y))
    print('valid_X:', np.shape(valid_X), '\tvalid_Y:', np.shape(valid_Y))
    print('test_X:', np.shape(test_X), '\ttest_Y:', np.shape(test_Y))
    # print np.reshape(train_X[0], (28, 28))
    # print train_Y[0]

    print(u'\n**********读取Lecun Mnist数据集的压缩文件**********')
    t = time()
    mnistSet = loadLecunMnistSet()
    train_X, train_Y, test_X, test_Y = mnistSet[0], mnistSet[1], mnistSet[2], mnistSet[3]
    print("Total time:", time() - t)
    print('train_X:', np.shape(train_X), '\ttrain_Y:', np.shape(train_Y))
    print('test_X:', np.shape(test_X), '\ttest_Y:', np.shape(test_Y))
    # print np.reshape(train_X[0], (28, 28))
    # print train_Y[0]
