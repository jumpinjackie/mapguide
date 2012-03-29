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
#include "Command.h"

#include "ProfilingDefs.h"

static const int Profiling_Service = (int)MgPacketParser::msiProfiling;

IMPLEMENT_CREATE_SERVICE(MgProxyProfilingService)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgProxyProfilingService object
/// </summary>
MgProxyProfilingService::MgProxyProfilingService() : MgProfilingService()
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Dispose this object
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgProxyProfilingService::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the warnings if any
/// </summary>
void MgProxyProfilingService::SetWarning(MgWarnings* warning)
{
    if (warning)
    {
        Ptr<MgStringCollection> ptrCol = warning->GetMessages();
        this->m_warning->AddMessages(ptrCol);
        warning->Release();
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Profile the process of rendering all dynamic layers in the specified 
/// MgMap to a dynamic overlay image with a transparent background.
/// </summary>
/// <param name="map">Input
/// map object containing current state of map.
/// </param>
/// <param name="selection">Input
/// map feature selection. Specifies the selected features on the map
/// </param>
/// <param name="options">Input
/// rendering options
/// </param>
/// <returns>
/// A byte reader containing the profile result
/// </returns>
MgByteReader* MgProxyProfilingService::ProfileRenderDynamicOverlay(
    MgMap* map,
    MgSelection* selection,
    MgRenderingOptions* options)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                              // Connection
                        MgCommand::knObject,                                    // Return type expected
                        MgProfilingServiceOpId::ProfileRenderDynamicOverlay,    // Command Code
                        3,                                                      // No of arguments
                        Profiling_Service,                                      // Service Id
                        BUILD_VERSION(2,4,0),                                   // Operation version
                        MgCommand::knObject, map,                               // Argument#1
                        MgCommand::knObject, selection,                         // Argument#2
                        MgCommand::knObject, options,                           // Argument#3
                        MgCommand::knNone);                                     // End of arguments

    SetWarning(cmd.GetWarningObject());

    return (MgByteReader*)cmd.GetReturnValue().val.m_obj;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Profile the process of rendering a MgMap to the requested image 
/// format with specified center point and scale.
/// </summary>
/// <param name="map">Input
/// map object containing current state of map.
/// </param>
/// <param name="selection">Input
/// map feature selection. Specifies the selected features on the map
/// </param>
/// <param name="center">Input
/// map center point. Specifies the center point for the map
/// </param>
/// <param name="scale">Input
/// map scale. Specifies the scale for the map
/// </param>
/// <param name="width">Input
/// image width. Specifies the image width in pixels
/// </param>
/// <param name="height">Input
/// image height. Specifies the image height in pixels
/// </param>
/// <param name="backgroundColor">Input
/// background color. Specifies the map background color
/// </param>
/// <param name="format">Input
/// image format. Defines the format of the resulting image
/// </param>
/// <param name="bKeepSelection">Input
/// true if you want to keep the selection
/// </param>
/// <returns>
/// A byte reader containing the profile result
/// </returns>
MgByteReader* MgProxyProfilingService::ProfileRenderMap(
    MgMap* map,
    MgSelection* selection,
    MgCoordinate* center,
    double scale,
    INT32 width,
    INT32 height,
    MgColor* backgroundColor,
    CREFSTRING format,
    bool bKeepSelection)
{
    MgCommand cmd;
    cmd.ExecuteCommand(m_connProp,                                      // Connection
                        MgCommand::knObject,                            // Return type expected
                        MgProfilingServiceOpId::ProfileRenderMap,       // Command Code
                        9,                                              // No of arguments
                        Profiling_Service,                              // Service Id
                        BUILD_VERSION(2,4,0),                           // Operation version
                        MgCommand::knObject, map,                       // Argument#1
                        MgCommand::knObject, selection,                 // Argument#2
                        MgCommand::knObject, center,                    // Argument#3
                        MgCommand::knDouble, scale,                     // Argument#4
                        MgCommand::knInt32, width,                      // Argument#5
                        MgCommand::knInt32, height,                     // Argument#6
                        MgCommand::knObject, backgroundColor,           // Argument#7
                        MgCommand::knString, &format,                   // Argument#8
                        MgCommand::knInt8, (INT8)bKeepSelection,        // Argument#9
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
void MgProxyProfilingService::SetConnectionProperties(MgConnectionProperties* connProp)
{
    m_connProp = SAFE_ADDREF(connProp);
}
