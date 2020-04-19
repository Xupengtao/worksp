class Solution:
    def intToRoman(self, num: int) -> str:
    #     RomanNumUnit = ['I','V','X','L','C','D','M','','']
    #     def RomanChange(num,order):
    #         RomanTmp = ''
    #         if num == 4:
    #             RomanTmp = RomanNumUnit[2*order] + RomanNumUnit[2*order + 1]
    #             num = 0
    #         elif num == 9:
    #             RomanTmp = RomanNumUnit[2*order] + RomanNumUnit[2*order + 2]
    #             num = 0
    #         elif num >= 5 and num < 9:
    #             RomanTmp = RomanNumUnit[2*order + 1]
    #             num = num - 5
    #         for i in range(0,num):
    #             RomanTmp += RomanNumUnit[2*order]
    #         return RomanTmp

    #     RomanNum = ''
    #     numTmp = 0
    #     i = 3
    #     while i >= 0:
    #         numTmp = num // (10**i)
    #         num = num % (10**i)
    #         RomanNum += RomanChange(numTmp,i)
    #         i = i - 1
    #     return RomanNum
        # 把阿拉伯数字与罗马数字可能出现的所有情况和对应关系，放在两个数组中
        # 并且按照阿拉伯数字的大小降序排列，这是贪心选择思想
        nums = [1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1]
        romans = ["M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"]

        index = 0
        res = ''
        while index < 13:
            # 注意：这里是等于号，表示尽量使用大的"面值"
            while num >= nums[index]:
                res += romans[index]
                num -= nums[index]
            index += 1
        return res

# 作者：liweiwei1419
# 链接：https://leetcode-cn.com/problems/integer-to-roman/solution/tan-xin-suan-fa-by-liweiwei1419/
# 来源：力扣（LeetCode）
# 著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。

# c++ 贪心算法
# class Solution {
# public:
#     string intToRoman(int num) {
#         map<int,string> mapRom = {{1,"I"},{4,"IV"},{5,"V"},{9,"IX"},{10,"X"},{40,"XL"},{50,"L"}, {90,"XC"},{100,"C"},                                              {400,"CD"},{500,"D"},{900,"CM"}, {1000,"M"} };
#         map<int,string>::reverse_iterator  r_iter;
        
#         string ret;
       
#         r_iter=mapRom.rbegin();
#         while(r_iter!=mapRom.rend())
#         {
#             if(num >= r_iter->first)
#             {
#                 ret += r_iter->second;
#                 num-= r_iter->first;
#             }
#             else
#                 r_iter++;
#         }
#         return ret;
#     }

# };

# 作者：lan-tian-25
# 链接：https://leetcode-cn.com/problems/integer-to-roman/solution/ctan-xin-suan-fa-by-lan-tian-25/
# 来源：力扣（LeetCode）
# 著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。