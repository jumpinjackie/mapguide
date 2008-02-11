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

#ifndef MGPACKAGELOGHANDLER_H_
#define MGPACKAGELOGHANDLER_H_

class MG_SERVER_MANAGER_API MgPackageLogEntry
{
INTERNAL_API:

    static const STRING ApiName;
    static const STRING StatusCode;

    static const STRING PackageName;
    static const STRING PackageDescription;
    static const STRING PackageDate;
    static const STRING PackageSize;

    static const STRING UserName;
    static const STRING ServerName;
    static const STRING ServerAddress;

    static const STRING StartTime;
    static const STRING EndTime;

    static const STRING OperationsFailed;
    static const STRING OperationsReceived;
    static const STRING AverageOperationTime;
    static const STRING ErrorMessage;
    static const STRING StackTrace;
    static const STRING OperationDetails;
};

class MG_SERVER_MANAGER_API MgPackageLogHandler : public MgDisposable
{
    DECLARE_CLASSNAME(MgPackageLogHandler)

/// Constructors/Destructor

public:

    explicit MgPackageLogHandler(CREFSTRING packagePathname);
    virtual ~MgPackageLogHandler();

private:

    // Unimplemented Constructors/Methods

    MgPackageLogHandler();
    MgPackageLogHandler(const MgPackageLogHandler&);
    MgPackageLogHandler& operator=(const MgPackageLogHandler&);

/// Methods

public:

    virtual void Dispose();

    CREFSTRING GetPackagePathname() const;
    CREFSTRING GetLogPathname() const;
    MgPackageStatusInformation& GetStatusInfo();

    void End();

protected:

    STRING GetCurrentTimestamp() const;
    void ReplaceReservedCharacters(REFSTRING str);

/// Data Members

protected:

    static const STRING sm_columnDelimiter;
    static const STRING sm_rowDelimiter;
    static const STRING sm_reservedCharacters;

    STRING m_packagePathname;
    STRING m_logPathname;
    MgPackageStatusInformation m_statusInfo;
};

/// Inline Methods

inline CREFSTRING MgPackageLogHandler::GetPackagePathname() const
{
    return m_packagePathname;
}

inline CREFSTRING MgPackageLogHandler::GetLogPathname() const
{
    return m_logPathname;
}

inline MgPackageStatusInformation& MgPackageLogHandler::GetStatusInfo()
{
    return m_statusInfo;
}

#endif
