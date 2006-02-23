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
                                                          GisString           * elementName,
                                                          GisXmlWriter        * writer);
    GWS_QUERYENGINE_API
    IGWSQueryDefinition * ReadQueryDefinition (GisXmlAttributeCollection* attrs);
    GWS_QUERYENGINE_API
    GisString *           QueryXmlHeader ();
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

    void                        ToXmlWriter (GisString  * elementName, GisXmlWriter * writer);

    virtual EGwsQueryType       Type () const = 0;
    virtual GisStringCollection * SelectList () = 0;

protected:
};

/// <summary>
/// Feature Query defintion. Represents a query against Feature Source
/// <summary>
class GWSFeatureQueryDefinition : public GWSQueryDefinition<IGWSFeatureQueryDefinition>
{
public:
                                GWSFeatureQueryDefinition  (
                                        GisStringCollection    * sellist,
                                        const GWSQualifiedName & name,
                                        FdoFilter              * filter);
                                GWSFeatureQueryDefinition  ();
    virtual                     ~GWSFeatureQueryDefinition () throw();

    virtual const GWSQualifiedName& ClassName () const;
    virtual GisStringCollection   * SelectList ();

    virtual FdoFilter *             Filter     ();
    virtual IGWSQualifiedNames*     QualifiedNames ();
    virtual GisStringCollection*    FeatureSourceNames ();
    virtual EGwsQueryType           Type () const {return eGwsQueryFeature; }
    virtual GisStringCollection *   OrderBy    ();
    virtual IGWSFeatureQueryDefinition *
                                     GetPrimaryQueryDefinition ();

protected:
    virtual void                Write    (GisXmlWriter * writer);
    // GisXmlSaxHandler protocol
    virtual GisXmlSaxHandler*   XmlStartElement (
                                          GisXmlSaxContext* ctx,
                                          GisString       * uri,
                                          GisString       * name,
                                          GisString       * qname,
                                          GisXmlAttributeCollection* attrs);

protected:
    GisPtr<GisStringCollection> m_sellist;
    GWSQualifiedName            m_classname;
    GisPtr<FdoFilter>           m_filter;
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
                                    GisStringCollection * leftProp,
                                    GisStringCollection * rightProp);
                                GWSJoinQueryDefinition ();

    virtual                     ~GWSJoinQueryDefinition () throw();

    virtual FdoFilter           * Filter     ();
    virtual GisStringCollection * SelectList ();

    virtual IGWSQueryDefinition * LeftQueryDefinition ();
    virtual IGWSQueryDefinition * RightQueryDefinition ();
    virtual GisStringCollection * LeftJoinAttributes ();
    virtual GisStringCollection * RightJoinAttributes ();
    virtual IGWSQualifiedNames*   QualifiedNames ();
    virtual GisStringCollection*  FeatureSourceNames ();
    virtual IGWSFeatureQueryDefinition *
                                  GetPrimaryQueryDefinition ();

protected:
    // GisXmlSaxHandler protocol
    virtual void                  Write    (GisXmlWriter * writer);
    virtual GisXmlSaxHandler*     XmlStartElement (
                                          GisXmlSaxContext* ctx,
                                          GisString       * uri,
                                          GisString       * name,
                                          GisString       * qname,
                                          GisXmlAttributeCollection* attrs);

protected:
    GisPtr<IGWSQueryDefinition>   m_leftQd;
    GisPtr<IGWSQueryDefinition>   m_rightQd;
    GisPtr<GisStringCollection>   m_leftAttrs;
    GisPtr<GisStringCollection>   m_rightAttrs;
};

// Left join query definition
class GWSLeftJoinQueryDefinition :  public GWSJoinQueryDefinition<IGWSLeftJoinQueryDefinition>
{
public:
                                GWSLeftJoinQueryDefinition  (
                                    IGWSQueryDefinition * leftQd,
                                    IGWSQueryDefinition * rightQd,
                                    GisStringCollection * leftProp,
                                    GisStringCollection * rightProp);
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
                                    GisStringCollection * leftProp,
                                    GisStringCollection * rightProp);
                                GWSEqualJoinQueryDefinition ();

    virtual                     ~GWSEqualJoinQueryDefinition () throw();

    virtual EGwsQueryType       Type () const {return eGwsQueryEqualJoin; }
};

#endif
