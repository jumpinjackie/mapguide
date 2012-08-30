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

#ifndef MG_LOG_ENTRY_DATA_H_MG
#define MG_LOG_ENTRY_DATA_H_MG

///////////////////////////////////////////////////////////////////////////
//  The LogEntryData class encapsulates the data needed by the log thread.
class MgdLogEntryData : public ACE_Data_Block
{
    DECLARE_CLASSNAME(MgdLogEntryData)

///////////////////////////////////////////////////////////////////////
///  Constructors/Destructors
public:
    MgdLogEntryData( enum MgLogType logType, CREFSTRING message, ACE_Log_Priority logPriority );
    virtual ~MgdLogEntryData();

///////////////////////////////////////////////////////////////////////
///  Accessors
public:
    enum MgLogType m_logType;
    STRING m_message;
    ACE_Log_Priority m_logPriority;

///////////////////////////////////////////////////////////////////////
///  Member Variables
protected:
};

#endif  //  MG_LOG_ENTRY_DATA_H_MG
