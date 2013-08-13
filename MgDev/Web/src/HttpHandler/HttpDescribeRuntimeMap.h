//
//  Copyright (C) 2004-2013 by Autodesk, Inc.
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

#ifndef _HTTP_DESCRIBE_RUNTIME_MAP_
#define _HTTP_DESCRIBE_RUNTIME_MAP_

typedef std::map<STRING, MdfModel::LayerDefinition*> LayerDefinitionMap;

class MgHttpDescribeRuntimeMap : public MgHttpRequestResponseHandler
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
    MgHttpDescribeRuntimeMap(MgHttpRequest *hRequest);

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
    MgRequestClassification GetRequestClassification() { return MgHttpRequestResponseHandler::mrcViewer; }

    /// <summary>
    /// This method is responsible for checking if
    /// a valid version was given
    /// </summary>
    /// <returns>Returns nothing</returns>
    virtual void ValidateOperationVersion();

private:
    STRING m_mapName;
    STRING m_iconFormat;
    INT32 m_iconWidth;
    INT32 m_iconHeight;
    INT32 m_requestDataMask;
    INT32 m_iconLimitPerScaleRange;
};

#endif  // _HTTP_CREATE_RUNTIME_MAP_
