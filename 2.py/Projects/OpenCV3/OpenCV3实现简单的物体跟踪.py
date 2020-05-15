# -*- coding: utf-8 -*-
import cv2  
import numpy as np  
import sys
import time
reload(sys)
sys.setdefaultencoding('utf8')
#打开USB摄像头
#camera=cv2.VideoCapture(0)  

#获得视频的格式  
camera= cv2.VideoCapture(0)
time.sleep(2)


#以下fps，size用不了
#获得码率及尺寸  
#fps = camera.get(cv2.CV_CAP_PROP_FPS)  
#size = (int(camera.get(cv2.CV_CAP_PROP_FRAME_WIDTH)), int(camera.get(cv2.CV_CAP_PROP_FRAME_HEIGHT)))  

#指定写视频的格式, I420-avi, MJPG-mp4  
#videoWriter = cv2.VideoWriter('oto_other.mp4', cv2.cv.CV_FOURCC('M', 'J', 'P', 'G'), fps, size)  

#设置窗口大小，缺省的过大
cv2.namedWindow("video",0);
cv2.resizeWindow("video", 640, 480);
cv2.namedWindow("BW-Video",0);
cv2.resizeWindow("BW-Video", 640, 480);
cv2.namedWindow("DI-Video",0);
cv2.resizeWindow("DI-Video", 640, 480);


firstframe=None

while (camera.isOpened()):  

    ret,frame = camera.read()
    #print ret, frame
    
    if not ret:  
        break
        
    gray=cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)  
    gray=cv2.GaussianBlur(gray,(21,21),0)  
    
    if firstframe is None:  
        firstframe=gray  
        continue  
    
    #获得与背景的差值，背景很重要，最好是除了移动的主体之外，什么都不懂，且背景中没有移动的主体
    frameDelta = cv2.absdiff(firstframe,gray)  
    
    #获得黑白的图片
    thresh = cv2.threshold(frameDelta, 65, 255, cv2.THRESH_BINARY)[1]  
    thresh = cv2.dilate(thresh, None, iterations=2)  
    
    #获得移动物体的轮廓，注意opencv3.0以后，返回参数的位置不同，轮廓在第2位
    cnts, hierarchy= cv2.findContours(thresh.copy(),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)  
    #print cnts
    
    #可以直接画出轮廓，但效果不如方框
    #cv2.drawContours(frame, hierarchy, -1, (0, 255, 0), 3)
 
    
    for c in cnts:
    
        # if the contour is too small, ignore it
        # 轮廓面积
        #area = cv2.contourArea(c)
        #print area
        # 周长，或者说，弧长；第二个参数的True表示该轮廓是否封闭
        #perimeter = cv2.arcLength(c,True)
        #sprint  perimeter
        
        if cv2.contourArea(c) <= 8000:
            continue
    
        # compute the bounding box for the contour, draw it on the frame,
        # and update the text
        
        # 计算轮廓的边界框，在当前帧中画出该框
        x,y,w,h = cv2.boundingRect(c)        
            
        #x,y,w,h=cv2.boundingRect(thresh)  
        frame=cv2.rectangle(frame,(x,y),(x+w,y+h),(0,0,255),3)  
        
        # 计算的最小轮廓
        #rect = cv2.minAreaRect(c)
        #print rect
    
    cv2.imshow(u"video", frame)  
    cv2.imshow(u"BW-Video", thresh)  
    cv2.imshow(u"DI-Video", frameDelta)  
    key = cv2.waitKey(1)
     
    if key == ord("q"):  
        break  

camera.release()  
cv2.destroyAllWindows()  
