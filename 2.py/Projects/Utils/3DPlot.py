# coding: utf-8
#/*
# * @Author: xpt 
# * @Date: 2020-01-03 10:14:13 
# * @Last Modified by:   xpt 
# * @Last Modified time: 2020-01-03 10:14:13 
# */

import cv2
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import matplotlib.pyplot as plt
import numpy as np

class _Plot3D:
    def __init__(self, Z):
        fig = plt.figure()
        ax = fig.gca(projection='3d')
        sizex = Z.shape[1]
        sizey = Z.shape[0]
        X = np.arange(0, sizex, 1)
        Y = np.arange(0, sizey, 1)
        X, Y = np.meshgrid(X, Y)
        surf = ax.plot_surface(X, Y, Z, rstride=1, cstride=1, cmap=cm.coolwarm, linewidth=0, antialiased=False)
        ax.set_zlim(0, 255)
        ax.zaxis.set_major_locator(LinearLocator(10))
        ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))
        fig.colorbar(surf, shrink=0.5, aspect=5)
        plt.show()

if __name__ == '__main__':
    img = cv2.imread("/home/admin/WorkSp/Analysis/设置density阈值为50生成图像_GaussianBlur滤波处理突显轮廓.bmp", 0)
    img = img[900:1000,600:800]
    Plot3D = _Plot3D(img)