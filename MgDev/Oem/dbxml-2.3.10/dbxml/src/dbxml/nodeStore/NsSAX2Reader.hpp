//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsSAX2Reader.hpp,v 1.18 2006/10/30 17:45:57 bostic Exp $
//
// The code in this file is derived from Apache's XmlSAX2Reader code
/*
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 1999-2003 The Apache Software Foundation.  All rights
 * reserved.
 *
 */

#ifndef __DBXMLNSSAX_HPP
#define __DBXMLNSSAX_HPP

#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/Parser.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/internal/VecAttributesImpl.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/util/RefStackOf.hpp>
#include <xercesc/util/SecurityManager.hpp>
#include <xercesc/util/ValueStackOf.hpp>
#include <xercesc/framework/XMLBufferMgr.hpp>

#include "NsEvent.hpp"
#include "../dataItem/DbXmlURIResolver.hpp"

XERCES_CPP_NAMESPACE_BEGIN

class DeclHandler;
class GrammarResolver;
class XMLGrammarPool;
class XMLResourceIdentifier;
class PSVIHandler;

XERCES_CPP_NAMESPACE_END

namespace DbXml {

class XmlInputStream;
class XmlManager;
class Transaction;

/**
 * This is a stripped-down version of the Xerces SAX2Reader.
 * It does not generate any SAX2 events, and therefore has no
 * support for set/get of SAX2 handlers.  Additional pieces may
 * yet be removed.
 *
 * It communicates with BDB XML through the class NsEventHandler16,
 * and by implementing an instance of NsEventAttrList.
 *
 * It also takes code from the Xerces AbstractDOMParser in order
 * to turn an internal subset (DTD) into a single text string, for
 * round-tripping purposes.
 */
class NsSAX2Reader :
	public XER_NS XMemory,
	public XER_NS SAX2XMLReader,
	public XER_NS XMLDocumentHandler,
	public XER_NS XMLErrorReporter,
	public XER_NS XMLEntityHandler,
	public XER_NS DocTypeHandler
{
public :
	/* construction. DbEnv is for error and warning logging */
	NsSAX2Reader(XmlManager &mgr, Transaction *txn, unsigned int flags,
		     XER_NS MemoryManager* const manager =
		     XER_NS XMLPlatformUtils::fgMemoryManager,
		     XER_NS XMLGrammarPool* const gramPool = 0);
	~NsSAX2Reader();

	void setNsEventHandler(NsEventHandler16 *handler) { fHandler = handler; }
	NsEventHandler16 *getNsEventHandler() const {
		return fHandler;
	}

	// Some methods that have been eliminated by BDB XML
	// They must remain, in stub form -- they are pure virtual.
	virtual XER_NS ContentHandler* getContentHandler() const { return NULL; }
	virtual XER_NS DTDHandler* getDTDHandler() const { return NULL; }
	virtual XER_NS ErrorHandler* getErrorHandler() const { return NULL; }
	virtual void setContentHandler(XER_NS ContentHandler* const handler) {}
	virtual void setDTDHandler(XER_NS DTDHandler* const handler) {}
        virtual void setErrorHandler(XER_NS ErrorHandler* const handler) {}
	virtual XER_NS LexicalHandler* getLexicalHandler() const { return NULL; }
	virtual void setLexicalHandler(XER_NS LexicalHandler* const handler) {}

	// The SAX2XMLReader Interface
        virtual XER_NS EntityResolver* getEntityResolver() const { return NULL; }
        virtual XER_NS XMLEntityResolver* getXMLEntityResolver() const { return NULL; }
	virtual XER_NS PSVIHandler* getPSVIHandler() const;
        virtual void setXMLEntityResolver(XER_NS XMLEntityResolver* const resolver) {}
	virtual void setPSVIHandler(XER_NS PSVIHandler* const handler);

	// feature/property get/set
	virtual bool getFeature(const XMLCh* const name) const;
	virtual void* getProperty(const XMLCh* const name) const;
        virtual void setEntityResolver(XER_NS EntityResolver* const resolver) {}
	virtual void setFeature(const XMLCh* const name, const bool value);
	virtual void setProperty(const XMLCh* const name, void* value);
	virtual void parse(XmlInputStream **is); // Consumes XmlInputStream
	virtual void parse(const XER_NS InputSource& source);
	virtual void parse(const XMLCh* const systemId);
	virtual void parse(const char* const systemId);
	
	// SAX 2.0-ext
	virtual XER_NS DeclHandler* getDeclarationHandler() const { return 0; }
	virtual void setDeclarationHandler(XER_NS DeclHandler* const handler) {}

	// Getter Methods
	virtual XER_NS XMLValidator* getValidator() const;
	virtual int getErrorCount() const;
	virtual bool getExitOnFirstFatalError() const;
	virtual bool getValidationConstraintFatal() const;
	virtual XER_NS Grammar* getGrammar(const XMLCh* const nameSpaceKey);
	virtual XER_NS Grammar* getRootGrammar();
	virtual const XMLCh* getURIText(unsigned int uriId) const;
	virtual unsigned int getSrcOffset() const;

	//  Setter Methods
	virtual void setValidator(XER_NS XMLValidator* valueToAdopt);
	virtual void setExitOnFirstFatalError(const bool newState);
	virtual void setValidationConstraintFatal(const bool newState);

	//  Progressive scan methods (not implemented)
	virtual bool parseFirst (const XMLCh* const systemId,
				 XER_NS XMLPScanToken&  toFill);
	virtual bool parseFirst (const char* const systemId,
				 XER_NS XMLPScanToken&  toFill);
	virtual bool parseFirst (const XER_NS InputSource& source,
				 XER_NS XMLPScanToken&  toFill);
	virtual bool parseNext(XER_NS XMLPScanToken& token);
	virtual void parseReset(XER_NS XMLPScanToken& token);

	// Grammar preparsing interface
	virtual XER_NS Grammar* loadGrammar(const XER_NS InputSource& source,
					    const short grammarType,
					    const bool toCache = false);
	virtual XER_NS Grammar* loadGrammar(const XMLCh* const systemId,
					    const short grammarType,
					    const bool toCache = false);
	virtual XER_NS Grammar* loadGrammar(const char* const systemId,
					    const short grammarType,
					    const bool toCache = false);
	virtual void resetCachedGrammarPool();

	// These two are not implemented
	virtual void installAdvDocHandler(XER_NS XMLDocumentHandler* const toInstall)
		{}
	virtual bool removeAdvDocHandler(XER_NS XMLDocumentHandler* const toRemove)
		{ return false; }

	//
	// End SAX2XMLReader interface
	//

	//
	// XMLDocumentHandler interface
	//
	virtual void docCharacters(const XMLCh* const chars,
				   const unsigned int length,
				   const bool cdataSection);

	virtual void docComment(const XMLCh* const comment);
	virtual void docPI(const XMLCh* const target,
			   const XMLCh* const data);
	virtual void endDocument();
	virtual void endElement(const XER_NS XMLElementDecl& elemDecl,
				const unsigned int urlId,
				const bool isRoot,
				const XMLCh* const elemPrefix=0);
	virtual void endEntityReference(const XER_NS XMLEntityDecl& entDecl);
	virtual void ignorableWhitespace(const XMLCh* const chars,
					 const unsigned int length,
					 const bool cdataSection);
	virtual void resetDocument();
	virtual void startDocument();
	virtual void startElement(const XER_NS XMLElementDecl& elemDecl,
				  const unsigned int urlId,
				  const XMLCh* const elemPrefix,
				  const XER_NS RefVectorOf<XER_NS XMLAttr>& attrList,
				  const unsigned int attrCount,
				  const bool isEmpty,
				  const bool isRoot);
	virtual void startEntityReference(const XER_NS XMLEntityDecl& entDecl);
	virtual void XMLDecl(const XMLCh* const versionStr,
			     const XMLCh* const encodingStr,
			     const XMLCh* const standaloneStr,
			     const XMLCh* const autoEncodingStr);
	// experimental
	virtual void elementTypeInfo(const XMLCh* const typeName,
				     const XMLCh* const typeURI);

	//
	// XMLErrorReporter interface
	//
	virtual void error(const unsigned int errCode,
			   const XMLCh* const msgDomain,
			   const XER_NS XMLErrorReporter::ErrTypes errType,
			   const XMLCh* const errorText,
			   const XMLCh* const systemId,
			   const XMLCh* const publicId,
			   const XMLSSize_t lineNum,
			   const XMLSSize_t colNum);
	virtual void resetErrors();

	//
	// XMLEntityHandler interface
	//
	virtual void endInputSource(const XER_NS InputSource& inputSource);
	virtual bool expandSystemId(const XMLCh* const systemId,
				    XER_NS XMLBuffer& toFill);
	virtual void resetEntities();
	virtual XER_NS InputSource* resolveEntity(const XMLCh* const publicId,
					   const XMLCh* const systemId,
					   const XMLCh* const baseURI = 0);
	virtual XER_NS InputSource* resolveEntity(XER_NS XMLResourceIdentifier*
					   resourceIdentifier);
	virtual void startInputSource(const XER_NS InputSource& inputSource);

	//
	// Deprecated DocTypeHandler Interface
	//
	virtual void attDef(const XER_NS DTDElementDecl& elemDecl,
			    const XER_NS DTDAttDef& attDef,
			    const bool ignoring);
	virtual void doctypeComment(const XMLCh* const comment);
	virtual void doctypeDecl(const XER_NS DTDElementDecl& elemDecl,
				 const XMLCh* const publicId,
				 const XMLCh* const systemId,
				 const bool hasIntSubset,
				 const bool hasExtSubset = false);
	virtual void doctypePI(const XMLCh* const target,
			       const XMLCh* const data);
	virtual void doctypeWhitespace(const XMLCh* const chars,
				       const unsigned int length);
	virtual void elementDecl(const XER_NS DTDElementDecl& decl,
				 const bool isIgnored);
	virtual void endAttList(const XER_NS DTDElementDecl& elemDecl);
	virtual void endIntSubset();
	virtual void endExtSubset();
	virtual void entityDecl(const XER_NS DTDEntityDecl& entityDecl,
				const bool isPEDecl,
				const bool isIgnored);
	virtual void resetDocType();
	virtual void notationDecl(const XER_NS XMLNotationDecl& notDecl,
				  const bool isIgnored);
	virtual void startAttList(const XER_NS DTDElementDecl& elemDecl);
	virtual void startIntSubset();
	virtual void startExtSubset();
	virtual void TextDecl(const XMLCh* const versionStr,
			      const XMLCh* const encodingStr);

	// Construction/initialization/cleanup
	NsSAX2Reader(const NsSAX2Reader&);
	NsSAX2Reader& operator=(const NsSAX2Reader&);
	void initialize(unsigned int flags);
	void cleanUp();

        //  Private data members
	//
	//  fElemDepth
	//      This is used to track the element nesting depth, so that we can
	//      know when we are inside content. This is so we can ignore char
	//      data outside of content.
	//
	//  fParseInProgress
	//      This flag is set once a parse starts. It is used to prevent
	//      multiple entrance or reentrance of the parser.
	//
	//  fScanner
	//      The scanner being used by this parser. It is created internally
	//      during construction.
	//
	//  fGrammarPool
	//      The grammar pool passed from external application
	//      (through derivatives),  which could be 0, not owned.
	//
	// ------------------------------------------------------------------
	bool                        fParseInProgress;
	unsigned int                fElemDepth;
	XER_NS PSVIHandler*         fPSVIHandler;
	XER_NS XMLScanner*          fScanner;
	XER_NS GrammarResolver*     fGrammarResolver;
	XER_NS XMLStringPool*       fURIStringPool;
	XER_NS XMLValidator*        fValidator;
	XER_NS MemoryManager*       fMemoryManager;
	XER_NS XMLGrammarPool*      fGrammarPool;
	// Added for DB XML
	XER_NS XMLBufferMgr         fBufMgr;
	XER_NS XMLBuffer&           fSubset;
	bool                        fReadingIntSubset;
	bool                        fHasExtSubset;
	bool                        fHasIntSubset;
	NsEventHandler16           *fHandler;
	XmlManager                 &fMgr;
	DbXmlURIResolver            fResolver;
	// end added for DBXML
	
	// ------------------------------------------------------------------
	// internal function used to set the state of the parser
	// ------------------------------------------------------------------
	void setValidationScheme(const ValSchemes newScheme);
	void setDoNamespaces(const bool newState);
	bool getDoNamespaces() const;
	void setDoSchema(const bool newState);
	bool getDoSchema() const;
};

/**
 * NsParserEventSource
 *
 * An implementation of NsPushEventSource that uses the NsSAX2Reader
 * to generate events from an InputSource.
 */
class NsParserEventSource : public NsPushEventSource16
{
public:
	enum NsParserFlags {
		NS_PARSER_WELL_FORMED =   0x01,
		NS_PARSER_MUST_VALIDATE = 0x02,
		NS_PARSER_WF_ONLY       = 0x04
	};
	NsParserEventSource(XmlManager &mgr, Transaction *txn,
			    unsigned int flags, XmlInputStream **input)
		: parser_(mgr, txn, flags), input_(input) {}

	virtual void start() {
		parser_.parse(input_);
	}
	virtual NsEventHandler16 *getNsEventHandler16(void) const {
		return parser_.getNsEventHandler();
	}
	virtual void setNsEventHandler16(NsEventHandler16 *handler) {
		parser_.setNsEventHandler(handler);
	}

	NsSAX2Reader &getParser() {
		return parser_;
	}

private:
	NsSAX2Reader parser_;
	XmlInputStream **input_;
};

inline XER_NS PSVIHandler* NsSAX2Reader::getPSVIHandler() const
{
	return fPSVIHandler;
}

inline bool NsSAX2Reader::getExitOnFirstFatalError() const
{
    return fScanner->getExitOnFirstFatal();
}

inline bool NsSAX2Reader::getValidationConstraintFatal() const
{
    return fScanner->getValidationConstraintFatal();
}

inline XER_NS Grammar* NsSAX2Reader::getRootGrammar()
{
    return fScanner->getRootGrammar();
}

inline const XMLCh* NsSAX2Reader::getURIText(unsigned int uriId) const
{
    return fScanner->getURIText(uriId);
}

inline unsigned int NsSAX2Reader::getSrcOffset() const
{
    return fScanner->getSrcOffset();
}

}

#endif
