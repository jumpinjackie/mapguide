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

#ifndef RASTER_PROPERTY_H
#define RASTER_PROPERTY_H

/// \defgroup MgRasterProperty MgRasterProperty
/// \ingroup Feature_Properties_Module
/// \{

class Raster;

/////////////////////////////////////////////////////////////////
/// \brief
/// Stores MgRaster data with an associated key name.
/// \remarks
/// You can store an instance of this class in an
/// MgPropertyCollection object. Use the key name to retrieve the
/// instance from the property collection.
///
class MG_PLATFORMBASE_API MgRasterProperty : public MgNullableProperty
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgRasterProperty)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgRasterProperty object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgRasterProperty(string name, MgRaster value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgRasterProperty(String name, MgRaster value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgRasterProperty(string name, MgRaster value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The name of the property.
    /// \param value (MgRaster)
    /// The value of the property.
    ///
    MgRasterProperty(CREFSTRING name, MgRaster* value);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the property value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgRaster GetValue();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgRaster GetValue();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgRaster GetValue();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the property value.
    ///
    MgRaster* GetValue();   /// __get, __set

    /////////////////////////////////////////////////////////
    /// \brief
    /// Gets the property type.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetPropertyType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetPropertyType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetPropertyType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the property type MgPropertyType::Raster.
    ///
    INT16 GetPropertyType();   /// __get, __inherited

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the property value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetValue(MgRaster value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetValue(MgRaster value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetValue(MgRaster value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (MgRaster)
    /// The value of the property.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetValue(MgRaster* value);

EXTERNAL_API:

    ////////////////////////////////////////////////////////
    /// \brief
    /// Destructor. Deletes the property and any information
    /// contained in it.
    ///
    ///
    virtual ~MgRasterProperty();

INTERNAL_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructor.
    ///
    MgRasterProperty();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class Id.
    ///
    /// \return
    /// Returns the integer value of the class Id.
    ///
    INT32 GetClassId();

    friend class MgPropertyCollection;
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the property name and value to XML.
    ///
    /// \return
    /// ByteReader for the XML.
    ///
    void ToXml(string &str, bool includeType = true, string rootElmName = "Property");

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Self destructing method.
    ///
    virtual void Dispose();

private:

    Ptr<MgRaster>   m_value;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_RasterProperty;
};
/// \}

#endif
