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

#ifndef _FS_WMS_GET_MAP_H
#define _FS_WMS_GET_MAP_H

class MgHttpWmsGetMap : public MgHttpRequestResponseHandler, public IMgOgcDataAccessor
{
HTTP_DECLARE_CREATE_OBJECT()

public:
    /// <summary>
    /// Initializes the common parameters of the request.
    /// </summary>
    /// <param name="name">Input
    /// MgHttpRequest
    /// This contains all the parameters of the request.
    /// </param>
    /// <returns>
    /// nothing
    /// </returns>
    MgHttpWmsGetMap(MgHttpRequest *hRequest);

    /// <summary>
    /// Executes the specific request.
    /// </summary>
    /// <param name="hResponse">Input
    /// This contains the response (including MgHttpResult and StatusCode) from the server.
    /// </param>
    void Execute(MgHttpResponse& hResponse);

    /// <summary>
    /// Returns the classification of this request/response handler
    /// </summary>
    /// <returns>
    /// Classification of handler
    /// </returns>
    MgRequestClassification GetRequestClassification() { return MgHttpRequestResponseHandler::mrcWms; }

    // IMgOgcDataAccessor method
    virtual void AcquireValidationData(MgOgcServer* ogcServer);

private:

    MgEnvelope* GetExtents();
    MgColor* GetBackgroundColor();

    int CompareNoCase( CREFSTRING str1, CREFSTRING str2 ) const;

    STRING m_version;
    STRING m_layers;
    STRING m_styles;
    STRING m_crs;
    STRING m_bbox;
    INT32 m_width;
    INT32 m_height;
    STRING m_format;
    bool m_transparent;
    STRING m_bgColor;
};

#endif  // _FS_WMS_GET_MAP_H
