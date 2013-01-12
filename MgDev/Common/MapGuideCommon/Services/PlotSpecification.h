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

#ifndef MG_PLOT_SPECIFICATION_H
#define MG_PLOT_SPECIFICATION_H

/// \defgroup MgPlotSpecification MgPlotSpecification
/// \ingroup Mapping_Service_Module
/// \{

////////////////////////////////////////////////////////////
/// \brief
/// Specifies the size and format of the \link ePlot ePlot \endlink
/// sheet to be created by the Mapping Service operations.
class MG_MAPGUIDE_API MgPlotSpecification : public MgSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgPlotSpecification)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifications for the plot.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPlotSpecification(float paperWidth, float paperHeight, string pageUnits, float left = 0, float top = 0, float right = 0, float bottom = 0);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPlotSpecification(float paperWidth, float paperHeight, String pageUnits, float left = 0, float top = 0, float right = 0, float bottom = 0);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPlotSpecification(float paperWidth, float paperHeight, string pageUnits, float left = 0, float top = 0, float right = 0, float bottom = 0);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param paperWidth (float)
    /// Input
    /// paper width in page units
    /// \param paperHeight (float)
    /// Input
    /// paper height in page units
    /// \param pageUnits (String/string)
    /// Input
    /// units for page size.  See MgPageUnitsType for valid units of measure
    /// \param left (float)
    /// Input
    /// The left margin between the plot and the page edge.
    /// \param top (float)
    /// Input
    /// The top margin between the plot and the page edge.
    /// \param right (float)
    /// Input
    /// The right margin between the plot and the page edge.
    /// \param bottom (float)
    /// Input
    /// The bottom margin between the plot and the page edge.
    ///
    /// \return
    /// Nothing
    ///
    MgPlotSpecification(float paperWidth, float paperHeight, CREFSTRING pageUnits,
        float left = 0, float top = 0, float right = 0, float bottom = 0);

    ///////////////////////////////////////////////////////
    /// \brief
    /// Gets the paper width.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// float GetPaperWidth();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// float GetPaperWidth();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// float GetPaperWidth();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the paper width in the current page units.
    ///
    float GetPaperWidth();

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the paper width.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetPaperWidth(float width);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetPaperWidth(float width);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetPaperWidth(float width);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param width (float)
    /// Paper width in the current page units.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetPaperWidth(float width);

    /////////////////////////////////////////////////////////
    /// \brief
    /// Gets the paper height.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// float GetPaperHeight();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// float GetPaperHeight();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// float GetPaperHeight();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the paper height in the current page units.
    ///
    float GetPaperHeight();

    ////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the paper height.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetPaperHeight(float height);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetPaperHeight(float height);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetPaperHeight(float height);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param height (float)
    /// Paper height in the current page units.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetPaperHeight(float height);

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the units of measure for the page.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetPageSizeUnits();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetPageSizeUnits();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetPageSizeUnits();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the units of measure for the page as a string. 'in'
    /// = inches, 'mm' = millimeters.
    ///
    STRING GetPageSizeUnits();

    ////////////////////////////////////////////////////////////////////
    /// \brief
    /// Set the units of measure for the page.
    ///
    /// \remarks
    /// NOTE: No conversion is performed on the current width and
    /// height when the unit is set.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetPageSizeUnits(string pageUnits);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetPageSizeUnits(String pageUnits);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetPageSizeUnits(string pageUnits);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param pageUnits (String/string)
    /// Units of measure for page. See
    /// MgPageUnitsType for valid units.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetPageSizeUnits(CREFSTRING pageUnits);

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// float GetMarginLeft();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// float GetMarginLeft();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// float GetMarginLeft();
    /// \htmlinclude SyntaxBottom.html
    ///
    float GetMarginLeft();

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// float GetMarginTop();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// float GetMarginTop();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// float GetMarginTop();
    /// \htmlinclude SyntaxBottom.html
    ///
    float GetMarginTop();

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// float GetMarginRight();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// float GetMarginRight();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// float GetMarginRight();
    /// \htmlinclude SyntaxBottom.html
    ///
    float GetMarginRight();

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// float GetMarginBottom();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// float GetMarginBottom();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// float GetMarginBottom();
    /// \htmlinclude SyntaxBottom.html
    ///
    float GetMarginBottom();

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetMargins(float left, float top, float right, float bottom);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetMargins(float left, float top, float right, float bottom);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetMargins(float left, float top, float right, float bottom);
    /// \htmlinclude SyntaxBottom.html
    ///
    void SetMargins(float left, float top, float right, float bottom);

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
    static const INT32 m_cls_id = MapGuide_MappingService_PlotSpecification;

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default constructor to shut Ptr<> up.
    ///
    MgPlotSpecification();

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
    float       m_paperWidth;
    float       m_paperHeight;
    STRING      m_pageUnits;

    float       m_marginLeft;
    float       m_marginTop;
    float       m_marginRight;
    float       m_marginBottom;
};
/// \}

#endif // MG_PLOT_SPECIFICATION_H
