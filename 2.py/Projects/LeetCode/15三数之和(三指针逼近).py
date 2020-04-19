class Solution:
    def threeSum(self, nums: [int]) -> [[int]]:
        # nums.sort()
        # Lsec = -1
        # Rsec = len(nums)
        # for i in range(len(nums)):
        #     if nums[i] < 0:
        #         Lsec = i
        #     if nums[i] > 0:
        #         Rsec = i
        #         break
        # if Lsec == -1 or Rsec == len(nums) + 1:
        #     if Rsec-Lsec-1 < 3:
        #         return []
        #     else:
        #         return[[0,0,0]]
        # NegativeSet = set(nums[:Lsec+1])
        # PositiveSet = set(nums[Rsec:])

        # sum2 = []
        # rst = []
        # def AnsAppend(i,j,iadj):
        #     if [nums[i],nums[j]] not in sum2:
        #         sum2.append([nums[i],nums[j]])
        #         rst.append([nums[i],nums[j],iadj])

        # NumTmp = 0
        # if Rsec - Lsec > 3:
        #     AnsAppend(Lsec+1,Lsec+2,0)
        # if Rsec - Lsec > 1:
        #     if Lsec < len(nums) - Rsec:
        #         for i in range(0,Lsec+1):
        #             if NumTmp == nums[i]:
        #                 continue
        #             NumTmp = nums[i]
        #             if -nums[i] in PositiveSet:
        #                 AnsAppend(i,Lsec+1,-nums[i])
        #     else:
        #         for i in range(Rsec,len(nums)):
        #             if NumTmp == nums[i]:
        #                 continue
        #             NumTmp = nums[i]
        #             if -nums[i] in NegativeSet:
        #                 AnsAppend(Rsec-1,i,-nums[i])
        # NumTmp = []
        # SumTmp = 0
        # for i in range(0,Lsec):
        #     for j in range(i+1,Lsec+1):
        #         if NumTmp == [nums[i],nums[j]]:
        #             continue
        #         NumTmp = [nums[i],nums[j]]
        #         SumTmp = -(nums[i]+nums[j])
        #         if SumTmp in PositiveSet:
        #             AnsAppend(i,j,SumTmp)
        # NumTmp = []
        # SumTmp = 0
        # for i in range(Rsec,len(nums)-1):
        #     for j in range(i+1,len(nums)):
        #         if NumTmp == [nums[i],nums[j]]:
        #             continue
        #         NumTmp = [nums[i],nums[j]]
        #         SumTmp = -(nums[i]+nums[j])
        #         if SumTmp in NegativeSet:
        #             AnsAppend(i,j,SumTmp)
        # return rst
        nums.sort()
        res =[]
        i = 0
        for i in range(len(nums)):
            if i == 0 or nums[i]>nums[i-1]:
                l = i+1
                r = len(nums)-1
                while l < r:
                    s = nums[i] + nums[l] +nums[r]
                    if s ==0:
                        res.append([nums[i],nums[l],nums[r]])
                        l +=1
                        r -=1
                        while l < r and nums[l] == nums[l-1]:
                            l += 1
                        while l < r and nums[r] == nums[r+1]:
                            r -= 1
                    elif s>0:
                        r -=1
                    else :
                        l +=1
        return res

tlist = [-1,1,0,2,0,5,-5,7,-6,-9,8]
solt = Solution()
print(solt.threeSum(tlist))