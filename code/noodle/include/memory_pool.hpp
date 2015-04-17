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
 * \file memory_pool.hpp
 * \brief memory pool
 */

#ifndef memory_pool_h
#define memory_pool_h

#include <boost/thread/mutex.hpp>

#include "noodle.hpp"

/*!
 * \brief memory pool
 */

class MemoryPool {
public:

    /*!
     * \brief create a new memory pool
     * \param max_ maximum memory size
     * \param begin_ minimum chunk size
     * \param end_ maximum chunk size
     * \param divide_ chunk size gap
     * \param singleChunk_ allocate chunk for each request
     * \param factor_ the coefficient to expand bucket
     */

    MemoryPool (int max_, int begin_, int end_, int divide_,
        bool singleChunk_ = false, float factor_ = 0.2f);

    /*!
     * \brief free memory pool
     */

    ~MemoryPool ();

    /*!
     * \brief get memory chunk from memory pool
     * \param size_ chunk size
     * \return memory chunk, zero means fail
     */

    void* alloc(int size_);

    /*!
     * \brief put memory chunk to memory pool
     * \param size_ chunk size
     * \param m_ memory chunk pointer
     */

    void free (int size_, void* m_);

    /*!
     * \brief get current pool size in bytes
     * \return current pool size
     */

    int getSize();

    /*!
     * \brief get max pool size in bytes
     * \return max pool size
     */

    int getMax();

    /*!
     * \brief get current memory size allocated from memory pool
     * \brief current memory size allocated from memory pool
     */

    int getActual ();

private:
    struct _Chunk
    {
        _Chunk* next; /* next chunk */
    };

    struct _Block
    {
        _Block* next; /* next block */
        char*      m; /* native memory block */
    };

    struct _Bucket
    {
        int      n;      /* pre-allocated chunk number */
        int      size;   /* chunk size */
        int      free;   /* free chunk number */
        int      employ; /* occupied chunk number */
        _Block*  block;  /* block list */
        _Chunk*  chunk;  /* chunk list */
    };

    inline int _getChunkNumber (int bucket);

    int _createBlock(_Block** p, int n);
    inline void _destroyBlock(_Block* p);

    inline void _createChunkList(_Chunk** chunk, char* m, int n, int size);
    inline int _getChunk(_Bucket* p_, char** m_);
    inline void _putChunk(_Bucket* p_, char* m_);

    int _createBucket(_Bucket** p, int n, int size);
    void _destroyBucket(_Bucket* p);
    int _expandBucket(_Bucket* p);
    inline int _getBucketIndex (int size_);

private:
    int         _max;    /*!< max memory size limitation */
    int         _size;   /*!< current memory size */
    int         _actual; /*!< current memory size allocated from memory pool */
    int         _begin;  /*!< the begin of memory pool_range which memory pool can handle */
    int         _end;    /*!< the end of memory pool_range which memory pool can handle */
    int         _divide; /*!< memory pool_range division, the multiple of 4 */
    float       _factor; /*!< default factor */
    int         _n;      /*!< bucket number */
    bool        _singleChunk; /*!< allocates separate chunk for each allocation demand */
    _Bucket**   _bucket; /*!< memory bucket */
    boost::mutex _mutex; /*!< binary lock */

    /* the minimum chunk size is sizeof(void*) */
    static const int POINTER_LENGTH = sizeof (void*);
};

#endif /* memory_pool_h */
