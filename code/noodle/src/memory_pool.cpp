/*
 * Copyright (c) 2009-2010, Dennis Wang (dennis.wangkk@gmail.com)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 \file memory_pool.cpp
 \brief the implement the memory pool
 */

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include <iostream>

#include "memory_pool.hpp"

MemoryPool::MemoryPool (int max_, int begin_, int end_, int divide_,
    bool singleChunk_, float factor_) {

    assert (0 < max_);
    assert (POINTER_LENGTH <= begin_);
    assert (begin_ < end_);
    assert (0 == (end_ - begin_) % POINTER_LENGTH);
    assert (0 < divide_);
    assert (0 == divide_ % POINTER_LENGTH);

    _max    = max_;
    _begin  = begin_;
    _end    = end_;
    _divide = divide_;
    _size   = 0;
    _actual = 0;
    _factor = factor_;
    _singleChunk = singleChunk_;
    _n      = (end_ - begin_ + divide_ - 1) / divide_ + 1; /* get bucket count */

    _bucket = (_Bucket**)malloc (_n * sizeof(_Bucket*));
    if (!_bucket) {
        throw std::runtime_error ("cannot allocate buckets");
    }
    memset(_bucket, 0, _n * sizeof (_Bucket*));
}

MemoryPool::~MemoryPool () {
    if (_actual) {
        throw std::runtime_error ("memory leak");
    }
    for (int i = 0; i < _n; i++) {
        // free the bucket which all chunks are free
        if (_bucket[i]) {
            _destroyBucket (_bucket[i]);
        }
    }
}

void* MemoryPool::alloc(int size_) {
    assert (size_);

    int   bucket = 0; // bucket number
    int   chunk  = 0; // chunk number need to allocate
    char* m      = 0; // allocated memory pointer

    // check whether the size is unsupported
    if ((_begin > size_) || (_end < size_)) {
        throw std::runtime_error ("unsupported memory size");
    }

    // round size to the multiple of divide
    bucket = _getBucketIndex (size_);

    // get the real size in pool
    size_ = _begin + bucket * _divide;

    boost::mutex::scoped_lock lock (_mutex);

    if (!_bucket[bucket]) {
        // bucket never created
        // get chunk number
        if (_singleChunk) {
            chunk = 1;
        } else {
            chunk = _getChunkNumber (bucket);
        }

        // check whether reach the max memory limitation
        if (_max < _size + size_ * chunk) {
            if (_singleChunk) {
                return 0;
            }
            // get the minimum chunk number can allocate
            chunk = (_max - _size) / size_;
            if (!chunk) {
                return 0;
            }
        }

        if (OK != _createBucket (&_bucket[bucket], chunk, size_)) {
            return 0;
        }

        // enlarge the size of pool
        _size += size_ * chunk;
        // never fail
        _getChunk(_bucket[bucket], &m);
        return m;
    } else {
        // try to get a chunk from bucket
        if (OK != _getChunk (_bucket[bucket], &m)) {
            // no available chunk, try to expand bucket             
            // expand the number chunk to n * factor
            if (_singleChunk) {
                _bucket[bucket]->n = 1;
            } else {
                _bucket[bucket]->n = (int)((float)(_bucket[bucket]->n) * _factor);
                if (!_bucket[bucket]->n) {
                    _bucket[bucket]->n = 1;
                }
            }

            // check whether reach the max memory limitation
            if (_max < _size + _bucket[bucket]->n * _bucket[bucket]->size) {
                if (_singleChunk) {
                    return 0;
                }
                // get the minimum chunk number can allocate
                _bucket[bucket]->n = (_max - _size) / _bucket[bucket]->size;
                if (!_bucket[bucket]->n) {
                    return 0;
                }
            }

            // expand bucket
            if (OK != _expandBucket (_bucket[bucket])) {
                return 0;
            }

            // never fail after expand
            _getChunk (_bucket[bucket], &m);
            _size += _bucket[bucket]->n * _bucket[bucket]->size;
            return m;
        }
    }

    return m;
}

void MemoryPool::free (int size_, void* m_) {
    assert (size_);
    assert (m_);

    // check whether the size is unsupported
    if ((_begin > size_) || (_end < size_)) {
        throw std::runtime_error ("unsupported memory size");
    }
    // get bucket number
    int bucket = _getBucketIndex (size_);

    boost::mutex::scoped_lock lock (_mutex);
    // put chunk
    _putChunk(_bucket[bucket], static_cast<char*>(m_));
}

int MemoryPool::getSize() {
    boost::mutex::scoped_lock lock (_mutex);
    return _size;
}

int MemoryPool::getMax() {
    return _max;
}

int MemoryPool::getActual () {
    boost::mutex::scoped_lock lock (_mutex);
    return _actual;
}

inline int MemoryPool::_getChunkNumber (int bucket_) {
    return (int)((1.0f / (float)(bucket_ + 1)) * (((float)(_n) * (1.0f + (float)(_n))) / 2.0f));
}

int MemoryPool::_createBlock(_Block** p_, int n_) {
    // allocates memory for block struct
    _Block* p = reinterpret_cast<_Block*>(malloc (sizeof (_Block)));
    if (!p) {
        return ERR_BAD_ALLOC;
    }

    // allocates memory block
    p->m = reinterpret_cast<char*>(malloc (n_));
    if (!p->m) {
        ::free (p);
        return ERR_BAD_ALLOC;
    }

    p->next = 0;
    *p_ = p;
    return OK;
}

inline void MemoryPool::_destroyBlock(_Block* p_) {
    ::free (p_->m);
    ::free (p_);
}

inline void MemoryPool::_createChunkList(_Chunk** chunk, char* m, int n,
    int size) {
    int i = 0;
    // make chunk list
    for (; i < n - 1; i++) {
        reinterpret_cast<_Chunk*>(m + i * size)->next =
            reinterpret_cast<_Chunk*>(m + (i + 1) * size);
    }
    reinterpret_cast<_Chunk*>(m + i * size)->next = 0;
    *chunk = reinterpret_cast<_Chunk*>(m);
}

inline int MemoryPool::_getChunk(_Bucket* p_, char** m_) {
    if (p_->chunk) {
        // chunk list is non-empty
        // return the head of chunk list, adjusts chunk list
        *m_ = reinterpret_cast<char*>(p_->chunk);
        p_->chunk = p_->chunk->next;
        p_->free--;
        p_->employ++;
        _actual += p_->size;
    } else {
        // chunk list is empty
        return ERR_BAD_ALLOC;
    }

    return OK;
}

inline void MemoryPool::_putChunk(_Bucket* p_, char* m_) {
    if (p_->chunk) {
        // chunk list is non-empty
        reinterpret_cast<_Chunk*>(m_)->next = p_->chunk;
        // set returned memory as head of chunk list
        p_->chunk = reinterpret_cast<_Chunk*>(m_);
    } else {
        // chunk list is empty
        // set head of chunk list
        p_->chunk = reinterpret_cast<_Chunk*>(m_);
        reinterpret_cast<_Chunk*>(m_)->next = 0;
    }
    p_->free++;
    p_->employ--;
    _actual -= p_->size;
}

int MemoryPool::_createBucket(_Bucket** p_, int n, int size_) {
    int ret = OK;

    _Bucket* p = reinterpret_cast<_Bucket*>(malloc (sizeof (_Bucket)));
    if (!p) {
        return ERR_BAD_ALLOC;
    }

    p->n      = n;
    p->size   = size_;
    p->free   = n;
    p->employ = 0;

    // allocates a new block for bucket
    ret = _createBlock(&p->block, n * size_);
    if (OK != ret) {
        ::free (p);
        return ret;
    }

    // make chunk list for block
    _createChunkList(&p->chunk, p->block->m, p->n, p->size);
    *p_ = p;
    return ret;
}

void MemoryPool::_destroyBucket(_Bucket* p_) {
    _Block* block = p_->block;
    _Block* temp  = 0;

    while (block) {
        temp = block->next;
        _destroyBlock(block);
        block = temp;
    }
    // free bucket struct
    ::free (p_);
}

int MemoryPool::_expandBucket(_Bucket* p_)
{
    int     ret   = OK;
    _Block* block = 0;
    // allocates a memory block
    ret = _createBlock(&block, p_->n * p_->size);
    if (OK != ret) {
        return ret;
    }
    // make chunk list of memory block
    _createChunkList(&p_->chunk, block->m, p_->n, p_->size);
    // the new block is head of block list
    block->next = p_->block;
    p_->block   = block;
    return ret;
}

inline int MemoryPool::_getBucketIndex (int size_) {
    return ((size_ - _begin + _divide - 1) & ~(_divide - 1)) / _divide;
}
