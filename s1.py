A, B, X, Y = map(int, input().split())
P = 2 * (A + B + X + Y)
ans = min(P - 2*min(B, Y), P - 2*min(A, X))
print(ans)