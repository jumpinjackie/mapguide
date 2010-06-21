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

#include "PlatformBase.h"
#include "Exception/ServiceNotAvailableException.h"
#include "Exception/NotImplementedException.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Default Constructor
/// </summary>
MgResourceService::MgResourceService() : MgService()
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
    return EnumerateResources(resource, depth, type, true);
}

MgByteReader* MgResourceService::EnumerateResources(
    MgResourceIdentifier* resource, INT32 depth, CREFSTRING type, bool computeChildren)
{
    STRING fromDate(L"");
    STRING toDate(L"");
    INT32 properties = (MgResourceHeaderProperties::General |
        MgResourceHeaderProperties::Security);

    if (computeChildren)
    {
        return EnumerateResources(resource, depth, type, properties, fromDate, toDate);
    }
    else
    {
        return EnumerateResources(resource, depth, type, properties, fromDate, toDate, computeChildren);
    }
}

MgByteReader* MgResourceService::EnumerateResources(
    MgResourceIdentifier* resource, INT32 depth, CREFSTRING type,
    INT32 properties, CREFSTRING fromDate, CREFSTRING toDate)
{
    return EnumerateResources(resource, depth, type, properties, fromDate, toDate, true);
}

MgByteReader* MgResourceService::EnumerateResources(
    MgResourceIdentifier* resource, INT32 depth, CREFSTRING type,
    INT32 properties, CREFSTRING fromDate, CREFSTRING toDate, bool computeChildren)
{
    throw new MgNotImplementedException(L"MgResourceService.EnumerateResources",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerate the resource documents in the specified repository.
///
STRING MgResourceService::EnumerateResourceDocuments(
    MgStringCollection* resources, CREFSTRING type, INT32 properties)
{
    throw new MgNotImplementedException(
        L"MgResourceService.EnumerateResourceDocuments",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Moves an existing resource to another location.
///
void MgResourceService::MoveResource(MgResourceIdentifier* sourceResource,
                   MgResourceIdentifier* destResource, bool overwrite)
{
    throw new MgNotImplementedException(L"MgResourceService.MoveResource",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgResourceService::MoveResource(MgResourceIdentifier* sourceResource,
                  MgResourceIdentifier* destResource, bool overwrite, bool cascade)
{
    throw new MgNotImplementedException(L"MgResourceService.MoveResource",
        __LINE__, __WFILE__, NULL, L"", NULL);
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

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the contents of the specified resources.
///
MgStringCollection* MgResourceService::GetResourceContents(MgStringCollection* resources,
                                        MgStringCollection* preProcessTags)
{
    throw new MgNotImplementedException(L"MgResourceService.GetResourceContents", __LINE__, __WFILE__, NULL, L"", NULL);
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



//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the metadata of the specified resource.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource.
/// </param>
/// <param name="content">
/// MgByteReader
/// </param>
/// <returns>
/// MgByteReader object representing the metadata.
/// </returns>
/// EXCEPTIONS:
///
void MgResourceService::SetResourceMetadata(
    MgResourceIdentifier* resource, MgByteReader* content)
{
    throw new MgNotImplementedException(L"MgResourceService.SetResourceMetadata", __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the metadata of the specified resource.
/// </summary>
/// <param name="resource">
/// Resource identifier describing the resource.
/// </param>
/// <returns>
/// MgByteReader object representing the metadata.
/// </returns>
/// EXCEPTIONS:
///
MgByteReader* MgResourceService::GetResourceMetadata(
    MgResourceIdentifier* resource)
{
    throw new MgNotImplementedException(L"MgResourceService.GetResourceMetadata", __LINE__, __WFILE__, NULL, L"", NULL);
}


bool MgResourceService::HasPermission(MgResourceIdentifier* resource, CREFSTRING permission)
{
    throw new MgNotImplementedException(L"MgResourceService.HasPermission", __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
/// \brief
/// Enumerates the unmanaged data
/// Resources of all types can be enumerated all at once, or only
/// resources of a given type.
///
MgByteReader* MgResourceService::EnumerateUnmanagedData(CREFSTRING path, bool recursive, CREFSTRING type, CREFSTRING filter)
{
    throw new MgNotImplementedException(L"MgResourceService.EnumerateUnmanagedData", __LINE__, __WFILE__, NULL, L"", NULL);
}
