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

#ifndef GwsCommonUtils_h
#define GwsCommonUtils_h

// forward declarations
class CGwsObject;

// typedefs
typedef std::wstring               WSTR;
typedef std::vector<WSTR>          WSTRARRAY;
typedef std::vector<GWSFeatureId>  GwsFeaturesIdVector;

//Useful Macros for generating message ids and throwing exceptions with status


#define GWS_MESSAGE_ID(stat) IDS_STRING_##stat
#define GWS_THROW(stat) throw IGWSException::Create(stat)
#define GWS_THROW2(stat,diaginfo) throw IGWSException::Create(stat,diaginfo)
#define GWS_THROW_DIAG(stat,diaginfo) throw GwsCommonFdoUtils::Create (stat,diaginfo)
#define GWS_RETHROW(fdoException, stat) \
        IGWSException* pContainerExc = IGWSException::Create(stat, fdoException); \
        if (fdoException != NULL) \
            fdoException->Release(); \
        throw pContainerExc
// throws status if status is an error
#define GWS_OKTHROW(stat) if (IGWSException::IsError (stat)) \
                                throw IGWSException::Create(stat)

#ifdef _DEBUG
#define _GWS_TRACE_DEBUG
#endif

#ifdef _GWS_TRACE_DEBUG
#define GWS_TRACE(x) GwsDebugUtils::TraceToFile(x)
#else
#define GWS_TRACE(x)
#endif

// case insensitive comparison for wstrings to be used in stl map
// templates
/*
typedef struct less_ignore_case_fast : std::less <const std::wstring>
{
    bool operator()(const std::wstring& s1, const std::wstring& s2) const
    {
        return _wcsicmp (s1.c_str(), s2.c_str()) < 0;
    };
} GwsIgnoreCase;
*/

// String to String case insensitive map
typedef std::map<WSTR,WSTR,GwsIgnoreCase> GwsStrToStrMap;

// Diagnostics parameters
typedef GwsStrToStrMap GwsDiagParameters;

// Data value collection the has create method
class CGwsDataValueCollection: public FdoDataValueCollection
{
public:
    static FdoDataValueCollection * Create ()
    {
        return new CGwsDataValueCollection ();
    }


protected:
    CGwsDataValueCollection () {}
    virtual ~CGwsDataValueCollection () {};

    virtual void Dispose()
    {
        delete this;
    }
};

// set of qualified names
class CGwsQualifiedNames: public GWSObject<IGWSQualifiedNames>
{
public:
                                    CGwsQualifiedNames ();
    virtual                         ~CGwsQualifiedNames () throw();
    virtual int                     IndexOf (const GWSQualifiedName & qname) const;
    virtual bool                    Insert(const GWSQualifiedName & qname);
    virtual const GWSQualifiedName &Get (int idx) const;
    virtual bool                    Remove(const GWSQualifiedName & qname);
    virtual bool                    Remove(int idx);
    virtual int                     Count() const;
    virtual void                    Clear();

protected:
    std::vector<GWSQualifiedName>   m_names;
};

// String collection SAX handler. Use to read XML file
class CGwsStringCollectionSaxHandler: public FdoXmlSaxHandler
{
public:
                 CGwsStringCollectionSaxHandler ();
    virtual      ~CGwsStringCollectionSaxHandler ();

    // returns pointer to string collection handler.
    // return object must noit be released
    GWS_COMMON_API
    static       CGwsStringCollectionSaxHandler * GetHandler ();

    GWS_COMMON_API
    void         SetDestination (FdoStringCollection * strcoll);
    GWS_COMMON_API
    virtual void XmlCharacters(FdoXmlSaxContext* ctx, FdoString* chars);

    GWS_COMMON_API
    static void Write (FdoXmlWriter * writer, FdoStringCollection * sellist);

protected:
    static CGwsStringCollectionSaxHandler m_shandler;
    static wchar_t                      * m_sdelimiter;

protected:
    FdoStringCollection * m_strcoll;
};

// Computed Identifier collection SAX handler. Handles the new select property
// list (list of elements, one per identifier. Each element has an attribute
// holding the property name. If the property is computed, the expression is
// in the element contents.
class CGwsComputedIdentifierCollectionSaxHandler: public FdoXmlSaxHandler
{
public:
                 CGwsComputedIdentifierCollectionSaxHandler ();
    virtual      ~CGwsComputedIdentifierCollectionSaxHandler ();

    // returns pointer to string collection handler.
    // return object must noit be released
    GWS_COMMON_API
    static       CGwsComputedIdentifierCollectionSaxHandler * GetHandler ();

    GWS_COMMON_API
    void         SetDestination (FdoIdentifierCollection * strcoll);

    GWS_COMMON_API
    void SetXml (FdoString* xmlElem, FdoString* xmlNameAttr);

    // Handles the start of a property element. Extracts the property name.
    GWS_COMMON_API
    FdoXmlSaxHandler * XmlStartElement(
        FdoXmlSaxContext    * ctx,
        FdoString           * uri,
        FdoString           * name,
        FdoString           * qname,
        FdoXmlAttributeCollection* attrs
    );

    // Handles the contents (computed property expression) of a property element
    GWS_COMMON_API
    virtual void XmlCharacters(FdoXmlSaxContext* ctx, FdoString* chars);

    // Adds an FdoIdentifier (from parsed name and expression) to the sellist.
    GWS_COMMON_API
    bool XmlEndElement(
        FdoXmlSaxContext    * ctx,
        FdoString           * uri,
        FdoString           * name,
        FdoString           * qname
    );

    GWS_COMMON_API
    static void Write (
        FdoXmlWriter * writer,
        FdoIdentifierCollection * sellist,
        FdoString* xmlElem,
        FdoString* xmlNameAttr
    );

protected:
    static CGwsComputedIdentifierCollectionSaxHandler m_cihandler;

protected:
    FdoIdentifierCollection * m_identcoll;
    WSTR m_colName;
    WSTR m_colExpr;
    WSTR m_xmlElem;
    WSTR m_xmlNameAttr;
};

// spatial context desctiption
// should we add all attributes?
class GwsSpatialContextDescription
{
public:
    GWS_COMMON_API
    GwsSpatialContextDescription ()
        : m_csname()
    {}

    GWS_COMMON_API
    GwsSpatialContextDescription (
                const GWSQualifiedName & classname,
                const WSTR & propname,
                const WSTR & scname,
                const WSTR & desc,
                const WSTR & csname,
                const double xytol,
                FdoIEnvelope * extents)
        : m_csname(csname.c_str(), eGwsCSWkt)
    {
        m_classname = classname;
        m_propname = propname;
        m_scname = scname;
        m_desc = desc;
        m_XYTolerance = xytol;
        SetExtents (extents);
    }

    GWS_COMMON_API
    GwsSpatialContextDescription (
                const WSTR & scname,
                const WSTR & desc,
                const WSTR & csname)
        : m_csname(csname.c_str(), eGwsCSWkt)
    {
        m_scname = scname;
        m_desc = desc;
        m_XYTolerance = .001;
    }

    GWS_COMMON_API
    GwsSpatialContextDescription (
                const GwsSpatialContextDescription & other)
    {
        * this = other;
    }

    GWS_COMMON_API
    void operator=(const GwsSpatialContextDescription & other)
    {
        m_classname = other.m_classname;
        m_propname = other.m_propname;
        m_scname = other.m_scname;
        m_desc = other.m_desc;
        m_csname = other.m_csname;
        m_XYTolerance = other.m_XYTolerance;
        m_extents = other.m_extents;
    }

    GWS_COMMON_API
    const GWSQualifiedName & ClassName () const
    {
        return m_classname;
    }

    GWS_COMMON_API
    FdoString * PropertyName () const
    {
        return m_propname.c_str ();
    }

    GWS_COMMON_API
    FdoString * SpatialContextName () const
    {
        return m_scname.c_str ();
    }

    GWS_COMMON_API
    FdoString * SpatialContextDesc () const
    {
        return m_desc.c_str ();
    }

    GWS_COMMON_API
    const GWSCoordinateSystem & CsName () const
    {
        return m_csname;
    }

    GWS_COMMON_API
    double XYTolerance () const
    {
        return m_XYTolerance;
    }

    GWS_COMMON_API
    FdoIEnvelope * Extents ()
    {
        if (m_extents != NULL)
            m_extents.p->AddRef ();
        return m_extents;
    }

    GWS_COMMON_API
    void SetClassName (const GWSQualifiedName & name)
    {
        m_classname = name;
    }

    GWS_COMMON_API
    void SetPropertyName (FdoString *  pname)
    {
        if (pname == NULL)
            m_propname.clear ();
        else
            m_propname = pname;
    }

    GWS_COMMON_API
    void SetSpatialContextName (FdoString * scname)
    {
        if (scname == NULL)
            m_scname.clear ();
        else
            m_scname = scname;
    }

    GWS_COMMON_API
    void SetSpatialContextDesc (FdoString * scdesc)
    {
        if (scdesc == NULL)
            m_desc.clear ();
        else
            m_desc = scdesc;
    }

    GWS_COMMON_API
    void SetCsNameWkt (FdoString * csnamewkt)
    {
        m_csname = GWSCoordinateSystem(csnamewkt ? csnamewkt : L"", eGwsCSWkt);
    }

    GWS_COMMON_API
    void SetCsNameAbbr (FdoString * csname)
    {
        m_csname = GWSCoordinateSystem(csname ? csname : L"", eGwsCSAbbr);
    }

    GWS_COMMON_API
    void SetExtents (FdoIEnvelope * env)
    {
        m_extents = env;
        if (m_extents != NULL)
            m_extents.p->AddRef ();
    }

    GWS_COMMON_API
    void SetXYTolerance (double xytol)
    {
        m_XYTolerance = xytol;
    }

private:
    GWSQualifiedName m_classname;   // qualified class name
    WSTR             m_propname;    // property name
    WSTR             m_scname;      // spatial context name
    WSTR             m_desc;        // sc description
    GWSCoordinateSystem  m_csname;  // coord sys name
    double           m_XYTolerance; // XY data tolerance
    FdoPtr<FdoIEnvelope> m_extents;
};

// vector of spatial context descriptors
typedef std::vector<GwsSpatialContextDescription> GwsSCDescriptors;

// Gws Common FDO utilities
namespace GwsCommonFdoUtils
{
    // given fdo connection and qualified class name, return class definition.
    // Method throws FdoException in case of failure
    GWS_COMMON_API
    void GetClassDefinition (
                        FdoIConnection         * pConn,
                        const GWSQualifiedName & classname,
                        FdoFeatureSchema     * & schema,
                        FdoClassDefinition   * & classDef);

     // gets identity properties for the class defintion
    // returns true in case when identity properties are found
    // (uses class hierarchy)
    GWS_COMMON_API
    bool  GetFdoClassIdentityProperties (
                                FdoClassDefinition                  * pClassDef,
                                FdoDataPropertyDefinitionCollection * & pIdentityProps);

    GWS_COMMON_API
    FdoPropertyDefinition * GetPropertyDefinition (
                                FdoClassDefinition                  * pClassDef,
                                FdoString                           * PropertyName);

    // make Fdo Qualified class name from the qualified name,
    // Data source portion of the classname is ignored.
    GWS_COMMON_API
    WSTR MakeFdoQualifiedName(const GWSQualifiedName & classname);

     // Given class definition retrieves revision property from it.
    // Zero length string if class doesn't have it
    GWS_COMMON_API
    WSTR GetRevisionProperty (FdoClassDefinition * classdef);

     // combine filters using logical operation
    // methods handles the case when either first or second filter is null.
    // in such case the other filter is returned addrefed
    GWS_COMMON_API
    FdoFilter * CombineFilters (
                        FdoFilter                   * flt1,
                        FdoFilter                   * flt2,
                        FdoBinaryLogicalOperations    op);

    // make feature id from identity properties
    GWS_COMMON_API
    GWSExtendedFeatureId  MakeFeatureId (
                                const GWSQualifiedName          &   classname,
                                FdoPtr<FdoPropertyValueCollection>  ident,
                                const wchar_t                     * ltname);
    // retrieves geometry name from the class definition
    GWS_COMMON_API
    bool GetGeometryName (FdoClassDefinition *pClassDef,
                          std::wstring &name);

    // Create exception from diagnostics object
    GWS_COMMON_API
    IGWSException *  Create (EGwsStatus   stat,
                             CGwsObject * diaginfo);

     // describe spatial context
    GWS_COMMON_API
    EGwsStatus       DescribeClassSC (
                        FdoIConnection * conn,
                        const GWSQualifiedName & classname,
                        GwsSpatialContextDescription & desc);


    // describe spatial context
    GWS_COMMON_API
    EGwsStatus       DescribeSC (
                        FdoIConnection * conn,
                        FdoString * scname,
                        GwsSpatialContextDescription & desc);
};

#ifdef _DEBUG
namespace GwsDebugUtils
{
    // output string to file. File is defined by the GWS_TRACE_FILE env
    // varibale
    GWS_COMMON_API
    void TraceToFile (const WSTR & msg);

    GWS_COMMON_API
    void TraceToFile (const WSTR & evar, const WSTR & msg);
};
#endif

#endif
