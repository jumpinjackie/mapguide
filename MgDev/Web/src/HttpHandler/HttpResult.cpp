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

#include "HttpHandler.h"
#include "HttpResult.h"

#ifdef _WIN32
    #undef GetMessage
#endif

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Create a new MgHttpResult object.
/// </summary>
MgHttpResult::MgHttpResult()
{
    m_StatusCode = HTTP_STATUS_OK;
    m_pResultObject = NULL;
}


MgHttpResult::MgHttpResult(MgDisposable* resultObject)
{
    m_StatusCode = HTTP_STATUS_OK;
    m_pResultObject = resultObject;

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor. This will clean up the result.
/// </summary>
MgHttpResult::~MgHttpResult()
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the status code of the result.
/// <returns>
/// Success/Error code.
/// </returns>
/// </summary>
STATUS MgHttpResult::GetStatusCode()
{
    return m_StatusCode;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the status code of the result.
/// <returns>
/// Nothing.
/// </returns>
/// </summary>
void MgHttpResult::SetStatusCode(STATUS status)
{
    m_StatusCode = status;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the error message in the result.
/// <returns>
/// Error message or empty string if error message not set.
/// </returns>
/// </summary>
STRING MgHttpResult::GetErrorMessage()
{
    return m_ErrorMessage;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the detailed error message in the result.
/// </summary>
STRING MgHttpResult::GetDetailedErrorMessage()
{
    return m_DetailedMessage;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the HTTP error message in the result.
/// <returns>
/// Error message or empty string if error message not set.
/// </returns>
/// </summary>
STRING MgHttpResult::GetHttpStatusMessage()
{
    return m_HttpStatusMessage;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the error message in the result.
/// <returns>
/// Nothing.
/// </returns>
/// </summary>
void MgHttpResult::SetErrorMessage(CREFSTRING errorMsg)
{
    m_ErrorMessage = errorMsg;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the detailed error message in the result.
/// </summary>
void MgHttpResult::SetDetailedErrorMessage(CREFSTRING errorMsg)
{
    m_DetailedMessage = errorMsg;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the HTTP error message in the result.
/// <returns>
/// Nothing.
/// </returns>
/// </summary>
void MgHttpResult::SetHttpStatusMessage(CREFSTRING errorMsg)
{
    m_HttpStatusMessage = errorMsg;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the result object.
/// <returns>
/// Pointer to an object
/// </returns>
/// </summary>
MgDisposable* MgHttpResult::GetResultObject()
{
    return SAFE_ADDREF((MgDisposable*)m_pResultObject);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the result object.
/// </summary>
/// <param name="resultObject">
/// object returned by the API
/// </param>
/// <returns>
/// Nothing.
/// </returns>
void MgHttpResult::SetResultObject(MgDisposable* resultObject, CREFSTRING contentType)
{
    m_pResultObject = SAFE_ADDREF(resultObject);
    m_contentType = contentType;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the result content type.
/// <returns>
    /// Content type (e.g. "text/xml")
/// </returns>
/// </summary>
STRING MgHttpResult::GetResultContentType()
{
    return m_contentType;
}

void MgHttpResult::Dispose()
{
    delete this;
}

INT32 MgHttpResult::GetClassId()
{
    return m_cls_id;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the error information based on a Mg exception.
/// </summary>
///----------------------------------------------------------------------------

void MgHttpResult::SetErrorInfo(MgHttpRequest* awRequest,
    MgException* mgException)
{
#ifdef _DEBUG
    assert(0 != mgException);
#endif

    STATUS httpStatusCode = HTTP_STATUS_INTERNAL_SERVER_ERROR;
    STRING httpStatusMessage;
    STRING errorMessage;
    STRING detailedMessage;

    try
    {
        // Map Mg exception to HTTP result.

        httpStatusMessage = mgException->GetClassName();
        errorMessage = mgException->GetMessage();
        detailedMessage = mgException->GetDetails();

#ifdef _DEBUG
        detailedMessage += L"\n";
        detailedMessage += mgException->GetStackTrace();
#endif

        httpStatusCode = HTTP_STATUS_MG_ERROR;
    }
    catch (MgException* e)
    {
        SAFE_RELEASE(e);
    }
    catch (...)
    {
    }

    // Set the error information.

    try
    {
        if (HTTP_STATUS_INTERNAL_SERVER_ERROR == httpStatusCode)
        {
            // If we are here it is because we failed to get the exception information above
            httpStatusMessage = L"Invalid exception"; // TODO: Localize.
            errorMessage = L"Unable to process exception";          // TODO: Localize.
        }

        SetStatusCode(httpStatusCode);
        SetHttpStatusMessage(httpStatusMessage);
        SetErrorMessage(errorMessage);
        SetDetailedErrorMessage(detailedMessage);
    }
    catch (MgException* e)
    {
        SAFE_RELEASE(e);
    }
    catch (...)
    {
    }
}
