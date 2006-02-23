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

#ifndef MGPACKAGESTATUSLOGMANAGER_H
#define MGPACKAGESTATUSLOGMANAGER_H

class MgPackageStatusLogManager : public MgDisposable
{
    DECLARE_CLASSNAME(MgPackageStatusLogManager)

public:
    MgPackageStatusLogManager(CREFSTRING status, CREFSTRING packageName, CREFSTRING date, INT32 size);
    virtual ~MgPackageStatusLogManager();

    // MgDisposable method
    virtual void Dispose();

    void SetStatus(CREFSTRING status);
    void SetError(CREFSTRING error);
    void SetErrorDetails(CREFSTRING details);
    void AddDetailsEntry(CREFSTRING entry);

    // Writes information to the log
    void UpdateLog();

private:
    // Default constructor
    MgPackageStatusLogManager();

    STRING m_logName;
    STRING m_packageStatus;
    STRING m_packageName;
    STRING m_packageDate;
    INT32 m_packageSize;
    STRING m_packageError;
    STRING m_packageErrorDetails;

    Ptr<MgStringCollection> m_entries;

    ACE_Recursive_Thread_Mutex m_mutex;

    STRING GetTimeStamp();
    void WriteLineToFile(CREFSTRING entry, FILE* file);
};

#endif
