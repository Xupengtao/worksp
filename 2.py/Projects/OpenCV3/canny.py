import cv2
import numpy as np

img = cv2.imread('/home/admin/WorkSp/Analysis/设置density阈值为50生成图像_高通滤波处理突显轮廓.bmp', 0)
cv2.imwrite('/home/admin/WorkSp/Analysis/设置density阈值为50生成图像_Canny边缘检测.bmp', cv2.Canny(img, 200, 300))
cv2.imshow("canny", cv2.imread('/home/admin/WorkSp/Analysis/设置density阈值为50生成图像_Canny边缘检测.bmp'))
cv2.waitKey()
cv2.destroyAllWindows()
