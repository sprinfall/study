#ifndef _ALGORITHM_HPP_
#define _ALGORITHM_HPP_

#include <algorithm>
#include <ctime>
#include <vector>
#include <cassert>
#include <stdexcept>

//-----------------------------------------------------------------------------

// SPLIT
// number of comparisons: n-1; running time: Θ(n)
template <typename RanIt>
RanIt split(RanIt begin, RanIt end)
{
	if (end < begin) {
		throw std::range_error("bad range");
	}
	if (end < begin + 2) { return begin; } // < 2 elements

	RanIt i = begin;
	for (RanIt j = begin+1; j < end; ++j) {
		if (*j < *begin) { // note the difference from *j <= *begin
			++i;
			if (j != i) { std::swap(*j, *i); }
		}
	}
	if (i != begin) { std::swap(*begin, *i); }

	return i;
}

// Use x instead of first member as the split element
template <typename RanIt, typename T>
RanIt split(RanIt begin, RanIt end, T x)
{
	if (end < begin) {
		throw std::range_error("bad range");
	}

	RanIt found = std::find(begin, end, x);
	if (found != end) {
		std::swap(*begin, *found);
		return split(begin, end);
	}
	return end;
}

// RANDOMIZED-SPLIT
// number of comparisons: n-1; running time: Θ(n)
template <typename RanIt>
RanIt rsplit(RanIt begin, RanIt end)
{
	if (end < begin) {
		throw std::range_error("bad range");
	}
	if (end < begin + 2) { return begin; } // < 2 elements
	const size_t N = end - begin;

	srand((unsigned int)time(0));
	size_t v = rand() % (N-1) + 1; // [1, N)
	std::swap(*begin, *(begin+v));

	return split(begin, end);
}

//-----------------------------------------------------------------------------

// QUICKSORT
// running time: Θ(n log n) in average case, Θ(n * n) in worst case
template <typename RanIt>
void qsort(RanIt begin, RanIt end)
{
	if (end < begin) {
		throw std::range_error("bad range");
	}
	if (end > begin + 1) { // more than 1 element
		RanIt w = split(begin, end);
		qsort(begin, w);
		qsort(w+1, end);
	}
}

// RANDOMIZED-QUICKSORT
// running time: Θ(n log n) as expected
template <typename RanIt>
void rqsort(RanIt begin, RanIt end)
{
	if (end < begin) {
		throw std::range_error("bad range");
	}
	if (end > begin + 1) { // more than 1 element
		RanIt w = rsplit(begin, end);
		rqsort(begin, w);
		rqsort(w + 1, end);
	}
}

//-----------------------------------------------------------------------------

// SELECT
// running time: Θ(n) in average case
template <typename RanIt>
RanIt select(RanIt begin, RanIt end, size_t k)
{
	typedef std::iterator_traits<RanIt>::value_type value_type;
	typedef std::vector<value_type> array_type;

	if (end < begin + 2) { // <= 1 element
		return begin; // k should be 1
	}
	const size_t N = end - begin; // >= 2
	if (k > N) { k = N; }
	else if (k < 1) { k = 1; }

	if (N < 44) {
		std::sort(begin, end);
		return begin + k - 1;
	}

	// get the middle value of the middle values of all groups
	const size_t group_num = N / 5;
	array_type mvalues(group_num);
	for (size_t i = 0; i < group_num; ++i) {
		std::sort(begin + 5*i, begin + 5*i + 5);
		mvalues[i] = *(begin + 5*i + 2);
	}
	size_t mindex = group_num%2 == 0 ? group_num/2 : group_num/2 + 1;
	value_type mmvalue = *select(mvalues.begin(), mvalues.end(), mindex);

	// split the input array into three groups
	// A1: [begin, w); A2: [w, w2); A3: [w2, end)
	RanIt w = split(begin, end, mmvalue);
	assert(w != end); // mmvalue must be in [begin, end)
	RanIt w2 = w;
	while (w2 < end && *w2 == mmvalue) { ++w2; }

	if (w >= begin + k) { return select(begin, w, k); }
	else if (w2 >= begin + k) { return w; }
	else { return select(w2, end, k - (w2 - begin)); };
}

// RANDOMIZED-SELECT
// running time: Θ(n) as expected
template <typename RanIt>
RanIt rselect(RanIt begin, RanIt end, size_t k)
{
	if (end < begin + 2) { // <= 1 element
		return begin; // k should be 1
	}
	const size_t N = end - begin; // >= 2
	if (k > N) { k = N; }
	else if (k < 1) { k = 1; }

	// split the input array into three groups
	// A1: [begin, w); A2: [w, w2); A3: [w2, end)
	RanIt w = rsplit(begin, end);
	RanIt w2 = w;
	while (w2 < end && *w2 == *w) { ++w2; }

	if (w >= begin + k) { return rselect(begin, w, k); }
	else if (w2 >= begin + k) { return w; }
	else { return rselect(w2, end, k - (w2 - begin)); };
}

#endif // _ALGORITHM_HPP_
