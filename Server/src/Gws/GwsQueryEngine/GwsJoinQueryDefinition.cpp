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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "GwsQueryEngineImp.h"

/////////////////////////////////////////////////////////////////////
//
// template class GWSJoinQueryDefinition
//
/////////////////////////////////////////////////////////////////////
template<class T>
GWSJoinQueryDefinition<T>::GWSJoinQueryDefinition  (
    const FdoString     * joinName,
    const FdoString     * joinDelimiter,
    bool                  forceOneToOne,
    IGWSQueryDefinition * leftQd,
    IGWSQueryDefinition * rightQd,
    FdoStringCollection * leftProp,
    FdoStringCollection * rightProp
)
:GWSQueryDefinition<T> ()

{
    assert(joinName);
    if(NULL != joinName)
        m_joinName   = joinName;
    if(NULL != joinDelimiter)
        m_joinDelimiter = joinDelimiter;
    m_forceOneToOne = forceOneToOne;

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
FdoIdentifierCollection * GWSJoinQueryDefinition<T>::SelectList ()
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
FdoStringCollection * GWSJoinQueryDefinition<T>::LeftJoinAttributes ()
{
    if (m_leftAttrs != NULL)
        m_leftAttrs.p->AddRef ();
    return m_leftAttrs;
}

template<class T>
FdoStringCollection * GWSJoinQueryDefinition<T>::RightJoinAttributes ()
{
    if (m_rightAttrs != NULL)
        m_rightAttrs.p->AddRef ();
    return m_rightAttrs;
}

template<class T>
IGWSQualifiedNames* GWSJoinQueryDefinition<T>::QualifiedNames ()
{
    FdoPtr<IGWSQualifiedNames> lqnames = m_leftQd->QualifiedNames ();
    FdoPtr<IGWSQualifiedNames> rqnames = m_rightQd->QualifiedNames ();

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
FdoStringCollection*  GWSJoinQueryDefinition<T>::FeatureSourceNames ()
{

    FdoPtr<FdoStringCollection> lfsnames = m_leftQd->FeatureSourceNames ();
    FdoPtr<FdoStringCollection> rfsnames = m_rightQd->FeatureSourceNames ();

    FdoStringCollection * fsnames = FdoStringCollection::Create ();
    int i;
    for (i = 0; lfsnames != NULL && i < lfsnames->GetCount (); i ++) {
        if (! fsnames->Contains (lfsnames->GetItem (i))) {
            fsnames->Add (lfsnames->GetString (i));
        }
    }

    for (i = 0; rfsnames != NULL && i < rfsnames->GetCount (); i ++) {
        if (! fsnames->Contains (rfsnames->GetItem (i))) {
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
void GWSJoinQueryDefinition<T>::Write (FdoXmlWriter * writer)
{
    if (m_leftQd != NULL) {
        GwsQueryDefinitionXmlHelpers::WriteQueryDefinitionWithHeader (m_leftQd, NULL, writer);
    }
    if (m_rightQd != NULL) {
        GwsQueryDefinitionXmlHelpers::WriteQueryDefinitionWithHeader (m_rightQd, NULL, writer);
    }
    FdoStringP lattrs = m_leftAttrs->ToString ();

    writer->WriteStartElement (GwsQueryXml::xmlGwsLeftJoinAttributes);
    writer->WriteAttribute (GwsQueryXml::xmlGwsJoinAttibuteNames, lattrs);
    writer->WriteEndElement ();

    FdoStringP rattrs = m_rightAttrs->ToString ();

    writer->WriteStartElement (GwsQueryXml::xmlGwsRightJoinAttributes);
    writer->WriteAttribute (GwsQueryXml::xmlGwsJoinAttibuteNames, rattrs);
    writer->WriteEndElement ();

}
template<class T>
FdoXmlSaxHandler*  GWSJoinQueryDefinition<T>::XmlStartElement (
    FdoXmlSaxContext* ctx,
    FdoString       * uri,
    FdoString       * name,
    FdoString       * qname,
    FdoXmlAttributeCollection* attrs
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
            FdoPtr<FdoXmlAttribute> attr = attrs->GetItem (i);

            FdoString * name  = attr->GetName ();
            FdoString * value = attr->GetValue ();
            if (_wcsicmp (name, GwsQueryXml::xmlGwsJoinAttibuteNames) == 0) {
                m_leftAttrs = FdoStringCollection::Create (value, L",");
            }
        }
    } else if (! _wcsicmp (name,GwsQueryXml::xmlGwsRightJoinAttributes)) {
         for (int i = 0; i < attrs->GetCount (); i ++) {
            FdoPtr<FdoXmlAttribute> attr = attrs->GetItem (i);

            FdoString * name  = attr->GetName ();
            FdoString * value = attr->GetValue ();
            if (_wcsicmp (name, GwsQueryXml::xmlGwsJoinAttibuteNames) == 0) {
                m_rightAttrs = FdoStringCollection::Create (value, L",");
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
    const FdoString     * joinName,
    const FdoString     * joinDelimiter,
    bool                  forceOneToOne,
    IGWSQueryDefinition * leftQd,
    IGWSQueryDefinition * rightQd,
    FdoStringCollection * leftProp,
    FdoStringCollection * rightProp
)
:GWSJoinQueryDefinition<IGWSLeftJoinQueryDefinition> (joinName, joinDelimiter, forceOneToOne, leftQd, rightQd, leftProp, rightProp)

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
    const FdoString     * joinName,
    const FdoString     * joinDelimiter,
    bool                  forceOneToOne,
    IGWSQueryDefinition * leftQd,
    IGWSQueryDefinition * rightQd,
    FdoStringCollection * leftProp,
    FdoStringCollection * rightProp
)
:GWSJoinQueryDefinition<IGWSEqualJoinQueryDefinition> (joinName, joinDelimiter, forceOneToOne, leftQd, rightQd, leftProp, rightProp)

{
}

GWSEqualJoinQueryDefinition::GWSEqualJoinQueryDefinition ()
{
}

GWSEqualJoinQueryDefinition::~GWSEqualJoinQueryDefinition () throw()
{
}


