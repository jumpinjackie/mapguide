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

#ifndef MGPRINTLAYOUTBASE_H_
#define MGPRINTLAYOUTBASE_H_

// Forward Declarations
BEGIN_NAMESPACE_MDFMODEL
class PrintLayoutDefinition;
END_NAMESPACE_MDFMODEL

class MgMargin;
class MgSize2D;
class MgPrintLayoutServiceBase;
class MgPrintLayoutElementCollection;

////////////////////////////////////////////////////////////
/// \brief
/// Defines the MgPrintLayoutBase object.
///
class MG_PLATFORMBASE_API MgPrintLayoutBase : public MgResource
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgPrintLayoutBase)

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgPrintLayoutBase object.
    ///
    MgPrintLayoutBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys an MgPrintLayoutBase object.
    ///
    ~MgPrintLayoutBase();

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of this layout.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the name of the layout as a string.
    ///
    virtual STRING GetName();  /// __get

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the collection of print layout elements.
    ///
    virtual MgPrintLayoutElementCollection* GetElements();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the extent of the print layout.
    ///
    virtual MgEnvelope* GetExtent();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the paper size used by this print layout.
    ///
    virtual MgSize2D* GetPaperSize();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the intended printing device.
    ///
    virtual STRING GetDeviceName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the canonical name of the media e.g. Letter, A4, etc.
    ///
    virtual STRING GetMediaName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the orientation setting. Supported values: Orientation0, Orientation90, Orientation180, Orientation270.
    ///
    virtual STRING GetOrientation();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the sizes of the paper margins.
    ///
    virtual MgMargin* GetPaperMargin();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the units of the paper dimensions.
    ///
    virtual STRING GetUnits();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the background color of the print layout - may not be supported in all output formats and devices.
    ///
    virtual MgColor* GetBackgroundColor();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Callback method invoked when a print layout element is added to the group collection
    ///
    virtual void OnPrintLayoutElementAdded(MgPrintLayoutElementBase* element);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Callback method invoked when a print layout element is removed from the print layout element collection
    ///
    virtual void OnPrintLayoutElementRemoved(MgPrintLayoutElementBase* element);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Forces this element object to refresh itself from its resource
    ///
    virtual void ForceRefresh(MgResourceService* resourceService);

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Does it allow to set the name
    ///
    /// \return
    /// true - allows to set the name
    /// false - can not set the name
    ///
    virtual bool CanSetName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Populates the print layout from the resource XML string.
    ///
    virtual void PopulateFromResource(
        MgPrintLayoutServiceBase* printLayoutService,
        MgResourceService* resourceService,
        CREFSTRING layoutXml);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Populates the print layout from the MDF representation.
    ///
    virtual void PopulateFromResource(
        MgPrintLayoutServiceBase* printLayoutService,
        MgResourceService* resourceService,
        MdfModel::PrintLayoutDefinition *layoutDef);

protected:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class ID.
    ///
    /// \return
    /// The integer value.
    ///
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Disposes this object.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void Dispose();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the name of the resource type.
    ///
    virtual const char* GetResourceTypeName();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_PrintLayoutService_PrintLayoutBase;

protected:

    STRING m_name;
    STRING m_units;
    STRING m_mediaName;
    STRING m_deviceName;
    STRING m_orientation;
    Ptr<MgColor> m_backgroundColor;
    Ptr<MgSize2D> m_paperSize;
    Ptr<MgEnvelope> m_extent;
    Ptr<MgMargin> m_paperMargin;
    Ptr<MgPrintLayoutElementCollection> m_elements;
};

#endif
