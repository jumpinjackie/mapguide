/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: XQillaLexer.hpp,v 1.10 2006/11/01 16:37:21 jpcs Exp $
 */

#ifndef XQILLA_LEXER_H
#define XQILLA_LEXER_H

#define YY_CHAR unsigned short

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLChar.hpp>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/simple-api/XQilla.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/exceptions/XQException.hpp>

typedef struct yyltype
{
  int first_line;
  int first_column;

  int last_line;
  int last_column;

  int first_offset;
  int last_offset;
} yyltype;
#define YYLTYPE yyltype
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1

#define YY_USER_ACTION  userAction(yytext, yyleng);

#define YYLLOC_DEFAULT(Current, Rhs, N)         \
  do                  \
    if (N)                \
    {                \
      (Current).first_line   = (Rhs)[1].first_line;  \
      (Current).first_column = (Rhs)[1].first_column;  \
      (Current).last_line    = (Rhs)[N].last_line;    \
      (Current).last_column  = (Rhs)[N].last_column;  \
      (Current).first_offset = (Rhs)[1].first_offset;  \
      (Current).last_offset  = (Rhs)[N].last_offset;   \
    }                \
    else                \
    {                \
      (Current).first_line   = (Current).last_line   =    \
        (Rhs)[0].last_line;        \
      (Current).first_column = (Current).last_column =    \
        (Rhs)[0].last_column;        \
      (Current).first_offset = (Current).last_offset =    \
        (Rhs)[0].last_offset;        \
    }                \
  while (0)

class XQILLA_API CXQillaLexer
{
public:
  virtual ~CXQillaLexer() {}
  CXQillaLexer(XPath2MemoryManager* memMgr, const XMLCh *queryFile, const XMLCh* query, XQilla::Language lang)
  {
    m_index=m_position=0;
    m_lineno=m_columnno=1;
    m_memMgr=memMgr;
    m_szQueryFile=queryFile;
    m_szQuery=XPath2Utils::normalizeEOL(query, m_memMgr);
    m_nLength=XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(m_szQuery);
    m_nOpenComments=0;
    m_bGenerateEOF=m_bReportWhiteSpace=false;
    m_bGenerateErrorException=true;
    m_language = lang;
  }

  const XMLCh* getQueryString() { return m_szQuery; }
  void setGenerateEOF(bool bEnable) { m_bGenerateEOF=bEnable; }
  void setGenerateWhiteSpace(bool bEnable) { m_bReportWhiteSpace=bEnable; }
  void setGenerateErrorException(bool bEnable) { m_bGenerateErrorException=bEnable; }

  virtual int error(const char* message)
  {
    Error(message, m_lineno, m_columnno);
    return 0;
  }

  virtual int error(const yyltype &pos, const char* message)
  {
    Error(message, pos.first_line, pos.first_column);
    return 0;
  }

  virtual int error(XQilla::Language lang, const char* where, unsigned int line, unsigned int column)
  {
    Error(lang, where, line, column);
    return 0;
  }

  virtual void beginComment()
  {
    m_nOpenComments++;
  }
  virtual void endComment()
  {
    if(m_nOpenComments==0)
    {
      error("Unbalanced comment found");
      return;
    }
    m_nOpenComments--;
  }
  bool isCommentClosed()
  {
    return m_nOpenComments==0;
  }

  XMLCh* allocate_string(const XMLCh* src, int len=-1)
  {
    if(len==-1)
      return (XMLCh*)m_memMgr->getPooledString(src);
    return (XMLCh*)XPath2Utils::subString(src,0,len,m_memMgr);
  }

  void checkCharRef(XMLCh* text, int len)
  {
    if(len<2 || text[0]!=XERCES_CPP_NAMESPACE_QUALIFIER chAmpersand || text[1]!=XERCES_CPP_NAMESPACE_QUALIFIER chPound)
    {
      error("Invalid entity reference");
      return;
    }
    if(text[len-1]!=XERCES_CPP_NAMESPACE_QUALIFIER chSemiColon)
    {
      error("Unterminated entity reference");
      return;
    }
    int i=2;
    unsigned int radix = 10;
    if(text[i]==XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x)
    {
      i++;
      radix=16;
    }
    unsigned int value = 0;
    for(int q=i;q<len-1;q++)
    {
      unsigned int nextVal;
      XMLCh nextCh=text[q];
      if ((nextCh >= XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0) && (nextCh <= XERCES_CPP_NAMESPACE_QUALIFIER chDigit_9))
        nextVal = (unsigned int)(nextCh - XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0);
      else if ((nextCh >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A) && (nextCh <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F))
        nextVal= (unsigned int)(10 + (nextCh - XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A));
      else if ((nextCh >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a) && (nextCh <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f))
        nextVal = (unsigned int)(10 + (nextCh - XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a));
      else
    {
      error("Unterminated entity reference");
      return;
    }
    if (nextVal >= radix)
    {
      error("Invalid digit inside entity reference");
      return;
    }
    else
      value = (value * radix) + nextVal;
    }
    if(!XERCES_CPP_NAMESPACE_QUALIFIER XMLChar1_0::isXMLChar(value))
      error("Entity reference is not a valid XML character [err:XQST0090]");
  }

  XMLCh* allocate_string_and_unescape(XMLCh toBeUnescaped, const XMLCh* src, int len)
  {
    int j=0;
    XMLCh* dst=(XMLCh*)m_memMgr->allocate((len+1)*sizeof(XMLCh));
    for(int i=0;i<len;i++)
    {
      if(src[i]=='&')
      {
        static XMLCh szAmp[]={  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m,
                                XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chSemiColon,
                                XERCES_CPP_NAMESPACE_QUALIFIER chNull };
        static XMLCh szQuot[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u,
                                XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
                                XERCES_CPP_NAMESPACE_QUALIFIER chSemiColon, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
        static XMLCh szApos[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p,
                                XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s,
                                XERCES_CPP_NAMESPACE_QUALIFIER chSemiColon, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
        static XMLCh szLt[]={   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
                                XERCES_CPP_NAMESPACE_QUALIFIER chSemiColon, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
        static XMLCh szGt[]={   XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t,
                                XERCES_CPP_NAMESPACE_QUALIFIER chSemiColon, XERCES_CPP_NAMESPACE_QUALIFIER chNull };
        // entity reference
        if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareNString(src+i+1,szAmp,4)==0) {
          dst[j++]='&'; i+=4;
        } else if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareNString(src+i+1,szQuot,5)==0) {
          dst[j++]='"'; i+=5;
        } else if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareNString(src+i+1,szApos,5)==0) {
          dst[j++]='\''; i+=5;
        } else if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareNString(src+i+1,szLt,3)==0) {
          dst[j++]='<'; i+=3;
        } else if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::compareNString(src+i+1,szGt,3)==0) {
          dst[j++]='>'; i+=3;
        } else if(*(src+i+1)==XERCES_CPP_NAMESPACE_QUALIFIER chPound) {
          unsigned int value = 0;
          unsigned int radix = 10;
          i+=2;
          if(*(src+i)==XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x)
          {
            i++;
            radix=16;
          }
          int k=i;
          while(k<len && src[k]!=';') k++;
          if(k==len)
            error("Unterminated entity reference");
          else
          {
            for(int q=i;q<k;q++)
            {
              unsigned int nextVal=255;
              XMLCh nextCh=*(src+q);
              if ((nextCh >= XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0) && (nextCh <= XERCES_CPP_NAMESPACE_QUALIFIER chDigit_9))
                nextVal = (unsigned int)(nextCh - XERCES_CPP_NAMESPACE_QUALIFIER chDigit_0);
              else if ((nextCh >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A) && (nextCh <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_F))
                nextVal= (unsigned int)(10 + (nextCh - XERCES_CPP_NAMESPACE_QUALIFIER chLatin_A));
              else if ((nextCh >= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a) && (nextCh <= XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f))
                nextVal = (unsigned int)(10 + (nextCh - XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a));
              else
                error("Invalid character inside entity reference");
              if (nextVal >= radix)
                error("Invalid digit inside entity reference");
              else
                value = (value * radix) + nextVal;
            }
            if(!XERCES_CPP_NAMESPACE_QUALIFIER XMLChar1_0::isXMLChar(value))
              error("Entity reference is not a valid XML character");
            else
            {
              if (value <= 0xFFFD)
                dst[j++]=value;
              else if (value >= 0x10000 && value <= 0x10FFFF)
              {
                value -= 0x10000;
                dst[j++]= XMLCh((value >> 10) + 0xD800);
                dst[j++]= XMLCh((value & 0x3FF) + 0xDC00);
              }
            }
            i=k;
          }
        } else
	      error("Invalid entity reference");
      }
	  else
        dst[j++]=src[i];
      if(src[i]==toBeUnescaped && (i+1)<len && src[i]==src[i+1])
        i++;
    }
    dst[j++]=0;
    return dst;
  }

  XQilla::Language getLanguage() const { return m_language; }
  bool isFullText() const { return m_language == XQilla::XQUERY_FULLTEXT || m_language == XQilla::XPATH2_FULLTEXT; }
  bool isXQuery() const { return m_language == XQilla::XQUERY || m_language == XQilla::XQUERY_FULLTEXT; }
  bool isXPath() const { return m_language == XQilla::XPATH2 || m_language == XQilla::XPATH2_FULLTEXT; }

protected:
  virtual int Read( YY_CHAR* buf, int max_size )
  {
    int nToRead=m_nLength-m_position;
    if(nToRead==0)
      return 0;

    if(nToRead>max_size)
      nToRead=max_size;
    XERCES_CPP_NAMESPACE_QUALIFIER XMLString::subString((XMLCh*)buf,m_szQuery,m_position,m_position+nToRead);
    m_position+=nToRead;
    return nToRead;
  }

  virtual void Error( const char* msg, int line, int col )
  {
    if(!m_bGenerateErrorException)
      return;
    if(strstr(msg, "[err:")!=NULL)
      XQSimpleThrow(X(msg), m_szQueryFile, m_lineno, m_columnno);
    const XMLCh* szMsg=XPath2Utils::concatStrings(X(msg), X(" [err:XPST0003]"), m_memMgr);
    XQSimpleThrow(szMsg, m_szQueryFile, m_lineno, m_columnno);
  }

  virtual void Error(XQilla::Language lang, const char *where, unsigned int line, unsigned int col)
  {
		if(!m_bGenerateErrorException)
			return;
    char *l;
    switch(lang) {
    case XQilla::XQUERY: l = "XQuery"; break;
    case XQilla::XPATH2: l = "XPath 2.0"; break;
    case XQilla::XQUERY_FULLTEXT: l = "XQuery Full-Text"; break;
    case XQilla::XPATH2_FULLTEXT: l = "XPath 2.0 Full-Text"; break;
    }

    const XMLCh *szMsg = XPath2Utils::concatStrings(X("Invalid "), X(l), X(" syntax: "), m_memMgr);
    szMsg = XPath2Utils::concatStrings(szMsg, X(where), X(" [err:XPST0003]"), m_memMgr);
    XQSimpleThrow(szMsg, m_szQueryFile, line, col);
  }

  virtual void userAction(YY_CHAR* text, int length)
  {
    m_yyloc.first_line=m_lineno; m_yyloc.first_column=m_columnno; m_yyloc.first_offset=m_index;
    m_index+=length;
    for(int i=0;i<length;i++)
      {
        if(text[i]=='\n')
          {
            m_lineno++;
            m_columnno=1;
          }
        else if(text[i]!='\r')
          m_columnno++;
      }
    m_yyloc.last_line=m_lineno;
    m_yyloc.last_column=m_columnno;
    m_yyloc.last_offset=m_index -1;
  }

  virtual void undoUserAction()
  {
    // undo the changes done in the user action, or there will be a hole in the (first,last) sequence
    m_lineno=m_yyloc.last_line=m_yyloc.first_line;
    m_columnno=m_yyloc.last_column=m_yyloc.first_column;
    m_index=m_yyloc.last_offset=m_yyloc.first_offset;
  }

  const XMLCh* m_szQueryFile;
  const XMLCh* m_szQuery;
  unsigned int m_nLength;
  int m_position,m_index;
  XPath2MemoryManager* m_memMgr;
  int m_lineno,m_columnno;

  YYLTYPE m_yyloc;

  int m_nOpenComments;
  bool m_bGenerateEOF, m_bReportWhiteSpace, m_bGenerateErrorException;

  XQilla::Language m_language;
};

#endif // #ifdef XQILLA_LEXER_H

