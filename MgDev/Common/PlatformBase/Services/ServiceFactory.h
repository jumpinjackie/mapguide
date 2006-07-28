//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef MGSERVICEFACTORY_H_
#define MGSERVICEFACTORY_H_

#include "PlatformBase.h"

#define DECLARE_CREATE_SERVICE() \
public: \
    static MgService* CreateService();

#define IMPLEMENT_CREATE_SERVICE(class_name) \
    MgService* class_name::CreateService() \
        { return new class_name(); }

/// \cond INTERNAL
/// Typedef for class creation functions
typedef MgService* (*ServiceCreatorFunc)();
/// \endcond INTERNAL


/// INTERNAL_CLASS
/// \cond INTERNAL
/// \brief
/// Service registration for Foundation derived applications
class MG_PLATFORMBASE_API MgServiceFactory : public MgDisposable
{
    DECLARE_CLASSNAME(MgServiceFactory)

INTERNAL_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Object constructor
    ///
    MgServiceFactory();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns global object instance
    ///
    /// \return
    /// Service factory for application
    ///
    static MgServiceFactory* GetInstance();

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
    static Ptr<MgServiceFactory> m_serviceFactory;

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Declaration of Service Registry
    typedef map<INT16, ServiceCreatorFunc> ServiceRegistry;

    map<ServerConnectionType, ServiceRegistry> m_serviceCreators;

};

/// \endcond

#endif
