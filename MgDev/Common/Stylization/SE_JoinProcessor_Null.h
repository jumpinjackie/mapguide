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

#ifndef SE_JOINPROCESSOR_NULL_H_
#define SE_JOINPROCESSOR_NULL_H_

#include "SE_JoinProcessor.h"

struct NullData
{
    typedef NullData TX_INFO;

    const NullData& operator()(const SE_Join<NullData>& /*join*/) { return *this; }
    const NullData& operator()(const SE_Cap<NullData>& /*cap*/) { return *this; }
};

typedef SE_JoinProcessor<NullData> SE_JoinProcessor_Null;

#endif //SE_JOINPROCESSOR_NULL_H_
