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

#ifndef _MG_GEOMETRIC_PROPERTY_DEFINITION_H
#define _MG_GEOMETRIC_PROPERTY_DEFINITION_H

/// \defgroup MgGeometricPropertyDefinition MgGeometricPropertyDefinition
/// \ingroup Feature_Schema_Module
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// Defines a geometric property belonging to a feature class.
///
class MG_PLATFORMBASE_API MgGeometricPropertyDefinition : public MgPropertyDefinition
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgGeometricPropertyDefinition)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgGeometricPropertyDefinition object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgGeometricPropertyDefinition(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgGeometricPropertyDefinition(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgGeometricPropertyDefinition(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The name of the property.
    ///
    MgGeometricPropertyDefinition(CREFSTRING name);

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the MgFeatureGeometricType types which specify the
    /// categories of the geometries (point, line, surface, solid)
    /// that can be stored in this geometric property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetGeometryTypes();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetGeometryTypes();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetGeometryTypes();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the bit-wise OR combination of one or more of the
    /// MgFeatureGeometricType constants.
    ///
    INT32 GetGeometryTypes();  /// __get, __set

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the specific geometry types that can be stored in this geometric
    /// property. The returned value is a list of geometry types that are
    /// supported.  The caller does NOT own the array of types and should not free its memory.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgGeometryTypeInfo * GetSpecificGeometryTypes();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgGeometryTypeInfo * GetSpecificGeometryTypes();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgGeometryTypeInfo * GetSpecificGeometryTypes();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a list of geometry types that are supported.
    ///
    MgGeometryTypeInfo * GetSpecificGeometryTypes();  /// __get, __set

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a Boolean value that indicates whether this geometric
    /// property is read-only.
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
    /// Returns a Boolean value that indicates whether this
    /// geometric property is read-only.
    ///
    bool GetReadOnly();   /// __get, __set

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a Boolean value that indicates whether the points in the
    /// geometry contained in this property include elevation
    /// (Z-coordinate) values.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetHasElevation();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetHasElevation();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetHasElevation();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a Boolean value that indicates whether the points in
    /// the geometry contained in this property includes elevation
    /// (Z-coordinate) values
    ///
    bool GetHasElevation();   /// __get, __set

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a Boolean value that indicates whether the points in the
    /// geometry contained in this property includes measurement
    /// values (M-ordinates).
    ///
    /// \remarks
    /// An example of a measurement value is the number on a
    /// mile-marker associated with a railroad line. When defining
    /// the MgLineString object which represents the railroad line,
    /// you provide X and Y ordinates to record the geographical
    /// position of the mile marker, and the number on the mile
    /// marker is stored in the M ordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetHasMeasure();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetHasMeasure();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetHasMeasure();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the points in the geometry contained in this
    /// property include measurement values (M-ordinates); otherwise
    /// returns false.
    ///
    bool GetHasMeasure();   /// __get, __set

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Spatial Context name associated with this geometric
    /// property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetSpatialContextAssociation();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetSpatialContextAssociation();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetSpatialContextAssociation();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a String value representing the Spatial Context
    /// name. If the value is empty then the geometric property is
    /// associated with all the spatial contexts in the datastore.
    ///
    /// \see
    /// MgFeatureService::GetSpatialContexts.
    STRING GetSpatialContextAssociation();   /// __get, __set

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the categories of geometries (point, line, surface,
    /// solid) that can be stored in this geometric property.
    ///
    /// \remarks
    /// If the category is set to point, then MgPoint and
    /// MgMultiPoint geometries can be stored in this property. If
    /// the category is also set to line, then MgLineString,
    /// MgCurveString, MgMultiLineString, and MgMultiCurveString
    /// geometries can also be stored in this property. If the
    /// category is also set to surface, then MgPolygon,
    /// MgMultiPolygon, MgCurvePolygon, and MgMultiCurvePolygon
    /// geometries can also be stored in this property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetGeometryTypes(int types);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetGeometryTypes(int types);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetGeometryTypes(int types);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param types (int)
    /// One or more of the MgFeatureGeometricType
    /// constants combined by a bit-wise OR operation.
    ///
    void SetGeometryTypes(INT32 types);

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the specific list of geometry types that can be stored in this
    /// geometric property. The provided value is a list of geometry types
    /// that are supported.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// SetSpecificGeometryTypes(MgGeometryTypeInfo * typeInfo);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// SetSpecificGeometryTypes(MgGeometryTypeInfo * typeInfo);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// SetSpecificGeometryTypes(MgGeometryTypeInfo * typeInfo);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param typeInfo (MgGeometryTypeInfo)
    /// The specific set of geometry types that can be stored in this
    /// geometric property.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetSpecificGeometryTypes(MgGeometryTypeInfo * typeInfo);

    /////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether this geometric property is read-only.
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

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether the geometry contained in this property
    /// can include elevation values, that is, whether the
    /// coordinates in the geometry contain Z-ordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetHasElevation(bool hasElevation);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetHasElevation(boolean hasElevation);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetHasElevation(bool hasElevation);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param hasElevation (boolean/bool)
    /// True configures this property so that it can
    /// contain a geometry whose coordinates have elevation
    /// values (Z-ordinates), and false configures it so
    /// that the points in the geometry cannot take an
    /// elevation value.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetHasElevation(bool hasElevation);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether the points in the geometry contained in
    /// this property can include a measurement value (M-ordinate).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetHasMeasure(bool hasMeasure);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetHasMeasure(boolean hasMeasure);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetHasMeasure(bool hasMeasure);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param hasMeasure (boolean/bool)
    /// True configures the property to take
    /// geometries whose coordinates have an M-ordinate
    /// and false configures it so that M-ordinates
    /// are not permissible.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetHasMeasure(bool hasMeasure);

    //////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets a Spatial Context association for this geometric
    /// property.
    ///
    /// \remarks
    /// If not set, this name defaults to the Spatial Context name
    /// active in the datastore.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetSpatialContextAssociation(string spatialContextName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetSpatialContextAssociation(String spatialContextName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetSpatialContextAssociation(string spatialContextName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param spatialContextName (String/string)
    /// The Spatial Context name to
    /// be set.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetSpatialContextAssociation(CREFSTRING spatialContextName);

INTERNAL_API:

    MgGeometricPropertyDefinition();
    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);
    void Initialize();
    void ToXml(string& xmlStr, bool includeType);

    virtual void Dispose()
    {
        delete this;
    }

private:

    INT32       m_geometricTypes;
    Ptr<MgGeometryTypeInfo> m_geometryTypeInfo;
    bool        m_readOnly;
    bool        m_hasElevation;
    bool        m_hasMeasure;
    STRING      m_associatedSCName;
    STRING      m_serializedXml;

INTERNAL_API:

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_GeometricPropertyDefinition;
};
/// \}

#endif
