import cv2
import numpy as np

epsilonMin = 0

def BarUpdate(val = 0):
    global epsilonMin
    epsilonMin  = cv2.getTrackbarPos('epsilon_min' , 'hull')

img = cv2.imread("/home/admin/WorkSp/Analysis/设置density阈值为50生成图像_GaussianBlur边缘检测.bmp", cv2.IMREAD_UNCHANGED)
cv2.imshow("img", img)
# img = cv2.pyrDown(img)
# cv2.imshow("pyrDown", img)
# ret, img = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY)
# cv2.imshow("thresh", img)
black = cv2.cvtColor(np.zeros((img.shape[1], img.shape[0]), dtype=np.uint8), cv2.COLOR_GRAY2BGR)
image, contours, hier = cv2.findContours(img, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
cv2.namedWindow('hull')
cv2.createTrackbar('epsilon_min', 'hull', 0, 10, BarUpdate)
for cnt in contours:
    epsilon = 0.01 * cv2.arcLength(cnt,True)        #计算图像轮廓周长
    if(epsilon < epsilonMin):
        continue
    approx = cv2.approxPolyDP(cnt,epsilon,True)     #多边形拟合
    hull = cv2.convexHull(cnt)                      #计算图像凸包
    cv2.drawContours(black, [cnt], -1, (0, 255, 0), 1)
    rect = cv2.minAreaRect(cnt)                     #计算最小矩形区域
    box = cv2.boxPoints(rect)
    box = np.int0(box)
    # cv2.drawContours(black, [box], 0, (255, 0, 0), 1)
    x, y, w, h = cv2.boundingRect(cnt)              #计算边界框
    cv2.rectangle(black, (x,y), (x+w, y+h), (0, 0, 255), 1)
    LeftCenter = 0                                  #计算顶点直线拟合
    LeftCount = 0
    RightCenter = 0
    RightCount = 0
    radius = 5
    if((cnt.shape[0] <= 1) or (w <= 1)):
        continue
    else:
        cntsize = cnt.shape[0]
        for i in range(0, cntsize):
            if(cnt[i][0][0] == x):
                LeftCenter = LeftCenter + cnt[i][0][1]
                LeftCount = LeftCount + 1
            elif(cnt[i][0][0] == x+w-1):
                RightCenter = RightCenter + cnt[i][0][1]
                RightCount = RightCount + 1
        LeftCenter = (x, int(LeftCenter/LeftCount))
        RightCenter = (x+w-1, int(RightCenter/RightCount))
    black = cv2.circle(black, LeftCenter, radius, (180, 60, 128), 1)
    black = cv2.circle(black, RightCenter, radius, (180, 60, 128), 1)
    black = cv2.line(black, LeftCenter, RightCenter, (100, 180, 100), 1)
    # cv2.drawContours(black, [approx], -1, (255, 255, 0), 2)
    # cv2.drawContours(black, [hull], -1, (0, 0, 255), 2)
cv2.imwrite("/home/admin/WorkSp/Analysis/设置density阈值为50生成图像_GaussianBlur边缘轮廓检测.bmp", black)
cv2.imshow("hull", black)
cv2.waitKey()
cv2.destroyAllWindows()
