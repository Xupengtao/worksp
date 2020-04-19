class Solution:
    def longestCommonPrefix(self, strs) -> str:
        # def strequ(str1,str2):
        #     if str1 == None:
        #         return None
        #     if str2 == None:
        #         return str1
        #     if str1 == str2:
        #         return str1
        #     else:
        #         return None
        # strsLenList = [i for i in map(len,strs)]
        # if strsLenList == []:
        #     return ''
        # tslistminnum = min(strsLenList)
        # equMaxstr = ''
        # equTmp = ''
        # from functools import reduce
        # for i in range(0,tslistminnum):
        #     def StrsCat(strs):
        #         return strs[0:i+1]
        #     equTmp = reduce(strequ,map(StrsCat,strs))
        #     if equTmp != None:
        #         equMaxstr = strs[0][0:i+1]
        # return equMaxstr
        #执行用时 :44 ms, 在所有 python3 提交中击败了83.37%的用户
        #内存消耗 :13.9 MB, 在所有 python3 提交中击败了5.53%的用户
        s = ""
        for i in zip(*strs):
            if len(set(i)) == 1:
                s += i[0]
            else:
                break           
        return s
        # 作者：QQqun902025048
        # 链接：https://leetcode-cn.com/problems/longest-common-prefix/solution/2-xing-python-by-knifezhu-2/
        # 执行用时 :36 ms, 在所有 python3 提交中击败了98.10%的用户
        # 内存消耗 :13.9 MB, 在所有 python3 提交中击败了5.53%的用户
        # r = [len(set(c)) == 1 for c in zip(*strs)] + [0]
        # return strs[0][:r.index(0)] if strs else ''


solutution = Solution()
solutution.longestCommonPrefix(["flower","flow","flight"])