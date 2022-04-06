def exchange(nums):
        a,b = [],[]
        for item in nums:
            print(item)
            if item % 2 == 0:
                a.append(item)
            else:
                b.append(item)
        print(a)
        print(b)
        return b + a


nums = [1,2,3,4]
print(exchange(nums))

a = [-2,1,-3,4,-1,2,1,-5,4]
sum = 0
for item in a:
    if item > 0:
        sum += item
print(sum)


d = {}
c = [1,2,3,4,5,5,5,6]


str = "build yuor name!"
str = str.strip()
s = str.split()
s.reverse()
print(' '.join(s))

import sys

print(sys.maxsize)


def strToInt(str):
        str = str.strip()
        di = {'0':0,'1':1,'2':2,'3':3,'4':4,'5':5,'6':6,'7':7,'8':8,'9':9}
        if str[0]  not in ['0','1','2','3','4','5','6','7','8','9','-']:
            return 0
        if str is "": 
            return 0
        str = str.split(' ')

        ret = str[0]
        bol = 0
        if ret[0] is '-':
            bol = 1
        ret = ret.strip('-')

        max = 2 ** 31 - 1
        min = -2 ** 31
        sum = di[ret[0]]
        for i in range(1,len(ret)):
            sum = sum * 10 + di[ret[i]]
        if bol is 1:
            sum = -sum
        if sum > max:
            return max
        elif sum < min:
            return min
        else:
            return sum
print(strToInt("   -42"))

s = "nihao"
for item in s:
    print(item)
