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

#ifndef MG_SERVER_CONNECTION_H_
#define MG_SERVER_CONNECTION_H_

/// \cond INTERNAL

#ifdef _WIN32
#undef CreateService
#endif

class MgServerConnectionImp;
class MgUserInformation;
class MgStreamHelper;
class MG_MAPGUIDE_API MgServerConnectionStack;
class ACE_Time_Value;

//////////////////////////////////////////////////////////////////
/// \brief
/// Represents a connection to a MapGuide server
/// INTERNAL_ONLY:
class MG_MAPGUIDE_API MgServerConnection : public MgServerConnectionBase
{
    DECLARE_CLASSNAME(MgServerConnection)

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Return whether or not the connection is open
    ///
    /// \return
    /// true if the connection is open
    ///
    bool IsOpen();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determine whether or not the connection is stale.
    ///
    /// \param referenceTime
    /// Optional time used in reference to last used time for the connection.
    ///
    /// \return
    /// true if the connection is stale, false otherwise.
    ///
    bool IsStale(ACE_Time_Value* referenceTime = NULL);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Disconnects from the MapGuide server.  Close can be called multiple
    /// times without generating an exception.
    ///
    /// \return
    /// None
    ///
    void Close();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default Constructor. Needed for Linux build
    ///
    MgServerConnection();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor. This will clean up the connection.
    ///
    ~MgServerConnection();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Establishes a TCP/IP connection to a MapGuide server.  Uses an internal
    /// connection pool to service the request.  Connections created using
    /// this scheme are automatically returned to the pool when release by the caller
    ///
    /// \param userInformation
    /// User information to authenticate against
    /// \param connProp
    /// Connection properties for target server
    ///
    /// \return
    /// Opened map server connection
    ///
    /// \exception MgConnectionNotOpenException
    ///
    static MgServerConnection* Acquire(MgUserInformation* userInformation, MgConnectionProperties* connProp);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the TCP/IP connection to a MapGuide server from the
    /// internal connection pool.
    ///
    /// \param connProp
    /// Connection properties for target server
    ///
    /// \return
    /// Nothing
    ///
    static void Remove(MgConnectionProperties* connProp);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Internal method used for getting stream helper
    /// which will then be used in serialize/deserialize
    ///
    /// \return
    /// MgStreamHelper which has method to write or read data
    /// over the wire
    ///
    MgStream* GetStream();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Internal method used for getting connection properties
    ///
    /// \return
    /// MgConnectionProperties for connection
    /// over the wire
    ///
    MgConnectionProperties* GetConnectionProperties();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Set the owning stack for this connection.  Connection returns to
    /// the stack once the refcount hits one on release
    ///
    /// \param stack
    /// Owning stack for connection
    ///
    void SetStack(MgServerConnectionStack* stack);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns last used time for the connection
    ///
    /// \return
    /// Last used time
    ///
    ACE_Time_Value* LastUsed();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Overloaded Release for stack management
    ///
    INT32 Release();

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
     INT32 GetClassId();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Self destructor
    ///
    /// \return
    /// Nothing
    ///
    virtual void Dispose();

private:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Establishes a connection to a MapGuide server
    ///
    /// \param userInformation
    /// User information to authenticate against
    /// \param connProp
    /// Connection properties for target server
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgAuthorizationFailed
    /// \exception MgTargetNotFound
    /// \exception MgConnectionCanNotBeEstablished
    ///
    void Open(MgUserInformation* userInformation, MgConnectionProperties* connProp);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Internal method used for getting stream helper
    /// which will then be used in serialize/deserialize
    ///
    /// \return
    /// MgStreamHelper which has method to write or read data
    /// over the wire
    ///
    MgStreamHelper* GetMgStreamHelper();

    static const INT32 m_cls_id = MapGuide_Service_ServerConnection;
    static const time_t sm_kStaleTime;
    static ACE_Recursive_Thread_Mutex sm_mutex;

    bool m_isOpen;

    Ptr<MgStream> m_stream;
    Ptr<MgConnectionProperties> m_connProp;
    ACE_Time_Value* m_lastUsed;
    MgServerConnectionStack* m_stack;

    MgServerConnectionImp* m_serverConnImp;
};
/// \endcond

#endif


