//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#include "Foundation.h"

/////////////////////////////////////////////////////////////////
///<summary>
/// Default Constructor
///</summary>
MgStream::MgStream() : MgStreamReader(NULL), MgStreamWriter(NULL)
{}

/////////////////////////////////////////////////////////////////
///<summary>
/// Constructors
///</summary>
///<param name="streamHelper">
/// Stream helper object holding ACE_SOCK_STREAM
///</param>
MgStream::MgStream( MgStreamHelper* streamHelper ) : MgStreamReader(streamHelper), MgStreamWriter(streamHelper)
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destructor
///</summary>
MgStream::~MgStream()
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Gets the class id
///</summary>
///<returns>
///class id
///</returns>
 INT32 MgStream::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Self destructing method
///</summary>
 void MgStream::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Returns the stream helper contained by this stream
///</summary>
///
MgStreamHelper* MgStream::GetStreamHelper()
{
    return MgStreamReader::GetStreamHelper();
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Sets the stream helper for this stream
///
///
void MgStream::SetStreamHelper(MgStreamHelper* helper)
{
    MgStreamWriter::m_sHelper = SAFE_ADDREF(helper);
    MgStreamReader::m_sHelper = SAFE_ADDREF(helper);
}
