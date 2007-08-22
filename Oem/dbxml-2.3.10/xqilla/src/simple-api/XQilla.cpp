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
 * $Id: XQilla.cpp,v 1.14 2006/11/01 16:37:22 jpcs Exp $
 */

//////////////////////////////////////////////////////////////////////
// XQEvaluator.cpp: implementation of the XQEvaluator class.
//////////////////////////////////////////////////////////////////////

#include <xqilla/simple-api/XQilla.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/context/XQDebugCallback.hpp>
#include <xqilla/exceptions/ContextException.hpp>
#include <xqilla/context/impl/XQContextImpl.hpp>
#include <xqilla/utils/XQillaPlatformUtils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include "../lexer/XQLexer.hpp"

#include <xercesc/util/XMLURL.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/framework/XMLRecognizer.hpp>
#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/util/Janitor.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XQilla::XQilla(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr)
{
  XQillaPlatformUtils::initialize(memMgr);
}

XQilla::~XQilla()
{
  XQillaPlatformUtils::terminate();
}

XQQuery* XQilla::parse(const XMLCh* inputQuery, Language language, DynamicContext* context,
                       const XMLCh* queryFile, unsigned int flags,
                       MemoryManager *memMgr)
{
  bool contextOwned = (flags & NO_ADOPT_CONTEXT) == 0;
  if(context == 0) {
    contextOwned = true;
    context = createContext();
  }

  Janitor<XQQuery> query(new (memMgr) XQQuery(inputQuery, context, contextOwned, memMgr));

  try {
    XQLexer lexer(context->getMemoryManager(), queryFile, inputQuery, language);

    XQParserArgs args;
    args._context=context;
    args._lexer=&lexer;
    args._query=query.get();
    args._query->setFile(queryFile);

    XQParser::yyparse(&args);

    // Perform static resolution, if requested
    if((flags & NO_STATIC_RESOLUTION) == 0) {
      args._query->staticResolution(context);
    }
  }
  catch(XQException& e) {
    // parsing errors and staticResolution don't invoke ReportFirstError, so do it here
    if(context->getDebugCallback() && context->isDebuggingEnabled()) 
      context->getDebugCallback()->ReportFirstError(context, e.getError(), queryFile, e.getXQueryLine());
    throw e;
  }

  return query.release();
}

XQQuery* XQilla::parse(const InputSource& querySrc, Language language, DynamicContext* context,
                       unsigned int flags, MemoryManager *memMgr)
{
  XMLBuffer moduleText;
  try {
    if(!readQuery(querySrc, memMgr, moduleText))
    {
      XMLBuffer buf(1023,context->getMemoryManager());
      buf.set(X("Cannot read query content from "));
      buf.append(querySrc.getSystemId());
      buf.append(X(" [err:XQST0059]"));
      XQThrow2(ContextException,X("XQilla::parse"), buf.getRawBuffer());
    }
  } catch(XMLException& e) {
    XMLBuffer buf(1023,context->getMemoryManager());
    buf.set(X("Exception reading query content: "));
    buf.append(e.getMessage());
    XQThrow2(ContextException,X("XQilla::parse"), buf.getRawBuffer());
  }

  return parse(moduleText.getRawBuffer(), language, context, querySrc.getSystemId(), flags, memMgr);
}

XQQuery* XQilla::parseFromURI(const XMLCh* queryFile, Language language, DynamicContext* context,
                              unsigned int flags, MemoryManager *memMgr)
{
  XMLBuffer moduleText;
  try {
    if(!readQuery(queryFile, memMgr, moduleText))
    {
      XMLBuffer buf(1023,context->getMemoryManager());
      buf.set(X("Cannot read query content from "));
      buf.append(queryFile);
      buf.append(X(" [err:XQST0059]"));
      XQThrow2(ContextException,X("XQilla::parseFromURI"), buf.getRawBuffer());
    }
  } catch(XMLException& e) {
    XMLBuffer buf(1023,context->getMemoryManager());
    buf.set(X("Exception reading query content: "));
    buf.append(e.getMessage());
    XQThrow2(ContextException,X("XQilla::parseFromURI"), buf.getRawBuffer());
  }

  return parse(moduleText.getRawBuffer(), language, context, queryFile, flags, memMgr);
}

DynamicContext *XQilla::createContext(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr)
{
  return new (memMgr) XQContextImpl(memMgr, 0, 0);
}

#ifdef _DEBUG
  #define BUFFER_SIZE 128
#else
  #define BUFFER_SIZE 8192
#endif

bool XQilla::readQuery(const XMLCh* queryFile, MemoryManager* memMgr, XMLBuffer& queryText)
{
    Janitor<InputSource> srcToFill(NULL);
    try {
      XMLURL urlTmp(queryFile);
      if (urlTmp.isRelative())
        throw MalformedURLException(__FILE__, __LINE__, XMLExcepts::URL_NoProtocolPresent);
      srcToFill.reset(new (memMgr) URLInputSource(urlTmp));
    }
    catch(const MalformedURLException&) {
      srcToFill.reset(new (memMgr) LocalFileInputSource(queryFile));
    }
    return readQuery(*srcToFill.get(), memMgr, queryText);
}

bool XQilla::readQuery(const InputSource& querySrc, MemoryManager* memMgr, XMLBuffer& queryText)
{
    BinInputStream* stream=querySrc.makeStream();
    if(stream==NULL)
      return false;
    Janitor<BinInputStream> janStream(stream);
    XMLByte buffer[BUFFER_SIZE];
    unsigned int nRead=stream->readBytes(buffer,BUFFER_SIZE);
    Janitor<XMLTranscoder> transcoder(NULL);
    XMLTransService::Codes retCode;
    if(querySrc.getEncoding()==NULL) {
      XMLRecognizer::Encodings encoding=XMLRecognizer::basicEncodingProbe(buffer,BUFFER_SIZE);
      transcoder.reset(XMLPlatformUtils::fgTransService->makeNewTranscoderFor(encoding, retCode, BUFFER_SIZE, memMgr));
    }
    else {
      transcoder.reset(XMLPlatformUtils::fgTransService->makeNewTranscoderFor(querySrc.getEncoding(), retCode, BUFFER_SIZE, memMgr));
    }
    XMLCh tempBuff[BUFFER_SIZE];
    unsigned char charSizes[BUFFER_SIZE];
    unsigned int bytesEaten=0, nOffset=0;
    unsigned int nCount=transcoder->transcodeFrom(buffer,nRead,tempBuff,BUFFER_SIZE,bytesEaten,charSizes);
    queryText.set(tempBuff,nCount);
    if(bytesEaten<nRead){
      nOffset=nRead-bytesEaten;
      memmove(buffer, buffer+bytesEaten, nRead-bytesEaten);
    }

    while((nRead=stream->readBytes(buffer+nOffset,BUFFER_SIZE-nOffset))>0) {
      nCount=transcoder->transcodeFrom(buffer,nRead,tempBuff,BUFFER_SIZE,bytesEaten,charSizes);
      queryText.append(tempBuff,nCount);
      if(bytesEaten<nRead) {
        nOffset=nRead-bytesEaten;
        memmove(buffer, buffer+bytesEaten, nRead-bytesEaten);
      }
    }
    return true;
}
