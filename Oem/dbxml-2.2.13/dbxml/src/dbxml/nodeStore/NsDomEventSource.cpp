//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsDomEventSource.cpp,v 1.11 2005/09/20 14:35:54 jsnelson Exp $
//

#include "NsUtil.hpp"
#include "NsDomEventSource.hpp"
#include "NsDom.hpp"
#include "NsDocument.hpp"
#include "../UTF8.hpp"

namespace DbXml
{
class NsDomEventNodeList {
public:
	enum Progress {
		Start,
		XmlDecl,
		Children,
		End
	};

	NsDomEventNodeList(NsDomEventNodeList *p)
		: progress(Start), prefix(0), uri(0), localname(0), utf16(false), child(0), parent(p) {}
	
	NsDomElement *element;
	Progress progress;

	const xmlbyte_t *prefix;
	const xmlbyte_t *uri;
	xmlbyte_t *localname;
	bool utf16;

	NsDomNav *child;

	NsDomEventNodeList *parent;
};
}

using namespace DbXml;

NsDomEventSource8::NsDomEventSource8(NsDomElement *node)
	: current_(new NsDomEventNodeList(0))
{
	current_->element = node;
}

NsDomEventSource8::~NsDomEventSource8()
{
	if(current_ != 0) {
		NsDocument *doc = current_->element->getNsDocument();

		while(current_ != 0) {
			NsDomEventNodeList *tmp = current_;
			current_ = tmp->parent;

			if(tmp->utf16 && tmp->localname != 0) {
				doc->getMemoryManager()->deallocate(tmp->localname);
			}
			delete tmp;
		}
	}
}


bool NsDomEventSource8::startEvent(NsEventHandler8 *handler)
{
	NsDocument *doc = current_->element->getNsDocument();

	if(current_->element->getNsNodeType() == nsNodeDocument) {
		handler->startDocument(doc->getSniffedEncodingStr());
		current_->progress = NsDomEventNodeList::XmlDecl;
		return true;
	} else {
		nsNode_t *node = current_->element->getNsNode();
		uint32_t attrcount = nsNumAttrs(node);
		current_->utf16 = nsIsUTF16(node);

		const nsName_t *name = nsName(node);
		current_->prefix = (name->n_prefix == NS_NOPREFIX ? 0 :
			doc->getPrefix8(nsNamePrefix(node)));
		current_->uri = (nsHasUri(node) ? doc->getUri8(nsUriIndex(node)) : 0);
		if(current_->utf16) NsUtil::nsToUTF8(doc->getMemoryManager(), &current_->localname,
			(const xmlch_t*)name->n_text.t_chars,
			name->n_text.t_len + 1, 0);
		else current_->localname = (xmlbyte_t*)name->n_text.t_chars;

		if(attrcount != 0) {
			NsEventNodeAttrList8 attrs(node->nd_attrs, *doc, current_->utf16);
			handler->startElement(current_->localname, current_->prefix, current_->uri, nsUriIndex(node),
				&attrs, attrcount, !nsHasChildNode(node),
				nsIsRoot(node), node);
		}
		else {
			handler->startElement(current_->localname, current_->prefix, current_->uri, nsUriIndex(node),
				0, 0, !nsHasChildNode(node),
				nsIsRoot(node), node);
		}
		current_->progress = NsDomEventNodeList::Children;
		return true;
	}
}

bool NsDomEventSource8::endEvent(NsEventHandler8 *handler)
{
	NsDocument *doc = current_->element->getNsDocument();

	bool result = false;

	if(current_->element->getNsNodeType() == nsNodeDocument) {
		handler->endDocument();
		result = true;
	} else {
		nsNode_t *node = current_->element->getNsNode();
		if(nsHasChildNode(node)) {
			handler->endElement(current_->localname, current_->prefix, current_->uri, nsIsRoot(node), node);
			result = true;
		}
	}

	NsDomEventNodeList *tmp = current_;
	current_ = tmp->parent;

	if(tmp->utf16 && tmp->localname != 0) {
		doc->getMemoryManager()->deallocate(tmp->localname);
	}
	delete tmp;

	return result;
}

bool NsDomEventSource8::xmlDeclEvent(NsEventHandler8 *handler)
{
	NsDocument *doc = current_->element->getNsDocument();

	current_->progress = NsDomEventNodeList::Children;

	if(doc->getXmlDecl8() != 0 ||
		doc->getEncodingStr8() != 0 ||
		doc->getStandaloneStr() != 0) {
		handler->xmlDecl(doc->getXmlDecl8(),
			doc->getEncodingStr8(),
			doc->getStandaloneStr());
		return true;
	}

	return false;
}

bool NsDomEventSource8::childEvent(NsEventHandler8 *handler)
{
	NsDocument *doc = current_->element->getNsDocument();

	if(current_->child == 0) {
		current_->child = current_->element->getNsFirstChild();
	} else {
		current_->child = current_->child->getNsNextSibling();
	}

	if(current_->child == 0) {
		current_->progress = NsDomEventNodeList::End;
		return false;
	} else {
		switch(current_->child->getNsNodeType()) {
		case nsNodeDocument:
		case nsNodeElement: {
			NsDomEventNodeList *new_current = new NsDomEventNodeList(current_);
			new_current->element = (NsDomElement*)current_->child;
			current_ = new_current;
			return false;
		}
		case nsNodeEntStart:
		case nsNodeEntEnd:
		case nsNodeText: {
			NsDomText *domtext = (NsDomText*)current_->child;

			nsNode_t *node = domtext->getNsNode();
			bool utf16 = nsIsUTF16(node);
			nsTextEntry_t *tentry = nsTextEntry(node, domtext->getIndex());
			nsText_t *text = &(tentry->te_text);
			uint32_t textType = nsTextType(tentry->te_type);

			bool needsEscaping = nsTextEntityChk(tentry->te_type);
			xmlbyte_t *value = 0;
			int vlen = 0;
			if(utf16) {
				enum checkType ctype =
					(textType == NS_TEXT ? isCharacters :
						ignore);
				vlen = NsUtil::nsToUTF8(doc->getMemoryManager(), &value,
					(const xmlch_t*)text->t_chars,
					text->t_len + 1, 0, &needsEscaping, ctype);
				vlen -= 1;
			} else {
				value = (xmlbyte_t*)text->t_chars;
				vlen = text->t_len;
			}

			try {
				switch(textType) {
				case NS_ENTSTART: {
					handler->startEntity(value, vlen);
					break;
				}
				case NS_ENTEND: {
					handler->endEntity(value, vlen);
					break;
				}
				case NS_CDATA: {
					handler->characters(value, vlen,
						true, false);
					break;
				}
				case NS_TEXT: {
					handler->characters(value, vlen,
						false, needsEscaping);
					break;
				}
				case NS_COMMENT: {
					handler->comment(value, vlen);
					break;
				}
				case NS_PINST: {
					//
					// PI is encoded target0data0
					//
					const xmlbyte_t *data = value;
					while (*data++); // get past NULL
					handler->processingInstruction(value, data);
					break;
				}
				case NS_SUBSET: {
					handler->docTypeDecl(value, vlen);
					break;
				}
				default: DBXML_ASSERT(0);
				}
			}
			catch(...) {
				if(utf16) doc->getMemoryManager()->deallocate(value);
				throw;
			}

			if(utf16) doc->getMemoryManager()->deallocate(value);
			return true;
		}
		default: DBXML_ASSERT(0);
		}
	}

	return false;
}

bool NsDomEventSource8::nextEvent(NsEventHandler8 *handler)
{
	bool result = false;
	if(current_ != 0) {
		NsDocument *doc = current_->element->getNsDocument();

		bool entityText = doc->getCreateEntityText();
		try {
			doc->setCreateEntityText(true);

			while(current_ != 0 && !result) {
				switch(current_->progress) {
				case NsDomEventNodeList::Start: {
					result = startEvent(handler);
					break;
				}
				case NsDomEventNodeList::XmlDecl: {
					result = xmlDeclEvent(handler);
					break;
				}
				case NsDomEventNodeList::Children: {
					result = childEvent(handler);
					break;
				}
				case NsDomEventNodeList::End: {
					result = endEvent(handler);
					break;
				}
				}
			}
		}
		catch(...) {
			doc->setCreateEntityText(entityText);
			throw;
		}
		doc->setCreateEntityText(entityText);
	}
	return result;
}
