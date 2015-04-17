#ifndef _SORT_HEAP_HPP_
#define _SORT_HEAP_HPP_
 
#include <utility>
 
template <class RanIt>
void shift_down(RanIt first, RanIt last, size_t i)
{
    const size_t N = last - first;
    while (true) {
        if ((i <<= 1) > N) { break; };
        if (i+1 <= N && *(first+i) > * (first+i-1)) {
            ++i;
        }
        if (*(first+i/2-1) < *(first+i-1)) {
            std::swap(*(first+i/2-1), *(first+i-1));
        } else {
            break;
        }
    }
}
 
// space: Θ(1); time: Θ(n)
template<class RanIt>
void make_heap(RanIt first, RanIt last)
{
    for (size_t i = (last - first)/2; i > 0; --i) {
        shift_down(first, last, i);
    }
}
 
// space: Θ(1); time: O(n log n)
template<class RanIt>
void sort_heap(RanIt first, RanIt last)
{
    make_heap(first, last);
    const size_t N = last - first;
    for (size_t j = N; j >= 2; --j) {
        std::swap(*first, *(first+j-1));
        shift_down(first, first+j-1, 1);
    }
}
 
#endif

