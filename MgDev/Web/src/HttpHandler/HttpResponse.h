//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef _HTTP_RESPONSE_H
#define _HTTP_RESPONSE_H

/// <summary>
/// Purpose of this class is to package response header variables,
/// response data and status code to return to clients.
/// </summary>
class MG_MAPAGENT_API MgHttpResponse : public MgGuardDisposable
{
    EXTERNAL_API:
        MgHttpResponse();

        /// <summary>
        /// Makes available the pointer to header.
        /// User will use header class instance to retrieve
        /// header information for a response.
        /// </summary>
        /// <returns>Pointer to MgHttpHeader class</returns>
        MgHttpHeader* GetHeader();

        /// <summary>
        /// Makes available the pointer to MgHttpResult.
        /// User will retrieve the actual contents
        /// from this result instance.
        /// </summary>
        /// <returns>Pointer to MgHttpResult or NULL</returns>
        MgHttpResult* GetResult();

    INTERNAL_API:
        /// <summary>
        /// Destructor to clean up internal objects
        /// </summary>
        virtual ~MgHttpResponse();

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


    private:
        Ptr<MgHttpHeader> m_header;
        Ptr<MgHttpResult> m_result;
CLASS_ID:
        static const INT32 m_cls_id = HttpHandler_MapAgent_HttpResponse;
};

#endif
