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
    return "PrintLayout";
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the name of the layout.
///
STRING MgPrintLayoutBase::GetName()
{
    return m_name;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Does it allow to set the name
///
bool MgPrintLayoutBase::CanSetName()
{
    return true; // via Create
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the paper size used by this print layout.
///
MgSize2D* MgPrintLayoutBase::GetPaperSize()
{
    return SAFE_ADDREF(m_paperSize.p);
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the name of the intended printing device.
///
STRING MgPrintLayoutBase::GetDeviceName()
{
    return m_deviceName;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the canonical name of the media e.g. Letter, A4, etc.
///
STRING MgPrintLayoutBase::GetMediaName()
{
    return m_mediaName;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the orientation setting. Supported values: Orientation0, Orientation90, Orientation180, Orientation270.
///
STRING MgPrintLayoutBase::GetOrientation()
{
    return m_orientation;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the sizes of the paper margins.
///
MgMargin* MgPrintLayoutBase::GetPaperMargin()
{
    return SAFE_ADDREF(m_paperMargin.p);
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the units of the paper dimensions.
///
STRING MgPrintLayoutBase::GetUnits()
{
    return m_units;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the background color of the print layout - may not be supported in all output formats and devices.
///
MgColor* MgPrintLayoutBase::GetBackgroundColor()
{
    return SAFE_ADDREF(m_backgroundColor.p);
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
/// Serializes data to TCP/IP stream.
///
void MgPrintLayoutBase::Serialize(MgStream* stream)
{
    stream->WriteString(m_name);
    stream->WriteString(m_units);
    stream->WriteString(m_mediaName);
    stream->WriteString(m_deviceName);
    stream->WriteString(m_orientation);

    stream->WriteObject(m_backgroundColor);
    stream->WriteObject(m_paperSize);
    stream->WriteObject(m_paperMargin);
    stream->WriteObject(m_elements);
    stream->WriteObject(m_extent);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgPrintLayoutBase::Deserialize(MgStream* stream)
{
    stream->GetString(m_name);
    stream->GetString(m_units);
    stream->GetString(m_mediaName);
    stream->GetString(m_deviceName);
    stream->GetString(m_orientation);

    m_backgroundColor = static_cast<MgColor*>(stream->GetObject());
    m_paperSize = static_cast<MgSize2D*>(stream->GetObject());
    m_paperMargin = static_cast<MgMargin*>(stream->GetObject());
    m_elements = static_cast<MgPrintLayoutElementCollection*>(stream->GetObject());
    m_extent = static_cast<MgEnvelope*>(stream->GetObject());
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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Forces this object to refresh itself from the associated resource
///
void MgPrintLayoutBase::ForceRefresh(MgResourceService* resourceService)
{
    if (NULL != m_resId.p)
    {
        this->Open(resourceService, m_resId);
    }
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Populates the print layout from the resource XML string.
///
void MgPrintLayoutBase::PopulateFromResource(
    MgPrintLayoutServiceBase* printLayoutService,
    MgResourceService* resourceService,
    CREFSTRING layoutXml)
{
    assert(!layoutXml.empty());

    // Parse the resource XML.
    string strXml = MgUtil::WideCharToMultiByte(layoutXml);
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
    std::auto_ptr<MdfModel::PrintLayoutDefinition> layoutDef(parser.DetachPrintLayoutDefinition());
    PopulateFromResource(printLayoutService, resourceService, layoutDef.get());
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Populates the print layout from the MDF representation.
///
void MgPrintLayoutBase::PopulateFromResource(
    MgPrintLayoutServiceBase* printLayoutService,
    MgResourceService* resourceService,
    MdfModel::PrintLayoutDefinition *layoutDef)
{
    // Reset the data.
    m_elements->Clear();
    m_extent = NULL;
    m_backgroundColor = NULL;
    m_paperMargin = NULL;
    m_paperSize = NULL;

    m_name = L"";
    m_units = L"";
    m_mediaName = L"";
    m_deviceName = L"";
    m_orientation = L"";

    // Validate inputs.
    assert(NULL != printLayoutService && NULL != resourceService && NULL != layoutDef);
    if (NULL == printLayoutService || NULL == resourceService || NULL == layoutDef)
        return;

    // Populate data
    m_name = layoutDef->GetName();
    m_units = layoutDef->GetUnits();
    m_mediaName = layoutDef->GetMediaName();
    m_deviceName = layoutDef->GetDeviceName();
    m_orientation = layoutDef->GetOrientation();

    // Set the Color
    MdfModel::Color* bgColor = layoutDef->GetBackgroundColor();
    if (NULL != bgColor)
        m_backgroundColor = new MgColor(bgColor->GetRed(), bgColor->GetGreen(), bgColor->GetBlue(), bgColor->GetAlpha());

    // Set the paper margin
    MdfModel::Thickness* layoutMargin = layoutDef->GetPaperMargin();
    if (NULL != layoutMargin)
        m_paperMargin = new MgMargin(layoutMargin->GetLeft(), layoutMargin->GetTop(), layoutMargin->GetRight(), layoutMargin->GetBottom());

    // Set the paper size
    MdfModel::Size2D* layoutSize = layoutDef->GetPaperSize();
    if (NULL != layoutSize)
        m_paperSize = new MgSize2D(layoutSize->GetHeight(), layoutSize->GetWidth());

    // Populate the data.
    MdfModel::PrintLayoutElementCollection* elements = layoutDef->GetElements();
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
                newElement->PopulateFromResource(thisElement);
                m_elements->Add(newElement);
            }
        }
    }

    Extent3D* extent = layoutDef->GetExtent();
    if (NULL != extent)
    {
        Point3D* minPt = extent->GetMinimumPoint();
        Point3D* maxPt = extent->GetMaximumPoint();
        assert(NULL != minPt && NULL != maxPt);

        Ptr<MgCoordinateXYZ> lowerLeft  = new MgCoordinateXYZ(minPt->GetX(), minPt->GetY(), minPt->GetZ());
        Ptr<MgCoordinateXYZ> upperRight = new MgCoordinateXYZ(maxPt->GetX(), maxPt->GetY(), maxPt->GetZ());

        m_extent = new MgEnvelope(lowerLeft, upperRight);
    }
}

