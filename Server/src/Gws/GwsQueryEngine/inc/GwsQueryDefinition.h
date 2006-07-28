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

#ifndef gwsquerydefinition_h
#define gwsquerydefinition_h

class GWSFilter;


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
};

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
    virtual FdoStringCollection * SelectList () = 0;

protected:
};

/// <summary>
/// Feature Query defintion. Represents a query against Feature Source
/// <summary>
class GWSFeatureQueryDefinition : public GWSQueryDefinition<IGWSFeatureQueryDefinition>
{
public:
                                GWSFeatureQueryDefinition  (
                                        FdoStringCollection    * sellist,
                                        const GWSQualifiedName & name,
                                        FdoFilter              * filter);
                                GWSFeatureQueryDefinition  ();
    virtual                     ~GWSFeatureQueryDefinition () throw();

    virtual const GWSQualifiedName& ClassName () const;
    virtual FdoStringCollection   * SelectList ();

    virtual FdoFilter *             Filter     ();
    virtual IGWSQualifiedNames*     QualifiedNames ();
    virtual FdoStringCollection*    FeatureSourceNames ();
    virtual EGwsQueryType           Type () const {return eGwsQueryFeature; }
    virtual FdoStringCollection *   OrderBy    ();
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
    FdoPtr<FdoStringCollection> m_sellist;
    GWSQualifiedName            m_classname;
    FdoPtr<FdoFilter>           m_filter;
};


/// <summary>
/// LEft join query definition. Represents equal join of two query definitions
/// join condition is a collection of class attributes is a are defined by
/// <summary>
template <class T>
class GWSJoinQueryDefinition : public GWSQueryDefinition<T>
{
public:
                                GWSJoinQueryDefinition  (
                                    IGWSQueryDefinition * leftQd,
                                    IGWSQueryDefinition * rightQd,
                                    FdoStringCollection * leftProp,
                                    FdoStringCollection * rightProp);
                                GWSJoinQueryDefinition ();

    virtual                     ~GWSJoinQueryDefinition () throw();

    virtual FdoFilter           * Filter     ();
    virtual FdoStringCollection * SelectList ();

    virtual IGWSQueryDefinition * LeftQueryDefinition ();
    virtual IGWSQueryDefinition * RightQueryDefinition ();
    virtual FdoStringCollection * LeftJoinAttributes ();
    virtual FdoStringCollection * RightJoinAttributes ();
    virtual IGWSQualifiedNames*   QualifiedNames ();
    virtual FdoStringCollection*  FeatureSourceNames ();
    virtual IGWSFeatureQueryDefinition *
                                  GetPrimaryQueryDefinition ();

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
};

// Left join query definition
class GWSLeftJoinQueryDefinition :  public GWSJoinQueryDefinition<IGWSLeftJoinQueryDefinition>
{
public:
                                GWSLeftJoinQueryDefinition  (
                                    IGWSQueryDefinition * leftQd,
                                    IGWSQueryDefinition * rightQd,
                                    FdoStringCollection * leftProp,
                                    FdoStringCollection * rightProp);
                                GWSLeftJoinQueryDefinition ();

    virtual                     ~GWSLeftJoinQueryDefinition () throw();

    virtual EGwsQueryType       Type () const {return eGwsQueryLeftOuterJoin; }
};


// Equal join query definition
class GWSEqualJoinQueryDefinition :  public GWSJoinQueryDefinition<IGWSEqualJoinQueryDefinition>
{
public:

                                GWSEqualJoinQueryDefinition  (
                                    IGWSQueryDefinition * leftQd,
                                    IGWSQueryDefinition * rightQd,
                                    FdoStringCollection * leftProp,
                                    FdoStringCollection * rightProp);
                                GWSEqualJoinQueryDefinition ();

    virtual                     ~GWSEqualJoinQueryDefinition () throw();

    virtual EGwsQueryType       Type () const {return eGwsQueryEqualJoin; }
};

#endif
