# -*- coding: utf-8 -*-
"""
Created on Sun Nov 10 00:16:37 2019

@author: wolai
"""

class Solution(object):
#     def isMatch(self, text, pattern):
#         dp = [[False] * (len(pattern) + 1) for _ in range(len(text) + 1)]

#         dp[-1][-1] = True
#         for i in range(len(text), -1, -1):
#             for j in range(len(pattern) - 1, -1, -1):
#                 first_match = i < len(text) and pattern[j] in {text[i], '.'}
#                 if j+1 < len(pattern) and pattern[j+1] == '*':
#                     dp[i][j] = dp[i][j+2] or first_match and dp[i+1][j]
#                 else:
#                     dp[i][j] = first_match and dp[i+1][j+1]

#         return dp[0][0]
    def isMatch(self, text, pattern):
        memo = {}
        def dp(i, j):
            if (i, j) not in memo:
                if j == len(pattern):
                    ans = i == len(text)
                else:
                    first_match = i < len(text) and pattern[j] in {text[i], '.'}
                    if j+1 < len(pattern) and pattern[j+1] == '*':
                        ans = dp(i, j+2) or first_match and dp(i+1, j)
                    else:
                        ans = first_match and dp(i+1, j+1)

                memo[i, j] = ans
            return memo[i, j]

        return dp(0, 0)
    
s = "mississippi"
p = "mis*is*ip*."
sol = Solution()
print(sol.isMatch(s,p))