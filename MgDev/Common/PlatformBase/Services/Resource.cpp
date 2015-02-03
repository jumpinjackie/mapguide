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

#include "PlatformBase.h"
#include "ResourceService.h"

STRING MgResource::m_resourceDataTag = L"RuntimeData";

// Construct a MgResource object
MgResource::MgResource()
{
    m_resId = NULL;
}


// Returns the ResourceIdentifier of this resource or null if the resource has not been opened or saved.
//
MgResourceIdentifier* MgResource::GetResourceId()
{
    return SAFE_ADDREF((MgResourceIdentifier*)m_resId);
}

// Opens the resource using the specified resource service and resource identifier.
//
void MgResource::Open(MgResourceService* resourceService, MgResourceIdentifier* resourceId)
{
    m_resId = resourceId;
    SAFE_ADDREF((MgResourceIdentifier*)m_resId);

    Ptr<MgByteReader> breader = resourceService->GetResourceData(m_resId, m_resourceDataTag);

    //get the byte reader content into a memory stream
    MgByteSink sink(breader);
    Ptr<MgByte> bytes = sink.ToBuffer();
    Ptr<MgMemoryStreamHelper> streamHelper =
        new MgMemoryStreamHelper((INT8*) bytes->Bytes(), bytes->GetLength(), false);
    Ptr<MgStream> stream = new MgStream(streamHelper);

    //let the object deserialize itself from the memory stream
    Deserialize(stream);
}

// Saves the resource using the specified resource service and resource identifier.
// This method assumes a valid resource identifier has already been established
// for this resource via either Open or Save
//
void MgResource::Save(MgResourceService* resourceService)
{
    CHECKNULL(m_resId.p, L"MgResource.Save");

    SerializeToRepository(resourceService, false);
}

// Saves the resource using the specified resource service and resource identifier.
//
void MgResource::Save(MgResourceService* resourceService, MgResourceIdentifier* resourceId)
{
    m_resId = SAFE_ADDREF(resourceId);
    CHECKNULL(m_resId.p, L"MgResource.Save");

    SerializeToRepository(resourceService, true);
}

void MgResource::SerializeToRepository(MgResourceService* resourceService, bool create)
{
    //let the object serialize itself in a memory stream
    Ptr<MgMemoryStreamHelper> streamHelper = new MgMemoryStreamHelper();

    Ptr<MgStream> stream = new MgStream(streamHelper);

    Serialize(stream);

    //Make a byte reader out of the memory stream
    Ptr<MgByteSource> bsource = new MgByteSource((BYTE_ARRAY_IN)streamHelper->GetBuffer(), streamHelper->GetLength());
    Ptr<MgByteReader> resourceData = bsource->GetReader();

    if(create)
    {
        //Create fake content for this resource, needed for adding the resource
        //Fill to a length greater than 256 bytes to reduce dbmxl repository contention.
        //We will set dbxml to the minimum page size of 512 bytes
        const char* resTypeName = GetResourceTypeName();
        string filler(256,' ');
        string xmlContent = string("<?xml version=\"1.0\" encoding=\"UTF-8\"?><") + resTypeName + ">" + filler.c_str() + "</" + resTypeName + ">";
        Ptr<MgByteSource> bsource1 = new MgByteSource((BYTE_ARRAY_IN)xmlContent.c_str(), (INT32)xmlContent.length());
        Ptr<MgByteReader> content = bsource1->GetReader();

        resourceService->SetResource(m_resId, content, NULL);
    }

    resourceService->SetResourceData(m_resId, m_resourceDataTag, L"Stream", resourceData);
}

// Destruct a MgResource object
//
MgResource::~MgResource()
{
}
