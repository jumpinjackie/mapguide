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
#include "GwsQueryEngineImp.h"


/////////////////////////////////////////////////////////////////////
//
// class GWSFeatureQueryDefinition
//
/////////////////////////////////////////////////////////////////////

GWSFeatureQueryDefinition::GWSFeatureQueryDefinition ()
{
    m_filter = NULL;
}


GWSFeatureQueryDefinition::GWSFeatureQueryDefinition (
    FdoIdentifierCollection    * sellist,
    const GWSQualifiedName & classname,
    FdoFilter              * filter
)
:GWSQueryDefinition<IGWSFeatureQueryDefinition> ()
{
    m_sellist = sellist;
    if (sellist != NULL)
        sellist->AddRef ();
    m_classname = classname;
    m_filter = filter;
    if (filter != NULL)
        filter->AddRef ();
}


GWSFeatureQueryDefinition::~GWSFeatureQueryDefinition () throw()
{
}


const GWSQualifiedName & GWSFeatureQueryDefinition::ClassName () const
{
    return m_classname;
}


FdoIdentifierCollection * GWSFeatureQueryDefinition::SelectList ()
{
    if (m_sellist != NULL) {
        m_sellist.p->AddRef ();
    }
    return m_sellist;
}


IGWSQualifiedNames * GWSFeatureQueryDefinition::QualifiedNames ()
{
    IGWSQualifiedNames * qnames = IGWSQualifiedNames::Create ();
    qnames->Insert (m_classname);
    return qnames;
}


FdoStringCollection* GWSFeatureQueryDefinition::FeatureSourceNames ()
{
    FdoStringCollection * fsnames = FdoStringCollection::Create ();

    if (m_classname.FeatureSource () != NULL && * m_classname.FeatureSource () != 0) {
        fsnames->Add (m_classname.FeatureSource ());
    }
    return fsnames;
}


FdoFilter * GWSFeatureQueryDefinition::Filter ()
{
    if (m_filter == NULL)
        return NULL;

    FdoFilter* pFilterRet(NULL);
    m_filter.CopyTo(&pFilterRet);
    return pFilterRet;
}


IGWSFeatureQueryDefinition * GWSFeatureQueryDefinition::GetPrimaryQueryDefinition ()
{
    AddRef ();
    return this;
}


void GWSFeatureQueryDefinition::Write (FdoXmlWriter * writer)
{
    writer->WriteStartElement (GwsQueryXml::xmlGwsQualifiedClassName);
    writer->WriteAttribute (GwsQueryXml::xmlGwsClassFeatureSource,m_classname.FeatureSource ());
    writer->WriteAttribute (GwsQueryXml::xmlGwsClassSchema,m_classname.Schema ());
    writer->WriteAttribute (GwsQueryXml::xmlGwsClassName,m_classname.Name ());
    writer->WriteEndElement ();

    if (m_sellist != NULL && m_sellist->GetCount () != 0) {
        // Write the select property list including the computed properties.
        writer->WriteStartElement (GwsQueryXml::xmlGwsQuerySelectExpressionList);
        CGwsComputedIdentifierCollectionSaxHandler::Write (
            writer,
            m_sellist,
            GwsQueryXml::xmlGwsQuerySelectExpression,
            GwsQueryXml::xmlGwsQuerySelectExpressionName
        );
        writer->WriteEndElement ();
    }

    if (m_filter != NULL) {
        FdoString * str = m_filter->ToString ();
        writer->WriteStartElement (GwsQueryXml::xmlGwsFilter);
        writer->WriteAttribute (GwsQueryXml::xmlGwsFilterStringValue, str);
        writer->WriteEndElement ();
    }
}


FdoXmlSaxHandler * GWSFeatureQueryDefinition::XmlStartElement(
    FdoXmlSaxContext    * ctx,
    FdoString           * uri,
    FdoString           * name,
    FdoString           * qname,
    FdoXmlAttributeCollection* attrs
)
{
    if (! _wcsicmp (name, GwsQueryXml::xmlGwsQualifiedClassName)) {
        WSTR    fsname;
        WSTR    schema;
        WSTR    classname;
        for (int i = 0; i < attrs->GetCount (); i ++) {
            FdoPtr<FdoXmlAttribute> attr = attrs->GetItem (i);

            FdoString * name  = attr->GetName ();
            FdoString * value = attr->GetValue ();
            if (_wcsicmp (name, GwsQueryXml::xmlGwsClassFeatureSource) == 0) {
                fsname = value;
            } else if (_wcsicmp (name, GwsQueryXml::xmlGwsClassSchema) == 0) {
                schema = value;
            } else if (_wcsicmp (name, GwsQueryXml::xmlGwsClassName) == 0) {
                classname = value;
            }
        }

        if (! schema.empty () && ! classname.empty ()) {
            m_classname = GWSQualifiedName (fsname.c_str (), schema.c_str (), classname.c_str ());

        } else {
            throw FdoException::Create (L"Failed to parse xml");
        }
    } else if (! _wcsicmp (name, GwsQueryXml::xmlGwsQuerySelectExpressionList)) {
        // Handle the select property list.
        m_sellist = FdoIdentifierCollection::Create();
        CGwsComputedIdentifierCollectionSaxHandler::GetHandler ()->SetDestination (m_sellist);
        CGwsComputedIdentifierCollectionSaxHandler::GetHandler ()->SetXml (GwsQueryXml::xmlGwsQuerySelectExpression, GwsQueryXml::xmlGwsQuerySelectExpressionName);
        return CGwsComputedIdentifierCollectionSaxHandler::GetHandler ();

    } else if (! _wcsicmp (name, GwsQueryXml::xmlGwsFilter)) {

        for (int i = 0; i < attrs->GetCount (); i ++) {
            FdoPtr<FdoXmlAttribute> attr = attrs->GetItem (i);
            if (_wcsicmp (attr->GetName (), GwsQueryXml::xmlGwsFilterStringValue) == 0) {
                FdoString * value = attr->GetValue ();
                if (* value != 0)
                    m_filter = FdoFilter::Parse (value); // exception is caught
                                                         // by the outermost caller
                                                         // CGws::Open ()
            }
        }
    }

    return CGwsObject::XmlStartElement (ctx, uri, name, qname, attrs);
}
