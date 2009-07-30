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

MG_IMPL_DYNCREATE(MgPrintLayoutBase)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgPrintLayoutBase object.
///
MgPrintLayoutBase::MgPrintLayoutBase()
{
    m_elements = new MgPrintLayoutElementCollection(this);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgPrintLayoutBase object.
///
MgPrintLayoutBase::~MgPrintLayoutBase()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgPrintLayoutBase::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgPrintLayoutBase::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the name of the resource type.
///
const char* MgPrintLayoutBase::GetResourceTypeName()
{
    return "PrintLayoutDefinition";
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the name of the layout.
///
STRING MgPrintLayoutBase::GetName()
{
    return L"";
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Does it allow to set the name
///
bool MgPrintLayoutBase::CanSetName()
{
    return true; // via Create
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
void MgPrintLayoutBase::Serialize(MgStream* stream)
{
    stream->WriteObject(m_elements);
    stream->WriteObject(m_extent);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgPrintLayoutBase::Deserialize(MgStream* stream)
{
    m_elements = static_cast<MgPrintLayoutElementCollection*>(stream->GetObject());
    m_extent = static_cast<MgEnvelope*>(stream->GetObject());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the collection of print layout elements.
///
MgPrintLayoutElementCollection* MgPrintLayoutBase::GetElements()
{
    return SAFE_ADDREF(m_elements.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the extent of the print layout.
///
MgEnvelope* MgPrintLayoutBase::GetExtent()
{
    return SAFE_ADDREF(m_extent.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Callback method invoked when a print layout element is added to the group collection
///
void MgPrintLayoutBase::OnPrintLayoutElementAdded(MgPrintLayoutElementBase* element)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Callback method invoked when a print layout element is removed from the print layout element collection
///
void MgPrintLayoutBase::OnPrintLayoutElementRemoved(MgPrintLayoutElementBase* element)
{
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Populates the print layout from the resource XML string.
///
void MgPrintLayoutBase::PopulateFromResource(
    MgPrintLayoutServiceBase* printLayoutService,
    MgResourceService* resourceService,
    CREFSTRING resourceXml)
{
    assert(!resourceXml.empty());

    // Parse the resource XML.
    string strXml = MgUtil::WideCharToMultiByte(resourceXml);
    MdfParser::SAX2Parser parser;
    parser.ParseString(strXml.c_str(), strXml.length());

    if (!parser.GetSucceeded()) 
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);

        throw new MgXmlParserException(L"MgPrintLayoutBase.PopulateFromResource",
            __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }

    // Populate the data.
    std::auto_ptr<MdfModel::PrintLayoutDefinition> resourceDef(parser.DetachPrintLayoutDefinition());
    this->PopulateFromResource(printLayoutService, resourceService, resourceDef.get());
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Populates the print layout from the MDF representation.
///
void MgPrintLayoutBase::PopulateFromResource(
    MgPrintLayoutServiceBase* printLayoutService,
    MgResourceService* resourceService,
    MdfModel::PrintLayoutDefinition *resourceDef)
{
    // Reset the data.
    m_elements->Clear();
    m_extent = NULL;

    // Populate the data.
    assert(NULL != printLayoutService && NULL != resourceService && NULL != resourceDef);
    if (NULL != printLayoutService && NULL != resourceService && NULL != resourceDef)
    {
        MdfModel::PrintLayoutElementCollection* elements = resourceDef->GetElements();
        if (NULL != elements)
        {
            for (int i = 0; i < elements->GetCount(); ++i)
            {
                PrintLayoutElement* thisElement = elements->GetAt(i);
                assert(NULL != thisElement);
                if (NULL != thisElement)
                {
                    Ptr<MgResourceIdentifier> resourceId = new MgResourceIdentifier(thisElement->GetResourceId());
                    Ptr<MgPrintLayoutElementBase> newElement = printLayoutService->CreatePrintLayoutElement(
                        resourceService, resourceId);
                    m_elements->Add(newElement);
                }
            }
        }

        Extent3D* extent = resourceDef->GetExtent();
        if (NULL != extent)
        {
            Point3D* minPt = extent->GetMinimumPoint();
            Point3D* maxPt = extent->GetMinimumPoint();
            assert(NULL != minPt && NULL != maxPt);

            Ptr<MgCoordinateXYZ> lowerLeft  = new MgCoordinateXYZ(minPt->GetX(), minPt->GetY(), minPt->GetZ());
            Ptr<MgCoordinateXYZ> upperRight = new MgCoordinateXYZ(maxPt->GetX(), maxPt->GetY(), maxPt->GetZ());
        
            m_extent = new MgEnvelope(lowerLeft, upperRight);
        }
    }
}

