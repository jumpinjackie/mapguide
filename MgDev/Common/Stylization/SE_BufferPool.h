//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#ifndef SE_BUFFERPOOL_H_
#define SE_BUFFERPOOL_H_

#include "SE_Bounds.h"
#include "SE_LineBuffer.h"
#include "SE_LineStorage.h"


//---------------------------------------------
// Object pool for buffers
//---------------------------------------------

class SE_BufferPool
{
public:
    STYLIZATION_API SE_BufferPool();
    STYLIZATION_API virtual ~SE_BufferPool();
    STYLIZATION_API SE_LineBuffer* NewLineBuffer(int requestSize);
    STYLIZATION_API SE_Bounds* NewBounds(int size);
    STYLIZATION_API SE_LineStorage* NewLineStorage(int requestSize);
    STYLIZATION_API void FreeBounds(SE_Bounds* bounds);
    STYLIZATION_API void FreeLineBuffer(SE_LineBuffer*);
    STYLIZATION_API void FreeLineStorage(SE_LineStorage*);

private:
    DataValueStack<SE_LineBuffer> m_lb_pool;
    DataValueStack<SE_Bounds> m_bnd_pool;
    DataValueStack<SE_LineStorage> m_ls_pool;
};

#endif
