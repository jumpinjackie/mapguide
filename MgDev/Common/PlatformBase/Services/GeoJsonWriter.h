//
//  Copyright (C) 2004-2017 by Autodesk, Inc.
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

#ifndef _MGGEOJSONWRITER_H
#define _MGGEOJSONWRITER_H

/// \defgroup MgGeoJsonWriter MgGeoJsonWriter
/// \ingroup Feature_Service_classes
/// \{

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// A helper class to output GeoJSON from feature readers
///
class MG_PLATFORMBASE_API MgGeoJsonWriter : public MgGuardDisposable
{
PUBLISHED_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a MgGeoJsonWriter object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgGeoJsonWriter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgGeoJsonWriter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgGeoJsonWriter();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgGeoJsonWriter();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the current feature in the given feature reader to GeoJSON
    ///
    /// \remarks
    /// The id and geometry properties are inferred from the class definition returned by the feature reader
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string FeatureToGeoJson(MgFeatureReader featureReader, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String FeatureToGeoJson(MgFeatureReader featureReader, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string FeatureToGeoJson(MgFeatureReader featureReader, MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param featureReader (MgFeatureReader)
    /// The feature reader
    ///
    /// \param transform (MgTransform)
    /// An optional transform
    ///
    /// \return
    /// Returns the GeoJSON output as a string.
    ///
    STRING FeatureToGeoJson(MgFeatureReader* featureReader, MgTransform* transform);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the current feature in the given feature reader to GeoJSON
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string FeatureToGeoJson(MgFeatureReader featureReader, MgTransform transform, string idPropertyName, string geomPropName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String FeatureToGeoJson(MgFeatureReader featureReader, MgTransform transform, String idPropertyName, String geomPropName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string FeatureToGeoJson(MgFeatureReader featureReader, MgTransform transform, string idPropertyName, string geomPropName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param featureReader (MgFeatureReader)
    /// The feature reader
    ///
    /// \param transform (MgTransform)
    /// An optional transform
    ///
    /// \param idPropertyName (String/string)
    /// The name of the id property. The value of the property specified will be written to the top-level "id" property of the GeoJSON
    ///
    /// \param geomPropName (String/string)
    /// The name of the geometry property. The value of the property specified will be written to the top-level "geometry" property
    /// of the GeoJSON. If this property is not specified, no geometry is written.
    ///
    /// \return
    /// Returns the GeoJSON output as a string.
    ///
    STRING FeatureToGeoJson(MgReader* reader, MgTransform* transform, CREFSTRING idPropertyName, CREFSTRING geomPropName);

protected:
    virtual void Dispose();

private:
    static STRING EscapeJsonString(CREFSTRING str);
    void ValueToString(MgReader* reader, INT32 index, REFSTRING str);
    void ToGeoJson(MgGeometry* geom, REFSTRING str, bool bIncludePrefix = true);
    void CoordinateToGeoJson(MgCoordinate* coord, REFSTRING str);
    void CoordinatesToGeoJson(MgCoordinateIterator* coords, REFSTRING str);
    void PolygonToGeoJson(MgPolygon* poly, REFSTRING str);
    void MultiPointToGeoJson(MgMultiPoint* multi, REFSTRING str);
    void MultiLineStringToGeoJson(MgMultiLineString* multi, REFSTRING str);
    void MultiPolygonToGeoJson(MgMultiPolygon* multi, REFSTRING str);

    Ptr<MgAgfReaderWriter> m_agfRw;
};
/// \}

#endif