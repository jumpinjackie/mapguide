//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsEvent.cpp,v 1.12 2005/06/09 09:37:23 jsnelson Exp $
//

#include "NsEvent.hpp"
#include "NsUtil.hpp"
#include "NsDocument.hpp"
#include "../UTF8.hpp"
#include <xercesc/framework/MemoryManager.hpp>

using namespace DbXml;

//
// NsPushEventSourceTranslator8 implementation
//
NsPushEventSourceTranslator8::NsPushEventSourceTranslator8(NsPushEventSource *source,
						   NsEventTranslator *translator)
	: source_(source), translator_(translator)
{
	source_->setNsEventHandler(translator_);
}

NsPushEventSourceTranslator8::~NsPushEventSourceTranslator8()
{
	delete source_;
	delete translator_;
}

void NsPushEventSourceTranslator8::start()
{
	source_->start();
}

NsEventHandler8 *NsPushEventSourceTranslator8::getNsEventHandler(void) const
{
	return translator_->getNsEventHandler();
}

void NsPushEventSourceTranslator8::setNsEventHandler(NsEventHandler8 *handler)
{
	translator_->setNsEventHandler(handler);
}

//
// NsEventHandler(8) implementation
//
NsEventAttrList::~NsEventAttrList()
{
}

void
NsEventHandler::docTypeDecl(const xmlch_t *, uint32_t)
{
	
}

void
NsEventHandler::startEntity(const xmlch_t *, uint32_t)
{
}

void
NsEventHandler::endEntity(const xmlch_t *, uint32_t)
{
}

void
NsEventHandler8::docTypeDecl(const xmlbyte_t *, uint32_t)
{
	
}

void
NsEventHandler8::startEntity(const xmlbyte_t *, uint32_t)
{
}

void
NsEventHandler8::endEntity(const xmlbyte_t *, uint32_t)
{
}

//
// NsEventNodeAttrList8 implementation
// NOTE: little/no range, or existence checking performed on accessors
//
NsEventNodeAttrList8::NsEventNodeAttrList8(nsAttrList_t *list, NsDocument &doc, bool isUTF16)
	: _list(list),
	  _doc(doc),
	  _qnames(0),
	  _isUTF16(isUTF16),
	  _localnames(0),
	  _values(0)
{
}

NsEventNodeAttrList8::~NsEventNodeAttrList8()
{
	XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = _doc.getMemoryManager();
	xmlbyte_t **i, **end;

	if(_qnames != 0) {
		end = _qnames + _list->al_nattrs;
		for(i = _qnames; i != end; ++i) {
			if(*i != 0) mm->deallocate(*i);
		}
		mm->deallocate(_qnames);
	}

	if(_localnames) {
		end = _localnames + _list->al_nattrs;
		for(i = _localnames; i != end; ++i) {
			if(*i != 0) mm->deallocate(*i);
		}
		mm->deallocate(_localnames);
	}

	if(_values) {
		end = _values + _list->al_nattrs;
		for(i = _values; i != end; ++i) {
			if(*i != 0) mm->deallocate(*i);
		}
		mm->deallocate(_values);
	}
}

const xmlbyte_t *
NsEventNodeAttrList8::localName(int index) const
{
	if(_isUTF16) {
		if(_localnames == 0) {
			size_t size = _list->al_nattrs * sizeof(xmlbyte_t*);
			_localnames = (xmlbyte_t**)_doc
				.getMemoryManager()->allocate(size);
			if(_localnames == 0)
				NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
							 "localName failed to allocate memory",
							 __FILE__, __LINE__);
			memset(_localnames, 0, size);
		}

		xmlbyte_t *result = _localnames[index];
		if(result == 0) {
			// Lazily transcode
			const xmlch_t *name = (const xmlch_t*)_list->al_attrs[index].a_name8;
			NsUtil::nsToUTF8(_doc.getMemoryManager(), &_localnames[index],
					 name, NsUtil::nsStringLen(name) + 1, 0);
			result = _localnames[index];
		}
		return result;
	}
	else {
		return (xmlbyte_t *)(_list->al_attrs[index].a_name8);
	}
}

const xmlbyte_t *
NsEventNodeAttrList8::value(int index) const
{
	if(_isUTF16) {
		if(_values == 0) {
			size_t size = _list->al_nattrs * sizeof(xmlbyte_t*);
			_values = (xmlbyte_t**)_doc
				.getMemoryManager()->allocate(size);
			if(_values == 0)
				NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
							 "value failed to allocate memory",
							 __FILE__, __LINE__);
			memset(_values, 0, size);
		}

		xmlbyte_t *result = _values[index];
		if(result == 0) {
			// Lazily transcode
			const xmlch_t *value = (const xmlch_t*)_list->al_attrs[index].a_value;
			NsUtil::nsToUTF8(_doc.getMemoryManager(), &_values[index],
					 value, NsUtil::nsStringLen(value) + 1, 0);
			result = _values[index];
		}
		return result;
	}
	else {
		return (const xmlbyte_t *)(_list->al_attrs[index].a_value);
	}
}

int
NsEventNodeAttrList8::numAttributes(void) const
{
	if (_list)
		return (_list->al_nattrs);
	return 0;
}

const xmlbyte_t *
NsEventNodeAttrList8::prefix(int index) const
{
	int prefix = _list->al_attrs[index].a_prefix;
	return _doc.getPrefix8(prefix);
}

const xmlbyte_t *
NsEventNodeAttrList8::uri(int index) const
{
	int uri = _list->al_attrs[index].a_uri;
	return _doc.getUri8(uri);
}

const xmlbyte_t *
NsEventNodeAttrList8::typeName(int index) const
{
	int type = _list->al_attrs[index].a_type;
	return _doc.getPrefix8(type);
}

const xmlbyte_t *
NsEventNodeAttrList8::typeUri(int index) const
{
	int type = _list->al_attrs[index].a_type;
	return (_doc.getUriForPrefix8(type));
}

const xmlbyte_t *
NsEventNodeAttrList8::qname(int index)
{
	if(_qnames == 0) {
		size_t size = _list->al_nattrs * sizeof(xmlbyte_t*);
		_qnames = (xmlbyte_t**)_doc
			.getMemoryManager()->allocate(size);
		if(_qnames == 0)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "qname failed to allocate memory",
						 __FILE__, __LINE__);
		memset(_qnames, 0, size);
	}

	xmlbyte_t *result = _qnames[index];
	if (result == 0) {
		int len = 0;
		qnameBuf(index, NULL, &len);
		_qnames[index] = (xmlbyte_t *)_doc.getMemoryManager()->allocate(len);
		result = _qnames[index];
		if(result == 0)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
						 "qname failed to allocate memory",
						 __FILE__, __LINE__);
		qnameBuf(index, result, &len);
	}
	return result;
}

const xmlbyte_t *
NsEventNodeAttrList8::qnameBuf(int index, xmlbyte_t *dest,
			       int *destLen) const
{
	nsAttr_t *attr = &_list->al_attrs[index];
	int len = 0;

	int prefix = attr->a_prefix;
	const xmlbyte_t *prefix8 = NULL;
	int plen = 0;
	if (prefix != NS_NOPREFIX) {
		prefix8 = _doc.getPrefix8(prefix);
		plen = NsUtil::nsStringLen(prefix8);
		len += plen + 1; // add for ':'
	}

	const xmlbyte_t *localname8 = localName(index);
	int nlen = NsUtil::nsStringLen(localname8);
	len += nlen + 1; // add for null

	if (*destLen < len) {
		// not enough space
		*destLen = len;
		return NULL;
	}
	*destLen = len;

	xmlbyte_t *ptr = dest;
	if (prefix8) {
		memcpy(ptr, prefix8, plen);
		ptr += plen;
		*ptr = ':';
		ptr += 1;
	}
	memcpy(ptr, localname8, nlen + 1);

	return dest;
}

const bool
NsEventNodeAttrList8::needsEscape(int index) const
{
	nsAttr_t *attr = &_list->al_attrs[index];
	if (attr->a_flags & NS_ATTR_ENT)
		return true;
	return false;
}

const bool
NsEventNodeAttrList8::isSpecified(int index) const
{
	nsAttr_t *attr = &_list->al_attrs[index];
	if (attr->a_flags & NS_ATTR_NOT_SPECIFIED)
		return false;
	return true;
}
