#ifndef _CHECKERBOARD_HPP_ 
#define _CHECKERBOARD_HPP_ 
 
// Almost from [http://en.wikipedia.org/wiki/Dynamic_programming]: 
// Consider a checkerboard with n ¡Á n squares and each square has a cost 
// associated with it. 
// You had a checker that could start at any square on the first row and 
// you wanted to know the shortest path (sum of the costs of the visited 
// squares are at a minimum) to get to the last row, assuming the checker 
// could move only diagonally left forward, diagonally right forward, or 
// straight forward. 
 
#include <utility> 
#include <cassert> 
#include <list> 
 
// matrix is a two-dimensional array; this macro gets matrix[i][j] 
#define _V(matrix, n, i, j) (*(matrix + (i)*(n) + j)) 
 
int __minimum_cost(int *costs, size_t n, size_t i, size_t j) 
{ 
    if (i == n-1) { return _V(costs, n, i, j); } 
    int mincost = __minimum_cost(costs, n, i+1, j); 
    if (j > 0) { 
        mincost = std::min(mincost, __minimum_cost(costs, n, i+1, j-1)); 
    } 
    if (j < n-1) { 
        mincost = std::min(mincost, __minimum_cost(costs, n, i+1, j+1)); 
    } 
    return mincost + _V(costs, n, i, j); 
} 
 
// top-down approach 
// only computes the path-cost, not the actual path 
int minimum_cost(int *costs, size_t n) 
{ 
    assert(n > 1); 
    int mincost = __minimum_cost(costs, n, 0, 0); 
    for (size_t j = 1; j < n; ++j) { 
        int jcost = __minimum_cost(costs, n, 0, j); 
        if (jcost < mincost) { mincost = jcost; } 
    } 
    return mincost; 
} 
 
void shortest_path_array(int *costs, int *q, int *p, size_t n) 
{ 
    assert(n > 1); 
 
    for (size_t j = 0; j < n; ++j) { 
        _V(q, n, 0, j) = _V(costs, n, 0, j); 
    } 
 
    for (size_t i = 1; i < n; ++i) { 
        for (size_t j = 0; j < n; ++j) { 
            int mincost = _V(q, n, i-1, j); 
            if (j > 0) { mincost = std::min(mincost, _V(q, n, i-1, j-1)); } 
            if (j < n-1) { mincost = std::min(mincost, _V(q, n, i-1, j+1)); } 
            _V(q, n, i, j) = mincost + _V(costs, n, i, j); 
 
            // -1: left; 0: straight; 1: right 
            if (mincost == _V(q, n, i-1, j-1)) { _V(p, n, i, j) = -1; } 
            else if (mincost == _V(q, n, i-1, j)) { _V(p, n, i, j) = 0; } 
            else { _V(p, n, i, j) = 1; } 
        } 
    } 
} 
 
void get_shortest_path(int *p, size_t i, size_t j, size_t n, std::list<size_t>& path) 
{ 
    path.push_front(j); 
    if (i == 1) { 
        path.push_front(j + _V(p, n, i, j)); 
    } 
    else { 
        get_shortest_path(p, i-1, j + _V(p, n, i, j), n, path); 
    } 
} 
 
// bottom-up approach 
// computes the actual path and the path-cost 
// q is an array used to save the computed path-cost 
// p is a predecessor array which basically says where paths come from 
int shortest_path(int *costs, size_t n, std::list<size_t>& path) 
{ 
    int *q = new int[n*n]; 
    int *p = new int[n*n]; 
    shortest_path_array(costs, q, p, n); 
    size_t index = 0; 
    int mincost = _V(q, n, n-1, 0); 
    for (size_t j = 1; j < n; ++j) { 
        int t = _V(q, n, n-1, j); 
        if (t < mincost) { 
            mincost = t; 
            index = j; 
        } 
    } 
 
    get_shortest_path(p, n-1, index, n, path); 
    delete []q; 
    delete []p; 
    return mincost; 
} 
 
#endif // _CHECKERBOARD_HPP_ 

/* 
int checkerboard[][5] = { 
    {6, 7, 4, 7, 8}, // 4 
    {7, 6, 1, 1, 4}, // 1 
    {3, 5, 7, 8, 2}, // 2 
    {2, 6, 7, 0, 2}, // 0 
    {7, 3, 5, 6, 1} // 1 -> 8 
}; 
 
int mincost = minimum_cost((int *)checkerboard, 5); 
std::cout << mincost << std::endl; 
 
std::list<size_t> path; 
int min_cost = shortest_path((int *)checkerboard, 5, path); 
std::cout << min_cost << std::endl; 
std::copy(path.begin(), path.end(), std::ostream_iterator<size_t>(std::cout, " ")); 
*/

