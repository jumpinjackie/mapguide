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
CGwsComputedIdentifierCollectionSaxHandler CGwsComputedIdentifierCollectionSaxHandler::m_cihandler;

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
    static_cast<void>(&(ctx)); // For "unreferenced formal parameter" warning

    if (m_strcoll) {
        FdoPtr<FdoStringCollection> tcoll = FdoStringCollection::Create (chars, m_sdelimiter);
        m_strcoll->Append (* tcoll);
    }
}

CGwsComputedIdentifierCollectionSaxHandler::CGwsComputedIdentifierCollectionSaxHandler ()
{
    m_identcoll = NULL;
}

CGwsComputedIdentifierCollectionSaxHandler::~CGwsComputedIdentifierCollectionSaxHandler ()
{
}
CGwsComputedIdentifierCollectionSaxHandler * CGwsComputedIdentifierCollectionSaxHandler::GetHandler ()
{
    return & m_cihandler;
}

void CGwsComputedIdentifierCollectionSaxHandler::SetDestination (FdoIdentifierCollection * identcoll)
{
    m_identcoll = identcoll;
}

void CGwsComputedIdentifierCollectionSaxHandler::SetXml (FdoString* xmlElem, FdoString* xmlNameAttr)
{
    m_xmlElem = xmlElem;
    m_xmlNameAttr = xmlNameAttr;
}
void CGwsComputedIdentifierCollectionSaxHandler::Write (
    FdoXmlWriter * writer,
    FdoIdentifierCollection * sellist,
    FdoString* xmlElem,
    FdoString* xmlNameAttr
)
{
    if (sellist) {
        for ( int i = 0; i < sellist->GetCount(); i++ ) {
            FdoPtr<FdoIdentifier> ident = sellist->GetItem(i);

            writer->WriteStartElement (xmlElem);
            writer->WriteAttribute (xmlNameAttr, ident->GetName());

            // If property is computed, write out the expression.
            FdoComputedIdentifier* compIdent = dynamic_cast<FdoComputedIdentifier*>(ident.p);

            if ( compIdent ) {
                FdoPtr<FdoExpression> expr = compIdent->GetExpression();
                writer->WriteCharacters (expr->ToString());
            }
            writer->WriteEndElement ();
        }
    }
}

FdoXmlSaxHandler * CGwsComputedIdentifierCollectionSaxHandler::XmlStartElement(
    FdoXmlSaxContext    * /*ctx*/,
    FdoString           * /*uri*/,
    FdoString           * name,
    FdoString           * /*qname*/,
    FdoXmlAttributeCollection* attrs
)
{
    if ( wcscmp(name, m_xmlElem.c_str()) == 0 ) {
        FdoXmlAttributeP attr = attrs->FindItem(m_xmlNameAttr.c_str());
        if ( attr )
            m_colName = attr->GetValue();
        else
            m_colName = L"";

        m_colExpr = L"";
    }

    return this;
}

void CGwsComputedIdentifierCollectionSaxHandler::XmlCharacters(FdoXmlSaxContext* /*ctx*/, FdoString* chars)
{
    m_colExpr += chars;
}

bool CGwsComputedIdentifierCollectionSaxHandler::XmlEndElement(
    FdoXmlSaxContext    * /*ctx*/,
    FdoString           * /*uri*/,
    FdoString           * name,
    FdoString           * /*qname*/
)
{
    if ( wcscmp(name, m_xmlElem.c_str()) == 0 ) {
        if (m_identcoll && (m_colName.size() != 0)) {
            FdoPtr<FdoIdentifier> ident;
            if ( m_colExpr.size() != 0 ) {
                FdoPtr<FdoExpression> expr = FdoExpression::Parse(m_colExpr.c_str());
                ident = FdoComputedIdentifier::Create(m_colName.c_str(), expr);
            }
            else {
                ident = FdoIdentifier::Create(m_colName.c_str());
            }

            m_identcoll->Add(ident);
        }
    }

    return false;
}
