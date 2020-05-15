import cv2
import numpy as np

imgpath = '/home/admin/WorkSp/Analysis/z.bmp'

if __name__ == '__main__':
    img = cv2.imread(imgpath, 0)
    cv2.imshow("img", img)
    cv2.waitKey()
    cv2.destroyAllWindows()