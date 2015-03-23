//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#include "stdafx.h"
#include "SE_BufferPool.h"
#include <cstdlib>

SE_BufferPool::SE_BufferPool()
{
}


SE_BufferPool::~SE_BufferPool()
{
    while (!m_selb_pool.empty())
        delete m_selb_pool.pop();
    while (!m_bnd_pool.empty())
        free(m_bnd_pool.pop());
}


SE_LineBuffer* SE_BufferPool::NewSELineBuffer(SE_BufferPool* pool, int requestSize)
{
    if (pool && !pool->m_selb_pool.empty())
    {
        SE_LineBuffer* lb = pool->m_selb_pool.pop();
        lb->Reset();
        return lb;
    }

    return new SE_LineBuffer(requestSize, pool);
}


SE_Bounds* SE_BufferPool::NewBounds(SE_BufferPool* pool, int size)
{
    if (pool && !pool->m_bnd_pool.empty())
    {
        SE_Bounds* bounds = pool->m_bnd_pool.pop();
        if (bounds->capacity >= size)
        {
            bounds->size = 0;
            bounds->min[0] = bounds->min[1] = +DBL_MAX;
            bounds->max[0] = bounds->max[1] = -DBL_MAX;
            return bounds;
        }
        free(bounds);
    }

    SE_Bounds* bounds = (SE_Bounds*)malloc(sizeof(SE_Bounds) + 2*size*sizeof(double));
    bounds->hull = (double*)((char*)(bounds) + sizeof(SE_Bounds));
    bounds->capacity = size;
    bounds->size = 0;
    bounds->min[0] = bounds->min[1] = +DBL_MAX;
    bounds->max[0] = bounds->max[1] = -DBL_MAX;
    bounds->pool = pool;
    return bounds;
}


void SE_BufferPool::FreeSELineBuffer(SE_BufferPool* pool, SE_LineBuffer* lb)
{
    if (pool)
        pool->m_selb_pool.push(lb);
    else
        delete lb;
}


void SE_BufferPool::FreeBounds(SE_BufferPool* pool, SE_Bounds* bounds)
{
    if (pool)
        pool->m_bnd_pool.push(bounds);
    else
        free(bounds);
}
