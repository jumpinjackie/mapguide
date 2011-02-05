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
#include "MapGuideCommon.h"

#ifndef MG_SITEINFO_H_
#define MG_SITEINFO_H_

class MG_MAPGUIDE_API MgSiteInfo : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgSiteInfo)

EXTERNAL_API:

    enum MgSiteStatus
    {
        Ok,
        UnableToConnect,
        TimedOut,
        Uninitialized
    };

    enum MgPortType
    {
        Client,
        Site,
        Admin
    };


/// Constructors/Destructor

    MgSiteInfo();
    MgSiteInfo(CREFSTRING target, INT32 sitePort, INT32 clientPort, INT32 adminPort);
    virtual ~MgSiteInfo();

/// Methods

    STRING GetTarget();
    INT32 GetPort(MgPortType type);
    MgSiteStatus GetStatus();
    void SetStatus(MgSiteStatus);

INTERNAL_API:

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

    // Create a new MgSiteInfo instance, and initialize it from a hex string
    MgSiteInfo(CREFSTRING hexString);

    // Retrieve a hex string that encodes the site info connection params
    STRING ToHexString();

    static const INT32 HexStringLength = 20;

protected:

    INT32 GetClassId();
    virtual void Dispose();

/// Data Members

private:

    STRING m_target;
    INT32 m_clientPort;
    INT32 m_sitePort;
    INT32 m_adminPort;
    MgSiteStatus m_status;

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Service_SiteInfo;

};

#endif // MG_SITEINFO_H_
