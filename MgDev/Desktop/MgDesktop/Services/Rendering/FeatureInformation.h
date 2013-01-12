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

#ifndef _MG_FEATUREINFORMATION_H_
#define _MG_FEATUREINFORMATION_H_

class MgdFeatureInformation;
template class MG_DESKTOP_API Ptr<MgdFeatureInformation>;

class MgdSelection;
/// \ingroup Desktop_Misc_Module
/// \{
///////////////////////////////////////////////////////////
/// \brief
/// Information about features.
///
class MG_DESKTOP_API MgdFeatureInformation : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgdFeatureInformation)

PUBLISHED_API:

    /////////////////////////////////////////
    /// \brief
    /// Get the selection set the described features are part of.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgSelection GetSelection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgSelection GetSelection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgSelection GetSelection();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgdSelection* GetSelection();

    /////////////////////////////////////////
    /// \brief
    /// Get the tooltip text.
    ///
    /// \remarks
    /// Valid only when there is only one feature described in this object
    ////
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetTooltip();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetTooltip();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetTooltip();
    /// \htmlinclude SyntaxBottom.html
    ///
    ///
    STRING GetTooltip();

    /////////////////////////////////////////
    /// \brief
    /// Get the hyperlink URL text
    ///
    /// \remarks
    /// Valid only when there is only one feature described in this object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetHyperlink();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetHyperlink();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetHyperlink();
    /// \htmlinclude SyntaxBottom.html
    ///

    STRING GetHyperlink();

    /////////////////////////////////////////
    /// \brief
    /// Get the collection of properties for the described feature.
    ///
    /// \remarks
    /// Valid only when there is only one feature described in this object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPropertyCollection GetProperties();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPropertyCollection GetProperties();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPropertyCollection GetHyperlink();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgPropertyCollection* GetProperties();

EXTERNAL_API:

    //////////////////////////////////////////////////////////
    /// \brief
    /// Creates an XML document containing the feature information.
    ///
    /// \return
    /// A pointer to an MgByteReader object.
    ///
    MgByteReader* ToXml();

INTERNAL_API:

    /////////////////////////////////////////
    /// Construct an empty MgFeatureInformation object
    ///
    MgdFeatureInformation();

    /////////////////////////////////////////
    /// Set the selection
    ///
    void SetSelection(MgdSelection* selection);

    /////////////////////////////////////////
    /// Set the properties
    ///
    void SetProperties(MgPropertyCollection* properties);

    /////////////////////////////////////////
    /// Set the tooltip text
    ///
    void SetTooltip(CREFSTRING tooltip);

    /////////////////////////////////////////
    /// Set the hyperlink URL
    ///
    void SetHyperlink(CREFSTRING hyperlink);

    /////////////////////////////////////////
    /// Serialize to a stream
    ///
    virtual void Serialize(MgStream* stream);

    /////////////////////////////////////////
    /// Deserialize from a stream
    ///
    virtual void Deserialize(MgStream* stream);

    /////////////////////////////////////////
    /// Get the class ID
    ///
    virtual INT32 GetClassId() { return m_cls_id; }

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_RenderingService_FeatureInformation;

protected:

    /////////////////////////////////////////
    /// Dispose the object
    ///
    virtual void Dispose()
    {
        delete this;
    }

private:
    Ptr<MgdSelection>            m_selection;
    Ptr<MgPropertyCollection>   m_properties;
    STRING                      m_tooltip;
    STRING                      m_hyperlink;
};
/// \}

#endif
