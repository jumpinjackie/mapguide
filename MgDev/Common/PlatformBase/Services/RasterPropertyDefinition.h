//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _MG_RASTER_PROPERTY_DEFINITION_H_
#define _MG_RASTER_PROPERTY_DEFINITION_H_

/// \defgroup MgRasterPropertyDefinition MgRasterPropertyDefinition
/// \ingroup Feature_Schema_Module
/// \{

class MgStream;

//////////////////////////////////////////////////////////////
/// \brief
/// Defines a feature property which contains a raster image.
///
class MG_PLATFORMBASE_API MgRasterPropertyDefinition : public MgPropertyDefinition
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgRasterPropertyDefinition)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a MgRasterPropertyDefinition object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgRasterPropertyDefinition(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgRasterPropertyDefinition(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgRasterPropertyDefinition(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The name of the property.
    ///
    MgRasterPropertyDefinition(CREFSTRING name);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a Boolean value that indicates whether this property
    /// is read-only.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetReadOnly();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetReadOnly();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetReadOnly();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if this property is read-only; otherwise
    /// returns false.
    ///
    bool GetReadOnly();   /// __get, __set

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a Boolean value that indicates whether this property's
    /// value can be null.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetNullable();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetNullable();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetNullable();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a Boolean value that indicates whether this
    /// property's value can be null.
    ///
    bool GetNullable();   /// __get, __set

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the default size of the image file in the horizontal
    /// direction, in pixels (number of columns).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetDefaultImageXSize();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetDefaultImageXSize();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetDefaultImageXSize();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the default horizontal image size in pixels (number
    /// of columns).
    ///
    INT32 GetDefaultImageXSize();   /// __get, __set

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the default size of the image file in the vertical
    /// direction, in pixels (number of rows).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetDefaultImageYSize();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetDefaultImageYSize();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetDefaultImageYSize();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the default vertical image size in pixels (number of
    /// rows).
    ///
    INT32 GetDefaultImageYSize();   /// __get, __set

    /////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets a Boolean value that specifies whether this property is
    /// read-only.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetReadOnly(bool value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetReadOnly(boolean value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetReadOnly(bool value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (boolean/bool)
    /// True configures the property to be read-only
    /// and false configures it to be user settable.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetReadOnly(bool value);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets a Boolean value that indicates whether this property's
    /// value can be null.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetNullable(bool value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetNullable(boolean value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetNullable(bool value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (boolean/bool)
    /// True configures the property to be nullable,
    /// and false configures it to disallow setting it
    /// to a null value.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetNullable(bool value);

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the default size of the image file in the horizontal
    /// direction, in pixels (number of columns).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetDefaultImageXSize(int size);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetDefaultImageXSize(int size);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetDefaultImageXSize(int size);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param size (int)
    /// The desired default horizontal image size in
    /// pixels (number of columns).
    ///
    /// \return
    /// Returns nothing.
    ///
    ///
    void SetDefaultImageXSize (INT32 size);

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the default size of the image file in the vertical
    /// direction, in pixels (number of rows).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetDefaultImageYSize(int size);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetDefaultImageYSize(int size);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetDefaultImageYSize(int size);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param size (int)
    /// The desired default vertical image size in
    /// pixels (number of rows).
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetDefaultImageYSize (INT32 size);

INTERNAL_API:

    MgRasterPropertyDefinition();
    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);
    void Initialize();
    void ToXml(string& xmlStr, bool includeType);

private:

    bool         m_readOnly;
    bool         m_nullable;
    INT32        m_sizeX;
    INT32        m_sizeY;
    STRING       m_serializedXml;

INTERNAL_API:

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_RasterPropertyDefinition;
};
/// \}

#endif
