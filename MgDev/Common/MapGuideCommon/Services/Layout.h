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

#ifndef MG_LAYOUT_H
#define MG_LAYOUT_H

#include "UnitType.h"

/// \defgroup MgLayout MgLayout
/// \ingroup Mapping_Service_Module
/// \{

class MgResourceIdentifier;

////////////////////////////////////////////////////////////
/// \brief
/// Specifies the ResourceId of the PrintLayout to use for a particular
/// map, the title and the type of units to use in the layout.
class MG_MAPGUIDE_API MgLayout : public MgSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgLayout)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Initializes a new MgLayout object given a layout definition, title and unit type.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayout(MgResourceIdentifier layoutDefinition, string title, string unitType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayout(MgResourceIdentifier layoutDefinition, String title, String unitType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayout(MgResourceIdentifier layoutDefinition, string title, string unitType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param layoutDefinition (MgResourceIdentifier)
    /// An MgResourceIdentifier that specifies the
    /// location of the print layout definition in a resource
    /// repository.
    /// \param title (String/string)
    /// The title to use in the print layout
    /// \param unitType (String/string)
    /// The type of units to use in the print layout
    ///
    /// \return
    /// Nothing
    ///
    MgLayout(MgResourceIdentifier* layoutDefinition, CREFSTRING title, CREFSTRING unitType);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the Layout to use when generating the EPlot DWF.  If not
    /// specified, then the plotted map will fill the space within the page
    /// margins.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgResourceIdentifier GetLayout();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgResourceIdentifier GetLayout();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgResourceIdentifier GetLayout();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The resource identifier for the layout definition.
    ///
    MgResourceIdentifier* GetLayout();

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Set the Layout to use when generating the EPlot DWF.  If not
    /// specified, then the plotted map will fill the space within the page
    /// margins.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetLayout(MgResourceIdentifier plotLayout);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetLayout(MgResourceIdentifier plotLayout);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetLayout(MgResourceIdentifier plotLayout);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param plotLayout (MgResourceIdentifier)
    /// An MgResourceIdentifier that specifies the
    /// location of the print layout definition in a resource
    /// repository.
    ///
    /// \return
    /// Nothing
    ///
    void SetLayout(MgResourceIdentifier* plotLayout);

    ///////////////////////////////////////////////////////
    /// \brief
    /// Gets the layout title.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetTitle();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetTitle();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetTitle();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the title for the plot.
    ///
    STRING GetTitle();

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the layout title.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetTitle(string title);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetTitle(String title);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetTitle(string title);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param title (String/string)
    /// The title for the layout.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetTitle(CREFSTRING title);

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the layout scale units (US/English or metric units).
    /// This value determines the units used in the scale bar definition.
    /// This value is ignored if a layout is not specified.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetUnitType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetUnitType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetUnitType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The layout scale units.
    ///
    STRING GetUnitType();

    ////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the layout scale units (US/English or metric units).
    /// This value determines the units used in the scale bar definition.
    /// This value is ignored if a layout is not specified.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetUnitType(string unitType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetUnitType(String unitType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetUnitType(string unitType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param unitType (String/string)
    /// The layout scale units
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetUnitType(CREFSTRING unitType);

protected:

    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose()
    {
        delete this;
    }

    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId() { return m_cls_id; }

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_MappingService_Layout;

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgLayout object.
    ///
    /// \return
    /// Nothing
    ///
    MgLayout();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroy a MgLayout object
    ///
    ~MgLayout();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

private:

    MgResourceIdentifier* m_layout;
    STRING m_title;
    STRING m_unitType;
};
/// \}

#endif // MG_LAYOUT_H
