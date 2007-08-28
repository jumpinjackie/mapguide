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

#ifndef SE_JOIN_H
#define SE_JOIN_H

#include "SE_JoinBuffer.h"

/* The base class for joins; the constructor of SE_BaseJoin initializes the world-to-join 
 * and join-to-world transforms. */
class SE_Join
{
public:    
    virtual void Transform(SE_JoinElement* joins) = 0;
};

#endif // SE_JOIN_H
