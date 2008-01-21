//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsEventWriter.cpp,v 1.14 2006/11/03 17:26:11 gmf Exp $
//

#include "../DbXmlInternal.hpp"
#include "../Container.hpp"
#include "../Document.hpp"
#include "../UpdateContext.hpp"
#include "NsEventWriter.hpp"
#include "NsNode.hpp"
#include "NsEvent.hpp"
#include "NsWriter.hpp"
#include "NsConstants.hpp"

using namespace DbXml;
using namespace std;

#define CHECK_SUCCESS() \
if (!success_)throwBadWrite("XmlEventWriter: cannot write after an exception is thrown")
#define CHECK_NULL(p) if (p && (*p == '\0')) p = 0

#define CANT_BE_NULL(p, f, n) \
if (!p) {  \
	string msg = "XmlEventWriter::";	\
	msg += f;				\
	msg += ": argument cannot be null: ";	\
	msg += n;				\
	throwBadWrite(msg.c_str());		\
}


/*
 * NsEventWriter implementation
 */
NsEventWriter::NsEventWriter(Container *container, UpdateContext *uc,
			     Document *document, u_int32_t flags,
			     XER_NS MemoryManager *mmgr)
	: NsHandlerBase(mmgr, &nsdoc_, 0), nsdoc_(0, mmgr),
	  uc_(uc), cont_(container), doc_(document), ndd_(0),
	  docId_(document->getID()), writer_(0),
	  writerStream_(0), mustBeEnd_(false), success_(true),
	  needsStartElement_(false), attrsToGo_(0)
{
	DBXML_ASSERT(uc_ && cont_ && doc_);
	uc_->acquire();
	cont_->acquire();
	ndd_ = cont_->getDocumentDB()->getNsDocumentDatabase();
	OperationContext &oc = uc_->getOperationContext();
	nsdoc_.initDoc(oc.txn(), ndd_, cont_->getDictionaryDB(),
		       docId_, flags);
	// note NsDocument and Document objects are entirely separate
}

//
// This method is used for simple upgrade to copy documents.
// There is probably no actual container object involved.
//
NsEventWriter::NsEventWriter(NsDocumentDatabase *ndd,
			     DictionaryDatabase *dict,
			     const DocID &id,
			     XER_NS MemoryManager *mmgr)
	: NsHandlerBase(mmgr, &nsdoc_, 0), nsdoc_(0, mmgr),
	  uc_(0), cont_(0), doc_(0), ndd_(ndd),
	  docId_(id), writer_(0),
	  writerStream_(0), mustBeEnd_(false), success_(true),
	  needsStartElement_(false), attrsToGo_(0)
{
	// no transaction, and no flags allowed (easy to add if
	// necessary, later)
	nsdoc_.initDoc(0, ndd_, dict, docId_, 0);
}

NsEventWriter::~NsEventWriter()
{
	if (uc_)
		uc_->release();
	if (cont_)
		cont_->release();
	if (writerStream_)
		delete writerStream_;
	if (writer_)
		delete writer_;
}

void NsEventWriter::close()
{
	if (success_ && !mustBeEnd_)
		throwBadWrite("XmlEventWriter: cannot call close before document is complete");
	delete this;
}

// implement close.
void NsEventWriter::completeDoc()
{
	if (success_ && cont_) {
		DBXML_ASSERT(doc_ && uc_);
		if (!ndd_) {
			DBXML_ASSERT(writerStream_);
			// put output of writer into doc_
			DbtOut *data = new DbtOut();
			data->setNoCopy(writerStream_->buffer.donateBuffer(),
					writerStream_->buffer.getOccupancy());
			doc_->setContentAsDbt(&data, true); // consumes data
		}
		int err = cont_->getDocumentDB()->addContent(*doc_, *uc_);
		if (err == 0) {
			doc_->setContentModified(false);
			cont_->completeAddDocument(*doc_, *uc_);
		}
	}
}

void NsEventWriter::createNsWriter()
{
	writerStream_ = new BufferNsStream();
	if (writerStream_) {
		writer_ = new NsWriter(writerStream_);
	}
	if (!writerStream_ || !writer_)
		throw XmlException(XmlException::NO_MEMORY_ERROR,
				   "Failed to allocate memory for XmlEventWriter");
}

void NsEventWriter::exceptionNotify(DbException &de)
{
	if (success_) {
		success_ = false;
		if (cont_)
			cont_->getDocumentDB()->addContentException(*doc_,
								    *uc_, true);
	}
	throw de;
}

void NsEventWriter::exceptionNotify(XmlException &xe)
{
	if (success_) {
		success_ = false;
		if (cont_)
			cont_->getDocumentDB()->addContentException(
				*doc_, *uc_, false);
	}
	throw xe;
}

void NsEventWriter::throwBadWrite(const char *msg)
{
	XmlException xe(XmlException::EVENT_ERROR, msg);
	exceptionNotify(xe);
}

void NsEventWriter::doStartElem(NsEventNodeAttrList *attrs)
{
	try {
		NsNode *node = _current;
		needsStartElement_ = false;
		// need to use std::string to store prefix and uri
		// since their storage may be reused
		std::string spref;
		std::string suri;
		const xmlbyte_t *tpref = 0, *turi = 0;
		if (node->hasUri()) {
			suri = _doc->getStringForID(node->uriIndex());
			turi = (const xmlbyte_t *) suri.c_str();
			if (node->hasNamePrefix()) {
				spref = _doc->getStringForID(node->namePrefix());
				tpref = (const xmlbyte_t *) spref.c_str();
			}
		}
		const xmlbyte_t *tname = (const xmlbyte_t *) node->getNameChars();
		// constructor is ok with empty attr list.
		NsEventNodeAttrList alist(node->getAttrList(), *_doc);
		if (_ewriter)
			_ewriter->writeStartElementWithAttrs(
				tname, tpref, turi,
				alist.numAttributes(), &alist, node, isEmpty_);
		if (writer_)
			writer_->writeStartElementWithAttrs(
				tname, tpref, turi,
				alist.numAttributes(), &alist, node, isEmpty_);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
	catch (DbException &de) {
		exceptionNotify(de);
	}
}

void NsEventWriter::writeAttribute(const unsigned char *localName,
				   const unsigned char *prefix,
				   const unsigned char *uri,
				   const unsigned char *value,
				   bool isSpecified)
{
	CHECK_NULL(localName);
	CHECK_NULL(prefix);
	CHECK_NULL(uri);
	CHECK_NULL(value);
	CHECK_SUCCESS();
	CANT_BE_NULL(localName, "writeAttribute", "localName");
	if (!needsStartElement_ || !_current || !attrsToGo_)
		throwBadWrite("writeAttribute called out of order");
	try {
		_current->addAttr(_doc, prefix, uri, localName,
				  value, isSpecified);
		if (--attrsToGo_ == 0) {
			NsEventNodeAttrList alist(_current->getAttrList(), *_doc);
			doStartElem(&alist);
			// if it's empty, do end as well
			if (isEmpty_) {
				endElem();
				if (_current->isDoc())
					mustBeEnd_ = true;
				isEmpty_ = false;
			}
		}
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
	catch (DbException &de) {
		exceptionNotify(de);
	}
}

void NsEventWriter::writeText(XmlEventReader::XmlEventType type,
			      const unsigned char *text,
			      int length)
{
	CHECK_NULL(text);
	CHECK_SUCCESS();
	if (!_current)
		throwBadWrite("writeText: requires writeStartDocument");
	try {
		if (needsStartElement_) {
			// only do startElement from here if no attrs
			if (attrsToGo_)
				throwBadWrite("writeText called before all attributes written");
			doStartElem(NULL);
		}
		uint32_t textType;
		switch (type) {
		case XmlEventReader::Whitespace:
			textType = NS_TEXT|NS_IGNORABLE;
			break;
		case XmlEventReader::Characters:
			textType = NS_TEXT;
			break;
		case XmlEventReader::CDATA:
			textType = NS_CDATA;
			break;
		case XmlEventReader::Comment:
			textType = NS_COMMENT;
			break;
		default:
			throwBadWrite("writeText called with bad event");
			break;
		}
		addText((void*)text, length, textType, true, false);
		bool needsEscape;
		if ((type != XmlEventReader::Whitespace) &&
		    (type != XmlEventReader::Comment)){
			DBXML_ASSERT(_textList);
			needsEscape =
				(_textList->tl_text[_textList->tl_ntext-1].te_type
				 & NS_ENTITY_CHK) != 0;
		}
		if (_ewriter)
			_ewriter->writeTextWithEscape(type, text,
						      length, needsEscape);
		if (writer_)
			writer_->writeTextWithEscape(type, text,
						     length, needsEscape);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
	catch (DbException &de) {
		exceptionNotify(de);
	}
}

void NsEventWriter::writeProcessingInstruction(const unsigned char *target,
						const unsigned char *data)
{
	CHECK_NULL(target);
	CHECK_NULL(data);
	CHECK_SUCCESS();
	CANT_BE_NULL(target, "writeProcessingInstruction", "target");
	if (!_current)
		throwBadWrite("writeProcessingInstructio: requires writeStartDocument");
	try {
		if (needsStartElement_) {
			// only do startElement from here if no attrs
			if (attrsToGo_)
				throwBadWrite("writeProcessingInstruction called before all attributes written");
			doStartElem(NULL);
		}
		
		// last arg (len) is ignored if not donating
		addPI((void*)target, (void*)data, true, false, 0); 
		if (_ewriter)
			_ewriter->writeProcessingInstruction(target, data);
		if (writer_)
			writer_->writeProcessingInstruction(target, data);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
	catch (DbException &de) {
		exceptionNotify(de);
	}
}

void NsEventWriter::writeStartElement(const unsigned char *localName,
				      const unsigned char *prefix,
				      const unsigned char *uri,
				      int numAttributes,
				      bool isEmpty)
{
	CHECK_NULL(localName);
	CHECK_NULL(prefix);
	CHECK_NULL(uri);
	CHECK_SUCCESS();
	CANT_BE_NULL(localName, "writeStartElement", "localName");
	if (!_current)
		throwBadWrite("writeStartElement: requires writeStartDocument");
	if (mustBeEnd_)
		throwBadWrite("writeStartElement: document can only have one root");
	try {
		NsNode *node = NsNode::allocNode(_memManager, numAttributes,
						 NS_STANDALONE);
		nextId(node->getNid());
		startElem(node, localName, (const char *)uri,
			  (const char *)prefix, true, false);
		isEmpty_ = isEmpty;
		if (numAttributes == 0) {
			needsStartElement_ = false;
			if (_ewriter)
				_ewriter->writeStartElementWithAttrs(
					localName, prefix, uri,
					numAttributes, NULL,
					node, isEmpty_);
			if (writer_)
				writer_->writeStartElementWithAttrs(
					localName, prefix, uri,
					numAttributes, NULL,
					node, isEmpty_);
			if (isEmpty_) {
				endElem();
				if (_current->isDoc())
					mustBeEnd_ = true;
				isEmpty_ = false;
			}
		} else {
			needsStartElement_ = true;
			attrsToGo_ = numAttributes;
		}
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
	catch (DbException &de) {
		exceptionNotify(de);
	}
}

void NsEventWriter::writeEndElement(const unsigned char *localName,
				     const unsigned char *prefix,
				     const unsigned char *uri)
{
	CHECK_NULL(localName);
	CHECK_NULL(prefix);
	CHECK_NULL(uri);
	CHECK_SUCCESS();
	try {
		if (needsStartElement_) {
			// only do startElement from here if no attrs
			if (attrsToGo_)
				throwBadWrite("writeEndElement called before all attributes written");
			doStartElem(NULL);
		}
		if (isEmpty_)
			throwBadWrite("writeEndElement called for empty element");
		if (!_current)
			throwBadWrite("writeEndElement called with no current element");
		
		// do event before formally ending the element, as state will change
		if (_ewriter)
			_ewriter->writeEndElementWithNode(localName, prefix,
							  uri, _current);
		if (writer_)
			writer_->writeEndElementWithNode(localName, prefix,
							 uri, _current);
		endElem();
		if (_current->isDoc())
			mustBeEnd_ = true;
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
	catch (DbException &de) {
		exceptionNotify(de);
	}
}
	
void NsEventWriter::writeDTD(const unsigned char *dtd, int length)
{
	CHECK_NULL(dtd);
	CHECK_SUCCESS();
	if (_current) {
		if (!_current->isDoc())
			throwBadWrite("writeDTD: must occur before content");
	} else
		throwBadWrite("writeDTD: requires writeStartDocument");
	try {
		addText((void*)dtd, length, NS_SUBSET, true, false);
		if (_ewriter)
			_ewriter->writeDTD(dtd, length);
		if (writer_)
			writer_->writeDTD(dtd, length);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
	catch (DbException &de) {
		exceptionNotify(de);
	}
}

void NsEventWriter::writeStartDocument(const unsigned char *version,
				       const unsigned char *encoding,
				       const unsigned char *standalone)
{
	CHECK_NULL(version);
	CHECK_NULL(encoding);
	CHECK_NULL(standalone);
	CHECK_SUCCESS();
	if (_current)
		throwBadWrite("writeStartDocument: must be the first event written");
	try {
		// create the document node
		NsNode *node = NsNode::allocNode(_memManager, 0, NS_STANDALONE);
		node->setFlag(NS_ISDOCUMENT);
		_current = node;
		nextId(node->getNid());
		DBXML_ASSERT(node->getNid()->isDocRootNid());
		
		// prepare for xmlDecl event and perform more doc initialization
		
		if (version && *version) {
			if (NsUtil::nsStringEqual(version, _decl8_1_0 ))
				_doc->setXmlDecl(NS_DECL_1_0);
			else if (NsUtil::nsStringEqual(version, _decl8_1_1 ))
				_doc->setXmlDecl(NS_DECL_1_1);
			else
				throwBadWrite("writeStartDocument: bad XML decl");
		}
		if (encoding && *encoding) {
			_doc->setEncodingStr(
				NsUtil::nsStringDup(_doc->getMemoryManager(),
						    encoding, 0));
		}
		if (standalone) {
			if (NsUtil::nsStringEqual(standalone,
						  (const unsigned char *)"yes"))
				_doc->setStandalone(true);
			else
				_doc->setStandalone(false);
		}
		// event handler
		if (_ewriter)
			_ewriter->writeStartDocument(version, encoding,
						     standalone);
		if (writer_)
			writer_->writeStartDocument(version, encoding,
						    standalone);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
	catch (DbException &de) {
		exceptionNotify(de);
	}
}

void NsEventWriter::writeEndDocument()
{
	CHECK_SUCCESS();
	if (!_current)
		throwBadWrite("writeEndDocument: requires writeStartDocument");
	if (!mustBeEnd_)
		throwBadWrite("writeEndDocument: called before document is complete");
	try {
		if (_ewriter)
			_ewriter->writeEndDocument();
		if (writer_)
			writer_->writeEndDocument();
		endDoc();
		completeDoc();
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
	catch (DbException &de) {
		exceptionNotify(de);
	}
}

void NsEventWriter::writeStartEntity(const unsigned char *name,
				      bool expandedInfoFollows)
{
	CHECK_NULL(name);
	CHECK_SUCCESS();
	CANT_BE_NULL(name, "writeStartEntity", "name");
	if (!_current)
		throwBadWrite("writeStartEntity: requires writeStartDocument");
	try {
		if (_ewriter)
			_ewriter->writeStartEntity(name, expandedInfoFollows);
		if (writer_)
			_ewriter->writeStartEntity(name, expandedInfoFollows);
		int len = NsUtil::nsStringLen(name);
		addText((void*)name, len, NS_ENTSTART, true, false);
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
	catch (DbException &de) {
		exceptionNotify(de);
	}
}

void NsEventWriter::writeEndEntity(const unsigned char *name)
{
	CHECK_NULL(name);
	CHECK_SUCCESS();
	if (!_current)
		throwBadWrite("writeEndEntity: requires writeStartDocument");
	try {
		int len = NsUtil::nsStringLen(name);
		if (_ewriter)
			_ewriter->writeEndEntity(name);
		if (writer_)
			writer_->writeEndEntity(name);
		addText((void*)name, len, NS_ENTEND, true, false);
		
	}
	catch (XmlException &xe) {
		exceptionNotify(xe);
	}
	catch (DbException &de) {
		exceptionNotify(de);
	}
}
