import cv2
import numpy as np

imgpath = '/home/admin/WorkSp/Analysis/设置density阈值为50生成图像_GaussianBlur边缘轮廓检测.bmp'
img = cv2.imread(imgpath)
imgscreenshot = None
cv2.namedWindow('screenshot')

def BarUpdate(val = 0):
    global imgscreenshot
    left  = cv2.getTrackbarPos('left' , 'img')
    right = cv2.getTrackbarPos('right', 'img')
    up    = cv2.getTrackbarPos('up'   , 'img')
    down  = cv2.getTrackbarPos('down' , 'img')
    imgscreenshot = img[left:right, up:down]
    cv2.imshow("screenshot", imgscreenshot)

if __name__ == '__main__':
    cv2.namedWindow('img')
    cv2.createTrackbar('left' , 'img', 0, img.shape[1], BarUpdate)
    cv2.createTrackbar('right', 'img', img.shape[1], img.shape[1], BarUpdate)
    cv2.createTrackbar('up'   , 'img', 0, img.shape[0], BarUpdate)
    cv2.createTrackbar('down' , 'img', img.shape[0], img.shape[0], BarUpdate)
    cv2.imshow("img", img)
    cv2.waitKey()
    cv2.imwrite("/home/admin/WorkSp/Analysis/screenshot_设置density阈值为50生成图像_GaussianBlur边缘轮廓检测.bmp", imgscreenshot)
    cv2.destroyAllWindows()