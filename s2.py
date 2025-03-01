import sys

S = input()
c = int(input())

pairs = []
num = 0
char = ''

# parse the input string
for ch in S:
    if ch.isalpha():
        if char:
            pairs.append((char, num))
        char, num = ch, 0
    else:
        num = num * 10 + int(ch)
pairs.append((char, num))

total_length = sum(num for _, num in pairs)
c %= total_length  # reduce c within bounds using modulo

# find the character corresponding to the index
for char, num in pairs:
    if c < num:
        print(char)
        sys.exit()
    c -= num