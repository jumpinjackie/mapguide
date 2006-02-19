//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQEvaluator.cpp,v 1.15 2005/07/14 15:21:11 jsnelson Exp $
//

//////////////////////////////////////////////////////////////////////
// XQEvaluator.cpp: implementation of the XQEvaluator class.
//////////////////////////////////////////////////////////////////////

#include "xquery/XQEngine.hpp"
#include "xquery/XQEvaluator.hpp"
#include "xquery/XQQuery.hpp"
#include "xquery/XQDebugCallback.hpp"
#include "Scanner.hpp"

#include <xercesc/util/XMLURL.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/framework/XMLRecognizer.hpp>
#include <xercesc/framework/URLInputSource.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/util/Janitor.hpp>

#include <pathan/dataItem/StaticResolutionContext.hpp>
#include <pathan/exceptions/ContextException.hpp>
#include <pathan/DynamicContext.hpp>

/*static*/ XQQuery* XQEvaluator::parse(const XMLCh* inputQuery, XQContext* context, bool staticallyResolve/*=true*/, const XMLCh* queryFile/*=NULL*/)
{
  XQueryParserArgs args;
  try
  {
      CXQueryScanner scanner(context->getMemoryManager(), inputQuery);

      args._context=context;
      args._scanner=&scanner;
      args._query=new (context->getMemoryManager()) XQQuery(inputQuery, context->getMemoryManager());
      args._query->setFile(queryFile);

	    XQuery::yyparse(&args);

      // Perform static resolution, if requested
      if(staticallyResolve) {
        args._query->staticResolution(context);
      }
  }
  catch(XQException& e)
  {
      // parsing errors and staticResolution don't invoke ReportFirstError, so do it here
      if(context->getDebugCallback() && context->isDebuggingEnabled()) 
          context->getDebugCallback()->ReportFirstError(context, e.getError(), queryFile, e.m_nLine);
      throw XQException(e.getError(),e.m_szFile?e.m_szFile:queryFile,e.m_nLine,e.m_nColumn);
  }
  catch(DSLException& e)
  {
      if(context->getDebugCallback() && context->isDebuggingEnabled()) 
          context->getDebugCallback()->ReportFirstError(context, e.getError(), queryFile, 0);
      throw XQException(e.getError(),queryFile,0,0);
  }

  return args._query;
}

/*static*/ XQQuery* XQEvaluator::parse(const XERCES_CPP_NAMESPACE_QUALIFIER InputSource& querySrc, XQContext* context, bool staticallyResolve/*=true*/)
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer moduleText;
  if(!XQEvaluator::readQuery(querySrc, context->getMemoryManager(), moduleText)) {
    DSLthrow(ContextException,X("XQEvaluator::parse"), X("Exception reading query content"));
  }

  return parse(moduleText.getRawBuffer(), context, staticallyResolve, querySrc.getSystemId());
}

/*static*/ XQQuery* XQEvaluator::parseFromURI(const XMLCh* queryFile, XQContext* context, bool staticallyResolve/*=true*/)
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer moduleText;
  if(!XQEvaluator::readQuery(queryFile, context->getMemoryManager(), moduleText)) {
    DSLthrow(ContextException,X("XQEvaluator::parseFromUri"), X("Exception reading query content"));
  }

  return parse(moduleText.getRawBuffer(), context, staticallyResolve, queryFile);
}

#ifdef _DEBUG
  #define BUFFER_SIZE 128
#else
  #define BUFFER_SIZE 8192
#endif

/*static*/ bool XQEvaluator::readQuery(const XMLCh* queryFile, XPath2MemoryManager* memMgr, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& queryText)
{
    XERCES_CPP_NAMESPACE_QUALIFIER InputSource* srcToFill=NULL;
    try
    {
        XERCES_CPP_NAMESPACE_QUALIFIER XMLURL urlTmp(queryFile);
        if (urlTmp.isRelative())
            throw XERCES_CPP_NAMESPACE_QUALIFIER MalformedURLException(__FILE__, __LINE__, XERCES_CPP_NAMESPACE_QUALIFIER XMLExcepts::URL_NoProtocolPresent);
        srcToFill = new (memMgr) XERCES_CPP_NAMESPACE_QUALIFIER URLInputSource(urlTmp);
    }
    catch(const XERCES_CPP_NAMESPACE_QUALIFIER MalformedURLException&)
    {
        srcToFill = new (memMgr) XERCES_CPP_NAMESPACE_QUALIFIER LocalFileInputSource(queryFile);
    }
    XERCES_CPP_NAMESPACE_QUALIFIER Janitor<XERCES_CPP_NAMESPACE_QUALIFIER InputSource> janSource(srcToFill);
    return readQuery(*srcToFill, memMgr, queryText);
}

/*static*/ bool XQEvaluator::readQuery(const XERCES_CPP_NAMESPACE_QUALIFIER InputSource& querySrc, XPath2MemoryManager* memMgr, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer& queryText)
{
	try
	{
        XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream* stream=querySrc.makeStream();
        if(stream==NULL)
            return false;
        XERCES_CPP_NAMESPACE_QUALIFIER Janitor<XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream> janStream(stream);
        XMLByte buffer[BUFFER_SIZE];
        unsigned int nRead=stream->readBytes(buffer,BUFFER_SIZE);
        XERCES_CPP_NAMESPACE_QUALIFIER XMLTranscoder* transcoder=NULL;
        XERCES_CPP_NAMESPACE_QUALIFIER XMLTransService::Codes retCode;
        if(querySrc.getEncoding()==NULL)
        {
            XERCES_CPP_NAMESPACE_QUALIFIER XMLRecognizer::Encodings encoding=XERCES_CPP_NAMESPACE_QUALIFIER XMLRecognizer::basicEncodingProbe(buffer,BUFFER_SIZE);
            transcoder=XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgTransService->makeNewTranscoderFor(encoding, retCode, BUFFER_SIZE, memMgr);
        }
        else
            transcoder=XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgTransService->makeNewTranscoderFor(querySrc.getEncoding(), retCode, BUFFER_SIZE, memMgr);
        XERCES_CPP_NAMESPACE_QUALIFIER Janitor<XERCES_CPP_NAMESPACE_QUALIFIER XMLTranscoder> janTrans(transcoder);
        XMLCh tempBuff[BUFFER_SIZE];
        unsigned char charSizes[BUFFER_SIZE];
        unsigned int bytesEaten=0, nOffset=0;
        unsigned int nCount=transcoder->transcodeFrom(buffer,nRead,tempBuff,BUFFER_SIZE,bytesEaten,charSizes);
        queryText.set(tempBuff,nCount);
        if(bytesEaten<nRead)
        {
            nOffset=nRead-bytesEaten;
            memmove(buffer, buffer+bytesEaten, nRead-bytesEaten);
        }

        while((nRead=stream->readBytes(buffer+nOffset,BUFFER_SIZE-nOffset))>0)
        {
            nCount=transcoder->transcodeFrom(buffer,nRead,tempBuff,BUFFER_SIZE,bytesEaten,charSizes);
            queryText.append(tempBuff,nCount);
            if(bytesEaten<nRead)
            {
                nOffset=nRead-bytesEaten;
                memmove(buffer, buffer+bytesEaten, nRead-bytesEaten);
            }
        }
	}
	catch(...)
	{
        return false;
	}
    return true;
}
