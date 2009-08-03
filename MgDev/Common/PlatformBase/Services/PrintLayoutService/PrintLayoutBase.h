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
    /// Callback method invoked when a print layout element is added to the group collection
    ///
    virtual void OnPrintLayoutElementAdded(MgPrintLayoutElementBase* element);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Callback method invoked when a print layout element is removed from the print layout element collection
    ///
    virtual void OnPrintLayoutElementRemoved(MgPrintLayoutElementBase* element);

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

    Ptr<MgPrintLayoutElementCollection> m_elements;
    Ptr<MgEnvelope> m_extent;
};

#endif
