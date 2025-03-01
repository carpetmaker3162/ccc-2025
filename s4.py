# 10/15 on DMOJ (TLE)
#
# overall approach:
# 1. storing states
#    - for each room, store a mapping (in room_temps) from a neighboring room to the required
#      temperature for the connecting tunnel
#    - a state is a pair (room, temp)
#    - add a starting state (1, 0)
#
# 2. tracking the best cost
#    - use a list of dictionaries (`best`) to record the best-known cost for reaching a room with
#      a given tunnel temperature
#    - best[R][T] = minimum cost to reach room R with temperature T
#
# 3. dijkstra
#    - in the PQ, store tuples of (cost, current room, previous room) which help track the
#      temperature requirement from the last tunnel used
#    - process each tunnel only once. remove a tunnel after going through it
#    - consider transitions from the current room to its neighbors, adding the cost of adjusting
#      the temperature (abs. difference) when moving to the next tunnel.
#    - stop when any state with room N is reached
#
# credit: defAnton

import sys
import heapq

input = lambda: sys.stdin.readline().strip()

N, M = map(int, input().split())
room_temps = [{} for _ in range(N+1)]

for _ in range(M):
    a, b, c = map(int, input().split())
    room_temps[a][b] = c
    room_temps[b][a] = c

room_temps[0][1] = 0
room_temps[1][0] = 0

best = [{room_temps[j][i]: -1 for i in room_temps[j]} for j in range(N+1)]
best[1][0] = -1  # (room 1, temp 0)
q = [(0, 1, 0)]

while q:
    cost, room, prev = heapq.heappop(q)
    if room == N:
        print(cost)
        sys.exit()

    # process each tunnel only once? (avoid cycles)
    if room in room_temps[prev]:
        prev_temp = room_temps[prev][room]
        room_temps[prev].pop(room)
        room_temps[room].pop(prev)
    else:
        continue

    if best[room][prev_temp] == -1:
        best[room][prev_temp] = cost
        for i in room_temps[room]:
            next_temp = room_temps[room][i]
            if best[i][next_temp] == -1:
                heapq.heappush(q, (cost + abs(next_temp - prev_temp), i, room))