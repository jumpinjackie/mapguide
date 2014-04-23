#ifndef _SIMPLE_XML_PARSER_H_
#define _SIMPLE_XML_PARSER_H_

using namespace std;

struct _tagXMLAttr;
typedef _tagXMLAttr XAttr, *LPXAttr;
typedef std::vector<LPXAttr> XAttrs;

struct _tagXMLNode;
typedef _tagXMLNode XNode, *LPXNode;
typedef std::vector<LPXNode> XNodes, *LPXNodes;

struct _tagXMLDocument;
typedef struct _tagXMLDocument XDoc, *LPXDoc;

// Entity Encode/Decode Support
typedef struct _tagXmlEntity
{
    char entity;                    // entity ( & " ' < > )
    char ref[10];                   // entity reference ( &amp; &quot; etc )
    int ref_len;                    // entity reference length
}XENTITY,*LPXENTITY;

typedef struct _tagXMLEntitys : public std::vector<XENTITY>
{
    LPXENTITY GetEntity( int entity );
    LPXENTITY GetEntity( const char* entity );
    int GetEntityCount( const char* str );
    int Ref2Entity( const char* estr, const char* str, int strlen );
    int Entity2Ref( const char* str, const char* estr, int estrlen );
    string Ref2Entity( const char* estr );
    string Entity2Ref( const char* str );

    _tagXMLEntitys(){};
    _tagXMLEntitys( LPXENTITY entities, int count );
}XENTITYS,*LPXENTITYS;
extern XENTITYS entityDefault;
string XRef2Entity( const char* estr );
string XEntity2Ref( const char* str );

typedef enum
{
    PIE_PARSE_WELFORMED = 0,
    PIE_ALONE_NOT_CLOSED,
    PIE_NOT_CLOSED,
    PIE_NOT_NESTED,
    PIE_ATTR_NO_VALUE
}PCODE;

// Parse info.
typedef struct _tagParseInfo
{
    bool        trim_value;         // [set] do trim when parse?
    bool        entity_value;       // [set] do convert from reference to entity? ( &lt; -> < )
    LPXENTITYS  entitys;            // [set] entity table for entity decode
    char        escape_value;       // [set] escape value (default '\\')
    bool        force_parse;        // [set] force parse even if xml is not welformed

    const char* xml;                // [get] xml source
    bool        erorr_occur;        // [get] is occurance of error?
    const char* error_pointer;      // [get] error position of xml source
    PCODE       error_code;         // [get] error code
    string      error_string;       // [get] error string

    LPXDoc      doc;
    _tagParseInfo() { trim_value = false; entity_value = true; force_parse = false; entitys = &entityDefault; xml = NULL; erorr_occur = false; error_pointer = NULL; error_code = PIE_PARSE_WELFORMED; escape_value = '\\'; }
}PARSEINFO,*LPPARSEINFO;
extern PARSEINFO piDefault;

// display optional environment
typedef struct _tagDispOption
{
    bool newline;               // newline when new tag
    bool reference_value;       // do convert from entity to reference ( < -> &lt; )
    char value_quotation_mark;  // val="" (default value quotation mark "
    LPXENTITYS entitys;         // entity table for entity encode

    int tab_base;               // internal usage
    _tagDispOption() { newline = true; reference_value = true; entitys = &entityDefault; tab_base = 0; value_quotation_mark = '"'; }
}DISP_OPT, *LPDISP_OPT;
extern DISP_OPT optDefault;

// XAttr : Attribute Implementation
typedef struct _tagXMLAttr
{
    string name;
    string value;

    _tagXMLNode* parent;

    string GetXML( LPDISP_OPT opt = &optDefault );
}XAttr, *LPXAttr;

typedef enum
{
    XNODE_ELEMENT,              // general node '<element>...</element>' or <element/>
    XNODE_PI,                   // <?xml version="1.0" ?>
    XNODE_COMMENT,              // <!-- comment -->
    XNODE_CDATA,                // <![CDATA[ cdata ]]>
    XNODE_DOC,                  // internal virtual root
}NODE_TYPE;

// XMLNode structure
typedef struct _tagXMLNode
{
    // name and value
    string name;
    string value;

    // internal variables
    LPXNode parent;     // parent node
    XNodes childs;      // child node
    XAttrs attrs;       // attributes
    NODE_TYPE type;     // node type
    LPXDoc doc;         // document

    // Load/Save XML
    const char* Load( const char* pszXml, LPPARSEINFO pi = &piDefault );
    string GetXML( LPDISP_OPT opt = &optDefault );
    string GetText( LPDISP_OPT opt = &optDefault );

    // internal load functions
    const char* LoadAttributes( const char* pszAttrs, LPPARSEINFO pi = &piDefault );
    const char* LoadAttributes( const char* pszAttrs, const char* pszEnd, LPPARSEINFO pi = &piDefault );
    const char* LoadProcessingInstrunction( const char* pszXml, LPPARSEINFO pi = &piDefault );
    const char* LoadComment( const char* pszXml, LPPARSEINFO pi = &piDefault );
    const char* LoadCDATA( const char* pszXml, LPPARSEINFO pi = &piDefault );

    // in own attribute list
    LPXAttr GetAttr( const char* attrname );
    const char* GetAttrValue( const char* attrname );
    XAttrs GetAttrs( const char* name );

    // in one level child nodes
    LPXNode GetChild( const char* name );
    const char* GetChildValue( const char* name );
    string GetChildText( const char* name, LPDISP_OPT opt = &optDefault );
    XNodes GetChilds( const char* name );
    XNodes GetChilds();

    LPXAttr GetChildAttr( const char* name, const char* attrname );
    const char* GetChildAttrValue( const char* name, const char* attrname );

    // search node
    LPXNode Find( const char* name );

    // modify DOM
    int     GetChildCount();
    LPXNode GetChild( int i );
    XNodes::iterator GetChildIterator( LPXNode node );
    LPXNode CreateNode( const char* name = NULL, const char* value = NULL );
    LPXNode AppendChild( const char* name = NULL, const char* value = NULL );
    LPXNode AppendChild( LPXNode node );
    bool    RemoveChild( LPXNode node );
    LPXNode DetachChild( LPXNode node );

    // node/branch copy
    void    CopyNode( LPXNode node );
    void    CopyBranch( LPXNode branch );
    void    _CopyBranch( LPXNode node );
    LPXNode AppendChildBranch( LPXNode node );

    // modify attribute
    LPXAttr GetAttr( int i );
    XAttrs::iterator GetAttrIterator( LPXAttr node );
    LPXAttr CreateAttr( const char* anem = NULL, const char* value = NULL );
    LPXAttr AppendAttr( const char* name = NULL, const char* value = NULL );
    LPXAttr AppendAttr( LPXAttr attr );
    bool    RemoveAttr( LPXAttr attr );
    LPXAttr DetachAttr( LPXAttr attr );

    // operator overloads
    LPXNode operator [] ( int i ) { return GetChild(i); }
    XNode& operator = ( XNode& node ) { CopyBranch(&node); return *this; }

    _tagXMLNode() { parent = NULL; doc = NULL; type = XNODE_ELEMENT; }
    ~_tagXMLNode();

    void Close();
}XNode, *LPXNode;

// XMLDocument structure
typedef struct _tagXMLDocument : public XNode
{
    PARSEINFO parse_info;

    _tagXMLDocument() { parent = NULL; doc = this; type = XNODE_DOC; }

    const char* Load( const char* pszXml, LPPARSEINFO pi = NULL );
    LPXNode GetRoot();

}XDoc, *LPXDoc;

// Helper Funtion
inline long XStr2Int( const char* str, long default_value = 0 )
{
    return ( str && *str ) ? atol(str) : default_value;
}

inline string TrimRight(const string& source, const string& t)
{
    string str = source;
    return str.erase ( str.find_last_not_of ( t ) + 1 ) ;
}

inline string TrimLeft(const string& source , const string& t)
{
    string str = source;
    return str.erase ( 0 , source.find_first_not_of ( t ) ) ;
}

inline string Trim(const string& source)
{
    string str = source;
    return TrimLeft ( TrimRight ( str , "\r\n\t " ) , "\r\n\t " ) ;
}

inline bool XIsEmptyString( const char* str )
{
    string s(str);
    s = Trim(s);

    return (s.length() == 0);
}

#endif
