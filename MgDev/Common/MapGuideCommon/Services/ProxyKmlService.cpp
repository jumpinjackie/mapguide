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

#include "MapGuideCommon.h"
#include "Command.h"

#include "KmlDefs.h"

static const int Kml_Service = (int)MgPacketParser::msiKml;

IMPLEMENT_CREATE_SERVICE(MgProxyKmlService)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgProxyKmlService object
/// </summary>
MgProxyKmlService::MgProxyKmlService() : MgKmlService()
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Dispose this object
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgProxyKmlService::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the warnings if any
/// </summary>
void MgProxyKmlService::SetWarning(MgWarnings* warning)
{
    if (warning)
    {
        Ptr<MgStringCollection> ptrCol = warning->GetMessages();
        this->m_warning->AddMessages(ptrCol);
        warning->Release();
    }
}

// Retrieve a map definition in KML format
MgByteReader* MgProxyKmlService::GetMapKml(
    MgMap* map,
    double dpi,
    CREFSTRING agentUri,
    CREFSTRING format)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knObject,                            // Return type expected
                        MgKmlServiceOpId::GetMapKml,                    // Command Code
                        4,                                              // No of arguments
                        Kml_Service,                                    // Service Id
                        BUILD_VERSION(1,0,0),                           // Operation version
                        MgCommand::knObject, map,                       // Argument#1
                        MgCommand::knDouble, dpi,                       // Argument#2
                        MgCommand::knString, &agentUri,                 // Argument#3
                        MgCommand::knString, &format,                   // Argument#4
                        MgCommand::knNone);                             // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

// Retrieve a layer definition in KML format
MgByteReader* MgProxyKmlService::GetLayerKml(
    MgLayer* layer,
    MgEnvelope* extents,
    INT32 width,
    INT32 height,
    double dpi,
    INT32 drawOrder,
    CREFSTRING agentUri,
    CREFSTRING format)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knObject,                            // Return type expected
                        MgKmlServiceOpId::GetLayerKml,                  // Command Code
                        8,                                              // No of arguments
                        Kml_Service,                                    // Service Id
                        BUILD_VERSION(1,0,0),                           // Operation version
                        MgCommand::knObject, layer,                     // Argument#1
                        MgCommand::knObject, extents,                   // Argument#2
                        MgCommand::knInt32, width,                      // Argument#3
                        MgCommand::knInt32, height,                     // Argument#4
                        MgCommand::knDouble, dpi,                       // Argument#5
                        MgCommand::knInt32, drawOrder,                  // Argument#6
                        MgCommand::knString, &agentUri,                 // Argument#7
                        MgCommand::knString, &format,                   // Argument#8
                        MgCommand::knNone);                             // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

// Retrieve feature geometries in KML format
MgByteReader* MgProxyKmlService::GetFeaturesKml(
    MgLayer* layer,
    MgEnvelope* extents,
    INT32 width,
    INT32 height,
    double dpi,
    INT32 drawOrder,
    CREFSTRING format)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knObject,                            // Return type expected
                        MgKmlServiceOpId::GetFeaturesKml,               // Command Code
                        7,                                              // No of arguments
                        Kml_Service,                                    // Service Id
                        BUILD_VERSION(1,0,0),                           // Operation version
                        MgCommand::knObject, layer,                     // Argument#1
                        MgCommand::knObject, extents,                   // Argument#2
                        MgCommand::knInt32, width,                      // Argument#3
                        MgCommand::knInt32, height,                     // Argument#4
                        MgCommand::knDouble, dpi,                       // Argument#5
                        MgCommand::knInt32, drawOrder,                  // Argument#6
                        MgCommand::knString, &format,                   // Argument#7
                        MgCommand::knNone);                             // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Sets the connection properties for the Proxy Service.  This
/// information tells the proxy object where to connect.
///
/// \param connProp
/// Connection properties for server
///
void MgProxyKmlService::SetConnectionProperties(MgConnectionProperties* connProp)
{
    m_connProp = SAFE_ADDREF(connProp);
}
