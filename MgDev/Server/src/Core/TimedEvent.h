//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef MGTIMEDEVENT_H_
#define MGTIMEDEVENT_H_

class MgTimedEvent
{
/// Enumerations

public:

    enum Identifier
    {
        ServiceRegistration                 = 10,
        SessionTimeout                      = 20,
        ConnectionTimeout                   = 30,
        DataConnectionTimeout               = 40,
        RepositoryCheckpoint                = 50,
        ResourceChange                      = 60,
        FeatureServiceCacheTimeLimit        = 70,
        DataTransactionTimeout              = 80,
    };

/// Constructors/Destructor

public:

    MgTimedEvent();
    virtual ~MgTimedEvent();

/// Methods

public:

    const long& GetId() const;
    void SetId(long id);

    INT32 GetTimeout() const;
    void SetTimeout(INT32 timeout);

/// Data Members

private:

    long m_id;
    INT32 m_timeout;
};

/// Inline Methods

inline const long& MgTimedEvent::GetId() const
{
    return m_id;
}

inline INT32 MgTimedEvent::GetTimeout() const
{
    return m_timeout;
}

#endif
