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

#ifndef MGSERVERINFORMATION_H_
#define MGSERVERINFORMATION_H_

class MgServerInformation;
template class MG_MAPGUIDE_API Ptr<MgServerInformation>;

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// Service types for Platform services.
class MG_MAPGUIDE_API MgServiceFlag
{
INTERNAL_API:
    ////////////////////////////////////////////////////////////////
    /// Resource Service
    static const INT32 ResourceService      = 0x00000001;


    ////////////////////////////////////////////////////////////////
    /// DWF Drawing Service
    static const INT32 DrawingService       = 0x00000002;


    ////////////////////////////////////////////////////////////////
    /// FDO Feature Service
    static const INT32 FeatureService       = 0x00000004;


    ////////////////////////////////////////////////////////////////
    /// Mapping Service
    static const INT32 MappingService       = 0x00000008;


    ////////////////////////////////////////////////////////////////
    /// Rendering Service
    static const INT32 RenderingService     = 0x00000010;


    ////////////////////////////////////////////////////////////////
    /// Tile Service
    static const INT32 TileService          = 0x00000020;

    ////////////////////////////////////////////////////////////////
    /// Kml Service
    static const INT32 KmlService           = 0x00000040;

    ////////////////////////////////////////////////////////////////
    /// Server Administration Service
    static const INT32 ServerAdminService   = 0x00000080;

    ////////////////////////////////////////////////////////////////
    /// Site Service
    static const INT32 SiteService          = 0x00000100;
};

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// This class manages server information.  Information includes the server
/// identifier, name, description, IP address, and available services.
class MG_MAPGUIDE_API MgServerInformation : public MgNamedSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgServerInformation)

EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an uninitialized object.
    ///
    MgServerInformation();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructs the object.
    ///
    virtual ~MgServerInformation();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the server.
    ///
    /// \return
    /// The server name.
    ///
    STRING GetName();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the server.
    ///
    /// \param name
    /// Name of the server.
    ///
    /// \return
    /// Nothing
    ///
    void SetName(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the description of the server.
    ///
    /// \return
    /// The server description.
    ///
    STRING GetDescription();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the description of the server.
    ///
    /// \param description
    /// Description of the server.
    ///
    /// \return
    /// Nothing
    ///
    void SetDescription(CREFSTRING description);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the IP address of the server.
    ///
    /// \return
    /// The server IP address.
    ///
    STRING GetAddress();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the IP address of the server.
    ///
    /// \param address
    /// IP address of the server.
    ///
    /// \return
    /// Nothing
    ///
    void SetAddress(CREFSTRING address);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Validates the server information.
    ///
    /// \return
    /// Nothing.
    ///
    void Validate();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs the object with the specified information.
    ///
    MgServerInformation(CREFSTRING identifier, CREFSTRING name,
        CREFSTRING description, CREFSTRING address);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Copy constructor.
    ///
    MgServerInformation(const MgServerInformation& serverInfo);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Assignment operator.
    ///
    MgServerInformation& operator=(const MgServerInformation& serverInfo);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Tests two objects for equality.
    ///
    bool operator==(const MgServerInformation &serverInfo) const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Tests two objects for inequality.
    ///
    bool operator!=(const MgServerInformation &serverInfo) const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Does it allow to set the name
    ///
    /// \return
    /// true - allows to set the name
    /// false - can not set the name
    ///
    bool CanSetName();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determine if this is a Site server.
    ///
    /// \return
    /// true if this is a Site server, false otherwise.
    ///
    bool IsSiteServer() const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the identifier of the server.
    ///
    /// \return
    /// The server identifier.
    ///
    STRING GetIdentifier();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the identifier of the server.
    ///
    /// \param identifier
    /// Identifier of the server.
    ///
    /// \return
    /// Nothing
    ///
    void SetIdentifier(CREFSTRING identifier);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether or not the server name is valid.
    ///
    /// \param name
    /// Name of the server.
    ///
    static void CheckName(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether or not the server description is valid.
    ///
    /// \param description
    /// Description of the server.
    ///
    static void CheckDescription(CREFSTRING description);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether or not the server IP address is valid.
    ///
    /// \param address
    /// IP address of the server.
    ///
    static void CheckAddress(CREFSTRING address);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified service name to a service type.
    ///
    /// \param serviceName
    /// Name of service.
    ///
    /// \return
    /// Type of service.
    ///
    static INT32 ToServiceType(CREFSTRING serviceName, bool strict = true);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified service type to a service name.
    ///
    /// \param serviceType
    /// Type of service.
    ///
    /// \return
    /// Name of service.
    ///
    static STRING ToServiceName(INT32 serviceType, bool strict = true);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified service type to a service flag.
    ///
    /// \param serviceType
    /// Type of service.
    ///
    /// \return
    /// Service flag.
    ///
    static INT32 ToServiceFlag(INT32 serviceType, bool strict = true);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified host properties to service flags.
    ///
    /// \param hostProps
    /// Host properties.
    /// \param initialFlags
    /// Initial service flags.
    ///
    /// \return
    /// Service flags.
    ///
    static INT32 ToServiceFlags(MgPropertyCollection* hostProps,
        INT32 initialFlags = 0);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the specified service is currently enabled.
    ///
    /// \param serviceType
    /// Type of service.
    ///
    /// \return
    /// true if the specified service is enabled, and false otherwise.
    ///
    bool IsServiceEnabled(INT32 serviceType) const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the bit-wise flags that specify which services should be enabled/disabled.
    ///
    /// \return
    /// The service flags.
    ///
    INT32 GetServiceFlags();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the service flags.
    ///
    /// \param serviceFlags
    /// Bit-wise flags that specify which services should be enabled/disabled.
    ///
    /// \return
    /// Nothing
    ///
    void SetServiceFlags(INT32 serviceFlags);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Copies the specified server information into this object.
    ///
    void CopyFrom(const MgServerInformation& serverInfo,
        bool sourceFromSiteServer);

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Disposes the object.
    ///
    virtual void Dispose();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the class ID.
    ///
    /// \return
    /// Class ID.
    ///
    INT32 GetClassId();

public:

    //////////////////////////////////////////////////////////////////
    /// NOTES: The numeric values of all the service types are also used by
    ///        sequential containers for fast indexed-based lookups.

    static const INT32 sm_knMaxNumberServices = 9;

private:

    struct MgServiceInfoMap
    {
        INT32 m_serviceType;
        STRING m_serviceName;
        INT32 m_serviceFlag; // Bit-wise service flag.
    };

    static const MgServiceInfoMap sm_serviceInfoMap[];

    STRING m_identifier;
    STRING m_name;
    STRING m_description;
    STRING m_address;
    INT32 m_serviceFlags;

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Service_ServerInformation;
};
/// \endcond

#endif
