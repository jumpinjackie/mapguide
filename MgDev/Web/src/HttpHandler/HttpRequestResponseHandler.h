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

#ifndef REQUEST_RESPONSE_HANDLER_H
#define REQUEST_RESPONSE_HANDLER_H

class MgHttpRequestResponseHandler : public MgDisposable
{
public:

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Type of Request/Response Handler
    /// </summary>
    enum MgRequestClassification
    {
        mrcViewer = 1,
        mrcAuthor,
        mrcWfs,
        mrcWms
    };

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructor.
    /// </summary>
    MgHttpRequestResponseHandler();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Destructor.
    /// </summary>
    virtual ~MgHttpRequestResponseHandler();


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
    void InitializeCommonParameters(MgHttpRequest *hRequest);

    /// <summary>
    /// Validates the common parameters of the request.
    /// </summary>
    /// <returns>
    /// nothing
    /// </returns>
    virtual void ValidateCommonParameters();

    /// <summary>
    /// Executes the specific request.
    /// </summary>
    /// <param name="hResponse">Input
    /// This contains the response (including MgHttpResult and StatusCode) from the server.
    /// </param>
    virtual void Execute(MgHttpResponse& hResponse) = 0;

    /// <summary>
    /// Validates the common parameters of the request.
    /// </summary>
    /// <param name="serviceType">Input
    /// This defines the service type to create.
    /// </param>
    /// <returns>
    /// Pointer to MgService based on serviceType.
    /// </returns>
    MgService* CreateService(INT16 serviceType);

    /// <summary>
    /// Returns the classification of this request/response handler
    /// </summary>
    /// <returns>
    /// Classification of handler
    /// </returns>
    virtual MgRequestClassification GetRequestClassification() { return MgHttpRequestResponseHandler::mrcAuthor; }

protected:
    /// <summary>
    /// This method is responsible for releasing all
    /// its member variables and then destruct itself.
    /// </summary>
    /// <returns>Returns nothing</returns>
    virtual void Dispose() { delete this; }

    /// <summary>
    /// This method is responsible for checking if
    /// a valid version was given
    /// </summary>
    /// <returns>Returns nothing</returns>
    virtual void ValidateOperationVersion();

    /// <summary>
    /// Converts the response to a different response format,
    /// if necessary
    /// </summary>
    /// <returns>Returns nothing</returns>
    virtual void ProcessFormatConversion(Ptr<MgByteReader> &byteReader);

    Ptr<MgHttpRequest> m_hRequest;
    STRING m_version;
    STRING m_responseFormat;
    Ptr<MgUserInformation> m_userInfo;
    Ptr<MgSiteConnection> m_siteConn;
};

#endif // REQUEST_RESPONSE_HANDLER_H
