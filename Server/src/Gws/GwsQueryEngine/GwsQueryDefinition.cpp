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
#include "GwsQueryEngineImp.h"


/////////////////////////////////////////////////////////////////////
//
// class GwsQueryXmlSaxHandler
//
/////////////////////////////////////////////////////////////////////

class GwsQueryXmlSaxHandler: public FdoXmlSaxHandler
{
public:
    std::vector<IGWSQueryDefinition *>    m_qdefs;
    WSTR                                  m_elemname;

    GwsQueryXmlSaxHandler (FdoString * elementName)
    {
        m_elemname = elementName;
    }

    virtual ~GwsQueryXmlSaxHandler ()
    {
        for (size_t i = 0; i < m_qdefs.size (); i ++) {
            m_qdefs[i]->Release ();
        }
        m_qdefs.clear ();
    }

    FdoXmlSaxHandler * XmlStartElement(
        FdoXmlSaxContext         * ctx,
        FdoString                * uri,
        FdoString                * name,
        FdoString                * qname,
        FdoXmlAttributeCollection* attrs
     )
    {
        if (! _wcsicmp (name, m_elemname.c_str ())) {
            IGWSQueryDefinition * qdef =
                GwsQueryDefinitionXmlHelpers::ReadQueryDefinition (attrs);
            m_qdefs.push_back (qdef);
        }
        return FdoXmlSaxHandler::XmlStartElement (ctx, uri, name, qname, attrs);
    }
};


/////////////////////////////////////////////////////////////////////
//
// class GWSQueryDefinition
//
/////////////////////////////////////////////////////////////////////

template<class T>
GWSQueryDefinition<T>::GWSQueryDefinition ()
{
    m_orderByList = NULL;
    m_orderingOption = FdoOrderingOption_Ascending;
}


template<class T>
GWSQueryDefinition<T>::~GWSQueryDefinition () throw()
{
}


template<class T>
void GWSQueryDefinition<T>::ToXmlWriter (
    FdoString    * elementName,
    FdoXmlWriter * writer
)
{
    GwsQueryDefinitionXmlHelpers::WriteQueryDefinitionWithHeader (this, elementName, writer);
}


template<class T>
FdoStringCollection * GWSQueryDefinition<T>::GetOrderBy ()
{
    FDO_SAFE_ADDREF(m_orderByList.p);
    return m_orderByList;
}


template<class T>
void GWSQueryDefinition<T>::SetOrderBy (FdoStringCollection* orderByList)
{
    m_orderByList = orderByList;
    FDO_SAFE_ADDREF(m_orderByList.p);
}


template<class T>
FdoOrderingOption GWSQueryDefinition<T>::GetOrderingOption ()
{
    return m_orderingOption;
}


template<class T>
void GWSQueryDefinition<T>::SetOrderingOption (FdoOrderingOption orderingOption)
{
    m_orderingOption = orderingOption;
}


EGwsStatus GwsQueryDefinitionXmlHelpers::WriteQueryDefinitionWithHeader (
    IGWSQueryDefinition * qdef,
    FdoString           * elementName,
    FdoXmlWriter        * writer
)
{
    if (elementName == NULL || * elementName == 0)
        writer->WriteStartElement (GwsQueryXml::xmlGwsQuery);
    else
        writer->WriteStartElement (elementName);

    CGwsObject * gwsobj = NULL;
    if (qdef != NULL) {
        switch (qdef->Type ()) {
        case eGwsQueryFeature:
            {
                gwsobj = dynamic_cast<CGwsObject*> (qdef);
                writer->WriteAttribute (GwsQueryXml::xmlGwsQueryTypeAttribute, GwsQueryXml::xmlGwsQueryTypeFeature);
            }
            break;
        case eGwsQueryLeftOuterJoin:
            {
                gwsobj = dynamic_cast<CGwsObject*> (qdef);
                writer->WriteAttribute (GwsQueryXml::xmlGwsQueryTypeAttribute, GwsQueryXml::xmlGwsQueryTypeLeftOuterJoin);
            }
            break;
        case eGwsQueryEqualJoin:
            {
                gwsobj = dynamic_cast<CGwsObject*> (qdef);
                writer->WriteAttribute (GwsQueryXml::xmlGwsQueryTypeAttribute, GwsQueryXml::xmlGwsQueryTypeEqualJoin);
            }
            break;
        }
        if (gwsobj != NULL) {
            gwsobj->Write (writer);
        }
    }
    writer->WriteEndElement ();
    if (gwsobj == NULL && qdef != NULL) {
        assert (false);
        return eGwsFailed;
    }
    return eGwsOk;
}


IGWSQueryDefinition * GwsQueryDefinitionXmlHelpers::ReadQueryDefinition (
    FdoXmlAttributeCollection* attrs
)
{
    IGWSQueryDefinition * qrydef = NULL;
    for (FdoInt32 i = 0; i < attrs->GetCount (); i ++) {

        FdoPtr<FdoXmlAttribute> attr = attrs->GetItem (i);

        FdoString * name  = attr->GetName ();
        FdoString * value = attr->GetValue ();
        if (_wcsicmp (name, GwsQueryXml::xmlGwsQueryTypeAttribute) == 0) {
            if (_wcsicmp (value, GwsQueryXml::xmlGwsQueryTypeFeature) == 0) {
                qrydef = new GWSFeatureQueryDefinition ();
                qrydef->AddRef ();
            } else if (_wcsicmp (value, GwsQueryXml::xmlGwsQueryTypeLeftOuterJoin) == 0) {
                qrydef = new GWSLeftJoinQueryDefinition ();
                qrydef->AddRef ();
            } else if (_wcsicmp (value, GwsQueryXml::xmlGwsQueryTypeEqualJoin) == 0) {
                qrydef = new GWSEqualJoinQueryDefinition ();
                qrydef->AddRef ();
            }
        }

    }
    return qrydef;
}


FdoString * GwsQueryDefinitionXmlHelpers::QueryXmlHeader ()
{
    return GwsQueryXml::xmlGwsQuery;
}


IGWSQueryDefinition * IGWSQueryDefinition::FromXmlReader (
    FdoString    * elementName,
    FdoXmlReader * reader
)
{
    FdoPtr<FdoXmlSaxContext> ctx = FdoXmlSaxContext::Create (reader);
    GwsQueryXmlSaxHandler handler (elementName);
    reader->Parse (& handler, ctx, true);

    IGWSQueryDefinition * qdef = NULL;

    if (! handler.m_qdefs.empty ()) {
        // get the first one.
        qdef = handler.m_qdefs [0];
    }
    if (qdef != NULL)
        qdef->AddRef ();
    return qdef;
}


IGWSQueryDefinition * IGWSQueryDefinition::FromXmlAttributes (
    FdoXmlAttributeCollection* attrs
)
{
    return GwsQueryDefinitionXmlHelpers::ReadQueryDefinition (attrs);
}


template class GWSQueryDefinition<IGWSEqualJoinQueryDefinition>;
template class GWSQueryDefinition<IGWSLeftJoinQueryDefinition>;
template class GWSQueryDefinition<IGWSFeatureQueryDefinition>;


bool
GwsQueryDefinitionXmlHelpers::CompareQueries(IGWSQueryDefinition* firstQuery
                                         , IGWSQueryDefinition* secondQuery)
{
    //default is false, if it makes it through the entire compare then
    //bRet get's set to true. Any break called before then ends
    //up returning false
    bool bRet = false;
    do
    {
        //special case. if both are null, nothing to compare
        if(NULL == firstQuery && NULL == secondQuery)
        {
            bRet = true;
            break;
        }

        //check if only one is NULL
        if(NULL == firstQuery || NULL == secondQuery)
        {
            break;
        }

        //compare if the query types match
        if(firstQuery->Type() != secondQuery->Type())
        {
            break;
        }

        //Additional checks if they are join queries
        if(firstQuery->Type() == eGwsQueryLeftOuterJoin
            || firstQuery->Type() == eGwsQueryEqualJoin)
        {
            IGWSJoinQueryDefinition* pFirstQuery = static_cast<IGWSJoinQueryDefinition*>(firstQuery);
            IGWSJoinQueryDefinition* pSecondQuery = static_cast<IGWSJoinQueryDefinition*>(secondQuery);
            assert(pFirstQuery);
            assert(pSecondQuery);

            // compare Force-one-to-one status
            if (pFirstQuery->ForceOneToOne() != pSecondQuery->ForceOneToOne())
            {
                break;
            }

            //compare the left and right query definitions(recursive call)
            if(!CompareQueries(pFirstQuery->LeftQueryDefinition()
                               , pSecondQuery->LeftQueryDefinition())
               ||
               !CompareQueries(pFirstQuery->RightQueryDefinition()
                               , pSecondQuery->RightQueryDefinition()))
            {
                break;
            }

            //compare the left join attributes
            if(!GwsQueryDefinitionXmlHelpers::CompareStringCollection(
                FdoPtr<FdoStringCollection>(pFirstQuery->LeftJoinAttributes())
                , FdoPtr<FdoStringCollection>(pSecondQuery->LeftJoinAttributes())))
            {
                break;
            }

            //compare the right join attributes
            if(!GwsQueryDefinitionXmlHelpers::CompareStringCollection(
                FdoPtr<FdoStringCollection>(pFirstQuery->RightJoinAttributes())
                , FdoPtr<FdoStringCollection>(pSecondQuery->RightJoinAttributes())))
            {
                break;
            }
        }
        //compare the select lists
        if(!GwsQueryUtils::CompareIdentifierCollection(
            FdoPtr<FdoIdentifierCollection>(firstQuery->SelectList())
            , FdoPtr<FdoIdentifierCollection>(secondQuery->SelectList())))
        {
            break;
        }

        //compare the qualified names array
        if(!GwsQueryDefinitionXmlHelpers::CompareQualifiedNames(
            FdoPtr<IGWSQualifiedNames>(firstQuery->QualifiedNames())
            , FdoPtr<IGWSQualifiedNames>(secondQuery->QualifiedNames())))
        {
            break;
        }

        //compare the FeatureSourceNames lists
        if(!GwsQueryDefinitionXmlHelpers::CompareStringCollection(
            FdoPtr<FdoStringCollection>(firstQuery->FeatureSourceNames())
            , FdoPtr<FdoStringCollection>(secondQuery->FeatureSourceNames())))
        {
            break;
        }

        //compare the OrderBy lists
        if(!GwsQueryDefinitionXmlHelpers::CompareToStringValues(
            firstQuery->Filter() , secondQuery->Filter()))
        {
            break;
        }

        //compare the feature queryand right query definitions(recursive call)
        if((firstQuery->GetPrimaryQueryDefinition() != firstQuery
            &&  secondQuery->GetPrimaryQueryDefinition() != secondQuery)
            && !CompareQueries(firstQuery->GetPrimaryQueryDefinition()
                               , secondQuery->GetPrimaryQueryDefinition()))
        {
            break;
        }
        //compare the OrderBy lists
        if(!GwsQueryDefinitionXmlHelpers::CompareStringCollection(
            FdoPtr<FdoStringCollection>(firstQuery->GetOrderBy())
            , FdoPtr<FdoStringCollection>(secondQuery->GetOrderBy())))
        {
            break;
        }

        if(firstQuery->GetOrderingOption() != secondQuery->GetOrderingOption())
        {
            break;
        }

        bRet = true;
    } while (false);
    return bRet;
}


//compare two string lists
bool
GwsQueryDefinitionXmlHelpers::CompareStringCollection(
    FdoStringCollection* firstString
    , FdoStringCollection* secondString)
{
    //**FLAG maybe do a loop compare? if not, use template function below
    if(NULL == firstString && NULL == secondString) return true;

    if((((NULL == firstString) && !(NULL == secondString))
        || (!(NULL == firstString) && (NULL == secondString)))
        ||  _wcsicmp(firstString->ToString() , secondString->ToString()) != 0) return false;
    return true;
}


template<typename T> bool
GwsQueryDefinitionXmlHelpers::CompareToStringValues(
    T* firstVal
    , T* secondVal)
{
    //**FLAG maybe do a loop compare?
    if(NULL == firstVal && NULL == secondVal) return true;

    if((((NULL == firstVal) && !(NULL == secondVal))
        || (!(NULL == firstVal) && (NULL == secondVal)))
        ||  _wcsicmp(firstVal->ToString() , secondVal->ToString()) != 0) return false;
    return true;
}


bool
GwsQueryDefinitionXmlHelpers::CompareQualifiedNames(
        IGWSQualifiedNames* firstNames
        , IGWSQualifiedNames* secondNames)
{
    //iof both are null, return true.
    if(NULL == firstNames && NULL == secondNames) return true;

    //if only one is null, return false
    if(((NULL == firstNames) && !(NULL == secondNames))
        || (!(NULL == firstNames) && (NULL == secondNames))) return false;

    //if there are a different number of qualified names, return false
    if(firstNames->Count() != secondNames->Count()) return false;

    for(int i = 0; i < firstNames->Count(); i++)
    {
        if(-1 == secondNames->IndexOf(firstNames->Get(i)))
        {
            return false;
        }
    }

    return true;
}
