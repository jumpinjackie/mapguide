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

#include "AceCommon.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Default Constructor
/// </summary>
MgResourceService::MgResourceService() : MgService(NULL)
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgResourceService object
/// </summary>
MgResourceService::MgResourceService(MgConnectionProperties* connection) : MgService(connection)
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Get the class Id
/// </summary>
/// <returns>
/// The integer value
/// </returns>
INT32 MgResourceService::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Dispose this object
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgResourceService::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Enumerates the resources in the specified repository.
/// Resources of all types can be enumerated all at once, or only
/// resources of a given type.
///
MgByteReader* MgResourceService::EnumerateResources(
    MgResourceIdentifier* resource, INT32 depth, CREFSTRING type)
{
    STRING fromDate(L"");
    STRING toDate(L"");
    INT32 properties = (MgResourceHeaderProperties::General |
        MgResourceHeaderProperties::Security);

    return EnumerateResources(resource, depth, type, properties, fromDate, toDate);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the content of the specified resource.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource.
/// </param>
/// <param name="preProcessTags">
/// Pre-processing to apply to resource before returning content.  An empty
/// string indicate no pre-processing. See MgResourcePreProcessingType for
/// a list of supported pre-processing tags.
/// </param>
/// <returns>
/// MgByteReader object representing the resource content.
/// </returns>
/// EXCEPTIONS:
/// MgInvalidRepositoryTypeException
/// MgInvalidRepositoryNameException
/// MgInvalidResourcePathException
/// MgInvalidResourceNameException
/// MgInvalidResourceTypeException
///
MgByteReader* MgResourceService::GetResourceContent(
    MgResourceIdentifier* resource)
{
    STRING preProcessTags(L"");

    return GetResourceContent(resource, preProcessTags);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns tagged data for the specified resource.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource.
/// </param>
/// <param name="dataName">
/// Name for data.  Either a resource-unique stream name for streams or a
/// resource-unique file name for file data.
/// </param>
/// <param name="preProcessTags">
/// Pre-processing to apply to resource data before returning.  An empty
/// string indicate no pre-processing. See MgResourcePreProcessingType for
/// a list of supported pre-processing tags.
/// </param>
/// <returns>
/// MgByteReader containing the previously updated or added tagged data.
/// </returns>
/// EXCEPTIONS:
/// MgRepositoryNotOpenException
///
/// MgResourceDataNotFoundException
/// MgInvalidResourceTypeException
///
MgByteReader* MgResourceService::GetResourceData(
    MgResourceIdentifier* resource, CREFSTRING dataName)
{
    STRING preProcessTags(L"");

    return GetResourceData(resource, dataName, preProcessTags);
}
