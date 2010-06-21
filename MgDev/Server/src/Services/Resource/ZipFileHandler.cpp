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

#include "ResourceServiceDefs.h"
#include "ZipFileHandler.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgZipFileHandler::MgZipFileHandler(CREFSTRING filePath,
    DWFZipFileDescriptor::teFileMode fileMode)
{
    ACE_ASSERT(!filePath.empty());

    MG_RESOURCE_SERVICE_TRY()

    DWFFile zipFile(filePath.c_str());

    m_zipFileDescriptor.reset(new DWFZipFileDescriptor(zipFile, fileMode));
    m_zipFileDescriptor->open();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgZipFileHandler.MgZipFileHandler")
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgZipFileHandler::~MgZipFileHandler()
{
}
