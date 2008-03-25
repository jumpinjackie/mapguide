//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _MG_SPATIAL_CONTEXT_READER_H
#define _MG_SPATIAL_CONTEXT_READER_H

class MgSpatialContextData;

/// \defgroup MgSpatialContextReader MgSpatialContextReader
/// \ingroup Feature_Service_classes
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Provides forward-only, read-only functionality for
/// enumerating spatial context information retrieved from a
/// feature source.
///
/// \remarks
/// You must call MgSpatialContextReader::ReadNext before you can
/// access any data.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// \code
/// <?php
/// function printSpatialContextReader($spatialContextReader)
/// {
///    global $agfReaderWriter;
///    global $logFileHandle;
///    while ($spatialContextReader->ReadNext())
///    {
///       $name = $spatialContextReader->GetName();
///       if ($name == NULL)
///           $name = "null";
///
///       fwrite($logFileHandle, "Spatial Context Name: "$name"n");
///       $description = $spatialContextReader->GetDescription();
///       if ($description == NULL)
///           $description = "null";
///
///       fwrite($logFileHandle, "Description: "$description"n");
///       $coordSysName = $spatialContextReader->GetCoordinateSystem();
///       if ($coordSysName == NULL)
///           $coordSysName = "null";
///
///       fwrite($logFileHandle, "Coordinate System Name: "$coordSysName"n");
///       $coordSysWkt = $spatialContextReader->GetCoordinateSystemWkt();
///       if ($coordSysWkt == NULL)
///           $coordSysWkt = "null";
///
///       fwrite($logFileHandle, "Coordinate System WKT: "$coordSysWkt"n");
///       $extentType = $spatialContextReader->GetExtentType();
///       fwrite($logFileHandle, "Extent Type: " . printExtentType($extentType) . "n");
///       $extentByteReader = $spatialContextReader->GetExtent();
///       if ($extentByteReader == NULL)
///       {
///          fwrite($logFileHandle, "MgSpatialContextReader::GetExtent() returned a NULL objectn");
///       }
///       else
///       {
///          $extentGeometry = $agfReaderWriter->Read($extentByteReader);
///          printAGeometry($extentGeometry);
///       }
///       $XYTolerance = $spatialContextReader->GetXYTolerance();
///       fwrite($logFileHandle, "XY Tolerance: $XYTolerancen");
///       $ZTolerance = $spatialContextReader->GetZTolerance();
///       fwrite($logFileHandle, "Z Tolerance: $ZTolerancen");
///       $isActive = $spatialContextReader->IsActive();
///       fwrite($logFileHandle, "Is Active: " . prtBool($isActive) . "n");
///    }
/// }
/// ?>
/// \endcode
///
/// \htmlinclude ExampleBottom.html
/// \note
/// See the \link FdoSpatialContextList_schema FdoSpatialContextList \endlink XML
/// schema for the specification of the contents of the
/// information about a spatial context.
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
/// <p>
/// The <c>DescSpatialContexts</c> method formats the contents of the
/// <c>MgSpatialContextReader</c> as a string for testing purposes.
/// The <c>CoordSysWktToTypeAndName</c> method extracts the coordinate system name
/// and type from the well-known text specification of the coordinate system.
/// The <c>MgByteReaderToWktText</c> method converts the geometry representing
/// the extent of the spatial context from a binary to a textual format.
/// The <c>MgSpatialContextExtentTypeToStr</c> method converts the extent type
/// from an enumeration constant to a textual format.
/// </p>
/// \code
/// using OSGeo.MapGuide;
/// private void DescSpatialContexts(MgSpatialContextReader reader)
/// {
///     String CoordSys;
///     String CoordSysType;
///     String CoordSysName;
///     String OgcSrsWkt;
///     String spatialContextInfo;
///     String spatialContextName;
///     MgByteReader byteReader;
///     String Extent;
///     testSpatialContexts = new ListDictionary();
///     coordSysNameToWkt = new ListDictionary();
///     while (reader.ReadNext())
///     {
///         CoordSys = reader.GetCoordinateSystem();
///         OgcSrsWkt = reader.GetCoordinateSystemWkt();
///         if (CoordSys == null)
///         {
///             CoordSys = "null";
///         }
///         else if (CoordSys == "")
///         {
///             CoordSys = "emptyString";
///         }
///         else if (CoordSys == OgcSrsWkt)
///         {
///             CoordSys = "duplicate of GetCoordinateSystemWkt()";
///         }
///         CoordSysWktToTypeAndName(OgcSrsWkt, out CoordSysType, out CoordSysName);
///         coordSysNameToWkt.Add(CoordSysName, OgcSrsWkt);
///         // byteReader contains FGF binary
///         byteReader = reader.GetExtent();
///         if (byteReader.GetLength() == 0)
///         {
///             Extent = "is empty";
///         }
///         else
///         {
///             Extent = MgByteReaderToWktText(byteReader);
///         }
///         spatialContextName = reader.GetName();
///         spatialContextInfo = "SpatialContextName=" + spatialContextName + ';' +
///             "GetCoordinateSystem()=" + CoordSys + ';' +
///             "CoordSysType=" + CoordSysType + ';' +
///             "CoordSysName=" + CoordSysName + ';' +
///             "GetCoordinateSystemWkt()=" + OgcSrsWkt + ';' +
///             "ExtentType=" + MgSpatialContextExtentTypeToStr(reader.GetExtentType()) + ';' +
///             "Extent=" + Extent + ';' +
///             "XYTolerance=" + reader.GetXYTolerance() + ';' +
///             "ZTolerance=" + reader.GetZTolerance() + ';' +
///             "Active=" + reader.IsActive() + ';';
///         testSpatialContexts.Add(spatialContextName, spatialContextInfo);
///     }
/// }
///
/// private String MgByteReaderToWktText(MgByteReader byteReader)
/// {
///     String wktText = null;
///     MgGeometry geometry = agfReaderWriter.Read(byteReader);
///     wktText = wktReaderWriter.Write(geometry);
///     return wktText;
/// }
///
/// private void CoordSysWktToTypeAndName(String coordSysWkt,
///     out String coordSysType, out String coordSysName)
/// {
///     String interim;
///     String pattern = @"^([A-Z_]+)\[([^\,]+)";
///     Regex r = new Regex(pattern);
///     Match m = r.Match(coordSysWkt);
///     GroupCollection gc = m.Groups;
///     coordSysType = gc[1].Value;
///     interim = gc[2].Value;
///     coordSysName = interim.Trim('"');
/// }
///
/// String MgSpatialContextExtentTypeToStr(Int32 extentType)
/// {
///     switch (extentType)
///     {
///         case 0: return "Static";
///         case 1: return "Dynamic";
///         default: return "InvalidMgSpatialContextExtentType: " + extentType;
///     }
/// }
///
/// private MgAgfReaderWriter agfReaderWriter;
/// private MgWktReaderWriter wktReaderWriter;
/// private MgSpatialContextReader spatialContextReader;
/// private MgFeatureService featureService;
/// private ListDictionary testSpatialContexts;
/// private ListDictionary coordSysNameToWkt;
///
/// agfReaderWriter = new MgAgfReaderWriter();
/// wktReaderWriter = new MgWktReaderWriter();
/// // see the MgFeatureService sample code
/// spatialContextReader = featureService.GetSpatialContexts(resourceId, false);
/// DescSpatialContexts(spatialContextReader);
/// \endcode
/// \htmlinclude ExampleBottom.html
///
///
class MG_PLATFORMBASE_API  MgSpatialContextReader : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgSpatialContextReader)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the spatial context currently being read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the name of the spatial context.
    ///
    STRING GetName();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the description of the spatial context currently being read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetDescription();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the description of the spatial context.
    ///
    STRING GetDescription();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the coordinate system of the spatial context
    /// currently being read. If the result is a null string, the
    /// coordinate system is unknown to the spatial context.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetCoordinateSystem();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetCoordinateSystem();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetCoordinateSystem();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system name or NULL if unknown.
    ///
    STRING GetCoordinateSystem();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the coordinate system currently being read
    /// in well-known text format.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetCoordinateSystemWkt();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetCoordinateSystemWkt();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetCoordinateSystemWkt();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the coordinate system name in OGC WKT format or NULL
    /// if unknown.
    ///
    STRING GetCoordinateSystemWkt();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the extent type of the spatial context currently being
    /// read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetExtentType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetExtentType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetExtentType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the extent type as an MgSpatialContextExtentType
    /// value.
    ///
    INT32 GetExtentType();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the \link extent extent \endlink of the spatial context
    /// currently being read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader GetExtent();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader GetExtent();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader GetExtent();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgByteReader object containing a geometry in AGF
    /// binary format. See \link MgAgfReaderWriter MgAgfReaderWriter Class \endlink
    /// for instructions on how to access the geometry.
    ///
    /// \note
    /// The XML output for the extent is expressed in terms of a
    /// lower left coordinate and an upper right coordinate. See \link FdoSpatialContextList_schema FdoSpatialContextList \endlink.
    ///
    ///
    MgByteReader* GetExtent();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the \link tolerance tolerance \endlink value used for both X and Y
    /// ordinates for the spatial context currently being read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// double GetXYTolerance();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// double GetXYTolerance();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// double GetXYTolerance();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the tolerance value.
    ///
    double GetXYTolerance();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the tolerance value for Z ordinates for the spatial
    /// context currently being read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// double GetZTolerance();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// double GetZTolerance();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// double GetZTolerance();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the tolerance value.
    ///
    double GetZTolerance();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether the spatial context currently being read
    /// is the active spatial context.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsActive();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsActive();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsActive();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the current spatial context is the active
    /// one; otherwise returns false.
    ///
    bool IsActive();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Advances the reader to the next spatial context.
    ///
    /// \remarks
    /// The initial position of the reader is prior to the first
    /// spatial context. Therefore, you must call ReadNext before you
    /// can access any data.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool ReadNext();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean ReadNext();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool ReadNext();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if there is a next item; otherwise returns
    /// false.
    ///
    ///
    bool ReadNext();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Closes the SpatialContextReader object, freeing any resources
    /// it may be holding.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Close();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Close();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Close();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    ///
    void Close();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Resets the SpatialContextReader object to the beginning just
    //  as if it had been newly created.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Reset();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Reset();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Reset();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    ///
    void Reset();

EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes the spatial context information to XML according
    /// to the \link FdoSpatialContextList_schema FdoSpatialContextList \endlink schema.
    ///
    /// \return
    /// Returns an MgByteReader object containing the XML
    /// serialization of the spatial context information.
    ///
    MgByteReader* ToXml();

INTERNAL_API:

    MgSpatialContextReader();
    ~MgSpatialContextReader();
    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);
    INT32 AddSpatialData(MgSpatialContextData* data);
    void SetProviderName(CREFSTRING providerName);

protected:

    void ToXml(string& str);

    void Dispose()
    {
        delete this;
    }

private:
    MgDisposableCollection m_spatialContextCol;
    INT32 m_currPos;
    STRING m_providerName;

INTERNAL_API:

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_SpatialContextReader;
};
/// \}

#endif
