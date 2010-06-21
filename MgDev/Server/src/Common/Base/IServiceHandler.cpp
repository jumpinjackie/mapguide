//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "IServiceHandler.h"

///////////////////////////////////////////////////////////////////////////
/// <summary>
/// The default constructor for an IMgServiceHandler object.
/// </summary>

IMgServiceHandler::IMgServiceHandler() :
    m_data(0)
{
};

///////////////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an IMgServiceHandler object using the given parameters.
/// </summary>
/// <param name = "data">
/// A pointer the the StreamData object that encapsulates the incoming
/// stream information.
/// </param>
/// <param name = "packet">
/// The Operation Packet that has already been read from the incoming
/// data stream.
/// </param>

IMgServiceHandler::IMgServiceHandler(MgStreamData* data,
    const MgOperationPacket& packet) :
    m_data(data),
    m_packet(packet)
{
}

///////////////////////////////////////////////////////////////////////////
/// <summary>
/// The destructor for an IMgServiceHandler object.
/// </summary>

IMgServiceHandler::~IMgServiceHandler()
{
}
