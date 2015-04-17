# KMP.py
# KMP string matcher in Python based on MIT book.
import sys

def kmp_matcher(T, P):
    "Search all matches in T with pattern P."
    n, m, q, s = len(T), len(P), 0, []
    f = compute_prefix_function(P)
    for i in range(0, n):
        while q > 0 and P[q] != T[i]: q = f[q-1]
        if P[q] == T[i]:
            if q == m-1:
                s.append(i-q) # finish one match
                q = f[q] # look for next match
            else: q = q+1
    return s

def compute_prefix_function(P):
    "Compute the prefix function for a given pattern."
    m, k, f = len(P), 0, [0]
    for q in range(1, m):
        while k > 0 and P[k] != P[q]: k = f[k-1]
        if P[k] == P[q]: k = k+1
        f.append(k)
    return f

if __name__ == '__main__':
    T, P = 'ccabcababcabc', 'cab'
    print(compute_prefix_function(P)) # just for testing
    print(kmp_matcher(T, P))

