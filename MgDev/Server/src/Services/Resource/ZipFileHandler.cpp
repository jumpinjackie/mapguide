//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDwfException
/// </exceptions>
///----------------------------------------------------------------------------

MgZipFileHandler::MgZipFileHandler(CREFSTRING zipPathname)
{
    assert(!zipPathname.empty());

    MG_RESOURCE_SERVICE_TRY()

    DWFFile zipFile(zipPathname.c_str());

    m_zipFileDescriptor.reset(new DWFZipFileDescriptor(zipFile, DWFZipFileDescriptor::eUnzip));
    m_zipFileDescriptor->open();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgZipFileHandler.MgZipFileHandler")
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgZipFileHandler::~MgZipFileHandler()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Extracts the content of an archived file to a stream of bytes.
/// The caller is responsible to de-allocate the returned object by calling
/// DWFCORE_FREE_OBJECT.
/// </summary>
///
/// <exceptions>
/// MgDwfException
/// </exceptions>
///----------------------------------------------------------------------------

DWFInputStream* MgZipFileHandler::Unzip(CREFSTRING archivedPathname)
{
    assert(!archivedPathname.empty());
    DWFInputStream* inputStream = NULL;

    MG_RESOURCE_SERVICE_TRY()

    DWFString archivedFile(archivedPathname.c_str());

    inputStream = m_zipFileDescriptor->unzip(archivedFile);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgZipFileHandler.Unzip")

    return inputStream;
}
