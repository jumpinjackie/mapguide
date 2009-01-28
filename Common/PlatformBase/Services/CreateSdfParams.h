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

#ifndef _MG_CREATE_SDF_PARAMS_H
#define _MG_CREATE_SDF_PARAMS_H

/// \defgroup MgCreateSdfParams MgCreateSdfParams
/// \ingroup Feature_Service_classes
/// \{

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Contains the parameters used to create an SDF feature source.
///
/// \remarks
/// Use MgFeatureService::DescribeSchema and
/// MgFeatureService::GetSpatialContexts to get the values needed
/// to construct and configure the MgCreateSdfParams object.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// \code
/// $schemaCollection = $featureService->DescribeSchema($activeFeatSrcResId, $schemaName, NULL);
/// $schema = $schemaCollection->GetItem(0);
/// $activeOnly = true;
/// $activeSpatialContextReader = $featureService->GetSpatialContexts($activeFeatSrcResId, $activeOnly);
/// $activeSpatialContextReader->ReadNext();
/// $spatialContextName = $activeSpatialContextReader->GetName();
/// $coordSysWkt = $activeSpatialContextReader->GetCoordinateSystemWkt();
/// $createSdfParams = new MgCreateSdfParams($spatialContextName, $coordSysWkt, $schema);
/// $createSdfParams->SetSpatialContextDescription($activeSpatialContextReader->GetDescription());
/// $createSdfParams->SetXYTolerance($activeSpatialContextReader->GetXYTolerance());
/// $createSdfParams->SetZTolerance($activeSpatialContextReader->GetZTolerance());
/// \endcode
/// \htmlinclude ExampleBottom.html
///
class MG_PLATFORMBASE_API MgCreateSdfParams : public MgFeatureSourceParams
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgCreateSdfParams)

PUBLISHED_API:
    //////////////////////////////////////////////////
    /// \brief
    /// Constructs an empty MgCreateSdfParams object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  MgCreateSdfParams();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  MgCreateSdfParams();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  MgCreateSdfParams();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgCreateSdfParams();

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgCreateSdfParams object with the supplied
    /// parameters.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  MgCreateSdfParams(string spatialContextName, string srsWkt, MgFeatureSchema featureSchema);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  MgCreateSdfParams(String spatialContextName, String srsWkt, MgFeatureSchema featureSchema);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  MgCreateSdfParams(string spatialContextName, string srsWkt, MgFeatureSchema featureSchema);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param spatialContextName (String/string)
    /// The name of the spatial context. This is
    /// a user defined spatial context for the
    /// SDF file. Only one spatial context is
    /// supported.
    /// \param srsWkt (String/string)
    /// An OGC WKT string representing the
    /// coordinate system for the SDF file.
    /// \param featureSchema (MgFeatureSchema)
    /// The schema definition for the SDF file.
    ///
    /// \return
    /// Returns nothing.
    ///
    MgCreateSdfParams(CREFSTRING spatialContextName, CREFSTRING srsWkt, MgFeatureSchema* featureSchema);

    ///////////////////////////////////////////////
    /// \brief
    /// Gets the user-defined spatial context name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  string GetSpatialContextName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  String GetSpatialContextName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  string GetSpatialContextName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the name of the spatial context.
    ///
    STRING GetSpatialContextName();  /// __get, __set

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the user-defined spatial context name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetSpatialContextName(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetSpatialContextName(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetSpatialContextName(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The name of the spatial context
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetSpatialContextName(CREFSTRING name);

    /////////////////////////////////////////////
    /// \brief
    /// Gets the spatial context description.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  string GetSpatialContextDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  String GetSpatialContextDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  string GetSpatialContextDescription();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the spatial context description.
    ///
    STRING GetSpatialContextDescription();  /// __get, __set

    //////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the spatial context description.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetSpatialContextDescription(string description);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetSpatialContextDescription(String description);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetSpatialContextDescription(string description);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param description (String/string)
    /// The description of the spatial
    /// context.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetSpatialContextDescription(CREFSTRING description);

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the well-known text description of the coordinate system.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  string GetCoordinateSystemWkt();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  String GetCoordinateSystemWkt();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  string GetCoordinateSystemWkt();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the OGC WKT description of the coordinate system.
    ///
    STRING GetCoordinateSystemWkt();  /// __get, __set

    //////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the well-know text string representing the coordinate system.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetCoordinateSystemWkt(string srsWkt);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetCoordinateSystemWkt(String srsWkt);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetCoordinateSystemWkt(string srsWkt);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param srsWkt (String/string)
    /// The OGC WKT string representing the coordinate system.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetCoordinateSystemWkt(CREFSTRING srsWkt);

    //////////////////////////////////////////////////////////
    /// \brief
    /// Gets the \link tolerance tolerance \endlink used for the X and Y
    /// ordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  double GetXYTolerance();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  double GetXYTolerance();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  double GetXYTolerance();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the tolerance used for the X and Y ordinates.
    ///
    double GetXYTolerance();  /// __get, __set

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the \link tolerance tolerance \endlink for the X and Y ordinates.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetXYTolerance(double tolerance);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetXYTolerance(double tolerance);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetXYTolerance(double tolerance);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param tolerance (double)
    /// The tolerance for the X and Y
    /// ordinates.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetXYTolerance(double tolerance);

    ////////////////////////////////////////////////////////
    /// \brief
    /// Gets the \link tolerance tolerance \endlink for the Z ordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  double GetZTolerance();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  double GetZTolerance();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  double GetZTolerance();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the tolerance for the Z ordinate.
    ///
    double GetZTolerance();  /// __get, __set

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the tolerance for the Z ordinate.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetZTolerance(double tolerance);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetZTolerance(double tolerance);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetZTolerance(double tolerance);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param tolerance (double)
    /// The tolerance for the Z ordinate.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetZTolerance(double tolerance);

    ////////////////////////////////////////////////
    /// \brief
    /// Gets the feature schema definition.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  MgFeatureSchema GetFeatureSchema();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  MgFeatureSchema GetFeatureSchema();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  MgFeatureSchema GetFeatureSchema();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a feature schema definition object.
    ///
    MgFeatureSchema* GetFeatureSchema();  /// __get, __set

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the feature schema definition.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetFeatureSchema(MgFeatureSchema featureSchema);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetFeatureSchema(MgFeatureSchema featureSchema);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetFeatureSchema(MgFeatureSchema featureSchema);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param featureSchema (MgFeatureSchema)
    /// The feature schema
    /// definition.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetFeatureSchema(MgFeatureSchema* featureSchema);

INTERNAL_API:

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

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Self destructing method.
    ///
    virtual void Dispose();

    virtual ~MgCreateSdfParams();

private:

    STRING m_spatialContextName;
    STRING m_spatialContextDescription;
    STRING m_srsWkt;
    double m_xyTolerance;
    double m_zTolerance;
    Ptr<MgFeatureSchema> m_featureSchema;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_CreateSdfParams;
};
/// \}

#endif
