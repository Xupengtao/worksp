#/*
# * @Author: xpt 
# * @Date: 2019-12-30 23:41:02 
# * @Last Modified by:   xpt 
# * @Last Modified time: 2019-12-30 23:41:02 
# */

import cv2
import numpy as np

def strokeEdges(src, dst, blurKsize = 7, edgeKsize = 5):
    if blurKsize >= 3:
        blurredSrc = cv2.medianBlur(src, blurKsize)
        graySrc = cv2.cvtColor(blurredSrc, cv2.COLOR_BGR2GRAY)
    else:
        graySrc = cv2.cvtColor(src, cv2.COLOR_BGR2GRAY)
    cv2.Laplacian(graySrc, cv2.CV_8U, graySrc, ksize = edgeKsize)
    cv2.imwrite("/home/admin/WorkSp/Analysis/设置density阈值为50生成图像_GaussianBlur边缘检测.bmp", graySrc)
    normalizedInverseAlpha = (1.0 / 255) * (255 - graySrc)
    channels = cv2.split(src)
    for channel in channels:
        channel[:] = channel * normalizedInverseAlpha
    cv2.merge(channels, dst)


def main():
    img = cv2.imread('/home/admin/WorkSp/Analysis/设置density阈值为50生成图像_GaussianBlur滤波处理突显轮廓.bmp')
    strokeEdges(img, img)
    cv2.imshow("strokeEdges", img)
    cv2.waitKey()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    print(__doc__)
    main()