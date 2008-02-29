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

#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H

/// <summary>
/// Purpose of this class is to store request header variables, request parameters
/// and request meta data for use in Common MapAgent API (CMA).
/// </summary>
class MG_MAPAGENT_API MgHttpRequest : public MgDisposable
{
    EXTERNAL_API:
        MgHttpRequest(CREFSTRING agentUri);

        /// <summary>
        /// Makes available the pointer to header class.
        /// User will use header class instance to add
        /// header information for a request.
        /// </summary>
        /// <returns>Pointer to MgHttpHeader class</returns>
        MgHttpHeader* GetHeader();

        /// <summary>
        /// Makes available the pointer to RequestParam class.
        /// User will use this class instance to add
        /// all parameters specified for a request.
        /// </summary>
        /// <returns>Pointer to MgHttpRequestParam class</returns>
        MgHttpRequestParam* GetRequestParam();

        /// <summary>
        /// Makes available the pointer to RequestMetadata class.
        /// User will use this class instance to add
        /// all required request metadata.
        /// </summary>
        /// <returns>Pointer to MgHttpRequestMetadata class</returns>
        MgHttpRequestMetadata* GetRequestMetadata();

        /// <summary>
        /// This method executes the request and returns the response
        /// object to the client.
        /// </summary>
        /// <returns>Pointer to MgHttpResponse object</returns>
        MgHttpResponse* Execute();

        /// <summary>
        /// Get the Uri of the agent which served this request
        /// </summary>
        STRING GetAgentUri();

    INTERNAL_API:
        MgHttpRequest();

        /// <summary>
        /// Destructor to clean up internal objects
        /// </summary>
        virtual ~MgHttpRequest();


    protected:
        /// <summary>
        /// This method is responsible for releasing all
        /// its member variables and then destruct itself.
        /// </summary>
        /// <returns>Returns nothing</returns>
        virtual void Dispose();

        /// <summary>
        /// Get the class Id
        /// </summary>
        /// <returns>
        /// The integer value
        /// </returns>
        INT32 GetClassId();

        /// <summary>
        /// This method returns a RequestResponseHandler based on the Operation
        /// name
        /// <param name="OperationName">Input
        /// Name of the operation to construct handler for
        /// </param>
        /// </summary>
        /// <returns>Pointer to MgHttpRequestResponseHandler object</returns>
        MgHttpRequestResponseHandler* CreateRequestResponseHandler(CREFSTRING OperationName, MgHttpResult* result);

    private:
        Ptr<MgHttpHeader> m_header;
        Ptr<MgHttpRequestMetadata> m_requestMetadata;
        Ptr<MgHttpRequestParam> m_requestParam;
        STRING m_agentUri;

CLASS_ID:
        static const INT32 m_cls_id = HttpHandler_MapAgent_HttpRequest;
};

#endif
