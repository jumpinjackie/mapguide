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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "GwsCommonImp.h"

/////////////////////////////////////////////////////////////////////
//
// String collection SAX handler. Use to read XML file
//
/////////////////////////////////////////////////////////////////////
CGwsStringCollectionSaxHandler CGwsStringCollectionSaxHandler::m_shandler;
wchar_t * CGwsStringCollectionSaxHandler::m_sdelimiter = L",";

///////////////////////////////////////////////////////////////////////////
//
// String collection sax handler
//
///////////////////////////////////////////////////////////////////////////
CGwsStringCollectionSaxHandler::CGwsStringCollectionSaxHandler ()
{
    m_strcoll = NULL;
}
CGwsStringCollectionSaxHandler::~CGwsStringCollectionSaxHandler ()
{
}
CGwsStringCollectionSaxHandler * CGwsStringCollectionSaxHandler::GetHandler ()
{
    return & m_shandler;
}

void CGwsStringCollectionSaxHandler::SetDestination (FdoStringCollection * strcoll)
{
    m_strcoll = strcoll;
}

void CGwsStringCollectionSaxHandler::Write (FdoXmlWriter * writer, FdoStringCollection * sellist)
{
    if (sellist) {
        FdoStringP str = sellist->ToString (CGwsStringCollectionSaxHandler::m_sdelimiter);
        writer->WriteCharacters (str);
    }
}


void CGwsStringCollectionSaxHandler::XmlCharacters(FdoXmlSaxContext* ctx, FdoString* chars)
{
    if (m_strcoll) {
        FdoPtr<FdoStringCollection> tcoll = FdoStringCollection::Create (chars, m_sdelimiter);
        m_strcoll->Append (* tcoll);
    }
}
