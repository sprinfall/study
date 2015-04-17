#ifndef _BALANCED_01_MATRIX_HPP_
#define _BALANCED_01_MATRIX_HPP_

// From [http://en.wikipedia.org/wiki/Dynamic_programming]:
// Consider the problem of assigning values, either zero or one, to the
// positions of an n x n matrix, n even, so that each row and each column
// contains exactly n/2 zeros and n/2 ones.

#include <list>
#include <vector>
#include <cassert>

typedef std::vector<bool> assignment_t;
typedef std::list<assignment_t> assignments_t;

void __all_assignments(size_t left1, size_t left0, size_t offset, assignments_t& assignments)
{
    static const size_t N = left1 * 2;
    static assignment_t assignment(N);

    assert(offset < N);

    if (left1 == 0 && left0 == 0) {
        assignments.push_back(assignment);
        return;
    }

    if (left1 > 0) {
        assignment[offset] = true;
        __all_assignments(left1 - 1, left0, offset + 1, assignments);
    }

    if (left0 > 0) {
        assignment[offset] = false;
        __all_assignments(left1, left0 - 1, offset + 1, assignments);
    }
}

// get all possible assignments for one row
void all_assignments(size_t n, assignments_t& assignments)
{
    assert(n >= 2);
    __all_assignments(n/2, n/2, 0, assignments);
}

typedef std::vector<std::pair<int, int> > args_t;

size_t __balanced_01_matrix(size_t k, size_t n, args_t& args)
{
    assert (k >= 1 && k <= n);

    assignments_t assignments;
    all_assignments(n, assignments);

    size_t sum = 0;

    // for each possible assignment
    assignments_t::iterator i;
    for (i = assignments.begin(); i != assignments.end(); ++i) {
        args_t nargs(args);

        bool is_negative = false;
        bool is_zero = true;

        for (size_t j = 0; j < i->size(); ++j) {
            if ((*i)[j] == false) { --nargs[j].first; }
            else { --nargs[j].second; }

            if (is_zero) {
                if (nargs[j].first != 0 || nargs[j].second != 0) {
                    is_zero = false;
                }
            }

            if (nargs[j].first < 0 || nargs[j].second < 0) {
                is_negative = true;
                break;
            }
        }

        if (is_negative) { continue; }
        if (is_zero && k == 1) { return 1; }

        sum += __balanced_01_matrix(k - 1, n, nargs);
    }

    return sum;
}

size_t balanced_01_matrix(size_t n)
{
    n = n / 2 * 2;
    if (n < 2) { n = 2; }
    args_t args(n, std::make_pair(n/2, n/2));
    return __balanced_01_matrix(n, n, args);
}

#endif // _BALANCED_01_MATRIX_HPP_

