#ifndef _LCSUBSEQ_HPP_
#define _LCSUBSEQ_HPP_

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <cassert>


template <class RanIt>
class LCSubseq
{
public:
    LCSubseq<RanIt>(RanIt xbegin, RanIt xend, RanIt ybegin, RanIt yend)
        : xbegin_(xbegin), xend_(xend), ybegin_(ybegin), yend_(yend)
    {
        assert(xend_ > xbegin_);
        assert(yend_ > ybegin_);

        M_ = xend_ - xbegin_;
        N_ = yend_ - ybegin_;
        length_ = computeL();
        backTrack(); // backTrackRecursive(M_, N_);
    }

    size_t getLength() const
    {
        return length_;
    }

    const std::list<size_t>& getOffsetX() const
    {
        return offsetX_;
    }

    const std::list<size_t>& getOffsetY() const
    {
        return offsetY_;
    }

private:
    size_t computeL()
    {
        L_.resize(M_ + 1);
        for (size_t i = 0; i <= M_; ++i) {
            L_[i].resize(N_ + 1);
            L_[i][N_] = 0;
        }
        L_[M_].assign(N_ + 1, 0);

        for (size_t i = M_; i > 0; --i) {
            for (size_t j = N_; j > 0; --j) {
                if (*(xbegin_+i-1) == *(ybegin_+j-1))
                    L_[i-1][j-1] = L_[i][j] + 1;
                else
                    L_[i-1][j-1] = std::max(L_[i-1][j], L_[i][j-1]);
            }
        }
        return L_[0][0];
    }

    // backTrack() is prefered 
    void backTrackRecursive(size_t i, size_t j)
    {
        if (i == 0 || j == 0) { return; }

        if (*(xbegin_+i-1) == *(ybegin_+j-1)) {
            offsetX_.push_front(i - 1);
            offsetY_.push_front(j - 1);
            backTrackRecursive(i - 1, j - 1);
            return;
        }

        if (L_[i][j-1] > L_[i-1][j])
            return backTrackRecursive(i, j - 1);
        else
            return backTrackRecursive(i - 1, j);
    }

    void backTrack()
    {
        for (size_t i = 0, j = 0; i < M_ && j < N_;) {
            if (*(xbegin_+i) == *(ybegin_+j)) {
                offsetX_.push_back(i);
                offsetY_.push_back(j);
                ++i; ++j;
            }
            else {
                if (L_[i+1][j] > L_[i][j+1]) ++i;
                else ++j;
            }
        }
    }

private:
    RanIt xbegin_, xend_, ybegin_, yend_;
    size_t M_, N_;

    std::vector<std::vector<size_t> > L_;

    size_t length_;
    std::list<size_t> offsetX_, offsetY_;
};

#endif // _LCSUBSEQ_HPP_

/*
std::string X("there should be sunshine after rain");
std::string Y("there should be laughter after pain");

LCSubseq<std::string::iterator> lcs(X.begin(), X.end(), Y.begin(), Y.end());
std::cout << lcs.getLength() << std::endl;

std::list<size_t> offsetX = lcs.getOffsetX();
std::list<size_t>::const_iterator iter;
for (iter = offsetX.begin(); iter != offsetX.end(); ++iter) {
std::cout << X[*iter];
}
std::cout << std::endl;
*/ 

