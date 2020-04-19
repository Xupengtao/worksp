import cv2
import sys
import numpy as np

img = cv2.imread('/home/admin/WorkSp/Analysis/BackUp/设置density阈值为50生成图像_GaussianBlur边缘检测_screenshot3.bmp')

gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

sift = cv2.BRISK_create()
keypoints, descriptor = sift.detectAndCompute(gray,None)

img = cv2.drawKeypoints(image=img, outImage=img, keypoints = keypoints, flags = 4, color = (51, 163, 236))

cv2.imshow('sift_keypoints', img)
while (True):
  if cv2.waitKey(100) & 0xff == ord("q"):
    break
cv2.destroyAllWindows()
