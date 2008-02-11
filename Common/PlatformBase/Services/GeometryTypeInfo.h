//
//  Copyright (C) 2007-2008 by Autodesk, Inc.
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

#ifndef _MG_GEOMETRY_TYPE_INFO_H
#define _MG_GEOMETRY_TYPE_INFO_H

#define MG_MAX_GEOMETRY_TYPE_SIZE    12

/// \defgroup MgGeometryTypeInfo MgGeometryTypeInfo
/// \ingroup Feature_Schema_Module
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// Defines a list of geometric types.  This is typically used for geometric property description.
///
class MG_PLATFORMBASE_API MgGeometryTypeInfo : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgGeometryTypeInfo)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgGeometryTypeInfo object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgGeometryTypeInfo();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgGeometryTypeInfo();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgGeometryTypeInfo();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgGeometryTypeInfo();

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the MgGeometryType value which specifies one of the
    /// types of FGF geometry (Point, LineString, CurveString, ...)
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetType(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetType(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetType(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The index value of the type to get.  This must be in the range
    /// of zero to MgGeometryTypeInfo::GetCount()-1.
    ///
    INT32 GetType(INT32 index);

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the count of geometry types in the list.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetCount();
    /// \htmlinclude SyntaxBottom.html
    ///
    INT32 GetCount();

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the geometry type value list.  Elements must be integers
    /// as defined by MgGeometryType.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// SetTypes(INT32 * types, int count);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// SetTypes(INT32 * types, int count);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// SetTypes(INT32 * types, int count);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param types (INT32 *)
    /// The list of types.  There must be no more than MG_MAX_GEOMETRY_TYPE_SIZE elements.
    ///
    /// \param count (int)
    /// The number of types in the list.  The maximum allowed value
    /// is MG_MAX_GEOMETRY_TYPE_SIZE.
    ///
    void SetTypes(INT32 * types, INT32 count);  /// __get, __set

INTERNAL_API:

    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);
    void Initialize();
    void ToXml(string& xmlStr, bool includeType);

    virtual void Dispose()
    {
        delete this;
    }

private:

    INT32           m_types[MG_MAX_GEOMETRY_TYPE_SIZE];  // See MgGeometryType
    INT32           m_count;
    STRING          m_serializedXml;

INTERNAL_API:

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_GeometryTypeInfo;
};
/// \}

#endif
