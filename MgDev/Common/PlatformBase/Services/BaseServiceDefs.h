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

#ifndef MGBASESERVICEDEFS_H
#define MGBASESERVICEDEFS_H

/// \defgroup MgServiceType MgServiceType
/// \ingroup Common_Module
/// \{

//////////////////////////////////////////////////////////////////////////////
/// \brief
/// Service types for Platform services.
class MG_PLATFORMBASE_API MgServiceType
{
PUBLISHED_API:
    ////////////////////////////////////////////////////////////////
    /// Resource Service
    static const INT16 ResourceService      = 0;


    ////////////////////////////////////////////////////////////////
    /// DWF Drawing Service
    static const INT16 DrawingService       = 1;


    ////////////////////////////////////////////////////////////////
    /// FDO Feature Service
    static const INT16 FeatureService       = 2;


    ////////////////////////////////////////////////////////////////
    /// Mapping Service
    static const INT16 MappingService       = 3;


    ////////////////////////////////////////////////////////////////
    /// Rendering Service
    static const INT16 RenderingService     = 4;


    ////////////////////////////////////////////////////////////////
    /// Tile Service
    static const INT16 TileService          = 5;

    ////////////////////////////////////////////////////////////////
    /// Kml Service
    static const INT16 KmlService           = 6;

    ////////////////////////////////////////////////////////////////
    /// Profiling Service
    static const INT16 ProfilingService     = 10;

INTERNAL_API:
    /// NOTES: The numeric values of all the service types are also used by
    ///        sequential containers for fast indexed-based lookups.
    static const INT16 ServerAdminService   = 7;    /// Server Administration Service
    static const INT16 SiteService          = 8;    /// Site Service

    static const INT16 PrintLayoutService   = 9;    /// Print Layout Service
};
/// \}


///////////////////////////////////////////////////////////////////////////////
/// \cond INTERNAL
/// \brief
/// Operation IDs for Site Admin.
///
/// \remarks
/// INTERNAL - Do not document.
///
class MG_PLATFORMBASE_API MgSiteOpId
{
INTERNAL_API:
    static const int Authenticate                       = 0x1111EB01;
    static const int EnumerateUsers                     = 0x1111EB02;
    static const int AddUser                            = 0x1111EB03;
    static const int UpdateUser                         = 0x1111EB04;
    static const int DeleteUsers                        = 0x1111EB05;
    static const int GrantRoleMembershipsToUsers        = 0x1111EB06;
    static const int RevokeRoleMembershipsFromUsers     = 0x1111EB07;
    static const int GrantGroupMembershipsToUsers       = 0x1111EB08;
    static const int RevokeGroupMembershipsFromUsers    = 0x1111EB09;
    static const int EnumerateGroups                    = 0x1111EB0A;
    static const int AddGroup                           = 0x1111EB0B;
    static const int UpdateGroup                        = 0x1111EB0C;
    static const int DeleteGroups                       = 0x1111EB0D;
    static const int GrantRoleMembershipsToGroups       = 0x1111EB0E;
    static const int RevokeRoleMembershipsFromGroups    = 0x1111EB0F;
    static const int EnumerateRoles                     = 0x1111EB10;
    static const int EnumerateServers                   = 0x1111EB11;
    static const int AddServer                          = 0x1111EB12;
    static const int UpdateServer                       = 0x1111EB13;
    static const int RemoveServer                       = 0x1111EB14;
    static const int RequestServer                      = 0x1111EB15;
    static const int CreateSession                      = 0x1111EB16;
    static const int DestroySession                     = 0x1111EB17;
    static const int GetUserForSession                  = 0x1111EB18;
    static const int GetSessionTimeout                  = 0x1111EB19;
};
/// \endcond


///////////////////////////////////////////////////////////////////////////////
/// \cond INTERNAL
/// \brief
/// MapGuide users.
///
class MG_PLATFORMBASE_API MgUser
{
EXTERNAL_API:
    /// Site Administrator (Built-in account for administering the site)
    static const STRING Administrator;  /// value("Administrator")

    /// Anonymous User (Built-in account for guests with Viewer roles)
    static const STRING Anonymous;      /// value("Anonymous")

    /// Map Author (Built-in account for users with Author roles)
    static const STRING Author;         /// value("Author")

    /// WFS User (Built-in account for WFS users with Viewer roles)
    static const STRING WfsUser;        /// value("WfsUser")

    /// WMS User (Built-in account for WMS users with Viewer roles)
    static const STRING WmsUser;        /// value("WmsUser")
};
/// \endcond


///////////////////////////////////////////////////////////////////////////////
/// \cond INTERNAL
/// \brief
/// MapGuide groups.
///
class MG_PLATFORMBASE_API MgGroup
{
EXTERNAL_API:
    /// Everyone group (Built-in group to include all users)
    static const STRING Everyone;       /// value("Everyone")
};
/// \endcond


///////////////////////////////////////////////////////////////////////////////
/// \cond INTERNAL
/// \brief
/// MapGuide roles.
///
class MG_PLATFORMBASE_API MgRole
{
EXTERNAL_API:
    /// Administrator role (with read/write permission to resources by default)
    static const STRING Administrator;  /// value("Administrator")

    /// Author role (with read/write permission to resources by default)
    static const STRING Author;         /// value("Author")

    /// Viewer role (with read-only permission to resources by default)
    static const STRING Viewer;         /// value("Viewer")

INTERNAL_API:
    /// WMS/WFS user role (with read-only permission to resources by default)
    static const STRING Ogc;
};
/// \endcond

#endif
