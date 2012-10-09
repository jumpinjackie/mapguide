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

#ifndef MgdLogDetail_H_
#define MgdLogDetail_H_

#include "MgDesktop.h"
#include "LogManager.h"

//
// MgdLogDetail maintains detailed information for both the Trace Log and the Error Log.
// It also handles writing of the trace log based on information contained in serverconfig.ini
//
// [GeneralProperties]
// LogsDetail = ResourceService:0,FeatureService:1
//
// LogsDetail determines the information logged for each service.  See LogDetail enum below
// for more information.
//
// Example:
// LogsDetail = ResourceService:0,FeatureService:1
// Only error messages without parameters are logged for Resource Service, and
// Error messages and warnings with parameters are logged for Feature Service
//
class MG_DESKTOP_API MgdLogDetail
{
    DECLARE_CLASSNAME(MgdLogDetail)

public:
/// Enumerations

    // Errors (exceptions) are logged without method parameters for the specified service
    // Warnings are not logged for this service
    // Traces are not logged for this service
    // Note:  This is the default level of detail.
    static const INT8 Error = 0;

    // Errors (exceptions) are logged with method parameters for the specified service
    // Warnings are logged with parameters for this service
    // Traces are not logged for this service
    static const INT8 Warning = 1;

    // All Warning detail, plus
    // Traces with parameters are logged for published Service API calls
    static const INT8 Trace = 2;

    // All Trace detail, plus
    // Traces with parameters are logged for internal API calls
    static const INT8 InternalTrace = 3;


/// Constructors/Destructor

public:

    // Defines a log entry for the specified service and detail level.  This entry will only be emitted
    // into trace log if the serverconfig.ini LogsDetail >= detail
    MgdLogDetail(INT32 serviceType, INT8 detail, CREFSTRING methodName, REFSTRING errorLogVar);
    virtual ~MgdLogDetail();

/// Methods

public:

    void AddResourceIdentifier(CREFSTRING paramName, MgResourceIdentifier* resId);
    void AddInt64(CREFSTRING paramName, INT64 paramValue);
    void AddInt32(CREFSTRING paramName, INT32 paramValue);
    void AddBool(CREFSTRING paramName, bool paramValue);
    void AddString(CREFSTRING paramName, CREFSTRING paramValue);
    void AddObject(CREFSTRING paramName, MgSerializable* object);
    void Create();

private:

    void Terminate();
    void AppendName(CREFSTRING paramName);
    bool ParamsActive();
    bool ShouldLog();


/// Data Members

private:

    bool m_bLoggingActive; // Are we writing to trace log?
    INT8 m_detail; // detail level
    INT8 m_minDetail; // minimum detail level from config file
    STRING m_params; // parameter string
    STRING m_methodName; // method name
    REFSTRING m_errorLogVar; // error log parameters variable
    STRING m_delimiter; // log delimiter
};

#endif
