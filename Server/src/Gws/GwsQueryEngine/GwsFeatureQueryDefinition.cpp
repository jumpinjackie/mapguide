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
// class GWSFeatureQueryDefinition
//
/////////////////////////////////////////////////////////////////////
GWSFeatureQueryDefinition::GWSFeatureQueryDefinition ()
{

    m_filter = NULL;
}

GWSFeatureQueryDefinition::GWSFeatureQueryDefinition (
    GisStringCollection    * sellist,
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

GisStringCollection * GWSFeatureQueryDefinition::SelectList ()
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

GisStringCollection* GWSFeatureQueryDefinition::FeatureSourceNames ()
{
    GisStringCollection * fsnames = GisStringCollection::Create ();

    if (m_classname.FeatureSource () != NULL && * m_classname.FeatureSource () != 0) {
        fsnames->Add (m_classname.FeatureSource ());
    }
    return fsnames;
}


FdoFilter * GWSFeatureQueryDefinition::Filter ()
{
    if (m_filter == NULL)
        return NULL;

    // TODO: this cloning is very in-efficient.
    // rewrite this code as soon as a better cloning will be
    // available.
    return FdoFilter::Parse (m_filter->ToString() );
}

GisStringCollection * GWSFeatureQueryDefinition::OrderBy    ()
{
    return NULL;
}

IGWSFeatureQueryDefinition * GWSFeatureQueryDefinition::GetPrimaryQueryDefinition ()
{
    AddRef ();
    return this;
}


void GWSFeatureQueryDefinition::Write (GisXmlWriter * writer)
{
    writer->WriteStartElement (GwsQueryXml::xmlGwsQualifiedClassName);
    writer->WriteAttribute (GwsQueryXml::xmlGwsClassFeatureSource,m_classname.FeatureSource ());
    writer->WriteAttribute (GwsQueryXml::xmlGwsClassSchema,m_classname.Schema ());
    writer->WriteAttribute (GwsQueryXml::xmlGwsClassName,m_classname.Name ());
    writer->WriteEndElement ();

    if (m_sellist != NULL && m_sellist->GetCount () != 0) {
        writer->WriteStartElement (GwsQueryXml::xmlGwsQuerySelectList);
        CGwsStringCollectionSaxHandler::Write (writer, m_sellist);
        writer->WriteEndElement ();
    }

    if (m_filter != NULL) {
        GisString * str = m_filter->ToString ();
        writer->WriteStartElement (GwsQueryXml::xmlGwsFilter);
        writer->WriteAttribute (GwsQueryXml::xmlGwsFilterStringValue, str);
        writer->WriteEndElement ();
    }
}


GisXmlSaxHandler * GWSFeatureQueryDefinition::XmlStartElement(
    GisXmlSaxContext    * ctx,
    GisString           * uri,
    GisString           * name,
    GisString           * qname,
    GisXmlAttributeCollection* attrs
)
{
    if (! _wcsicmp (name, GwsQueryXml::xmlGwsQualifiedClassName)) {
        WSTR    fsname;
        WSTR    schema;
        WSTR    classname;
        for (int i = 0; i < attrs->GetCount (); i ++) {
            GisPtr<GisXmlAttribute> attr = attrs->GetItem (i);

            GisString * name  = attr->GetName ();
            GisString * value = attr->GetValue ();
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
            throw GisException::Create (L"Failed to parse xml");
        }
    } else if (! _wcsicmp (name, GwsQueryXml::xmlGwsQuerySelectList)) {
        m_sellist = GisStringCollection::Create();
        CGwsStringCollectionSaxHandler::GetHandler ()->SetDestination (m_sellist);
        return CGwsStringCollectionSaxHandler::GetHandler ();

    } else if (! _wcsicmp (name, GwsQueryXml::xmlGwsFilter)) {

        for (int i = 0; i < attrs->GetCount (); i ++) {
            GisPtr<GisXmlAttribute> attr = attrs->GetItem (i);
            if (_wcsicmp (attr->GetName (), GwsQueryXml::xmlGwsFilterStringValue) == 0) {
                GisString * value = attr->GetValue ();
                if (* value != 0)
                    m_filter = FdoFilter::Parse (value); // exception is caught
                                                         // by the outermost caller
                                                         // CGws::Open ()
            }
        }
    }

    return CGwsObject::XmlStartElement (ctx, uri, name, qname, attrs);
}
