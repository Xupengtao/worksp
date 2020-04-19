class Solution:
    def maxArea(self, height) -> int:
        AreaTmp = 0
        maxAreaTmp = 0
        Len = len(height)
        preLoc  = 0
        rearLoc = Len - 1
        preMaxVal  = height[preLoc]
        rearMaxVal = height[rearLoc]
        maxAreaTmp = min(height[preLoc],height[rearLoc])*(Len - 1)
        while preLoc < rearLoc:
            if height[preLoc] < preMaxVal:
                preLoc = preLoc + 1
                continue
            if height[rearLoc] < rearMaxVal:
                rearLoc = rearLoc - 1
                continue
            preMaxVal  = height[preLoc]
            rearMaxVal = height[rearLoc]
            
            AreaTmp =  min(preMaxVal,rearMaxVal)*(rearLoc - preLoc)
            if AreaTmp > maxAreaTmp:
                maxAreaTmp = AreaTmp
            if preMaxVal < rearMaxVal:
                preLoc = preLoc + 1
            else:
                rearLoc = rearLoc - 1
        return maxAreaTmp