//
//  Copyright (C) 2007-2008 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "StylizationDefs.h"

/* Like a deque, but without the excitement. Or destructors. And a 2x growth factor. */
template<class DATA_TYPE> class SE_Deque
{
    DATA_TYPE* m_data;
    unsigned int m_start;
    unsigned int m_size;
    unsigned int m_mask;
    unsigned int m_len;

    void enlarge(int amount = 1);

public:
    SE_INLINE SE_Deque(unsigned int size = 7);
    SE_INLINE ~SE_Deque();

    SE_INLINE unsigned int size() const;
    SE_INLINE void clear();
    SE_INLINE void post_enlarge(int amount, const DATA_TYPE* init = NULL);

    SE_INLINE void push_head(const DATA_TYPE& elem);
    SE_INLINE DATA_TYPE& push_head();
    SE_INLINE void pop_head();
    SE_INLINE DATA_TYPE& head() const;

    SE_INLINE void push_tail(const DATA_TYPE& elem);
    SE_INLINE DATA_TYPE& push_tail();
    SE_INLINE void pop_tail();
    SE_INLINE DATA_TYPE& tail() const;

    SE_INLINE DATA_TYPE& operator[] (unsigned int index) const;
};


/******************************************************************************
 * SE_Deque function definitions                                              *
 ******************************************************************************/

template<class DATA_TYPE>
SE_Deque<DATA_TYPE>::SE_Deque(unsigned length) :
        m_start(0),
        m_size(0)
{
    /* Length set to a factor of 2 */
    m_len = 1;
    while(length)
        length >>= 1, m_len <<= 1;
    m_mask = m_len - 1;
    /* Use malloc over new to avoid constructors */
    m_data = static_cast<DATA_TYPE*>(malloc(m_len*sizeof(DATA_TYPE)));
}


template<class DATA_TYPE>
SE_Deque<DATA_TYPE>::~SE_Deque()
{
    free(m_data);
}


template<class DATA_TYPE>
void SE_Deque<DATA_TYPE>::enlarge(int amount)
{
    unsigned int newlen = (m_len << 1);
    while (m_size + amount >= newlen)
        newlen <<= 1;
    DATA_TYPE* newdata = static_cast<DATA_TYPE*>(malloc(newlen*sizeof(DATA_TYPE)));
    memcpy(newdata, m_data + m_start, (m_len - m_start)*sizeof(DATA_TYPE));
    memcpy(newdata + m_len - m_start, m_data, m_start*sizeof(DATA_TYPE));
    free(m_data);

    m_data = newdata;
    m_start = 0;
    m_len = newlen;
    m_mask = newlen - 1;
}


template<class DATA_TYPE>
void SE_Deque<DATA_TYPE>::post_enlarge(int amount, const DATA_TYPE* init)
{
    if (m_size + amount > m_len)
        enlarge(amount);
    if (init)
    {
        for(;amount--;++m_size)
            m_data[(m_start + m_size) & m_mask] = *init;
    }
    else
        m_size += amount;
}


template<class DATA_TYPE>
void SE_Deque<DATA_TYPE>::push_tail(const DATA_TYPE& elem)
{
    if (m_size == m_len)
        enlarge();
    m_data[(m_start + m_size++) & m_mask] = elem;
}


template<class DATA_TYPE>
DATA_TYPE& SE_Deque<DATA_TYPE>::push_tail()
{
    if (m_size == m_len)
        enlarge();
    return m_data[(m_start + m_size++) & m_mask];
}


template<class DATA_TYPE>
void SE_Deque<DATA_TYPE>::pop_tail()
{
    _ASSERT(m_size > 0);
    --m_size;
}


template<class DATA_TYPE>
void SE_Deque<DATA_TYPE>::push_head(const DATA_TYPE& elem)
{
    if (m_size == m_len)
        enlarge();
    m_start = (m_start - 1) & m_mask;
    m_size++;
    m_data[m_start] = elem;
}


template<class DATA_TYPE>
DATA_TYPE& SE_Deque<DATA_TYPE>::push_head()
{
    if (m_size == m_len)
        enlarge();
    m_start = (m_start - 1) & m_mask;
    m_size++;
    return m_data[m_start];
}


template<class DATA_TYPE>
void SE_Deque<DATA_TYPE>::pop_head()
{
    _ASSERT(m_size > 0);
    m_start = (m_start + 1) & m_mask;
    --m_size;
}


template<class DATA_TYPE>
unsigned int SE_Deque<DATA_TYPE>::size() const
{
    return m_size;
}


template<class DATA_TYPE>
void SE_Deque<DATA_TYPE>::clear()
{
    m_size = m_start = 0;
}


/* One benefit of this implementation is that out-of-bounds accesses are impossible.
 * Unexpected, perhaps, out-of-bounds, no ;p. */
template<class DATA_TYPE>
DATA_TYPE& SE_Deque<DATA_TYPE>::operator[] (unsigned int index) const
{
    _ASSERT(index >= 0 && index < m_size);
    return m_data[(m_start + index) & m_mask];
}


template<class DATA_TYPE>
DATA_TYPE& SE_Deque<DATA_TYPE>::head() const
{
    _ASSERT(m_size > 0);
    return m_data[m_start];
}


template<class DATA_TYPE>
DATA_TYPE& SE_Deque<DATA_TYPE>::tail() const
{
    _ASSERT(m_size > 0);
    return m_data[(m_start + m_size - 1) & m_mask];
}
