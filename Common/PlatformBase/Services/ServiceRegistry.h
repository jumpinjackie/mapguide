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

#ifndef MGSERVICEREGISTRY_H_
#define MGSERVICEREGISTRY_H_

#include "PlatformBase.h"

#ifdef _WIN32
#undef CreateService
#endif

/// \cond INTERNAL

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumeration for the server connection type.
/// INTERNAL API ONLY.  DO NOT DOCUMENT.
///
enum ServerConnectionType
{
    sctNone = 0,                        /// No connection type defined
    sctLocalInProc,                     /// No proxy class needed
    sctRemoteServerToServer,            /// Proxy implementation used
    sctServerToHttp                     /// Http requests are made
};
/// \endcond

#define DECLARE_CREATE_SERVICE() \
public: \
    static MgService* CreateService();

#define IMPLEMENT_CREATE_SERVICE(class_name) \
    MgService* class_name::CreateService() \
        { return new class_name(); }

/// Typedef for class creation functions
typedef MgService* (*ServiceCreatorFunc)();


/// \cond INTERNAL

/// INTERNAL_CLASS
/// \brief
/// Service registration for PlatformBase derived applications
class MG_PLATFORMBASE_API MgServiceRegistry : public MgDisposable
{
    DECLARE_CLASSNAME(MgServiceRegistry)

INTERNAL_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Object constructor
    ///
    MgServiceRegistry();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns global object instance
    ///
    /// \return
    /// Service factory for application
    ///
    static MgServiceRegistry* GetInstance();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Registers a Service creator with the factory
    ///
    void RegisterService(INT16 serviceType, ServiceCreatorFunc creator, ServerConnectionType connType = sctLocalInProc);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates Service
    ///
    /// \return
    /// Newly created Service of specified type
    ///
    MgService* CreateService(INT16 serviceType, ServerConnectionType connType = sctLocalInProc);

protected:

    virtual void Dispose();

private:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Internal singleton variable
    static Ptr<MgServiceRegistry> m_serviceRegistry;

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Declaration of Service Registry
    typedef map<INT16, ServiceCreatorFunc> ServiceRegistry;

    map<ServerConnectionType, ServiceRegistry> m_serviceCreators;

};
/// \endcond

#endif
