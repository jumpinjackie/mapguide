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

#ifndef MGSITECONNECTIONBASE_H_
#define MGSITECONNECTIONBASE_H_

#ifdef WIN32
#undef CreateService
#endif

/// \cond INTERNAL
//////////////////////////////////////////////////////////////////
/// \brief
/// Site connection base class.  Provides Service access for application
/// capabilities for the site.  This class should be extended for each
/// application environment to provide an API accessible MgSiteConnection
///
///
class MG_PLATFORMBASE_API MgServiceFactory  : public MgDisposable
{
    DECLARE_CLASSNAME(MgServiceFactory)

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a service factory.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgServiceFactory();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgServiceFactory();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgServiceFactory();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgServiceFactory();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a new instance of the specified service.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgService CreateService(int serviceType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgService CreateService(int serviceType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgService CreateService(int serviceType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param serviceType (int)
    /// Type of service to create.  See MgServiceType for valid service names.
    ///
    /// \return
    /// Returns the created service
    ///
    /// \exception MgServiceNotSupportedException if service type is not supported
    ///
    ///
    virtual MgService* CreateService(INT32 serviceType);

INTERNAL_API:

    virtual ~MgServiceFactory();

};
/// \}

#endif  //  MGSITECONNECTION_H_
