//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsDomReader.cpp,v 1.10 2006/11/17 18:18:29 jsnelson Exp $
//

#include "../DbXmlInternal.hpp"
#include "NsUtil.hpp"
#include "NsDomReader.hpp"
#include "NsDom.hpp"
#include "NsDocument.hpp"
#include "NsEvent.hpp"
#include "../UTF8.hpp"

namespace DbXml
{
class NsDomReaderNodeList {
public:
	enum Progress {
		Start,
		Children,
		End
	};

	NsDomReaderNodeList(NsDomReaderNodeList *p)
		: progress(Start), localname(0), utf16(false),
		  child(0), attrs(0), parent(p) {}

	~NsDomReaderNodeList() {
		if(utf16 && localname != 0) {
			delete [] localname;
		}
		delete attrs;
	}

	NsDomElement *element;
	Progress progress;

	xmlbyte_t *localname;
	bool utf16;

	NsDomNav *child;

	NsEventNodeAttrList *attrs;
	NsDomReaderNodeList *parent;
	DbtOut curUri;
	DbtOut curPrefix;
};
}

using namespace DbXml;

NsDomReader::NsDomReader(NsDomElement *node)
	: current_(new NsDomReaderNodeList(0)),
	  popElement_(false), deallocValue_(false),
	  entityCount_(0), doc_(0)
{
	current_->element = node;
	node_ = current_->element->getNsNode();
	doc_ = node->getNsDocument();
	savedEntityText_ = doc_->getCreateEntityText();
}

NsDomReader::~NsDomReader()
{
	doc_->setCreateEntityText(savedEntityText_);
	if (deallocValue_) {
		doc_->getMemoryManager()->deallocate(value_);
		deallocValue_ = false;
	}

	if(current_ != 0) {
		DBXML_ASSERT(doc_ == current_->element->getNsDocument());

		while(current_ != 0) {
			NsDomReaderNodeList *tmp = current_;
			current_ = tmp->parent;
			delete tmp;
		}
	}
}

//
// XmlEventReader methods
//

//
// generic methods
//

// event type and state

#if 0
bool
NsDomReader::hasNext() const
{
	if ((current_ == NULL) ||
		    (popElement_ && (current_->parent == NULL)))
		return false;
	return true;
}
#endif

//
// names and values
//

const unsigned char *
NsDomReader::getNamespaceURI() const
{
	if (!current_->localname)	
		throwIllegalOperation(type_, "getNamespaceURI");
	DBXML_ASSERT(node_);
	if (!node_->hasUri())
		return 0;
	doc_->getStringForID(node_->uriIndex(), current_->curUri);
	return (const unsigned char *)current_->curUri.get_data();
}

const unsigned char *
NsDomReader::getLocalName() const
{
	if (type_ == ProcessingInstruction) {
		// this is odd, but... the PI target is kept
		// in the value_ field, because of allocation issues, and
		// the data portion is in piData_
		return value_;
	}
	if (!current_->localname)
		throwIllegalOperation(type_, "getLocalName");
	return current_->localname;
}

const unsigned char *
NsDomReader::getPrefix() const
{
	if (!current_->localname)
		throwIllegalOperation(type_, "getPrefix");
	DBXML_ASSERT(node_);
	int32_t prefix = node_->namePrefix();
	if (prefix == NS_NOPREFIX)
		return 0;
	doc_->getStringForID(prefix, current_->curPrefix);
	return (const unsigned char *)current_->curPrefix.get_data();
}
	
const unsigned char *
NsDomReader::getValue(int &len) const
{
	if (type_ == ProcessingInstruction) {
		// this is odd, but... the PI target is kept
		// in the value_ field, because of allocation issues, and
		// the data portion is in piData_, and this method returns
		// data
		return piData_;
	}

	if (!value_) {
		throwIllegalOperation(type_, "getValue");
	}
	len = valueLen_;
	return value_;
}

// This method works for text strings and attributes values
bool
NsDomReader::needsEntityEscape(int index) const
{
	if (type_ == Characters) {
		return needsEscape_;
	} else if (type_ == StartElement) {
		ensureAttributes(index, "needsEntityEscape");
		return current_->attrs->needsEscape(index);
	} else if (type_ == CDATA)
		return false;
	throwIllegalOperation(type_, "needsEntityEscape");
	return true;
}

bool
NsDomReader::isEmptyElement() const {
	ensureType(StartElement, "isEmptyElement");
	return (!node_->hasChildNode());
}

// EventReader extensions
const NsNode *
NsDomReader::getNsNode() const
{
	return node_;
}

const unsigned char *
NsDomReader::getVersion() const
{
	ensureType(StartDocument, "getVersion");
	return (const unsigned char*) doc_->getXmlDecl();
}

const unsigned char *
NsDomReader::getEncoding() const
{
	ensureType(StartDocument, "getEncoding");
	return doc_->getEncodingStr();
}

bool
NsDomReader::isStandalone() const
{
	// if there is a standalone string, it's easy,
	// but if not, there is no way to know.  The document
	// may or may not have an external DTD ref, so
	// default to 'no'
	// TBD: think about keeping standalone state during
	// initial parse, as a document-level flag.
	ensureType(StartDocument, "isStandalone");
	const xmlbyte_t *sa = doc_->getStandaloneStr();
	if (sa && (::memcmp(sa, "yes", 3) == 0))
		return true;
	return false;
}

bool
NsDomReader::standaloneSet() const
{
	ensureType(StartDocument, "standaloneSet");
	if (doc_->getStandaloneStr())
		return true;
	return false;
}

bool
NsDomReader::encodingSet() const
{
	ensureType(StartDocument, "encodingSet");
	if (doc_->getEncodingStr())
		return true;
	return false;
}

//
// attribute access (includes namespace attributes)
// _node will be 0 if no attributes are present
//

int
NsDomReader::getAttributeCount() const
{
	ensureType(StartElement, "getAttributeCount");
	return node_->numAttrs();
}

bool
NsDomReader::isAttributeSpecified(int index) const
{
	ensureAttributes(index, "isAttributeSpecified");
	return current_->attrs->isSpecified(index);
}

const unsigned char *
NsDomReader::getAttributeLocalName(int index) const
{
	ensureAttributes(index, "getAttributeLocalName");
	return current_->attrs->localName(index);
}

const unsigned char *
NsDomReader::getAttributeNamespaceURI(int index) const
{
	ensureAttributes(index, "getAttributeNamespaceURI");
	return current_->attrs->uri(index);
}

const unsigned char *
NsDomReader::getAttributePrefix(int index) const
{
	ensureAttributes(index, "getAttributePrefix");
	return current_->attrs->prefix(index);
}

const unsigned char *
NsDomReader::getAttributeValue(int index) const
{
	ensureAttributes(index, "getAttributeValue");
	return current_->attrs->value(index);
}


//
// Implementation methods
//

XmlEventReader::XmlEventType
NsDomReader::next()
{
	bool result = false;
	if (popElement_) {
		popElement();
	}
	if (current_ == NULL) {
		// there is no next...
		throw XmlException(
			XmlException::EVENT_ERROR,
			"XmlEventReader::next() called when hasNext() is false");
	}

	if (deallocValue_) {
		doc_->getMemoryManager()->deallocate(value_);
		deallocValue_ = false;
	}
	value_ = 0;

	if(current_ != 0) {
		DBXML_ASSERT(doc_ == current_->element->getNsDocument());
		doc_->setCreateEntityText(reportEntityInfo_);
		
		while(current_ != 0 && !result) {
			switch(current_->progress) {
			case NsDomReaderNodeList::Start: {
				result = startEvent();
				break;
			}
			case NsDomReaderNodeList::Children: {
				result = childEvent();
				break;
			}
			case NsDomReaderNodeList::End: {
				result = endEvent();
				break;
			}
			}
			if (!result && popElement_)
				popElement();
		}
	}
	// set hasNext_
	if ((current_ == NULL)
	    || ((current_->parent == NULL) &&
		(popElement_ || !node_->hasChildNode()))
		)
		hasNext_ = false;
#if 0
	std::cout << "Event: " << type_;
	if (value_)
		std::cout << ", Value: " << ((*value_ == '\n') ? (unsigned char *)"\\n" : value_);
	std:: cout << std::endl << std::flush;
#endif
	return type_;
}

bool
NsDomReader::startEvent()
{
	DBXML_ASSERT(doc_ == current_->element->getNsDocument());
	bool result = false;
	if(current_->element->getNsNodeType() == nsNodeDocument) {
		type_ = StartDocument;
		// TBD -- doc_->getSniffedEncodingStr();
		current_->progress = NsDomReaderNodeList::Children;
		result = true;
	} else {
		type_ = StartElement;
		node_ = current_->element->getNsNode();
		current_->utf16 = node_->isUTF16();
		current_->attrs = 0;

		const nsName_t *name = node_->getName();
		if (current_->utf16) {
			DBXML_ASSERT(current_->localname == 0);
			NsUtil::nsToUTF8(0, &current_->localname,
					 (const xmlch_t*)name->n_text.t_chars,
					 name->n_text.t_len + 1, 0);
		} else
			current_->localname = (xmlbyte_t*)name->n_text.t_chars;

		if (node_->numAttrs())
			current_->attrs = new NsEventNodeAttrList(
				node_->getAttrList(), *doc_,
				current_->utf16);
		current_->progress = NsDomReaderNodeList::Children;
		result = true;
	}
	return (result && (entityCount_ == 0));
}

bool
NsDomReader::endEvent()
{
	DBXML_ASSERT(doc_ == current_->element->getNsDocument());

	popElement_ = true; // delete current before processing next event
	bool result = false;
	if(current_->element->getNsNodeType() == nsNodeDocument) {
		type_ = EndDocument;
		result = true;
	} else {
		if(node_->hasChildNode()) {
			type_ = EndElement;
			result = true;
		}
	}

	return (result && (entityCount_ == 0));
}

bool
NsDomReader::childEvent()
{
	DBXML_ASSERT(doc_ == current_->element->getNsDocument());

	if(current_->child == 0) {
		current_->child = current_->element->getNsFirstChild();
	} else {
		current_->child = current_->child->getNsNextSibling();
	}

	if(current_->child == 0) {
		current_->progress = NsDomReaderNodeList::End;
		return false; // no more children, do End* event
	} else {
		switch(current_->child->getNsNodeType()) {
		case nsNodeDocument:
		case nsNodeElement: {
			NsDomReaderNodeList *new_current =
				new NsDomReaderNodeList(current_);
			new_current->element = (NsDomElement*)current_->child;
			current_ = new_current;
			return false; // will cause caller to loop
		}
		case nsNodeEntStart:
		case nsNodeEntEnd:
		case nsNodeText: {
			piData_ = 0;
			NsDomText *domtext = (NsDomText*)current_->child;

			NsNode *node = domtext->getNsNode();
			bool utf16 = node->isUTF16();
			nsTextEntry_t *tentry = node->getTextEntry(domtext->getIndex());
			nsText_t *text = &(tentry->te_text);
			uint32_t textType = nsTextType(tentry->te_type);

			// avoid some work if eating entity info right now
			if ((entityCount_ != 0) && (textType != NS_ENTEND)
			    && (textType != NS_ENTSTART))
				return false;

			needsEscape_ = nsTextEntityChk(tentry->te_type);
			if(utf16) {
				value_ = 0;
				DBXML_ASSERT(!deallocValue_);
				enum checkType ctype =
					(textType == NS_TEXT ? isCharacters :
						ignore);
				valueLen_ = NsUtil::nsToUTF8(doc_->getMemoryManager(),
							&value_,
							(const xmlch_t*)text->t_chars,
							text->t_len + 1, 0,
							&needsEscape_, ctype);
				valueLen_ -= 1;
				deallocValue_ = true;
			} else {
				deallocValue_ = false;
				value_ = (xmlbyte_t*)text->t_chars;
				valueLen_ = text->t_len;
			}

			try {
				switch(textType) {
				case NS_ENTSTART: {
					type_ = StartEntityReference;
					if (!expandEntities_)
						entityCount_++;
					return true;
					break;
				}
				case NS_ENTEND: {
					type_ = EndEntityReference;
					if (!expandEntities_)
						entityCount_--;
					return true;
					break;
				}
				case NS_CDATA: {
					type_ = CDATA;
					break;
				}
				case NS_TEXT: {
					type_ = Characters;
					break;
				}
				case NS_COMMENT: {
					type_ = Comment;
					break;
				}
				case NS_PINST: {
					type_ = ProcessingInstruction;
					//
					// PI is encoded target0data0
					// Target stays in value_, data goes
					// in piData_
					//
					piData_ = value_;
					while (*piData_++); // get past NULL;
					valueLen_ = ::strlen(
						(const char*)piData_);
					break;
				}
				case NS_SUBSET: {
					type_ = DTD;
					break;
				}
				default: DBXML_ASSERT(0);
				}
			}
			catch(...) {
				throw;
			}
			// if entityCount is 0, we're not eating events,
			// so return true
			return (entityCount_ == 0);
		}
		default: DBXML_ASSERT(0);
		}
	}

	return false;
}

void
NsDomReader::popElement()
{
	popElement_ = false;
	NsDomReaderNodeList *tmp = current_;
	current_ = tmp->parent;
	if (current_)
		node_ = current_->element->getNsNode();
	delete tmp;
}

