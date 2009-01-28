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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "GwsCommonImp.h"


/////////////////////////////////////////////////////////////////////
//
// IGWSException static methods
//
/////////////////////////////////////////////////////////////////////
IGWSException * IGWSException::Create()
{
    return new CGwsException();
}

IGWSException * IGWSException::Create(GWSStatus * stati)
{
    CGwsStatus * stat = dynamic_cast<CGwsStatus *> (stati);
    return new CGwsException (*stat);
}


IGWSException* IGWSException::Create(FdoString* message, EGwsStatus type)
{
    return new CGwsException(message, type);
}

IGWSException* IGWSException::Create(
    FdoString* message,
    EGwsStatus type,
    FdoException* cause
)
{
    return new CGwsException(message, type, cause);
}

IGWSException* IGWSException::Create(
    EGwsStatus    fes,
    FdoException* pCause
)
{
    IGWSException * ex = CGwsException::Create (fes);
    ex->SetCause (pCause);
    return ex;
}


// This function simplifies the creation of an exception with a single
// status, but with 0 or more name value pairs.  This list needs to
// be explicitly terminated with a NULL.
IGWSException* IGWSException::Create(
    EGwsStatus      type,
    FdoException*   pCause,
    FdoString*      pName,
    FdoString*      pValue,
    ...)   // for other name value pairs, odd number means empty value.
{
    std::wstring str;
    IGWSException *pExc = CGwsException::Create (type, pCause);
    if(!pExc)
        return NULL;

    if (pName)
    {
        pExc->SetParameter(pName, pValue);
        va_list argList;
        va_start(argList, pValue);
        pName = va_arg(argList, FdoString*);
        while(pName != NULL)
        {
            pValue = va_arg(argList, FdoString*);
            if (pValue != NULL)            {
                pExc->SetParameter(pName, pValue);
            }
            else
            {
                pExc->SetParameter(pName, NULL);
                break;
            }
        }
        va_end(argList);
    }
    return pExc;
}



IGWSException* IGWSException::Create(EGwsStatus stat)
{
    return new CGwsException (stat);

}

IGWSException* IGWSException::Create(
    EGwsStatus type,
    GWSStatus*  stati
)
{
    CGwsException *pExc = new CGwsException(type);
    CGwsStatus * stat = dynamic_cast<CGwsStatus *> (stati);
    pExc->PushStatus (*stat);
    return pExc;
}


bool IGWSException::IsError (EGwsStatus stat)
{
    return CGwsObject::IsError (stat);
}

bool IGWSException::IsWarning (EGwsStatus stat)
{
    return CGwsObject::IsWarning (stat);
}

bool IGWSException::IsSuccess (EGwsStatus stat)
{
    return CGwsObject::IsSuccess (stat);
}

wchar_t * IGWSException::GetStatusMessage (
    EGwsStatus  stat,
    wchar_t   * msg,
    int         len
)
{
    if (msg != NULL) {
        std::wstring str;
        CGwsResourceModule::GwsStatusMessage (stat, str);
        int alen = gws_min ((int)str.length (), len - 1);
        wcsncpy (msg, str.c_str (), alen);
        *(msg + alen) = 0;
    }
    return msg;
}
