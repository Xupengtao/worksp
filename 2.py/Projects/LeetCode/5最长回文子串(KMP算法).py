# -*- coding: utf-8 -*-
"""
Created on Thu Oct 24 02:24:00 2019

@author: wolai
"""

class Solution(object):
    def longestPalindrome(self, s):
        """
        :type s: str
        :rtype: str
        """
        #------------------------ 执行用时 :108  ms, 在所有 python 提交中击败了89.80%的用户
        #------------------------ 内存消耗 :11.8 MB, 在所有 python 提交中击败了32.18%的用户
        # NewStr = "#"
        # for char in s:
        #     NewStr += char + "#"
        # PalindromeLens = {}
        # CoverRangeLoc = 0
        # CoverRange = 0
        # PalindromeLen = 0
        # MaxPalindromeLen = 0
        # MaxPalindromeStr = ""
        # StrLens = len(NewStr)
        # for i in range(StrLens):
        #     if i < CoverRange:
        #         if PalindromeLens[2 * CoverRangeLoc - i] + i < CoverRange:
        #             PalindromeLens[i] = PalindromeLens[2 * CoverRangeLoc - i]
        #             continue
        #         else:
        #             PalindromeLen = CoverRange - i
        #             pre = 2*i - CoverRange - 1
        #             tail = CoverRange + 1
        #     else:
        #         PalindromeLen = 0
        #         pre = i-1
        #         tail = i+1
        #     while pre >= 0 and tail < StrLens and NewStr[pre] == NewStr[tail]:
        #         PalindromeLen += 1
        #         pre -= 1
        #         tail += 1
        #     PalindromeLens[i] = PalindromeLen
        #     if MaxPalindromeLen < PalindromeLen:
        #         MaxPalindromeLen = PalindromeLen
        #         MaxPalindromeStr = NewStr[i-MaxPalindromeLen:i+MaxPalindromeLen+1]
        #         MaxPalindromeStr = MaxPalindromeStr.replace("#","")
        #     if tail > CoverRange:
        #         CoverRange = tail - 1
        #         CoverRangeLoc = i
        # return MaxPalindromeStr  
        #------------------------------------------------------------------------  
        # 马拉车算法
        # 先在字符串中间加符号隔开，使得奇偶回文数的形式统一
        # 然后用kmp的思想去优化中心扩散
        if len(s)== 0:return ""
        s_new  = '#' + '#'.join(s) + '#'
        #print(s_new)
        
        #已遍历的最大右边界
        mx = 0
        #对应的中心点
        mid = 0  
        
        l = len(s_new)
        # 扩散半径数组，初始值1或者0都可以，只是代表刚开始的时候扩散半径是多少而已
        p = [1]*l
        
        for i in range(l):
            if i<mx:
                # 这个时候可以用已经计算过的值
                # 不能超过已遍历的右边界
                # i对应的镜像 = 2*mid - i
                # 由mx定义可知半径最长不会超过mx-i
                p[i] = min(mx-i,p[2*mid-i])
            
            # 主要的优化已经在上面节省了时间，接下来就是正常的扩散
            while( i-p[i]>=0 and i+p[i]<l and s_new[i-p[i]] == s_new[i+p[i]]):
                p[i] += 1
            
            # 记录一下mx和mid
            if i+p[i] > mx:
                mx = i+p[i]
                mid = i
        
        maxr = max(p)
        ans = p.index(maxr)
        # 因为跳出循环的时候多加了1，所以实际上的扩散半径应该减1
        maxr -= 1

        return s_new[ans-maxr:ans+maxr+1].replace('#',"")
    
Sol = Solution()
s = "babadada"
print(Sol.longestPalindrome(s))     