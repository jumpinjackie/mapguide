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
#include <stdafx.h>

#include "GwsQueryEngineImp.h"

/////////////////////////////////////////////////////////////////////
//
// template class GWSJoinQueryDefinition
//
/////////////////////////////////////////////////////////////////////
template<class T>
GWSJoinQueryDefinition<T>::GWSJoinQueryDefinition  (
    IGWSQueryDefinition * leftQd,
    IGWSQueryDefinition * rightQd,
    GisStringCollection * leftProp,
    GisStringCollection * rightProp
)
:GWSQueryDefinition<T> ()

{
    m_leftQd     = leftQd;
    if (leftQd != NULL)
        leftQd->AddRef ();
    m_rightQd    = rightQd;
    if (rightQd != NULL)
        rightQd->AddRef ();
    m_leftAttrs  = leftProp;
    if (leftProp != NULL)
        leftProp->AddRef ();
    m_rightAttrs = rightProp;
    if (rightProp != NULL)
        rightProp->AddRef ();
}

template<class T>
GWSJoinQueryDefinition<T>::GWSJoinQueryDefinition ()
{
}

template<class T>
GWSJoinQueryDefinition<T>::~GWSJoinQueryDefinition () throw()
{
}

template<class T>
GisStringCollection * GWSJoinQueryDefinition<T>::SelectList ()
{
    if (m_leftQd == NULL)
        return NULL;
    return m_leftQd->SelectList ();
}


template<class T>
FdoFilter * GWSJoinQueryDefinition<T>::Filter ()
{
    if (m_leftQd == NULL)
        return NULL;

    return m_leftQd->Filter ();
}


template<class T>
IGWSQueryDefinition * GWSJoinQueryDefinition<T>::LeftQueryDefinition ()
{
    if (m_leftQd != NULL)
        m_leftQd.p->AddRef ();
    return m_leftQd;
}

template<class T>
IGWSQueryDefinition * GWSJoinQueryDefinition<T>::RightQueryDefinition ()
{
    if (m_rightQd != NULL)
        m_rightQd.p->AddRef ();
    return m_rightQd;
}

template<class T>
GisStringCollection * GWSJoinQueryDefinition<T>::LeftJoinAttributes ()
{
    if (m_leftAttrs != NULL)
        m_leftAttrs.p->AddRef ();
    return m_leftAttrs;
}

template<class T>
GisStringCollection * GWSJoinQueryDefinition<T>::RightJoinAttributes ()
{
    if (m_rightAttrs != NULL)
        m_rightAttrs.p->AddRef ();
    return m_rightAttrs;
}

template<class T>
IGWSQualifiedNames* GWSJoinQueryDefinition<T>::QualifiedNames ()
{
    GisPtr<IGWSQualifiedNames> lqnames = m_leftQd->QualifiedNames ();
    GisPtr<IGWSQualifiedNames> rqnames = m_rightQd->QualifiedNames ();

    IGWSQualifiedNames * qnames = IGWSQualifiedNames::Create ();
    int i;
    for (i = 0; lqnames != NULL && i < lqnames->Count (); i ++) {
        if (qnames->IndexOf (lqnames->Get (i)) < 0) {
            qnames->Insert (lqnames->Get (i));
        }
    }

    for (i = 0; rqnames != NULL && i < rqnames->Count (); i ++) {
        if (qnames->IndexOf (rqnames->Get (i)) < 0) {
            qnames->Insert (rqnames->Get (i));
        }
    }
    return qnames;
}

template<class T>
GisStringCollection*  GWSJoinQueryDefinition<T>::FeatureSourceNames ()
{
    GisPtr<GisStringCollection> lfsnames = m_leftQd->FeatureSourceNames ();
    GisPtr<GisStringCollection> rfsnames = m_rightQd->FeatureSourceNames ();

    GisStringCollection * fsnames = GisStringCollection::Create ();
    int i;
    for (i = 0; lfsnames != NULL && i < lfsnames->GetCount (); i ++) {
        GisStringElementP elem = lfsnames->GetItem (i);
        if (! fsnames->Contains (elem)) {
            fsnames->Add (lfsnames->GetString (i));
        }
    }

    for (i = 0; rfsnames != NULL && i < rfsnames->GetCount (); i ++) {
        GisStringElementP elem = rfsnames->GetItem (i);
        if (! fsnames->Contains (elem)) {
            fsnames->Add (rfsnames->GetString (i));
        }
    }
    return fsnames;
}

template<class T>
IGWSFeatureQueryDefinition * GWSJoinQueryDefinition<T>::GetPrimaryQueryDefinition ()
{
    return m_leftQd->GetPrimaryQueryDefinition ();
}


template<class T>
void GWSJoinQueryDefinition<T>::Write (GisXmlWriter * writer)
{
    if (m_leftQd != NULL) {
        GwsQueryDefinitionXmlHelpers::WriteQueryDefinitionWithHeader (m_leftQd, NULL, writer);
    }
    if (m_rightQd != NULL) {
        GwsQueryDefinitionXmlHelpers::WriteQueryDefinitionWithHeader (m_rightQd, NULL, writer);
    }
    GisStringP lattrs = m_leftAttrs->ToString ();

    writer->WriteStartElement (GwsQueryXml::xmlGwsLeftJoinAttributes);
    writer->WriteAttribute (GwsQueryXml::xmlGwsJoinAttibuteNames, lattrs);
    writer->WriteEndElement ();

    GisStringP rattrs = m_rightAttrs->ToString ();

    writer->WriteStartElement (GwsQueryXml::xmlGwsRightJoinAttributes);
    writer->WriteAttribute (GwsQueryXml::xmlGwsJoinAttibuteNames, rattrs);
    writer->WriteEndElement ();
}


template<class T>
GisXmlSaxHandler*  GWSJoinQueryDefinition<T>::XmlStartElement (
    GisXmlSaxContext* ctx,
    GisString       * uri,
    GisString       * name,
    GisString       * qname,
    GisXmlAttributeCollection* attrs
)
{
    CGwsObject * gwsobj = NULL;
    if (! _wcsicmp (name, GwsQueryXml::xmlGwsQuery)) {
        IGWSQueryDefinition * qrydef =
                        GwsQueryDefinitionXmlHelpers::ReadQueryDefinition (attrs);
        if (qrydef != NULL) {
            gwsobj = dynamic_cast<CGwsObject*>(qrydef);

            if (m_leftQd == NULL)
                m_leftQd = qrydef;
            else if (m_rightQd == NULL)
                m_rightQd = qrydef;

            return gwsobj;
        } else {
            // exception?

        }
    // left and right attributes reading are the same. Big deal
    } else if (! _wcsicmp (name, GwsQueryXml::xmlGwsLeftJoinAttributes)) {
        for (int i = 0; i < attrs->GetCount (); i ++) {
            GisPtr<GisXmlAttribute> attr = attrs->GetItem (i);

            GisString * name  = attr->GetName ();
            GisString * value = attr->GetValue ();
            if (_wcsicmp (name, GwsQueryXml::xmlGwsJoinAttibuteNames) == 0) {
                m_leftAttrs = GisStringCollection::Create (value, L",");
            }
        }
    } else if (! _wcsicmp (name,GwsQueryXml::xmlGwsRightJoinAttributes)) {
         for (int i = 0; i < attrs->GetCount (); i ++) {
            GisPtr<GisXmlAttribute> attr = attrs->GetItem (i);

            GisString * name  = attr->GetName ();
            GisString * value = attr->GetValue ();
            if (_wcsicmp (name, GwsQueryXml::xmlGwsJoinAttibuteNames) == 0) {
                m_rightAttrs = GisStringCollection::Create (value, L",");
            }
        }
    }

    return CGwsObject::XmlStartElement (ctx, uri, name, qname, attrs);
}



/////////////////////////////////////////////////////////////////////
//
// class GWSLeftJoinQueryDefinition
//
/////////////////////////////////////////////////////////////////////
GWSLeftJoinQueryDefinition::GWSLeftJoinQueryDefinition  (
    IGWSQueryDefinition * leftQd,
    IGWSQueryDefinition * rightQd,
    GisStringCollection * leftProp,
    GisStringCollection * rightProp
)
:GWSJoinQueryDefinition<IGWSLeftJoinQueryDefinition> (leftQd, rightQd, leftProp, rightProp)
{
}

GWSLeftJoinQueryDefinition::GWSLeftJoinQueryDefinition ()
{
}

GWSLeftJoinQueryDefinition::~GWSLeftJoinQueryDefinition () throw()
{
}


/////////////////////////////////////////////////////////////////////
//
// class GWSEqualJoinQueryDefinition
//
/////////////////////////////////////////////////////////////////////
GWSEqualJoinQueryDefinition::GWSEqualJoinQueryDefinition  (
    IGWSQueryDefinition * leftQd,
    IGWSQueryDefinition * rightQd,
    GisStringCollection * leftProp,
    GisStringCollection * rightProp
)
:GWSJoinQueryDefinition<IGWSEqualJoinQueryDefinition> (leftQd, rightQd, leftProp, rightProp)
{
}

GWSEqualJoinQueryDefinition::GWSEqualJoinQueryDefinition ()
{
}

GWSEqualJoinQueryDefinition::~GWSEqualJoinQueryDefinition () throw()
{
}
