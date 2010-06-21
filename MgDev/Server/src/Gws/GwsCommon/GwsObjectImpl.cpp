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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "GwsCommonImp.h"


/////////////////////////////////////////////////////////////////////
//
// Static data
//
/////////////////////////////////////////////////////////////////////
static CGwsStatus s_status;


/////////////////////////////////////////////////////////////////////
//
// class CGwsStatus
//
/////////////////////////////////////////////////////////////////////
CGwsStatus::CGwsStatus ()
{
    m_status = eGwsOk;
}
CGwsStatus::CGwsStatus (EGwsStatus stat)
{
    m_status = stat;

}
CGwsStatus::CGwsStatus (EGwsStatus stat, const WSTR & mess)
{
    m_status = stat;
    m_message = mess;
}

// copy constructor
CGwsStatus::CGwsStatus (const CGwsStatus & other)
{
   m_status = other.m_status;
   m_message = other.m_message;
   m_parameters = other.m_parameters;
}

CGwsStatus::~CGwsStatus ()
{
}

EGwsStatus CGwsStatus::Status() const
{
    return m_status;
}

EGwsStatus CGwsStatus::GetStatus() const
{
    return m_status;
}

FdoString *CGwsStatus::GetErrorMessage () const
{
    if (m_message.empty ()) {
        CGwsResourceModule::GwsStatusMessage (m_status, ((CGwsStatus *)this)->m_message);
    }
    return m_message.c_str();
}

const GwsDiagParameters & CGwsStatus::Parameters () const
{
    return m_parameters;

}

void CGwsStatus::SetParameter (const WSTR & name, const WSTR & value)
{
    GwsDiagParameters::iterator iter;
    iter = m_parameters.find (name);
    if (iter == m_parameters.end ()) {
        m_parameters.insert (GwsDiagParameters::value_type (name, value));
    } else {
        (*iter).second = value;
    }
}

void CGwsStatus::SetParameter (FdoString *pName, FdoString *pValue)
{
    if (!pName)
    {
        return;
    }
    WSTR name(pName), value;
    // handle possible NULL value
    if (pValue)
    {
        value = pValue;
    }
    GwsDiagParameters::iterator iter;
    iter = m_parameters.find (pName);
    if (iter == m_parameters.end ())
    {
        m_parameters.insert (GwsDiagParameters::value_type (name, value));
    } else {
        (*iter).second = value;
    }
}

bool
CGwsStatus::GetParameterAt(unsigned int index, FdoString*& pName, FdoString*& pValue) const
{
    if (index >= m_parameters.size())
    {
        return false;
    }
    GwsDiagParameters::const_iterator iter = m_parameters.begin();
    while(index--)
    {
        ++iter;
    }
    // take the iter to index.
    assert(iter != m_parameters.end());

    if(iter == m_parameters.end()) return false;

    pName = iter->first.c_str();
    pValue = iter->second.c_str();
    return true;
}

int CGwsStatus::GetParameterCount () const
{
    return (int) m_parameters.size();

}

void CGwsStatus::operator= (const CGwsStatus & other)
{
   m_status = other.m_status;
   m_message = other.m_message;
   m_parameters = other.m_parameters;
}


/////////////////////////////////////////////////////////////////////
//
// class CGwsObject
//
/////////////////////////////////////////////////////////////////////
CGwsObject::CGwsObject ()
{
}

CGwsObject::CGwsObject (const CGwsObject & other)
{
    * this = other;
}

CGwsObject::~CGwsObject ()
{

}

void CGwsObject::PushStatus (const CGwsStatus & stat) const
{
    GWSMutex mutex;
    mutex.Lock();
    ((CGwsObject *) this)->m_statuses.push_back (stat);
    mutex.Unlock();
}

void CGwsObject::PushFdoException (
    const CGwsStatus & stat,
    FdoException     * fdoEx
) const
{
    FdoException    * e = fdoEx;
    e->AddRef ();

    // do I need to reverse fdo exception chain?
    while (e != NULL) {
        PushStatus (CGwsStatus (eGwsFdoProviderError, e->GetExceptionMessage ()));
        FdoException * e1 = e->GetCause ();
        e->Release ();
        e = e1;

    }
    PushStatus (stat);

}



int CGwsObject::NumStatuses () const
{
    return (int) m_statuses.size ();
}


const CGwsStatus & CGwsObject::Status (int idx) const
{
    if (idx < NumStatuses ()) {
        return  m_statuses[NumStatuses() - idx - 1];
    }
    return s_status;

}


void CGwsObject::RemoveStatusAt (int idx)
{

    if (idx >= NumStatuses ())
        return;

    GWSSTATUS::iterator iter;
    int                 i;

    GWSMutex mutex;
    mutex.Lock();
    idx = NumStatuses () - idx - 1;
    for (i = 0, iter = m_statuses.begin ();
         i < idx && iter != m_statuses.end () ;
         i ++, iter ++) ;
    if (iter != m_statuses.end ())
        m_statuses.erase (iter);
    mutex.Unlock();
}

void CGwsObject::Clear  () const
{
    ((CGwsObject *) this)->m_statuses.clear ();
}

void CGwsObject::CopyStatus (const CGwsObject & other)
{
    for (size_t i = 0; i < other.m_statuses.size (); i ++) {
        m_statuses.push_back (other.m_statuses[i]);
    }
}

bool CGwsObject::IsError    (EGwsStatus stat)
{
    if (IsWarning (stat))
        return false;
    return stat != eGwsOk;
}

bool CGwsObject::IsSuccess  (EGwsStatus stat)
{
    return stat == eGwsOk;

}

bool CGwsObject::IsWarning  (EGwsStatus stat)
{
    return stat > eGwsOk && stat < eGwsFailed;
}


void CGwsObject::Write (FdoXmlWriter * writer)
{
    static_cast<void>(&(writer)); // For "unreferenced formal parameter" warning
}
