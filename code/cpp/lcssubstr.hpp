#ifndef _LCSUBSTR_HPP_
#define _LCSUBSTR_HPP_

#include <algorithm>
#include <vector>
#include <list>
#include <cassert>

// Almost from [http://en.wikipedia.org/wiki/Longest_common_substring_problem]:
// The longest common substring problem is to find the longest string (or strings)
// that is a substring (or are substrings) of two or more strings.
// You first find the longest common suffix for all pairs of prefixes of the
// strings.
// The maximal of these longest common suffixes of possible prefixes must be
// the longest common substrings ...

// Note: Substring is not limitted to "string" here.

// Keep only the last and current row of the DP table to save memory
// time: Θ(mn); space: Θ(min(m,n))
template <class RanIt>
size_t LCSubstr(RanIt xbegin, RanIt xend, RanIt ybegin, RanIt yend,
    std::list<size_t>& offsetX, std::list<size_t>& offsetY)
{
    assert(xend > xbegin);
    assert(yend > ybegin);

    size_t M = xend - xbegin;
    size_t N = yend - ybegin;

    std::list<size_t> *offsetM = &offsetX;
    std::list<size_t> *offsetN = &offsetY;

    if (M < N) {
        std::swap(M, N);
        std::swap(xbegin, ybegin);
        std::swap(xend, yend);
        offsetM = &offsetY;
        offsetN = &offsetX;
    }

    std::vector<std::vector<size_t> > L(2);
    L[0].resize(N + 1);
    L[1].resize(N + 1);
    L[0][0] = 0;
    L[1][0] = 0;
    L[0].assign(N + 1, 0);

    size_t z = 0;
    for (size_t i = 1; i <= M; ++i) {
        for (size_t j = 1; j <= N; ++j) {
            if (*(xbegin+i-1) == *(ybegin+j-1)) {
                L[i%2][j] = L[(i-1)%2][j-1] + 1;
                if (L[i%2][j] > z) {
                    z = L[i%2][j];
                    offsetM->clear();
                    offsetN->clear();
                }
                if (L[i%2][j] == z) {
                    offsetM->push_back(i - z);
                    offsetN->push_back(j - z);
                }
            }
        }
    }

    return z;
}

// time: Θ(mn); space: Θ(mn)
template <class RanIt>
size_t LCSubstr2(RanIt xbegin, RanIt xend, RanIt ybegin, RanIt yend,
    std::list<size_t>& offsetX, std::list<size_t>& offsetY)
{
    assert(xend > xbegin);
    assert(yend > ybegin);

    const size_t M = xend - xbegin;
    const size_t N = yend - ybegin;

    std::vector<std::vector<size_t> > L(M + 1);
    for (size_t i = 0; i <= M; ++i) {
        L[i].resize(N + 1);
        L[i][0] = 0;
    }
    L[0].assign(L[0].size(), 0);

    size_t z = 0;
    for (size_t i = 1; i <= M; ++i) {
        for (size_t j = 1; j <= N; ++j) {
            if (*(xbegin+i-1) == *(ybegin+j-1)) {
                L[i][j] = L[i-1][j-1] + 1;
                if (L[i][j] > z) {
                    z = L[i][j];
                    offsetX.clear();
                    offsetY.clear();
                }
                if (L[i][j] == z) {
                    offsetX.push_back(i - z);
                    offsetY.push_back(j - z);
                }
            }
        }
    }

    return z;
}

#endif // _LCSUBSTR_HPP_

