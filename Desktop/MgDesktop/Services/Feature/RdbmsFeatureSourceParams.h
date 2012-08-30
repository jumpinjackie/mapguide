#ifndef DESKTOP_RDBMS_FEATURE_SOURCE_PARAMS_H
#define DESKTOP_RDBMS_FEATURE_SOURCE_PARAMS_H

#include "MgDesktop.h"

// Parameter class for creating RDBMS data stores
//
//
/// \ingroup Desktop_Misc_Module
/// \{

///\brief
/// Contains the parameters used to create a relational feature source. This is the relational
/// analogue to the MgFileFeatureSourceParams class
///
/// \remarks
/// You may use MgFeatureService::DescribeSchema and
/// MgFeatureService::GetSpatialContexts to get the values needed, or create the
/// MgFeatureSchema from scratch to construct and configure the MgdRdbmsFeatureSourceParams 
/// object.\n\n
/// Only relational FDO providers that support the FdoICreateDataStore command
/// can be used with this class to create Feature Sources. As of this writing, the
/// known supported file-based FDO providers are:
///  \li OSGeo.SQLServerSpatial
///  \li OSGeo.MySQL
///  \li OSGeo.PostgreSQL
///
/// You cannot create file Feature Sources using this class. Use the MgFileFeatureSourceParams
/// class if you need to create file-based Feature Sources.
class MG_DESKTOP_API MgdRdbmsFeatureSourceParams : public MgFeatureSourceParams
{
PUBLISHED_API:
    //////////////////////////////////////////////////
    /// \brief
    /// Constructs an empty MgdRdbmsFeatureSourceParams object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  MgdRdbmsFeatureSourceParams();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  MgdRdbmsFeatureSourceParams();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  MgdRdbmsFeatureSourceParams();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgdRdbmsFeatureSourceParams();

    ///////////////////////////////////////////////
    /// \brief
    /// Gets the FDO provider name of the feature source.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  string GetProviderName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  String GetProviderName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  string GetProviderName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the FDO provider name of the feature source.
    ///
    STRING GetProviderName();   /// __get, __set

    ///////////////////////////////////////////////
    /// \brief
    /// Sets the FDO provider name of the feature source.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetProviderName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetProviderName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetProviderName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param providerName (String/string)
    /// The FDO provider name of the feature source
    /// without version number. For example, OSGeo.SDF.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetProviderName(CREFSTRING providerName);

    ///////////////////////////////////////////////
    /// \brief
    /// Gets the service name to create the relational data store in
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  string GetService();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  String GetService();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  string GetService();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the service name
    ///
    STRING GetService();   /// __get, __set

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the service to create the relational data store in
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetService(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetService(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetService(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The service name
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetService(CREFSTRING name);

    ///////////////////////////////////////////////
    /// \brief
    /// Gets the password connection parameter
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  string GetPassword();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  String GetPassword();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  string GetPassword();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the password connection parameter
    ///
    STRING GetPassword();   /// __get, __set

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the password connection parameter
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetPassword(String pwd);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetPassword(String pwd);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetPassword(string pwd);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param pwd (String/string)
    /// The password connection parameter
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetPassword(CREFSTRING pwd);

    ///////////////////////////////////////////////
    /// \brief
    /// Gets the username connection parameter
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  string GetUsername();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  String GetUsername();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  string GetUsername();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the username connection parameter
    ///
    STRING GetUsername();   /// __get, __set

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the username connection parameter
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetUsername(String username);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetUsername(String username);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetUsername(string username);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param username (String/string)
    /// The username connection parameter
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetUsername(CREFSTRING username);

    ///////////////////////////////////////////////
    /// \brief
    /// Gets the name of the data store
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  string GetDataStore();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  String GetDataStore();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  string GetDataStore();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the name of the data store
    ///
    STRING GetDataStore();  /// __get, __set

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the data store to create
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetDataStore(String dataStore);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetDataStore(String dataStore);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetDataStore(string dataStore);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param dataStore (String/string)
    /// The name of the data store to create
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetDataStore(CREFSTRING dataStore);

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

    //////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the coordinate system
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  String GetCoordinateSystemName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  String GetCoordinateSystemName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  String GetCoordinateSystemName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the name of the coordinate system
    ///
    STRING GetCoordinateSystemName();  /// __get, __set

    //////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the coordinate system
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetCoordinateSystemName(String csName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetCoordinateSystemName(String csName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetCoordinateSystemName(string csName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing
    ///
    void SetCoordinateSystemName(CREFSTRING csName);

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

    ////////////////////////////////////////////////////////
    /// \brief
    /// Gets the envelope that all features in this data store will encompass
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  MgEnvelope GetSpatialContextExtents();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  MgEnvelope GetSpatialContextExtents();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  MgEnvelope GetSpatialContextExtents();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The data store extents
    ///
    MgEnvelope* GetSpatialContextExtents();  /// __get, __set

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the envelope that all features in this data store will encompass
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetSpatialContextExtents(MgEnvelope extents);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetSpatialContextExtents(MgEnvelope extents);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetSpatialContextExtents(MgEnvelope extents);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param extents (MgEnvelope)
    /// The data store extents
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetSpatialContextExtents(MgEnvelope* extents);

    ////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this data store will be created with FDO metaschema
    /// tables. These tables have a f_ prefix when the data store is 
    /// inspected by non-FDO tools.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  bool GetIsFdoEnabled();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  bool GetIsFdoEnabled();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  bool GetIsFdoEnabled();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// The data store extents
    ///
    bool GetIsFdoEnabled();  /// __get, __set

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets whether the data store will be created with FDO metaschema
    /// tables. These tables have a f_ prefix when the data store is 
    /// inspected by non-FDO tools.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetIsFdoEnabled(bool useFdoMetadata);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetIsFdoEnabled(boolean useFdoMetadata);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetIsFdoEnabled(bool useFdoMetadata);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param useFdoMetadata (bool)
    /// true to create FDO metaschema tables. false otherwise
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetIsFdoEnabled(bool useFdoMetadata);

protected:
    virtual void Dispose();

    virtual ~MgdRdbmsFeatureSourceParams();

private:
    STRING m_provider;

    STRING m_service;
    STRING m_username;
    STRING m_password;
    STRING m_datastore;

    STRING m_scName;
    STRING m_scDescription;
    STRING m_scCsName;
    STRING m_scCsWkt;
    double m_xyTolerance;
    double m_zTolerance;
    bool m_useFdoMetadata;

    Ptr<MgFeatureSchema> m_schema;
    Ptr<MgEnvelope> m_extents;

INTERNAL_API:
    virtual void Serialize(MgStream* stream);
    virtual void Deserialize(MgStream* stream);
};
/// \}
#endif