//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Modify.cpp,v 1.41.2.1 2007/01/18 19:36:13 gmf Exp $
//

#include "DbXmlInternal.hpp"
#include <iostream>
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlQueryExpression.hpp"
#include "dbxml/XmlResults.hpp"
#include "dbxml/XmlUpdateContext.hpp"
#include "QueryExpression.hpp"
#include "QueryContext.hpp"
#include "Modify.hpp"
#include "Results.hpp"
#include "Transaction.hpp"
#include "Document.hpp"
#include "Manager.hpp"
#include "nodeStore/NsDocument.hpp"
#include "nodeStore/NsWriter.hpp"
#include "nodeStore/NsDomReader.hpp"
#include "nodeStore/NsXercesDom.hpp"

#include <xqilla/framework/XPath2MemoryManagerImpl.hpp>

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/EntityResolver.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLString.hpp> // for indexOf()
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

//////////////////////////////////
// ModifyStep
//////////////////////////////////

ModifyStep::ModifyStep(const XmlQueryExpression &expr)
	: expr_(expr)
{
	expr_->acquire();
}

ModifyStep::~ModifyStep()
{
	expr_->release();
}

unsigned int ModifyStep::execute(Transaction *transaction, Value *toModify,
                                 XmlQueryContext &context) const
{
	unsigned int modifications = 0;

	u_int32_t flags = 0;
	XmlManager &mgr = ((QueryContext&)context).getManager();
	if (transaction && !((Manager&)mgr).isCDBEnv())
		flags = DB_RMW; // use DB_RMW if transacted and not CDB

	XmlResults results = expr_->execute(transaction, toModify, context, flags);
	XmlValue value;
	while(results.next(value)) {

		// We assume toModify is a NODE (and not null)
		if(value.getType() == XmlValue::NODE &&
		   (toModify->asDocument() == value.asDocument())) {
			// We modify this one, because it comes from the same
			// document as the one passed in
			try {
				modify(value.asNode(), context);
			}
			catch(DOMException &e) {
				throw XmlException(XmlException::INVALID_VALUE, string("XmlModify::execute: modification error: ") + XMLChToUTF8(e.msg).str());
			}

			++modifications;
		} else {
			throw XmlException(XmlException::INVALID_VALUE,
					   "XmlModify::execute: Cannot perform a modification on an XmlValue that isn't either Node or Document type");
		}
	}

	return modifications;
}

//////////////////////////////////
// RemoveStep
//////////////////////////////////

void RemoveStep::modify(DOMNode *node, XmlQueryContext &context) const
{
	if(node->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
		((DOMAttr*)node)->getOwnerElement()->removeAttributeNode((DOMAttr*)node);
	}
	else {
		DOMNode *parentNode = node->getParentNode();
		if(!parentNode) {
			throw XmlException(XmlException::INVALID_VALUE,
					   "XmlModify::execute: cannot remove node that doesn't have a parent");
		}
		if(parentNode->getNodeType() == DOMNode::DOCUMENT_NODE &&
		   node->getNodeType() == DOMNode::ELEMENT_NODE) {
			throw XmlException(XmlException::INVALID_VALUE,
					   "XmlModify::execute: cannot remove the document element");
		}
		else {
			parentNode->removeChild(node);
		}
	}
}

//////////////////////////////////
// RenameStep
//////////////////////////////////

void RenameStep::modify(DOMNode *node, XmlQueryContext &context) const
{
	// rename element, attribute or PI (only things with names)
	// gmf: Xerces has experimental support for rename of elements and attrs,
	// so it is used.  Be prepared to reimplement if it changes.
	if(node->getNodeType() == DOMNode::ATTRIBUTE_NODE ||
	   node->getNodeType() == DOMNode::ELEMENT_NODE) {
		node->getOwnerDocument()->
			renameNode(node, node->getNamespaceURI(), name_.str());
	}
	else if(node->getNodeType() == DOMNode::PROCESSING_INSTRUCTION_NODE) {
		DOMProcessingInstruction *newNode =
			node->getOwnerDocument()->createProcessingInstruction(name_.str(), node->getNodeValue());
		node->getParentNode()->replaceChild(newNode, node);
	}
	else {
		throw XmlException(XmlException::INVALID_VALUE,
				   "XmlModify::execute: can only rename elements, attributes, and processing instructions");
	}
}

//////////////////////////////////
// UpdateStep
//////////////////////////////////

void UpdateStep::modify(DOMNode *node, XmlQueryContext &context) const
{
	node->setTextContent(content_.str());
}

//////////////////////////////////////////////////////
// Private methods for parsing DOM documents
//////////////////////////////////////////////////////

class DbXmlErrorHandler : public ErrorHandler
{
public:
        DbXmlErrorHandler() : error_(false), line_(0)
        {}
        ~DbXmlErrorHandler()
        {}
        virtual void warning(const SAXParseException& toCatch)
        {
                UNUSED(toCatch);
        }
        virtual void error(const SAXParseException& toCatch)
        {
                error_ = true;
                file_ = XMLChToUTF8(toCatch.getSystemId()).str();
                line_ = toCatch.getLineNumber();
                message_ = XMLChToUTF8(toCatch.getMessage()).str();
        }
        virtual void fatalError(const SAXParseException& toCatch)
        {
                error_ = true;
                file_ = XMLChToUTF8(toCatch.getSystemId()).str();
                line_ = toCatch.getLineNumber();
                message_ = XMLChToUTF8(toCatch.getMessage()).str();
        }
        virtual void resetErrors()
        {
                error_ = false;
        }
        bool parseError() const
        {
                return error_;
        }
        int line() const
        {
                return line_;
        }
        const std::string &file() const
        {
                return file_;
        }
        const std::string &message() const
        {
                return message_;
        }
private :
        DbXmlErrorHandler(const DbXmlErrorHandler&);
        void operator=(const DbXmlErrorHandler&);
        bool error_;
        int line_;
        std::string file_;
        std::string message_;
};

class DbXmlEntityResolver : public EntityResolver
{
public:
        DbXmlEntityResolver()
        {}
        ~DbXmlEntityResolver()
        {}
        virtual XERCES_CPP_NAMESPACE_QUALIFIER InputSource* resolveEntity(const XMLCh* const publicId, const XMLCh* const systemId)
        {
                UNUSED(publicId);
                UNUSED(systemId);
                return new MemBufInputSource(0, 0, "", false);
        }
private :
        DbXmlEntityResolver(const DbXmlEntityResolver&);
        void operator=(const DbXmlEntityResolver&);
};

// simple wrapper for parsing
XERCES_CPP_NAMESPACE_QUALIFIER
DOMDocument *_parseDoc(XPath2MemoryManager *memMgr,
		       XercesDOMParser *xmlParser,
		       const XMLByte* const srcBytes,
		       const unsigned int byteCount, const char * const id)
{
	DbXmlErrorHandler eh;
	DbXmlEntityResolver er;
	try {
		xmlParser->setErrorHandler(&eh);
		xmlParser->setEntityResolver(&er);
		MemBufInputSource theInputSource(srcBytes, byteCount, id,
						 false, memMgr);
		xmlParser->parse(theInputSource);
		return xmlParser->adoptDocument();
	} catch (XMLException &e) {
		throw XmlException(XmlException::DOM_PARSER_ERROR,
				   XMLChToUTF8(e.getMessage()).str(),
				   e.getSrcFile(), e.getSrcLine());
	}
	if (eh.parseError()) {
		throw XmlException(XmlException::DOM_PARSER_ERROR,
				   eh.message(), eh.file().c_str(), eh.line());
	}
	// not reached
	return 0;
}

//////////////////////////////////
// DOMContentStep
//////////////////////////////////

//
// Constructor that takes string content
//
DOMContentStep::DOMContentStep(const XmlQueryExpression &expr,
			       XmlModify::XmlObject type,
                               const string &name, const string &content,
                               XPath2MemoryManager *memMgr,
			       XercesDOMParser *domParser)
	: ModifyStep(expr),
	  document_(0),
	  content_(0),
	  name_(""),
	  results_(0)
{
	try {
		switch(type) {
		case XmlModify::Attribute: {
			document_ = DOMImplementation::getImplementation()->
				createDocument(memMgr);
			content_ = document_->createAttribute(
				UTF8ToXMLCh(name).str());
			content_->setNodeValue(UTF8ToXMLCh(content).str());
			break;
		}
		case XmlModify::Element: {
			// only need name_ for elements, in other
			// cases, it's consumed in this function
			name_ = name;
			
			// If name is non-null, do some error checking,
			// and compression of white space
			int pos = 0;
			if (name_.empty()) {
				if (content.empty()) {
					throw XmlException(
						XmlException::INVALID_VALUE,
						"XmlModify: Element insertion requires name or content to be non-null");
				}
				// in this path, squeeze out leading white
				// space in content -- it cannot be
				// significant
				char tmp = content[pos];
				while ((tmp == ' ') || (tmp == '\t')
				       || (tmp == '\n'))
					tmp = content[++pos];
				
			}

			if (!content.empty()) {
				string newContent = "<fake>" + content.substr(pos) +
					"</fake>";
				document_ = _parseDoc(memMgr, domParser,
						      (XMLByte*)newContent.data(),
						      newContent.length(), "fake");
				results_ = new ValueResults();
				results_->acquire();
				// get past "fake" node, if created
				content_ = document_->getDocumentElement()->getFirstChild();
			}
			// Insert each sibling as a new XmlValue
			// This means that it's possible to add content like
			// name = "newElement" content="<a/><a/>" (i.e. content need not
			// be rooted).
			DOMNode *current = content_;
			while (current) {
				// use empty XmlDocument -- not required
				results_->add(XmlValue(new NodeValue(current, XmlDocument())));
				current = current->getNextSibling();
			}

			break;
		}
		case XmlModify::Text: {
			document_ = DOMImplementation::getImplementation()->
				createDocument(memMgr);
			content_ = document_->createTextNode(
				UTF8ToXMLCh(content).str());
			break;
		}
		case XmlModify::ProcessingInstruction: {
			document_ = DOMImplementation::getImplementation()->
				createDocument(memMgr);
			content_ = document_->createProcessingInstruction(
				UTF8ToXMLCh(name).str(),
				UTF8ToXMLCh(content).str());
			break;
		}
		case XmlModify::Comment: {
			document_ = DOMImplementation::getImplementation()->
				createDocument(memMgr);
			content_ = document_->createComment(
				UTF8ToXMLCh(content).str());
			break;
		}
		default: {
			throw XmlException(
				XmlException::INVALID_VALUE,
				"XmlModify::addStep: can only create content for elements, attributes, text, comments and ProcessingInstructions");
		}
		}
	}
	catch(DOMException &e) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			string("XmlModify::addStep: error creating modification content: ") + XMLChToUTF8(e.msg).str());
	}

	if ((content_ == 0) &&
	    (type != XmlModify::Element) &&
	    (name_.empty())) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"XmlModify::addStep: Cannot create content for step");
	}
}

//
// constructor that takes XmlResults content
//
DOMContentStep::DOMContentStep(const XmlQueryExpression &expr,
			       XmlModify::XmlObject type,
                               const string &name, XmlResults &content)
	: ModifyStep(expr),
	  document_(0),
	  content_(0),
	  name_(name),
	  results_(0)
{
	if ((type == XmlModify::ProcessingInstruction) ||
	    (type == XmlModify::Attribute)){
		// only do Element/Text/Comment with XmlResults.  It'd be
		// possible to define transformations for other
		// types, but there's no obvious benefit
		throw XmlException(
			XmlException::INVALID_VALUE,
			"XmlModify: content in XmlResults can only be inserted as type XmlModify::Element, XmlModify::Text, or XmlModify::Comment");
	}
	if (content.size() != 0) {
		// this code is cribbed from ValueResults ctor,
		// adding a check that values *must* be nodes
		results_ = new ValueResults();
		results_->acquire();
		content.reset(); // just in case
		XmlValue value;
		while(content.next(value)) {
			if (value.getType() != XmlValue::NODE) {
				results_->release();
				results_ = 0;
				content.reset();
				throw XmlException(
					XmlException::INVALID_VALUE,
					"XmlModify: XmlResults contains non-NODE value");
			}
			results_->add(value);
		}
		content.reset(); // constructor iterates		}
	} else {
		// cannot have empty content and empty name
		if (name_.empty())
			throw XmlException(
				XmlException::INVALID_VALUE,
				"XmlModify: Element insertion requires name or content to be non-empty");
	}
}

DOMContentStep::~DOMContentStep()
{
	if (results_)
		results_->release();
	// The document is deleted by the XPath2MemoryManager
// 	if(document_) document_->release();
}

// inserts all new content as child elements, creating
// the first one from the name provided, if necessary.
// TBD: test namespace issues...
void DOMContentStep::insertChildren(DOMNode *parent, DOMNode *target,
				    NsDomInsertType itype, bool isAppend) const
{
	DOMElement *elem = 0;
	NsXDOMElement *xparent =
		(NsXDOMElement *) parent->getInterface(_nsXDomString);
	DBXML_ASSERT(xparent);
	if (!name_.empty()) {
		elem = xparent->getOwnerDocument()->
			createElement(UTF8ToXMLCh(name_).str());
		if (isAppend)
			xparent->appendChild(elem);
		else
			xparent->insertBefore(elem, target, itype);
		// reset parent to be the new element, since new content
		// will be appended to it.  The target is no longer
		// required, and treat the rest as append.
		xparent = (NsXDOMElement *) elem->getInterface(_nsXDomString);
		if (!content_ && !results_)
			return; // empty element -- done
		target = 0;
		isAppend = true;
	}
	// if content_ is set, and not an element node,
	// then there's only one node to insert
	if ((content_ &&
	     content_->getNodeType() != DOMNode::ELEMENT_NODE) &&
	    (!results_ || results_->size() == 1)) {
		if (isAppend)
			xparent->appendChild(
				xparent->getOwnerDocument()->
				importNode(content_, true));
		else
			xparent->insertBefore(
				xparent->getOwnerDocument()->
				importNode(content_, true),
				target, itype);
		return;
	}


	// inserting element(s); there may be more than one
	
	XmlResults results(results_);
	XmlValue val;
	DBXML_ASSERT(results.hasNext());
	// this will insert all values in the results list
	// in order, before the target.  If target is empty,
	// they are appended in order
	while (results.next(val)) {
		if (isAppend)
			xparent->appendChild(
				xparent->getOwnerDocument()->
				importNode(val.asNode(), true));
		else
			xparent->insertBefore(
				xparent->getOwnerDocument()->
				importNode(val.asNode(), true),
				target, itype);
	}
	results_->reset();
}

//////////////////////////////////
// InsertBeforeStep
//////////////////////////////////

void InsertBeforeStep::modify(DOMNode *node, XmlQueryContext &context) const
{
	if(node->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"XmlModify::execute: cannot insert before an attribute");
	} else {
		DOMNode *parentNode = node->getParentNode();
		if(!parentNode) {
			throw XmlException(XmlException::INVALID_VALUE,
					   "XmlModify::execute: cannot insert before a node with no parent");
		}
		if(parentNode->getNodeType() == DOMNode::DOCUMENT_NODE &&
		   (!content_ ||
		    content_->getNodeType() == DOMNode::ELEMENT_NODE)) {
			throw XmlException(XmlException::INVALID_VALUE,
					   "XmlModify::execute: Cannot create an element sibling of the document root");
		}
		if(content_ &&
		   content_->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
			if(parentNode->getNodeType() != DOMNode::ELEMENT_NODE) {
				throw XmlException(XmlException::INVALID_VALUE,
						   "XmlModify::execute: attributes can only be added to elements");
			}
			((DOMElement*)parentNode)->
				setAttributeNodeNS(
					importAttr((DOMAttr*)content_,
						   node->getOwnerDocument(),
						   context));
		} else {
			// insert as sibling, before current
			insertChildren(parentNode, node, nsDomInsertBefore);
		}
	}
}

//////////////////////////////////
// InsertAfterStep
//////////////////////////////////

void InsertAfterStep::modify(DOMNode *node, XmlQueryContext &context) const
{
	if(node->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "XmlModify::execute: cannot insert after an attribute");
	} else {
		DOMNode *parentNode = node->getParentNode();
		if(!parentNode) {
			throw XmlException(XmlException::INVALID_VALUE,
					   "XmlModify::execute: cannot insert after a node with no parent");
		}
		if(parentNode->getNodeType() == DOMNode::DOCUMENT_NODE &&
		   (!content_ ||
		    content_->getNodeType() == DOMNode::ELEMENT_NODE)) {
			throw XmlException(XmlException::INVALID_VALUE,
					   "XmlModify::execute: Cannot create an element sibling of the document root");
		}
		if(content_ &&
		   content_->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
			if(parentNode->getNodeType() != DOMNode::ELEMENT_NODE) {
				throw XmlException(XmlException::INVALID_VALUE,
						   "XmlModify::execute: attributes can only be added to elements");
			}
			((DOMElement*)parentNode)->setAttributeNodeNS(
				importAttr((DOMAttr*)content_,
					   node->getOwnerDocument(),
					   context));
		} else {
			// insert as sibling, before currents next sibling
			// (if getNextSibling returns null, the insertChildren acts like an append)
			insertChildren(parentNode, node->getNextSibling(),
				       nsDomInsertAfter);
		}
	}
}

//////////////////////////////////
// AppendStep
//////////////////////////////////

void AppendStep::modify(DOMNode *node, XmlQueryContext &context) const
{
	if(node->getNodeType() != DOMNode::ELEMENT_NODE) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "XmlModify::execute: Append operation is only valid for Element targets");
	} else {
		if(node->getNodeType() == DOMNode::DOCUMENT_NODE) {
			throw XmlException(XmlException::INVALID_VALUE,
					   "XmlModify::execute: Cannot create another element child of the document node");
		}
		if(content_ &&
		   content_->getNodeType() == DOMNode::ATTRIBUTE_NODE) {
			((DOMElement*)node)->setAttributeNodeNS(
				importAttr((DOMAttr*)content_,
					   node->getOwnerDocument(),
					   context));
		} else {
			// Performance note:  getting the child list is
			// cheap, but getLength() is expensive -- it
			// iterates the list, counting.  Avoid if possible.
			DOMNodeList *children = node->getChildNodes();
			if(location_ == -1 || location_ >=
			   (int)children->getLength()) {
				// Append can be modeled as insertBefore; but
				// since append is *much* faster, it's worth
				// special-casing
				insertChildren(node, NULL,
					       nsDomInsertAppend, true);
			} else {
				// treat append after a location as
				// insertAfter, for node id allocation
				// purposes
				insertChildren(node,
					       children->item(location_),
					       nsDomInsertAfter);
			}
		}
	}
}

//////////////////////////////////
// Modify
//////////////////////////////////

Modify::~Modify()
{
	if(memMgr_) delete memMgr_; // Also deletes the domParser_, and any DOM content made by the parser

	ModifyStep::Vector::iterator end = steps_.end();
	for(ModifyStep::Vector::iterator i = steps_.begin(); i != end; ++i) {
		delete *i;
	}
}

XPath2MemoryManager *Modify::getMemMgr() const
{
	if(memMgr_ == 0) {
		memMgr_ = new XPath2MemoryManagerImpl;
	}
	return memMgr_;
}

XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *Modify::getDOMParser()
{
	if(domParser_ == 0) {
		domParser_ = new (getMemMgr()) XercesDOMParser(0, getMemMgr());
		domParser_->setValidationScheme(XercesDOMParser::Val_Auto);
		domParser_->setDoNamespaces(true);
		domParser_->setDoSchema(true);
		domParser_->setLoadExternalDTD(true);
		domParser_->setCreateEntityReferenceNodes(false);
		domParser_->setValidationConstraintFatal(false);
	}
	return domParser_;
}

void Modify::addStep(ModifyStep *stepToAdopt)
{
	steps_.push_back(stepToAdopt);
}

unsigned int Modify::executeInternal(Transaction *txn, Value *toModify,
				     XmlQueryContext &context) const
{
	unsigned int modifications = 0;

	ModifyStep::Vector::const_iterator end = steps_.end();
	for(ModifyStep::Vector::const_iterator i = steps_.begin(); i != end; ++i) {
		modifications += (*i)->execute(txn, toModify, context);
	}

	return modifications;
}

void toUpperCase(string &str)
{
	for(unsigned int i = 0; i < str.size(); ++i)
		if(str[i] >= 'a' && str[i] <= 'z')
			str[i] = str[i] -'a' + 'A';
}

unsigned int Modify::changeEncoding(const XmlDocument &document) const
{
	unsigned int modifications = 0;

	TransactedContainer *container = ((Document&)document).getContainer();

	// Check for NLS document
	if(container && container->getContainerType() == XmlContainer::NodeContainer) {
		if(newEncoding_ != "") {
			throw XmlException(XmlException::INVALID_VALUE,
				"XmlModify::execute: Cannot change the encoding of a document in a NodeContainer");
		}
		else return 0;
	}

	// Work out the original encoding -
	// Use the encoding trom the XMLDecl, as this should be most specific.
	// If it isn't there, use the encoding that was auto-sensed (sniffed)
	// when the document was first parsed.
	((Document&)document).getContentAsDOM();
	NsDocument *nsdoc = ((Document&)document).getNsDocument();

	const char *oenc = (const char*)nsdoc->getEncodingStr();
	string orig_enc;
	if(oenc == 0) {
		const char *senc = (const char*)nsdoc->getSniffedEncodingStr();
		if(senc != 0) orig_enc = senc;
	} else {
		orig_enc = oenc;
	}

	// New encoding is the same as the old one if not specified
	string new_enc = newEncoding_;
	if(new_enc == "") {
		if(orig_enc == "") new_enc = "UTF-8";
		else new_enc = orig_enc;
	}

	// Upper case the strings, to do a case insensitive comparison
	toUpperCase(orig_enc);
	toUpperCase(new_enc);

	// Do we need to change the encoding decl?
	if(orig_enc != new_enc) {
		++modifications;

		const char *new_enc_name = newEncoding_.c_str();
		if(newEncoding_ == "") {
			new_enc_name = new_enc.c_str();
		}

		nsdoc->setEncodingStr((const xmlbyte_t*)new_enc_name);
	}

	if(new_enc != "UTF-8") {
		// Try to get a transcoder for the required encoding
		XMLTransService::Codes err;
		XMLTranscoder *trans = XMLPlatformUtils::fgTransService->
			makeNewTranscoderFor(new_enc.c_str(), err, /*blockSize*/1024 * 32,
				Globals::defaultMemoryManager);
		if(trans == 0) {
			ostringstream oss;
			oss << "XmlModify::execute: Unsupported encoding: " << new_enc;
			throw XmlException(XmlException::INVALID_VALUE, oss.str());
		}

		DbtOut *final_content = 0;
		try {
			// Make output writer
			BufferNsStream content;
			NsWriter writer(&content, /*writeRealEncoding*/true);

			// Make event generator
			NsDomReader reader(nsdoc->getDocumentNode());

			// Write the content (in UTF8)
			writer.writeFromReader(reader);

			// Transcode from UTF8 to UTF16
			UTF8ToXMLCh content16((const char*)content.buffer.getBuffer(),
				content.buffer.getOccupancy());

			// Create buffer that should be big enough for most encodings
			final_content = new DbtOut(0, content16.len() * 3);

			// Transcode from UTF16 to the destination encoding
			const XMLCh *source = content16.str();
			unsigned int sourceLen = content16.len();
			unsigned int bytesProduced = 0;
			unsigned int charsEaten;

			// Transcode the first chunk
			bytesProduced += trans->transcodeTo(source, sourceLen,
				((XMLByte*)final_content->get_data()),
				final_content->get_size(),
				charsEaten, XMLTranscoder::UnRep_RepChar);
			source += charsEaten;
			sourceLen -= charsEaten;

			while(sourceLen != 0) {
				// Resize the final_content buffer
				final_content->set_size(final_content->get_size() * 2);

				// Transcode some more
				bytesProduced += trans->transcodeTo(source, sourceLen,
					((XMLByte*)final_content->get_data()) + bytesProduced,
					final_content->get_size() - bytesProduced,
					charsEaten, XMLTranscoder::UnRep_RepChar);
				source += charsEaten;
				sourceLen -= charsEaten;
			}

			// Set the final_content size to the actual size
			final_content->set_size(bytesProduced);
		}
		catch(...) {
			delete final_content;
			delete trans;
			throw;
		}

		((Document&)document).setContentAsDbt(&final_content);
		delete trans;
	}

	return modifications;
}

void Modify::updateDocument(Transaction *txn, const XmlDocument &document,
			    XmlQueryContext &context,
			    XmlUpdateContext &uc) const
{
	TransactedContainer *container = ((Document&)document).getContainer();
	if(container && uc.getApplyChangesToContainers()) {
		try {
			int err = container->updateDocument(
				txn, (Document&)document, (UpdateContext&)uc);
			if(err != 0) throw XmlException(err);
		} catch (DbException &e) {
			throw XmlException(e);
		}
	}
}

unsigned int Modify::execute(Transaction *txn, Value *toModify,
			     XmlQueryContext &context,
			     XmlUpdateContext &uc) const
{
	unsigned int modifications = 0;
	if (!toModify) {
		if (steps_.size() == 1) {
			u_int32_t flags = 0;
			XmlManager &mgr = ((QueryContext&)context).getManager();
			if (txn && !((Manager&)mgr).isCDBEnv())
				flags = DB_RMW; // use DB_RMW if transacted and not CDB
			ModifyStep &step = *(steps_[0]);
			QueryExpression &expr = step.getExpression();
			XmlResults results = expr.execute(txn,
							  NULL, context, flags);
			// TBD: only make one of these, and re-use it.
			XmlQueryExpression newExpr = mgr.prepare(".", context);
			step.setExpression(newExpr);
			return execute(txn, results, context, uc);
		} else
			throw XmlException(XmlException::INVALID_VALUE,
					   "XmlModify::execute(): empty XmlValue: is only valid when used with a single modify step");
	}
	
	if (toModify->getType() == XmlValue::NODE) {
		modifications = executeInternal(txn, toModify, context);
		modifications += changeEncoding(toModify->asDocument());

		// Put it back in the database, if we need to
		if(modifications) {
			updateDocument(txn, toModify->asDocument(), context, uc);
		}
	} else {
		throw XmlException(XmlException::INVALID_VALUE,
				   "XmlModify::execute: Cannot perform a modification on an XmlValue that isn't either Node or Document type");
	}

	return modifications;
}

unsigned int Modify::execute(Transaction *txn, XmlResults &toModify,
			     XmlQueryContext &context, XmlUpdateContext &uc) const
{
	unsigned int modifications = 0;

	XmlDocument currentDoc;
	unsigned int currentDocModifications = 0;

	// Algorithm *requires* that results are grouped by document,
	// so that only one re-write is performed for each document involved.
	XmlValue value;
	bool next = toModify.next(value);
	if(next) {
		if(value.getType() == XmlValue::NODE) {
			currentDoc = value.asDocument();
		} else {
			throw XmlException(XmlException::INVALID_VALUE,
					   "XmlModify::execute: Cannot perform a modification on an XmlValue that isn't either Node or Document type");
		}
	}

	while(next) {
		currentDocModifications = executeInternal(txn, value,
							  context);

		next = toModify.next(value);
		if(next) {
			if(value.getType() == XmlValue::NODE) {

				if(currentDoc != value.asDocument()) {
					currentDocModifications +=
						changeEncoding(currentDoc);
					if(currentDocModifications) {
						updateDocument(txn, currentDoc,
							       context, uc);
					}
					modifications +=
						currentDocModifications;
					currentDoc = value.asDocument();
					currentDocModifications = 0;
				} else {
					// continue making modifications on the
					// current document, until such time
					// as we need to commit it
				}
			} else {
				throw XmlException(XmlException::INVALID_VALUE,
						   "XmlModify::execute: Cannot perform a modification on an XmlValue that isn't either Node or Document type");
			}
		} else {
			currentDocModifications +=
				changeEncoding(currentDoc);
			if(currentDocModifications) {
				updateDocument(txn, currentDoc, context, uc);
			}
			modifications += currentDocModifications;
		}
	}

	return modifications;
}

void Modify::setNewEncoding(const string &newEncoding)
{
	newEncoding_ = newEncoding;
}

// allow namespace-qualified attributes to be inserted,
// by using prefix/uri mappings in XmlQueryContext, along
// with a namespace prefix, if present in the attribute's qname.
// This function must return a node in the target DOMDocument (newdoc),
// so it can be used by setAttributeNS().
DOMAttr * ModifyStep::importAttr(DOMAttr *attr,
				 XERCES_CPP_NAMESPACE_QUALIFIER
				    DOMDocument *newdoc,
				 XmlQueryContext &context) const
{
	XMLCh *name = (XMLCh *)attr->getNodeName();
	const XMLCh *uri = attr->getNamespaceURI();
	int hasColon = -1;
	if (uri == 0) {
		hasColon = XMLString::indexOf(name, ':');
		if (hasColon != -1) {
			name[hasColon] = 0;
			XMLChToUTF8 prefix(name);
			std::string ns = context.getNamespace(prefix.str());
			name[hasColon] = ':';
			if (!ns.empty()) {
				UTF8ToXMLCh newuri(ns);
				DOMAttr *newAttr =
					newdoc->createAttributeNS(newuri.str(),
								  name);
				newAttr->setNodeValue(attr->getNodeValue());
				return newAttr;
			}
		}
	}
	// if no prefix, or already has uri, just import
	return (DOMAttr*)newdoc->importNode(attr, true);

}
