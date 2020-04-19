# -*- coding: utf-8 -*-
"""
Created on Thu Oct 24 00:46:47 2019

@author: wolai
"""

class Solution(object):
    def findMedianSortedArrays(self, nums1, nums2):
        """
        :type nums1: List[int]
        :type nums2: List[int]
        :rtype: float
        """
        #------------------------ 执行用时 :72 ms, 在所有 python 提交中击败了99.80%的用户
        #------------------------ 内存消耗 :11.8 MB, 在所有 python 提交中击败了16.18%的用户
        # m = len(nums1)
        # n = len(nums2)
        # LeftNums = 0
        # Loc1 = 0
        # Loc2 = 0
        # if (m+n)%2 == 0:
        #     Median = (m+n)/2 - 1
        #     while 1:
        #         if LeftNums == Median and m != 0 and n != 0:
        #             if(nums1[Loc1] < nums2[Loc2]):
        #                 Median = nums1[Loc1]
        #                 Loc1 += 1
        #                 if Loc1 != m:
        #                     if(nums1[Loc1] < nums2[Loc2]):
        #                         Median = (Median+nums1[Loc1])/2.0
        #                     else:
        #                         Median = (Median+nums2[Loc2])/2.0
        #                 else:
        #                     Median = (Median+nums2[Loc2])/2.0
        #             else:
        #                 Median = nums2[Loc2]
        #                 Loc2 += 1
        #                 if Loc2 != n:
        #                     if(nums1[Loc1] < nums2[Loc2]):
        #                         Median = (Median+nums1[Loc1])/2.0
        #                     else:
        #                         Median = (Median+nums2[Loc2])/2.0
        #                 else:
        #                     Median = (Median+nums1[Loc1])/2.0
        #             break
        #         if Loc1 < m and Loc2 < n:
        #             if(nums1[Loc1] < nums2[Loc2]):
        #                 LeftNums += 1
        #                 Loc1 += 1
        #             else:
        #                 LeftNums += 1
        #                 Loc2 += 1
        #         if Loc1 == m:
        #             Loc2 += Median - LeftNums
        #             Median = (nums2[Loc2]+nums2[Loc2+1])/2.0
        #             break
        #         elif Loc2 == n:
        #             Loc1 += Median - LeftNums
        #             Median = (nums1[Loc1]+nums1[Loc1+1])/2.0
        #             break
        # else:
        #     Median = int((m+n)/2)
        #     while 1:
        #         if LeftNums == Median and m != 0 and n != 0:
        #             if(nums1[Loc1] < nums2[Loc2]):
        #                 Median = nums1[Loc1]
        #             else:
        #                 Median = nums2[Loc2]
        #             break
        #         if Loc1 < m and Loc2 < n:
        #             if(nums1[Loc1] < nums2[Loc2]):
        #                 LeftNums += 1
        #                 Loc1 += 1
        #             else:
        #                 LeftNums += 1
        #                 Loc2 += 1
        #         if Loc1 == m:
        #             Loc2 += Median - LeftNums
        #             Median = nums2[Loc2]
        #             break
        #         elif Loc2 == n:
        #             Loc1 += Median - LeftNums
        #             Median = nums1[Loc1]
        #             break
        # return Median
        n1 = len(nums1)
        n2 = len(nums2)
        if n1 > n2:
            return self.findMedianSortedArrays(nums2,nums1)
        k = (n1 + n2 + 1)//2
        left = 0
        right = n1
        while left < right :
            m1 = left +(right - left)//2
            m2 = k - m1
            if nums1[m1] < nums2[m2-1]:
                left = m1 + 1
            else:
                right = m1
        m1 = left
        m2 = k - m1 
        c1 = max(nums1[m1-1] if m1 > 0 else float("-inf"), nums2[m2-1] if m2 > 0 else float("-inf") )
        if (n1 + n2) % 2 == 1:
            return c1
        c2 = min(nums1[m1] if m1 < n1 else float("inf"), nums2[m2] if m2 <n2 else float("inf"))
        return (c1 + c2) / 2.0

Sol = Solution()
nums1 = [1,2]
nums2 = [3,4]
print(Sol.findMedianSortedArrays(nums1,nums2))                   