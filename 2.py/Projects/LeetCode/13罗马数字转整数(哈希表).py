class Solution:
    def romanToInt(self, s) -> int:
        # RomanDict = {'I':1,'V':5,'X':10,'L':50,'C':100,'D':500,'M':1000}
        # RomanNum = 0
        # RomanTmp = 0
        # Converse = 0
        # for n,i in enumerate(s):
        #     if Converse == 1:
        #         if ((RomanDict[i] // RomanTmp) == 5) or (RomanDict[i] // RomanTmp == 10):
        #             RomanNum -= 2*RomanTmp
        #         Converse = 0
        #     RomanNum += RomanDict[i]
        #     if i == 'I' or i == 'X' or i == 'C':
        #         RomanTmp = RomanDict[i]
        #         Converse = 1
        # return RomanNum
         d = {'I':1, 'IV':3, 'V':5, 'IX':8, 'X':10, 'XL':30, 'L':50, 'XC':80, 'C':100, 'CD':300, 'D':500, 'CM':800, 'M':1000}
        return sum(d.get(s[max(i-1, 0):i+1], d[n]) for i, n in enumerate(s))

# 作者：QQqun902025048
# 链接：https://leetcode-cn.com/problems/roman-to-integer/solution/2-xing-python-on-by-knifezhu/
# 来源：力扣（LeetCode）
# 著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。

solut = Solution()
solut.romanToInt("MCMXCIV")

#c++ Hash Map
# class Solution {
# public:
#     int romanToInt(string s) {
#         unordered_map<string, int> m = {{"I", 1}, {"IV", 3}, {"IX", 8}, {"V", 5}, {"X", 10}, {"XL", 30}, {"XC", 80}, {"L", 50}, {"C", 100}, {"CD", 300}, {"CM", 800}, {"D", 500}, {"M", 1000}};
#         int r = m[s.substr(0, 1)];
#         for(int i=1; i<s.size(); ++i){
#             string two = s.substr(i-1, 2);
#             string one = s.substr(i, 1);
#             r += m[two] ? m[two] : m[one];
#         }
#         return r;
#     }
# };

# 作者：QQqun902025048
# 链接：https://leetcode-cn.com/problems/roman-to-integer/solution/2-xing-python-on-by-knifezhu/
# 来源：力扣（LeetCode）
# 著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。