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

#ifndef _MGMULTILINESTRING_H_
#define _MGMULTILINESTRING_H_

class MgMultiLineString;
template class MG_GEOMETRY_API Ptr<MgMultiLineString>;

/// \defgroup MgMultiLineString MgMultiLineString
/// \ingroup Geometry_Module_classes
/// \{

///////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// An MgMultiLineString is a one-dimensional aggregate geometry
/// whose elements are MgLineString geometries.
///
/// \remarks
/// The line strings
/// are not connected or ordered. An instance of this class is
/// constructed by calling a non-static
/// MgGeometryFactory::CreateMultiLineString() method and, once
/// constructed, is immutable.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// The following code shows the construction of an
/// MgMultiLineString geometry. Review the MgLineString example code.
/// \code
/// // A helper class additional to those created in the
/// // MgLineString example code is needed.
/// $lineStringCollection = new MgLineStringCollection();
///
/// // After each MgLineString geometry is constructed,
/// // it is added to an MgLineStringCollection.
/// $index = $lineStringCollection->Add($lineString);
/// echo "A line string is added to a line string collection at index: $index\n";
///
/// // construct the MgMultiLineString geometry
/// $multiLineString = $geometryFactory->CreateMultiLineString($lineStringCollection);
///
/// // print out the Agf Text string for the geometry
/// $multiLineStringAgfText = $wktReaderWriter->Write($multiLineString);
/// echo "AGF Text representation of MultiLineString: $multiLineStringAgfText\n";
/// \endcode
/// \htmlinclude ExampleBottom.html
///
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
/// The CreateAMultiLineStringXY method calls the CreateALineStringXY method.
/// The code for the CreateALineStringXY method is in the MgLineString example code.
///
/// \code
/// using OSGeo.MapGuide;
///
/// private MgWktReaderWriter wktReaderWriter;
/// private MgGeometryFactory geometryFactory;
/// private MgMultiLineString mlsLs0121Ls1121;
/// // the data for 1 linestring
/// private double[,] da0121 = { { 0, 1 }, { 2, 1 } };
/// // the data for another linestring
/// private double[,] da1121 = { { 1, 1 }, { 2, 1 } };
/// // the data for 2 linestrings
/// private double[][,] mlsLs0121Ls1121Data;
/// private String geometryAgfText;
///
/// public MgMultiLineString CreateAMultiLineStringXY(double[][,] lineStrings)
/// {
///     MgLineStringCollection lines = new MgLineStringCollection();
///     for (int i = 0; i < lineStrings.GetLength(0); i++)
///     {
///         lines.Add(CreateALineStringXY(lineStrings[i]));
///     }
///     return geometryFactory.CreateMultiLineString(lines);
/// }
///
/// geometryFactory = new MgGeometryFactory();
/// mlsLs0121Ls1121Data = new double[2][,];
/// mlsLs0121Ls1121Data[0] = da0121;
/// mlsLs0121Ls1121Data[1] = da1121;
/// mlsLs0121Ls1121 = CreateAMultiLineStringXY(mlsLs0121Ls1121Data);
///
/// // print out the Agf Text string for the geometry
/// wktReaderWriter = new MgWktReaderWriter();
/// geometryAgfText = wktReaderWriter.Write(mlsLs0121Ls1121);
/// // geometryAgfText now contains:
/// // "MULTILINESTRING XY ((0 1, 2 1), (1 1, 2 1))"
/// \endcode
/// \htmlinclude ExampleBottom.html
///
class MG_GEOMETRY_API MgMultiLineString : public MgAggregateGeometry
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMultiLineString)

PUBLISHED_API:
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetCount();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual INT32 GetCount();  /// __get, __inherited

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the line string at the specified index. The indexing
    /// starts at zero.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgLineString GetLineString(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgLineString GetLineString(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgLineString GetLineString(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// An integer specifying the location of the line
    /// string in the collection.
    ///
    /// \return
    /// The MgLineString at the specified index.
    ///
    virtual MgLineString* GetLineString(INT32 index);

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetGeometryType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetGeometryType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetGeometryType();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual INT32 GetGeometryType();  /// __get, __inherited

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetDimension();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetDimension();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetDimension();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual INT32 GetDimension();  /// __get, __inherited

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsEmpty();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean IsEmpty();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsEmpty();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual bool IsEmpty();

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsClosed();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean IsClosed();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsClosed();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual bool IsClosed();

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometricEntity Copy();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometricEntity Copy();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometricEntity Copy();
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual MgGeometricEntity* Copy();

    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometricEntity Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometricEntity Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometricEntity Transform(MgTransform transform);
    /// \htmlinclude SyntaxBottom.html
    ///
    virtual MgGeometricEntity* Transform(MgTransform* transform);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgMultiLineString object
    ///
    MgMultiLineString(MgLineStringCollection* lineStrings);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an empty MgMultiLineString object for deserialization
    ///
    MgMultiLineString();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the geometry at the specified position
    ///
    virtual MgGeometry* GetGeometryAt(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Convert to AWKT representation
    ///
    virtual void ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Convert to AWKT representation
    ///
    virtual STRING ToAwkt(bool is2dOnly);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an iterator over the coordinates included in this
    /// geometric entity.
    ///
    /// \return
    /// The coordinates of this geometric entity
    ///
    virtual MgCoordinateIterator* GetCoordinates();

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Calculate the envelope for this geometry
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual MgEnvelope* ComputeEnvelope();

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_MultiLineString;

private:
    Ptr<MgLineStringCollection>   m_lineStrings;
};
/// \}

#endif //_MGMULTILINESTRING_H_
