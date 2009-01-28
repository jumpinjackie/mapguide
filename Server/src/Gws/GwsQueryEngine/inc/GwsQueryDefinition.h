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

#ifndef gwsquerydefinition_h
#define gwsquerydefinition_h

class GWSFilter;


///////////////////////////////////////////////////////////////////////////////
namespace GwsQueryDefinitionXmlHelpers
{
    GWS_QUERYENGINE_API
    EGwsStatus            WriteQueryDefinitionWithHeader (IGWSQueryDefinition * qdef,
                                                          FdoString           * elementName,
                                                          FdoXmlWriter        * writer);
    GWS_QUERYENGINE_API
    IGWSQueryDefinition * ReadQueryDefinition (FdoXmlAttributeCollection* attrs);
    GWS_QUERYENGINE_API
    FdoString *           QueryXmlHeader ();
    GWS_QUERYENGINE_API
    bool                  CompareQueries(IGWSQueryDefinition* firstQuery
                                         , IGWSQueryDefinition* secondQuery);
    GWS_QUERYENGINE_API
    bool                  CompareStringCollection(FdoStringCollection* firstString
                                         , FdoStringCollection* secondString);
    GWS_QUERYENGINE_API
    bool                  CompareQualifiedNames(IGWSQualifiedNames* firstNames
                                         , IGWSQualifiedNames* secondNames);
    template<typename T>
    bool CompareToStringValues(T* firstVal, T* secondVal);

};


///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Query Definition. Base class for Feature query definition,
/// Relational query defintion and Join Query Definition
/// <summary>
template <class T>
class GWSQueryDefinition :  public GWSObject<T,GWSThreadModel>,
                            public CGwsObject
{
public:
                                GWSQueryDefinition ();
    virtual                     ~GWSQueryDefinition () throw();

    void                        ToXmlWriter (FdoString  * elementName, FdoXmlWriter * writer);

    virtual EGwsQueryType       Type () const = 0;
    virtual FdoIdentifierCollection * SelectList () = 0;

    virtual FdoStringCollection* GetOrderBy ();
    virtual void                 SetOrderBy (FdoStringCollection* orderByList);

    virtual FdoOrderingOption    GetOrderingOption ();
    virtual void                 SetOrderingOption (FdoOrderingOption orderingOption);

protected:
    FdoPtr<FdoStringCollection> m_orderByList;
    FdoOrderingOption           m_orderingOption;
};


///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Feature Query defintion. Represents a query against Feature Source
/// <summary>
class GWSFeatureQueryDefinition : public GWSQueryDefinition<IGWSFeatureQueryDefinition>
{
public:
                                GWSFeatureQueryDefinition  (
                                        FdoIdentifierCollection    * sellist,
                                        const GWSQualifiedName & name,
                                        FdoFilter              * filter);
                                GWSFeatureQueryDefinition  ();
    virtual                     ~GWSFeatureQueryDefinition () throw();

    virtual const GWSQualifiedName& ClassName () const;
    virtual FdoIdentifierCollection   * SelectList ();

    virtual FdoFilter *             Filter     ();
    virtual IGWSQualifiedNames*     QualifiedNames ();
    virtual FdoStringCollection*    FeatureSourceNames ();
    virtual EGwsQueryType           Type () const {return eGwsQueryFeature; }
    virtual IGWSFeatureQueryDefinition *
                                     GetPrimaryQueryDefinition ();

protected:
    virtual void                Write    (FdoXmlWriter * writer);
    // FdoXmlSaxHandler protocol
    virtual FdoXmlSaxHandler*   XmlStartElement (
                                          FdoXmlSaxContext* ctx,
                                          FdoString       * uri,
                                          FdoString       * name,
                                          FdoString       * qname,
                                          FdoXmlAttributeCollection* attrs);

protected:
    FdoPtr<FdoIdentifierCollection> m_sellist;
    GWSQualifiedName            m_classname;
    FdoPtr<FdoFilter>           m_filter;
};


///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Left join query definition. Represents equal join of two query definitions
/// join condition is a collection of class attributes is a are defined by
/// <summary>
template <class T>
class GWSJoinQueryDefinition : public GWSQueryDefinition<T>
{
public:
                                GWSJoinQueryDefinition  (
                                    const FdoString     * joinName,
                                    const FdoString     * joinDelimiter,
                                    bool                  forceOneToOne,
                                    IGWSQueryDefinition * leftQd,
                                    IGWSQueryDefinition * rightQd,
                                    FdoStringCollection * leftProp,
                                    FdoStringCollection * rightProp);
                                GWSJoinQueryDefinition ();

    virtual                     ~GWSJoinQueryDefinition () throw();

    virtual FdoFilter           * Filter     ();
    virtual FdoIdentifierCollection * SelectList ();

    virtual IGWSQueryDefinition * LeftQueryDefinition ();
    virtual IGWSQueryDefinition * RightQueryDefinition ();
    virtual FdoStringCollection * LeftJoinAttributes ();
    virtual FdoStringCollection * RightJoinAttributes ();
    virtual IGWSQualifiedNames*   QualifiedNames ();
    virtual FdoStringCollection*  FeatureSourceNames ();
    virtual IGWSFeatureQueryDefinition *
                                  GetPrimaryQueryDefinition ();
    virtual const FdoString*      JoinName() { return m_joinName.empty() ? NULL : m_joinName.c_str(); }
    virtual const FdoString*      JoinDelimiter() { return m_joinDelimiter.empty() ? L"" : m_joinDelimiter.c_str(); }  // RKL:  if m_joinDelimiter.empty() return "", instead of NULL
    virtual bool                  ForceOneToOne() { return m_forceOneToOne; }

protected:
    // FdoXmlSaxHandler protocol
    virtual void                  Write    (FdoXmlWriter * writer);
    virtual FdoXmlSaxHandler*     XmlStartElement (
                                          FdoXmlSaxContext* ctx,
                                          FdoString       * uri,
                                          FdoString       * name,
                                          FdoString       * qname,
                                          FdoXmlAttributeCollection* attrs);

protected:
    FdoPtr<IGWSQueryDefinition>   m_leftQd;
    FdoPtr<IGWSQueryDefinition>   m_rightQd;
    FdoPtr<FdoStringCollection>   m_leftAttrs;
    FdoPtr<FdoStringCollection>   m_rightAttrs;
    WSTR                          m_joinName;
    WSTR                          m_joinDelimiter;
    bool                          m_forceOneToOne;
};


///////////////////////////////////////////////////////////////////////////////
// Left join query definition
class GWSLeftJoinQueryDefinition :  public GWSJoinQueryDefinition<IGWSLeftJoinQueryDefinition>
{
    friend class CGwsLeftJoinQueryDefCollection;

public:
    GWS_QUERYENGINE_API         GWSLeftJoinQueryDefinition  (
                                    const FdoString     * joinName,
                                    const FdoString     * joinDelimiter,
                                    bool                  forceOneToOne,
                                    IGWSQueryDefinition * leftQd,
                                    IGWSQueryDefinition * rightQd,
                                    FdoStringCollection * leftProp,
                                    FdoStringCollection * rightProp);
    GWS_QUERYENGINE_API         GWSLeftJoinQueryDefinition ();

    virtual                     ~GWSLeftJoinQueryDefinition () throw();

    virtual EGwsQueryType       Type () const {return eGwsQueryLeftOuterJoin; }
};


///////////////////////////////////////////////////////////////////////////////
// Equal join query definition
class GWSEqualJoinQueryDefinition :  public GWSJoinQueryDefinition<IGWSEqualJoinQueryDefinition>
{
    friend class CGwsEqualJoinQueryDefCollection;

public:

    GWS_QUERYENGINE_API         GWSEqualJoinQueryDefinition  (
                                    const FdoString     * joinName,
                                    const FdoString     * joinDelimiter,
                                    bool                  forceOneToOne,
                                    IGWSQueryDefinition * leftQd,
                                    IGWSQueryDefinition * rightQd,
                                    FdoStringCollection * leftProp,
                                    FdoStringCollection * rightProp);
    GWS_QUERYENGINE_API         GWSEqualJoinQueryDefinition ();

    virtual                     ~GWSEqualJoinQueryDefinition () throw();

    virtual EGwsQueryType       Type () const {return eGwsQueryEqualJoin; }
};

#endif
