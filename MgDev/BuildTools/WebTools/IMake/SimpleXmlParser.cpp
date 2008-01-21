#include "stdafx.h"
#include "SimpleXmlParser.h"

#ifdef _T
#undef _T
#endif

#define _T(x) x

static const char chXMLTagOpen  = '<';
static const char chXMLTagClose = '>';
static const char chXMLTagPre   = '/';
static const char chXMLEscape   = '\\';   // for value field escape

static const char szXMLPIOpen[] = _T("<?");
static const char szXMLPIClose[] = _T("?>");
static const char szXMLCommentOpen[] = _T("<!--");
static const char szXMLCommentClose[] = _T("-->");
static const char szXMLCDATAOpen[] = _T("<![CDATA[");
static const char szXMLCDATAClose[] = _T("]]>");

static const XENTITY x_EntityTable[] = {
        { '&', _T("&amp;"), 5 } ,
        { '\"', _T("&quot;"), 6 } ,
        { '\'', _T("&apos;"), 6 } ,
        { '<', _T("&lt;"), 4 } ,
        { '>', _T("&gt;"), 4 }
    };

PARSEINFO piDefault;
DISP_OPT optDefault;
XENTITYS entityDefault((LPXENTITY)x_EntityTable, sizeof(x_EntityTable)/sizeof(x_EntityTable[0]) );

static XNodes::iterator emptyNodesIterator;
static XAttrs::iterator emptyAttrsIterator;

const char* _tcschrs( const char* psz, const char* pszchs )
{
    while( psz && *psz )
    {
        if( strchr( pszchs, *psz ) )
            return (const char*)psz;
        psz++;
    }
    return NULL;
}

const char* _tcsskip( const char* psz )
{
    //while( psz && *psz == ' ' && *psz == 13 && *psz == 10 ) psz++;
    while( psz && isspace(*psz) ) psz++;

    return (const char*)psz;
}

const char* _tcsechr( const char* psz, int ch, int escape )
{
    const char* pch = (const char*)psz;

    while( pch && *pch )
    {
        if( escape != 0 && *pch == escape )
            pch++;
        else
        if( *pch == ch )
            return (const char*)pch;
        pch++;
    }
    return pch;
}

int _tcselen( int escape, const char* srt, const char* end = NULL )
{
    int len = 0;
    const char* pch = srt;
    if( end==NULL ) end = (const char*)sizeof(long);
    const char* prev_escape = NULL;
    while( pch && *pch && pch<end )
    {
        if( escape != 0 && *pch == escape && prev_escape == NULL )
            prev_escape = pch;
        else
        {
            prev_escape = NULL;
            len++;
        }
        pch++;
    }
    return len;
}

void _tcsecpy( char* psz, int escape, const char* srt, const char* end = NULL )
{
    const char* pch = srt;
    if( end==NULL ) end = (const char*)sizeof(long);
    const char* prev_escape = NULL;
    while( pch && *pch && pch<end )
    {
        if( escape != 0 && *pch == escape && prev_escape == NULL )
            prev_escape = pch;
        else
        {
            prev_escape = NULL;
            *psz++ = *pch;
        }

        pch++;
    }

    *psz = '\0';
}

const char* _tcsepbrk( const char* psz, const char* chset, int escape )
{
    const char* pch = (const char*)psz;
    const char* prev_escape = NULL;
    while( pch && *pch )
    {
        if( escape != 0 && *pch == escape && prev_escape == NULL )
            prev_escape = pch;
        else
        {
            prev_escape = NULL;
            if( strchr( chset, *pch ) )
                return (const char*)pch;
        }
        pch++;
    }
    return pch;
}

int _tcsenicmp( const char* psz, const char* str, int len, int escape )
{
    const char* pch = (const char*)psz;
    const char* prev_escape = NULL;
    const char* des = (const char*)str;
    int i = 0;

    while( pch && *pch && i < len )
    {
        if( escape != 0 && *pch == escape && prev_escape == NULL )
            prev_escape = pch;
        else
        {
            prev_escape = NULL;
            if( tolower(*pch) != tolower(des[i]) )
                break;
            i++;
        }
        pch ++;
    }

    // find
    if( i == len )
        return 0;
    if( psz[i] > des[i] )
        return 1;
    return -1;
}

const char* _tcsenistr( const char* psz, const char* str, int len, int escape )
{
    const char* pch = (const char*)psz;
    const char* prev_escape = NULL;

    while( pch && *pch )
    {
        if( escape != 0 && *pch == escape && prev_escape == NULL )
            prev_escape = pch;
        else
        {
            prev_escape = NULL;
            if( _tcsenicmp( pch, str, len, escape ) == 0 )
                return (const char*)pch;
        }
        pch++;
    }
    return pch;
}

const char* _tcseistr( const char* psz, const char* str, int escape )
{
    int len = strlen( str );
    return _tcsenistr( psz, str, len, escape );
}

void _SetString( const char* psz, const char* end, string* ps, bool trim = false, int escape = 0 )
{
    //trim
    if( trim )
    {
        while( psz && psz < end && isspace(*psz) ) psz++;
        while( (end-1) && psz < (end-1) && isspace(*(end-1)) ) end--;
    }
    int len = end - psz;
    if( len <= 0 ) return;
    if( escape )
    {
        len = _tcselen( escape, psz, end );
        char* pss = new char[len + 1];
        _tcsecpy( pss, escape, psz, end );
        *ps = pss;
    }
    else
    {
        char* pss = new char[len + 1];
        memcpy( pss, psz, len );
        pss[len] = '\0';
        *ps = pss;
    }
}

_tagXMLNode::~_tagXMLNode()
{
    Close();
}

void _tagXMLNode::Close()
{
    int i;
    for(i = 0 ; i < (int)childs.size(); i ++)
    {
        LPXNode p = childs[i];
        if( p )
        {
            delete p; childs[i] = NULL;
        }
    }
    childs.clear();

    for(i = 0 ; i < (int)attrs.size(); i ++)
    {
        LPXAttr p = attrs[i];
        if( p )
        {
            delete p; attrs[i] = NULL;
        }
    }
    attrs.clear();
}

const char* _tagXMLNode::LoadAttributes( const char* pszAttrs , LPPARSEINFO pi /*= &piDefault*/)
{
    const char* xml = (const char*)pszAttrs;

    while( xml && *xml )
    {
        if((xml = _tcsskip( xml )) != NULL)
        {
            // close tag
            if( *xml == chXMLTagClose || *xml == chXMLTagPre )
                // wel-formed tag
                return xml;

            // XML Attr Name
            const char* pEnd = strpbrk( xml, " =" );
            if( pEnd == NULL )
            {
                // error
                if( pi->erorr_occur == false )
                {
                    pi->erorr_occur = true;
                    pi->error_pointer = xml;
                    pi->error_code = PIE_ATTR_NO_VALUE;

                    pi->error_string = name + " attribute has error ";
                }
                return NULL;
            }

            LPXAttr attr = new XAttr;
            attr->parent = this;

            // XML Attr Name
            _SetString( xml, pEnd, &attr->name );

            // add new attribute
            attrs.push_back( attr );
            xml = pEnd;

            // XML Attr Value
            if((xml = _tcsskip( xml )) != NULL)
            {
                //if( xml = strchr( xml, '=' ) )
                if( *xml == '=' )
                {
                    if((xml = _tcsskip(++xml)) != NULL)
                    {
                        // if " or '
                        // or none quote
                        int quote = *xml;
                        if( quote == '"' || quote == '\'' )
                            pEnd = _tcsechr( ++xml, quote, chXMLEscape );
                        else
                        {
                            //attr= value>
                            // none quote mode
                            //pEnd = _tcsechr( xml, ' ', '\\' );
                            pEnd = _tcsepbrk( xml, _T(" >"), chXMLEscape );
                        }

                        bool trim = pi->trim_value;
                        char escape = pi->escape_value;
                        //_SetString( xml, pEnd, &attr->value, trim, chXMLEscape );
                        _SetString( xml, pEnd, &attr->value, trim, escape );
                        xml = pEnd;
                        // ATTRVALUE
                        if( pi->entity_value && pi->entitys )
                            attr->value = pi->entitys->Ref2Entity(attr->value.c_str());

                        if( quote == '"' || quote == '\'' )
                            xml++;
                    }
                }
            }
        }
    }

    // not wel-formed tag
    return NULL;
}

const char* _tagXMLNode::LoadAttributes( const char* pszAttrs, const char* pszEnd, LPPARSEINFO pi /*= &piDefault*/ )
{
    const char* xml = (const char*)pszAttrs;

    while( xml && *xml )
    {
        if((xml = _tcsskip(xml)) != NULL)
        {
            // close tag
            if( xml >= pszEnd )
                // wel-formed tag
                return xml;

            // XML Attr Name
            const char* pEnd = strpbrk( xml, " =" );
            if( pEnd == NULL )
            {
                // error
                if( pi->erorr_occur == false )
                {
                    pi->erorr_occur = true;
                    pi->error_pointer = xml;
                    pi->error_code = PIE_ATTR_NO_VALUE;
                    pi->error_string = name + " attribute has error ";
                }
                return NULL;
            }

            LPXAttr attr = new XAttr;
            attr->parent = this;

            // XML Attr Name
            _SetString( xml, pEnd, &attr->name );

            // add new attribute
            attrs.push_back( attr );
            xml = pEnd;

            // XML Attr Value
            if((xml = _tcsskip(xml)) != NULL)
            {
                //if( xml = strchr( xml, '=' ) )
                if( *xml == '=' )
                {
                    if((xml = _tcsskip( ++xml )) != NULL)
                    {
                        // if " or '
                        // or none quote
                        int quote = *xml;
                        if( quote == '"' || quote == '\'' )
                            pEnd = _tcsechr( ++xml, quote, chXMLEscape );
                        else
                        {
                            //attr= value>
                            // none quote mode
                            //pEnd = _tcsechr( xml, ' ', '\\' );
                            pEnd = _tcsepbrk( xml, _T(" >"), chXMLEscape );
                        }

                        bool trim = pi->trim_value;
                        char escape = pi->escape_value;
                        //_SetString( xml, pEnd, &attr->value, trim, chXMLEscape );
                        _SetString( xml, pEnd, &attr->value, trim, escape );
                        xml = pEnd;
                        // ATTRVALUE
                        if( pi->entity_value && pi->entitys )
                            attr->value = pi->entitys->Ref2Entity(attr->value.c_str());

                        if( quote == '"' || quote == '\'' )
                            xml++;
                    }
                }
            }
        }
    }

    // not wel-formed tag
    return NULL;
}

const char* _tagXMLNode::LoadProcessingInstrunction( const char* pszXml, LPPARSEINFO pi /*= &piDefault*/ )
{
    // find the end of pi
    const char* end = _tcsenistr( pszXml, szXMLPIClose, sizeof(szXMLPIClose)-1, pi ? pi->escape_value : 0 );
    if( end == NULL )
        return NULL;

    // process pi
    if( doc )
    {
        const char* xml = (const char*)pszXml;

        LPXNode node = new XNode;
        node->parent = this;
        node->doc = doc;
        node->type = XNODE_PI;

        xml += sizeof(szXMLPIOpen)-1;
        const char* pTagEnd = strpbrk( xml, " ?>" );
        _SetString( xml, pTagEnd, &node->name );
        xml = pTagEnd;

        node->LoadAttributes( xml, end, pi );

        doc->childs.push_back( node );
    }

    end += sizeof(szXMLPIClose)-1;
    return end;
}

const char* _tagXMLNode::LoadComment( const char* pszXml, LPPARSEINFO pi /*= &piDefault*/ )
{
    // find the end of comment
    const char* end = _tcsenistr( pszXml, szXMLCommentClose, sizeof(szXMLCommentClose)-1, pi ? pi->escape_value : 0 );
    if( end == NULL )
        return NULL;

    // process comment
    LPXNode par = parent;
    if( parent == NULL && doc )
        par = (LPXNode)&doc;
    if( par )
    {
        const char* xml = (const char*)pszXml;
        xml += sizeof(szXMLCommentOpen)-1;

        LPXNode node = new XNode;
        node->parent = this;
        node->doc = doc;
        node->type = XNODE_COMMENT;
        node->name = _T("#COMMENT");
        _SetString( xml, end, &node->value, false );

        par->childs.push_back( node );
    }

    end += sizeof(szXMLCommentClose)-1;
    return end;
}

const char* _tagXMLNode::LoadCDATA( const char* pszXml, LPPARSEINFO pi /*= &piDefault*/ )
{
    // find the end of CDATA
    const char* end = _tcsenistr( pszXml, szXMLCDATAClose, sizeof(szXMLCDATAClose)-1, pi ? pi->escape_value : 0 );
    if( end == NULL )
        return NULL;

    // process CDATA
    LPXNode par = parent;
    if( parent == NULL && doc )
        par = (LPXNode)&doc;
    if( par )
    {
        const char* xml = (const char*)pszXml;
        xml += sizeof(szXMLCDATAOpen)-1;

        LPXNode node = new XNode;
        node->parent = this;
        node->doc = doc;
        node->type = XNODE_CDATA;
        node->name = _T("#CDATA");
        _SetString( xml, end, &node->value, false );

        par->childs.push_back( node );
    }

    end += sizeof(szXMLCDATAClose)-1;
    return end;
}

const char* LoadOtherNodes( LPXNode node, bool* pbRet, const char* pszXml, LPPARSEINFO pi /*= &piDefault*/ )
{
    const char* xml = (const char*)pszXml;
    bool do_other_type = true;
    *pbRet = false;

    while( xml && do_other_type )
    {
        do_other_type = false;

        xml = _tcsskip( xml );
        const char* prev = xml;
        // is PI( Processing Instruction ) Node?
        if( strncmp( xml, szXMLPIOpen, sizeof(szXMLPIOpen)-1 ) == 0 )
        {
            // processing instrunction parse
            // return pointer is next node of pi
            xml = node->LoadProcessingInstrunction( xml, pi );
            //if( xml == NULL )
            //    return NULL;
            // restart xml parse
        }

        if( xml != prev )
            do_other_type = true;
        xml = _tcsskip( xml );
        prev = xml;

        // is comment Node?
        if( strncmp( xml, szXMLCommentOpen, sizeof(szXMLCommentOpen)-1 ) == 0 )
        {
            // processing comment parse
            // return pointer is next node of comment
            xml = node->LoadComment( xml, pi );
            // comment node is terminal node
            if( node->parent && node->parent->type != XNODE_DOC
                && xml != prev )
            {
                *pbRet = true;
                return xml;
            }
            // restart xml parse when this node is root doc node
        }

        if( xml != prev )
            do_other_type = true;

        xml = _tcsskip( xml );
        prev = xml;
        // is CDATA Node?
        if( strncmp( xml, szXMLCDATAOpen, sizeof(szXMLCDATAOpen)-1 ) == 0 )
        {
            // processing CDATA parse
            // return pointer is next node of CDATA
            xml = node->LoadCDATA( xml, pi );
            // CDATA node is terminal node
            if( node->parent && node->parent->type != XNODE_DOC
                && xml != prev )
            {
                *pbRet = true;
                return xml;
            }
            // restart xml parse when this node is root doc node
        }

        if( xml != prev )
            do_other_type = true;
    }

    return xml;
}

const char* _tagXMLNode::Load( const char* pszXml, LPPARSEINFO pi /*= &piDefault*/ )
{
    // Close it
    Close();

    const char* xml = (const char*)pszXml;

    xml = strchr( xml, chXMLTagOpen );
    if( xml == NULL )
        return NULL;

    // Close Tag
    if( *(xml+1) == chXMLTagPre ) // </Close
        return xml;

    // Load Other Node before <Tag>(pi, comment, CDATA etc)
    bool bRet = false;
    const char* ret = NULL;
    ret = LoadOtherNodes( this, &bRet, xml, pi );
    if( ret != NULL )
        xml = ret;
    if( bRet )
        return xml;

    // XML Node Tag Name Open
    xml++;
    const char* pTagEnd = strpbrk( xml, " />\t\r\n" );
    _SetString( xml, pTagEnd, &name );
    xml = pTagEnd;
    // Generate XML Attributte List
    if((xml = LoadAttributes( xml, pi )) != NULL)
    {
        // alone tag <TAG ... />
        if( *xml == chXMLTagPre )
        {
            xml++;
            if( *xml == chXMLTagClose )
                // wel-formed tag
                return ++xml;
            else
            {
                // error: <TAG ... / >
                if( pi->erorr_occur == false )
                {
                    pi->erorr_occur = true;
                    pi->error_pointer = xml;
                    pi->error_code = PIE_ALONE_NOT_CLOSED;
                    pi->error_string = _T("Element must be closed.");
                }
                // not wel-formed tag
                return NULL;
            }
        }
        else
        // open/close tag <TAG ..> ... </TAG>
        //                        ^- current pointer
        {
            // if text value is not exist, then assign value
            //if( this->value.IsEmpty() || this->value == _T("") )
            if( XIsEmptyString( value.c_str() ) )
            {
                // Text Value
                const char* pEnd = _tcsechr( ++xml, chXMLTagOpen, chXMLEscape );
                if( pEnd == NULL )
                {
                    if( pi->erorr_occur == false )
                    {
                        pi->erorr_occur = true;
                        pi->error_pointer = xml;
                        pi->error_code = PIE_NOT_CLOSED;
                        pi->error_string = name + " must be closed with </" + name + ">";
                    }
                    // error cos not exist CloseTag </TAG>
                    return NULL;
                }

                bool trim = pi->trim_value;
                char escape = pi->escape_value;
                //_SetString( xml, pEnd, &value, trim, chXMLEscape );
                _SetString( xml, pEnd, &value, trim, escape );

                xml = pEnd;
                // TEXTVALUE reference
                if( pi->entity_value && pi->entitys )
                    value = pi->entitys->Ref2Entity(value.c_str());
            }

            // generate child nodes
            while( xml && *xml )
            {
                LPXNode node = new XNode;
                node->parent = this;
                node->doc = doc;
                node->type = type;

                xml = node->Load( xml,pi );
                if( XIsEmptyString(node->name.c_str()) == false )
                {
                    childs.push_back( node );

                }
                else
                {
                    delete node;
                }

                // open/close tag <TAG ..> ... </TAG>
                //                             ^- current pointer
                // CloseTag case
                if( xml && *xml && *(xml+1) && *xml == chXMLTagOpen && *(xml+1) == chXMLTagPre )
                {
                    // </Close>
                    xml+=2; // C

                    if((xml = _tcsskip( xml )) != NULL)
                    {
                        string closename;
                        const char* pEnd = strpbrk( xml, " >" );
                        if( pEnd == NULL )
                        {
                            if( pi->erorr_occur == false )
                            {
                                pi->erorr_occur = true;
                                pi->error_pointer = xml;
                                pi->error_code = PIE_NOT_CLOSED;
                                pi->error_string = "it must be closed with " + name;
                            }
                            // error
                            return NULL;
                        }
                        _SetString( xml, pEnd, &closename );
                        if( closename == this->name )
                        {
                            // wel-formed open/close
                            xml = pEnd+1;
                            // return '>' or ' ' after pointer
                            return xml;
                        }
                        else
                        {
                            xml = pEnd+1;
                            // 2004.6.15 - example <B> alone tag
                            // now it can parse with attribute 'force_arse'
                            if( pi->force_parse == false )
                            {
                                // not welformed open/close
                                if( pi->erorr_occur == false )
                                {
                                    pi->erorr_occur = true;
                                    pi->error_pointer = xml;
                                    pi->error_code = PIE_NOT_NESTED;
                                    //pi->error_string.Format(_T("'<%s> ... </%s>' is not wel-formed."), name, closename );
                                }
                                return NULL;
                            }
                        }
                    }
                }
                else    // Alone child Tag Loaded
                        // else 해야하는지 말아야하는지 의심간다.
                {

                    //if( xml && this->value.IsEmpty() && *xml !=chXMLTagOpen )
                    if( xml && XIsEmptyString( value.c_str() ) && *xml !=chXMLTagOpen )
                    {
                        // Text Value
                        const char* pEnd = _tcsechr( xml, chXMLTagOpen, chXMLEscape );
                        if( pEnd == NULL )
                        {
                            // error cos not exist CloseTag </TAG>
                            if( pi->erorr_occur == false )
                            {
                                pi->erorr_occur = true;
                                pi->error_pointer = xml;
                                pi->error_code = PIE_NOT_CLOSED;
                                pi->error_string = "it must be closed with </" + name;
                            }
                            return NULL;
                        }

                        bool trim = pi->trim_value;
                        char escape = pi->escape_value;
                        //_SetString( xml, pEnd, &value, trim, chXMLEscape );
                        _SetString( xml, pEnd, &value, trim, escape );

                        xml = pEnd;
                        //TEXTVALUE
                        if( pi->entity_value && pi->entitys )
                            value = pi->entitys->Ref2Entity(value.c_str());
                    }
                }
            }
        }
    }

    return xml;
}

const char* _tagXMLDocument::Load( const char* pszXml, LPPARSEINFO pi /*= NULL*/ )
{
    LPXNode node = new XNode;
    node->parent = (LPXNode)this;
    node->type = XNODE_ELEMENT;
    node->doc = this;
    const char* end;

    if( pi == NULL )
        pi = &parse_info;

    if( (end = node->Load( pszXml, pi )) == NULL )
    {
        delete node;
        return NULL;
    }

    childs.push_back( node );

    // Load Other Node after </Tag>(pi, comment, CDATA etc)
    const char* ret;
    bool bRet = false;
    ret = LoadOtherNodes( node, &bRet, end, pi );
    if( ret != NULL )
        end = ret;

    return end;
}

LPXNode _tagXMLDocument::GetRoot()
{
    XNodes::iterator it = childs.begin();
    for( ; it != childs.end() ; ++(it) )
    {
        LPXNode node = *it;
        if( node->type == XNODE_ELEMENT )
            return node;
    }
    return NULL;
}

string _tagXMLAttr::GetXML( LPDISP_OPT opt /*= &optDefault*/ )
{
    std::ostringstream os;
    //os << (const char*)name << "='" << (const char*)value << "' ";

    os << name.c_str() << "=" << (char)opt->value_quotation_mark
        << (opt->reference_value&&opt->entitys?opt->entitys->Entity2Ref(value.c_str()):value).c_str()
        << (char)opt->value_quotation_mark << " ";
    return os.str().c_str();
}

string _tagXMLNode::GetXML( LPDISP_OPT opt /*= &optDefault*/ )
{
    std::ostringstream os;

    // tab
    if( opt && opt->newline )
    {
        if( opt && opt->newline )
            os << "\r\n";
        for( int i = 0 ; i < opt->tab_base ; i++)
            os << '\t';
    }

    if( type == XNODE_DOC )
    {
        for( int i = 0 ; i < (int)childs.size(); i++ )
            os << childs[i]->GetXML( opt ).c_str();
        return os.str().c_str();
    }
    else
    if( type == XNODE_PI )
    {
        // <?TAG
        os << szXMLPIOpen << name.c_str();
        // <?TAG Attr1="Val1"
        if( attrs.empty() == false ) os << ' ';
        for( int i = 0 ; i < (int)attrs.size(); i++ )
        {
            os << (const char*)attrs[i]->GetXML(opt).c_str();
        }
        //?>
        os << szXMLPIClose;
        return os.str().c_str();
    }
    else
    if( type == XNODE_COMMENT )
    {
        // <--comment
        os << szXMLCommentOpen << value.c_str();
        //-->
        os << szXMLCommentClose;
        return os.str().c_str();
    }
    else
    if( type == XNODE_CDATA )
    {
        // <--comment
        os << szXMLCDATAOpen << value.c_str();
        //-->
        os << szXMLCDATAClose;
        return os.str().c_str();
    }

    // <TAG
    os << '<' << name.c_str();

    // <TAG Attr1="Val1"
    if( attrs.empty() == false ) os << ' ';
    for( int i = 0 ; i < (int)attrs.size(); i++ )
    {
        os << attrs[i]->GetXML(opt).c_str();
    }

    if( childs.empty() && XIsEmptyString(value.c_str()) )
    {
        // <TAG Attr1="Val1"/> alone tag
        os << "/>";
    }
    else
    {
        // <TAG Attr1="Val1"> and get child
        os << '>';
        if( opt && opt->newline && !childs.empty() )
        {
            opt->tab_base++;
        }

        for( int i = 0 ; i < (int)childs.size(); i++ )
            os << childs[i]->GetXML( opt ).c_str();

        // Text Value
        if( value != _T("") )
        {
            if( opt && opt->newline && !childs.empty() )
            {
                if( opt && opt->newline )
                    os << "\r\n";
                for( int i = 0 ; i < opt->tab_base ; i++)
                    os << '\t';
            }
            os << (opt->reference_value&&opt->entitys?opt->entitys->Entity2Ref(value.c_str()).c_str():value.c_str());
        }

        // </TAG> CloseTag
        if( opt && opt->newline && !childs.empty() )
        {
            os << "\r\n";
            for( int i = 0 ; i < opt->tab_base-1 ; i++)
                os << '\t';
        }
        os << "</" << name.c_str() << '>';

        if( opt && opt->newline )
        {
            if( !childs.empty() )
                opt->tab_base--;
        }
    }

    return os.str().c_str();
}

string _tagXMLNode::GetText( LPDISP_OPT opt /*= &optDefault*/ )
{
    std::ostringstream os;

    if( type == XNODE_DOC )
    {
        for( int i = 0 ; i < (int)childs.size(); i++ )
            os << (const char*)childs[i]->GetText( opt ).c_str();
    }
    else
    if( type == XNODE_PI )
    {
        // no text
    }
    else
    if( type == XNODE_COMMENT )
    {
        // no text
    }
    else
    if( type == XNODE_CDATA )
    {
        os << (const char*)value.c_str();
    }
    else
    if( type == XNODE_ELEMENT )
    {
        if( childs.empty() && XIsEmptyString(value.c_str()) )
        {
            // no text
        }
        else
        {
            // childs text
            for( int i = 0 ; i < (int)childs.size(); i++ )
                os << (const char*)childs[i]->GetText().c_str();

            // Text Value
            os << (opt->reference_value&&opt->entitys?opt->entitys->Entity2Ref(value.c_str()).c_str():value.c_str());
        }
    }

    return os.str().c_str();
}

LPXAttr _tagXMLNode::GetAttr( const char* attrname )
{
    for( int i = 0 ; i < (int)attrs.size(); i++ )
    {
        LPXAttr attr = attrs[i];
        if( attr )
        {
            if( attr->name == attrname )
                return attr;
        }
    }
    return NULL;
}

XAttrs _tagXMLNode::GetAttrs( const char* name )
{
    XAttrs attrs;
    for( int i = 0 ; i < (int)attrs.size(); i++ )
    {
        LPXAttr attr = attrs[i];
        if( attr )
        {
            if( attr->name == name )
                attrs.push_back( attr );
        }
    }
    return attrs;
}

const char* _tagXMLNode::GetAttrValue( const char* attrname )
{
    LPXAttr attr = GetAttr( attrname );
    return attr ? (const char*)attr->value.c_str() : NULL;
}

XNodes _tagXMLNode::GetChilds()
{
    return childs;
}

XNodes _tagXMLNode::GetChilds( const char* name )
{
    XNodes nodes;
    for( int i = 0 ; i < (int)childs.size(); i++ )
    {
        LPXNode node = childs[i];
        if( node )
        {
            if( node->name == name )
                nodes.push_back( node );
        }
    }
    return nodes;
}

LPXNode _tagXMLNode::GetChild( int i )
{
    if( i >= 0 && i < (int)childs.size() )
        return childs[i];
    return NULL;
}

int _tagXMLNode::GetChildCount()
{
    return childs.size();
}

LPXNode _tagXMLNode::GetChild( const char* name )
{
    for( int i = 0 ; i < (int)childs.size(); i++ )
    {
        LPXNode node = childs[i];
        if( node )
        {
            if( node->name == name )
                return node;
        }
    }
    return NULL;
}

const char* _tagXMLNode::GetChildValue( const char* name )
{
    LPXNode node = GetChild( name );
    return (node != NULL)? (const char*)node->value.c_str() : NULL;
}

string _tagXMLNode::GetChildText( const char* name, LPDISP_OPT opt /*= &optDefault*/ )
{
    LPXNode node = GetChild( name );
    return (node != NULL)? node->GetText(opt) : _T("");
}

LPXAttr _tagXMLNode::GetChildAttr( const char* name, const char* attrname )
{
    LPXNode node = GetChild(name);
    return node ? node->GetAttr(attrname) : NULL;
}

const char* _tagXMLNode::GetChildAttrValue( const char* name, const char* attrname )
{
    LPXAttr attr = GetChildAttr( name, attrname );
    return attr ? (const char*)attr->value.c_str() : NULL;
}

LPXNode _tagXMLNode::Find( const char* name )
{
    XNodes::iterator it = childs.begin();
    for( ; it != childs.end(); ++(it))
    {
        LPXNode child = *it;
        if( child->name == name )
            return child;

        XNodes::iterator it = child->childs.begin();
        for( ; it != child->childs.end(); ++(it))
        {
            LPXNode find = child->Find( name );
            if( find != NULL )
                return find;
        }
    }

    return NULL;
}

XNodes::iterator _tagXMLNode::GetChildIterator( LPXNode node )
{
    XNodes::iterator it = childs.begin();
    for( ; it != childs.end() ; ++(it) )
    {
        if( *it == node )
            return it;
    }
    return emptyNodesIterator;
}

LPXNode _tagXMLNode::AppendChild( const char* name /*= NULL*/, const char* value /*= NULL*/ )
{
    return AppendChild( CreateNode( name, value ) );
}

LPXNode _tagXMLNode::AppendChild( LPXNode node )
{
    node->parent = this;
    node->doc = doc;
    childs.push_back( node );
    return node;
}

bool _tagXMLNode::RemoveChild( LPXNode node )
{
    XNodes::iterator it = GetChildIterator( node );
    if(*it != NULL)
    {
        delete *it;
        childs.erase( it );
        return true;
    }
    return false;
}

LPXAttr _tagXMLNode::GetAttr( int i )
{
    if( i >= 0 && i < (int)attrs.size() )
        return attrs[i];
    return NULL;
}

XAttrs::iterator _tagXMLNode::GetAttrIterator( LPXAttr attr )
{
    XAttrs::iterator it = attrs.begin();
    for( ; it != attrs.end() ; ++(it) )
    {
        if( *it == attr )
            return it;
    }
    return emptyAttrsIterator;
}

LPXAttr _tagXMLNode::AppendAttr( LPXAttr attr )
{
    attr->parent = this;
    attrs.push_back( attr );
    return attr;
}

bool _tagXMLNode::RemoveAttr( LPXAttr attr )
{
    XAttrs::iterator it = GetAttrIterator( attr );
    if(*it != NULL)
    {
        delete *it;
        attrs.erase( it );
        return true;
    }
    return false;
}

LPXNode _tagXMLNode::CreateNode( const char* name /*= NULL*/, const char* value /*= NULL*/ )
{
    LPXNode node = new XNode;
    node->name = name;
    node->value = value;
    return node;
}

LPXAttr _tagXMLNode::CreateAttr( const char* name /*= NULL*/, const char* value /*= NULL*/ )
{
    LPXAttr attr = new XAttr;
    attr->name = name;
    attr->value = value;
    return attr;
}

LPXAttr _tagXMLNode::AppendAttr( const char* name /*= NULL*/, const char* value /*= NULL*/ )
{
    return AppendAttr( CreateAttr( name, value ) );
}

LPXNode _tagXMLNode::DetachChild( LPXNode node )
{
    XNodes::iterator it = GetChildIterator( node );
    if(*it != NULL)
    {
        childs.erase( it );
        return node;
    }
    return NULL;
}

LPXAttr _tagXMLNode::DetachAttr( LPXAttr attr )
{
    XAttrs::iterator it = GetAttrIterator( attr );
    if(*it != NULL)
    {
        attrs.erase( it );
        return attr;
    }
    return NULL;
}

void _tagXMLNode::CopyNode( LPXNode node )
{
    Close();

    doc = node->doc;
    parent = node->parent;
    name = node->name;
    value = node->value;
    type = node->type;

    // copy attributes
    for( int i = 0 ; i < (int)node->attrs.size(); i++)
    {
        LPXAttr attr = node->attrs[i];
        if( attr )
            AppendAttr( attr->name.c_str(), attr->value.c_str() );
    }
}

void _tagXMLNode::_CopyBranch( LPXNode node )
{
    CopyNode( node );

    for( int i = 0 ; i < (int)node->childs.size(); i++)
    {
        LPXNode child = node->childs[i];
        if( child )
        {
            LPXNode mychild = new XNode;
            mychild->CopyNode( child );
            AppendChild( mychild );

            mychild->_CopyBranch( child );
        }
    }
}

LPXNode _tagXMLNode::AppendChildBranch( LPXNode node )
{
    LPXNode child = new XNode;
    child->CopyBranch( node );

    return AppendChild( child );
}

void _tagXMLNode::CopyBranch( LPXNode branch )
{
    Close();

    _CopyBranch( branch );
}


_tagXMLEntitys::_tagXMLEntitys( LPXENTITY entities, int count )
{
    for( int i = 0; i < count; i++)
        push_back( entities[i] );
}

LPXENTITY _tagXMLEntitys::GetEntity( int entity )
{
    for( int i = 0 ; i < (int)size(); i ++ )
    {
        if( at(i).entity == entity )
            return LPXENTITY(&at(i));
    }
    return NULL;
}

LPXENTITY _tagXMLEntitys::GetEntity( const char* entity )
{
    for( int i = 0 ; i < (int)size(); i ++ )
    {
        const char* ref = (const char*)at(i).ref;
        const char* ps = entity;
        while( ref && *ref )
            if( *ref++ != *ps++ )
                break;
        if( ref && !*ref )  // found!
            return LPXENTITY(&at(i));
    }
    return NULL;
}

int _tagXMLEntitys::GetEntityCount( const char* str )
{
    int nCount = 0;
    const char* ps = (const char*)str;
    while( ps && *ps )
        if( GetEntity( *ps++ ) ) nCount ++;
    return nCount;
}

int _tagXMLEntitys::Ref2Entity( const char* estr, const char* str, int strlen )
{
    const char* pes = (const char*)estr;
    char* ps = (char*)str;
    const char* ps_end = ps+strlen;
    while( pes && *pes && ps < ps_end )
    {
        LPXENTITY ent = GetEntity( pes );
        if( ent )
        {
            // copy entity meanning char
            *ps = ent->entity;
            pes += ent->ref_len;
        }
        else
            *ps = *pes++;   // default character copy
        ps++;
    }
    *ps = '\0';

    // total copied characters
    return ps-str;
}

int _tagXMLEntitys::Entity2Ref( const char* str, const char* estr, int estrlen )
{
    const char* ps = (const char*)str;
    char* pes = (char*)estr;
    const char* pes_end = pes+estrlen;
    while( ps && *ps && pes < pes_end )
    {
        LPXENTITY ent = GetEntity( *ps );
        if( ent )
        {
            // copy entity string
            const char* ref = (const char*)ent->ref;
            while( ref && *ref )
                *pes++ = *ref++;
        }
        else
            *pes++ = *ps;   // default character copy
        ps++;
    }
    *pes = '\0';

    // total copied characters
    return pes-estr;
}

string _tagXMLEntitys::Ref2Entity( const char* estr )
{
    string es;
    if( estr )
    {
        int len = strlen(estr);
        char* esbuf = new char[len+1];
        if( esbuf )
            Ref2Entity( estr, esbuf, len );
        es = esbuf;
    }
    return es;
}

string _tagXMLEntitys::Entity2Ref( const char* str )
{
    string s;
    if( str )
    {
        int nEntityCount = GetEntityCount(str);
        if( nEntityCount == 0 )
            return string(str);
        int len = strlen(str) + nEntityCount*10 ;
        const char* sbuf = new char[len+1];
        if( sbuf )
            Entity2Ref( str, sbuf, len );
        s = sbuf;
    }
    return s;
}

string XRef2Entity( const char* estr )
{
    return entityDefault.Ref2Entity( estr );
}

string XEntity2Ref( const char* str )
{
    return entityDefault.Entity2Ref( str );
}
