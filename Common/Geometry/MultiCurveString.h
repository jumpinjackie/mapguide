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

#ifndef _MGMULTICURVESTRING_H_
#define _MGMULTICURVESTRING_H_

class MgMultiCurveString;
template class MG_GEOMETRY_API Ptr<MgMultiCurveString>;

/// \defgroup MgMultiCurveString MgMultiCurveString
/// \ingroup Geometry_Module_classes
/// \{

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// An MgMultiCurveString is a one-dimensional aggregate geometry
/// whose elements are MgCurveString geometries.
///
/// \remarks
/// The curve
/// strings are not connected or ordered. An instance of this
/// class is constructed by calling a non-static
/// MgGeometryFactory::CreateMultiCurveString() method and, once
/// constructed, is immutable.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// The following code shows the construction of an
/// MgMultiCurveString geometry. Review the MgCurveString example code.
///
/// \code
/// // A helper class additional to those created in the
/// // MgCurveString example code is needed.
/// $curveStringCollection = new MgCurveStringCollection();
///
/// // After each MgCurveString geometry is constructed,
/// // it is added to an MgCurveStringCollection.
/// $index = $curveStringCollection->Add($curveString);
/// echo "A curve string is added to a curve string collection at index: $index\n";
///
/// // construct the MgMultiCurveString geometry
/// $multiCurveString = $geometryFactory->CreateMultiCurveString($curveStringCollection);
///
/// // print out the Agf Text string for the geometry
/// $multiCurveStringAgfText = $wktReaderWriter->Write($multiCurveString);
/// echo "AGF Text representation of MultiCurveString: $multiCurveStringAgfText\n";
/// \endcode
/// \htmlinclude ExampleBottom.html
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
/// The CreateAMultiCurveStringXY method calls the CreateACurveStringXY method.
/// The code for the CreateACurveStringXY method is in the MgCurveString example code.
///
/// \code
/// using OSGeo.MapGuide;
///
/// private MgWktReaderWriter wktReaderWriter;
/// private MgMultiCurveString mcsCsAs001120Ls203132CsAs001120Ls203132;
/// // the following arrays are used for segments of a curve string
/// // the first coordinate in each array is actually a flag
/// // 0 indicates an arc segment and 1 indicates a linear segment
/// private double[,] as001120 = { { 0, 0 }, { 0, 0 }, { 1, 1 }, { 2, 0 } };
/// private double[,] ls203132 = { { 1, 1 }, { 2, 0 }, { 3, 1 }, { 3, 2 } };
/// private double[][,] csAs001120Ls203132Data;
/// // the data for 2 curvestrings
/// private double[][][,] mcsCsAs001120Ls203132CsAs001120Ls203132Data;
/// private String geometryAgfText;
///
/// public MgMultiCurveString CreateAMultiCurveStringXY(double[][][,] curveStrings)
/// {
///     MgCurveStringCollection curves = new MgCurveStringCollection();
///     for (int i = 0; i < curveStrings.GetLength(0); i++)
///     {
///         curves.Add(CreateACurveStringXY(curveStrings[i]));
///     }
///     return geometryFactory.CreateMultiCurveString(curves);
/// }
///
/// csAs001120Ls203132Data = new double[2][,];
/// csAs001120Ls203132Data[0] = as001120;
/// csAs001120Ls203132Data[1] = ls203132;
/// mcsCsAs001120Ls203132CsAs001120Ls203132Data = new double[2][][,];
/// mcsCsAs001120Ls203132CsAs001120Ls203132Data[0] = csAs001120Ls203132Data;
/// mcsCsAs001120Ls203132CsAs001120Ls203132Data[1] = csAs001120Ls203132Data;
/// mcsCsAs001120Ls203132CsAs001120Ls203132 =
///     CreateAMultiCurveStringXY(mcsCsAs001120Ls203132CsAs001120Ls203132Data);
///
/// // print out the Agf Text string for the geometry
/// wktReaderWriter = new MgWktReaderWriter();
/// geometryAgfText = wktReaderWriter.Write(mcsCsAs001120Ls203132CsAs001120Ls203132);
/// // geometryAgfText now contains the AGF representation of the multi-curve string
/// \endcode
/// \htmlinclude ExampleBottom.html
///
class MG_GEOMETRY_API MgMultiCurveString : public MgAggregateGeometry
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMultiCurveString)

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

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the curve string at the specified index. The indexing
    /// starts at zero.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgCurveString GetCurveString(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgCurveString GetCurveString(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgCurveString GetCurveString(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// An integer specifying the location of the curve
    /// string in the collection.
    ///
    /// \return
    /// The MgCurveString at the specified index.
    ///
    virtual MgCurveString* GetCurveString(INT32 index);

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

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a transformed copy of this geometric entity.
    ///
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
    /// \param transform (MgTransform)
    /// The MgTransform to use in constructing a copy of this geometric entity.
    ///
    /// \return
    /// An MgGeometricEntity that is a copy of this one with each coordinate
    /// transformed.
    ///
    virtual MgGeometricEntity* Transform(MgTransform* transform);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgMultiCurveString object
    ///
    MgMultiCurveString(MgCurveStringCollection* curveStrings);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an empty MgMultiCurveString object for deserialization
    ///
    MgMultiCurveString();

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
    /// MgEnvelope.
    ///
    virtual MgEnvelope* ComputeEnvelope();

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_MultiCurveString;

private:
    Ptr<MgCurveStringCollection>   m_curveStrings;
};
/// \}

#endif //_MGMULTICURVESTRING_H_
