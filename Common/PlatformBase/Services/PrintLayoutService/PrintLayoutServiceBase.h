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

#ifndef MGPRINTLAYOUTSERVICEBASE_H_
#define MGPRINTLAYOUTSERVICEBASE_H_

class MgResourceIdentifier;
class MgPrintLayoutBase;
class MgPrintLayoutElementBase;
class MgPrintLayoutElementFactoryBase;

typedef std::map<STRING, MgPrintLayoutElementBase*> MgPrintLayoutElementMap;
typedef std::map<STRING, MgPrintLayoutElementFactoryBase*> MgPrintLayoutElementFactoryMap;

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Manages print layouts, print layout elements and print layout element definitions.
///
/// \remarks
/// Contains methods to:
///   <ul>
///      <li>create a print layout</li>
///      <li>create a print layout element</li>
///      <li>create a print layout element definition</li>
///      <li>get an existing print layout element</li>
///      <li>get an existing print layout element definition</li>
///   </ul>
///
/// \ingroup Print_Layout_Service_classes
///
class MG_PLATFORMBASE_API MgPrintLayoutServiceBase : public MgService
{
INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgPrintLayoutServiceBase object.
    ///
    MgPrintLayoutServiceBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys an MgPrintLayoutServiceBase object.
    ///
    ~MgPrintLayoutServiceBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgPrintLayoutBase object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPrintLayoutBase CreatePrintLayout(MgResourceService resourceService, MgResourceIdentifier resId);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPrintLayoutBase CreatePrintLayout(MgResourceService resourceService, MgResourceIdentifier resId);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPrintLayoutBase CreatePrintLayout(MgResourceService resourceService, MgResourceIdentifier resId);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resourceService
    /// An MgResourceService that can be used to retrieve the print layout resource.
    /// \param resId (MgResourceIdentifier)
    /// Resource identifier of the print layout.
    ///
    /// \return
    /// Returns the realized print layout.
    ///
    virtual MgPrintLayoutBase* CreatePrintLayout(
        MgResourceService* resourceService,
        MgResourceIdentifier* resId) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgPrintLayoutElementBase object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPrintLayoutElementBase CreatePrintLayoutElement(MgResourceService resourceService, MgResourceIdentifier resId);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPrintLayoutElementBase CreatePrintLayoutElement(MgResourceService resourceService, MgResourceIdentifier resId);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPrintLayoutElementBase CreatePrintLayoutElement(MgResourceService resourceService, MgResourceIdentifier resId);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resourceService
    /// An MgResourceService that can be used to retrieve the print layout element resource.
    /// \param resId (MgResourceIdentifier)
    ///
    /// \return
    /// Returns the realized print layout element.
    ///
    virtual MgPrintLayoutElementBase* CreatePrintLayoutElement(
        MgResourceService* resourceService,
        MgResourceIdentifier* resId);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets an MgPrintLayoutElementBase object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPrintLayoutElementBase GetPrintLayoutElement(MgResourceIdentifier resId);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPrintLayoutElementBase GetPrintLayoutElement(MgResourceIdentifier resId);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPrintLayoutElementBase GetPrintLayoutElement(MgResourceIdentifier resId);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resId (MgResourceIdentifier)
    /// Resource identifier of an already existing print layout element.
    ///
    /// \return
    /// Returns the realized print layout element.
    ///
    virtual MgPrintLayoutElementBase* GetPrintLayoutElement(
        MgResourceIdentifier* resId) = 0;

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

    virtual MgPrintLayoutBase* RealizePrintLayout(MgResourceService* resourceService, 
        CREFSTRING resourceXml) = 0;

    virtual MgPrintLayoutElementBase* RealizePrintLayoutElement(MgResourceService* resourceService,
        CREFSTRING resourceXml);

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_PrintLayoutService_PrintLayoutServiceBase;

protected:

    MgPrintLayoutElementMap m_printLayoutElements;
    MgPrintLayoutElementFactoryMap m_printLayoutElementFactories;
};

#endif
