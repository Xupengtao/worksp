class Solution:
    def letterCombinations(self, digits: str):
        numDict = {'2':['a','b','c'],
                   '3':['d','e','f'],
                   '4':['g','h','i'],
                   '5':['j','k','l'],
                   '6':['m','n','o'],
                   '7':['p','q','r','s'],
                   '8':['t','u','v'],
                   '9':['w','x','y','z']
                    }
        if digits == '':
            return ''
        def numtochar(numStr):
            if numStr >= '2' and numStr <= '9':
                return numDict[numStr]
        numChar = map(numtochar,list(digits))
        def charListtoStrList(strList1,strList2):
            strlist = []
            for i in range(len(strList1)):
                for j in range(len(strList2)):
                    strlist.append(strList1[i]+strList2[j])
            return strlist
        from functools import reduce
        strlist = reduce(charListtoStrList,numChar)
        return strlist

tlist = "23"
solt = Solution()
print(solt.letterCombinations(tlist))