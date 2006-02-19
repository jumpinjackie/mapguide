//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

/// \defgroup MgFeatureInformation MgFeatureInformation
/// \ingroup Common_Module
/// \{

class MgFeatureInformation;
template class MG_SERVICE_API Ptr<MgFeatureInformation>;

class MgSelection;

/// Exposed API class for handling feature information
///
class MG_SERVICE_API MgFeatureInformation : public MgSerializable
{
    DECLARE_CREATE_OBJECT();
    DECLARE_CLASSNAME(MgFeatureInformation)

PUBLISHED_API:

    /////////////////////////////////////////
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
    /// Get the selection set the described features are part of.
    MgSelection* GetSelection();

    /////////////////////////////////////////
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
    /// Get the tooltip text
    /// Valid only when there is only one feature described in this object
    STRING GetTooltip();

    /////////////////////////////////////////
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
    /// Get the hyperlink URL text
    /// Valid only when there is only one feature described in this object.
    STRING GetHyperlink();

    /////////////////////////////////////////
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
    /// Get the collection of properties for the described feature.
    /// Valid only when there is only one feature described in this object.
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
    MgFeatureInformation();

    /////////////////////////////////////////
    /// Set the selection
    ///
    void SetSelection(MgSelection* selection);

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
    static const INT32 m_cls_id = RenderingService_FeatureInformation;

protected:

    /////////////////////////////////////////
    /// Dispose the object
    ///
    virtual void Dispose() { delete this; }

private:
    Ptr<MgSelection>            m_selection;
    Ptr<MgPropertyCollection>   m_properties;
    STRING                      m_tooltip;
    STRING                      m_hyperlink;
};
/// \}

#endif
