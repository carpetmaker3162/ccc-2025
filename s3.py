# 11/15 on DMOJ
# approach: maintain a data structure which maintains pens sorted by prettiness, for each color
# also maintain global lists containing the max / 2nd max prettiness pens. list should only contain 1 of each color
# be able to update these data structures in log time. this solution isn't able to update in true log time
# other solutions use std::multiset (C++) or a segment tree
# 
# credit: Ivan Li

from collections import defaultdict
from bisect import insort, bisect_left

N, M, Q = map(int, input().split())
arr = [list(map(int, input().split())) for _ in range(N)]  # each element is [color, prettiness]

# create a dictionary mapping each color to a list of prettiness values
vals = defaultdict(list)  # keys are colors, values are lists of prettiness values
for c, p in arr:
    vals[c].append(p)

# for each color:
#   - append a dummy value (-1) to ensure there are always at least 2 elements
#     (this avoids having to deal with an empty "second best" later)
#   - then sort the list so that the smallest values are first and the largest last
for v in vals:
    vals[v].append(-1)    # add dummy value (padding)
    vals[v].sort()        # sort in ascending order; so the best (largest) is at the end

### Global lists for best and second best value

# for each color, the best (maximum) value is the last element of its sorted list
# store these best values for all colors in a sorted list
maximum = sorted(v[-1] for v in vals.values())

# similarly, for each color, the candidate (or second-best) is the second-to-last element
second = sorted(v[-2] for v in vals.values())

# the base sum (total) is the sum of best values across all colors
total = sum(maximum)

# on the first iteration simply output the answer for the initial state
# for later iterations, process an update query then output the answer
for _ in range(Q + 1):
    if _ != 0:  # if not the initial state, process a query update
        t, i, x = map(int, input().split())
        i -= 1  # convert to 0-indexed

        # save the old color and prettiness of pen i
        old_c, old_p = arr[i]

        # update the pen’s information based on query type
        if t == 1:
            arr[i][0] = x
        else:
            arr[i][1] = x

        # after the update, record the new color and prettiness
        new_c, new_p = arr[i]

        ### Essentially remove the old pen entirely

        # remove the old best and second-best values for the old color from the global lists
        # use bisect_left to find the index of the current best for color old_c
        # do this because after the update, the best and second-best values may have changed
        maximum.pop(bisect_left(maximum, vals[old_c][-1]))
        second.pop(bisect_left(second, vals[old_c][-2]))
        # subtract the old best value from the total sum
        total -= vals[old_c][-1]
        # remove the old pen's prettiness from the list for its old color
        # bisect_left finds the index of the old prettiness value in the sorted list
        vals[old_c].pop(bisect_left(vals[old_c], old_p))
        
        ### If the pen's color changed, update the global lists for the new color
        
        if old_c != new_c:
            maximum.pop(bisect_left(maximum, vals[new_c][-1]))
            second.pop(bisect_left(second, vals[new_c][-2]))
        # subtract the current best value for the new color from the total, because we are about to update it
        total -= vals[new_c][-1]

        ### Restore the old color's global information after previous removal

        # if the pen changed color, update the global lists for the old color
        if old_c != new_c:
            # after removal, reinsert the updated best and second-best for the old color
            insort(maximum, vals[old_c][-1])
            insort(second, vals[old_c][-2])
        # add back the updated best value for the old color to the total sum
        total += vals[old_c][-1]

        ### Update the new color's data structures with the new prettiness value

        # insert the new prettiness value into the list for the new color
        insort(vals[new_c], new_p)
        # now update the global lists for the new color with its updated best and second-best
        insort(maximum, vals[new_c][-1])
        insort(second, vals[new_c][-2])
        # add the new best value for the new color to the total sum
        total += vals[new_c][-1]
    
    # if (second[-1] - maximum[0]) is positive, there is an extra gain possible by performing a switch
    print(total + max(0, (second[-1] - maximum[0])))