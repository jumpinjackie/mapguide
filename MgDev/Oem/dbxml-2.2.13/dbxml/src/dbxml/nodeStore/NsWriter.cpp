//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsWriter.cpp,v 1.21 2005/11/22 16:08:28 gmf Exp $
//

#include "NsUtil.hpp"
#include "NsWriter.hpp"
#include "NsConstants.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

using namespace DbXml;

//
// NsWriter implementation
//

NsWriter::NsWriter(bool writeRealEncoding)
	: _writeRealEncoding(writeRealEncoding),
	  _expandEntities(false)
{
	reset();
}

NsWriter::~NsWriter()
{
	// delete pre-defined namespace bindings
	int sz = _namespaces.size();
	while (sz--) {
		Binding *binding = _namespaces.back();
		DBXML_ASSERT(binding);
		delete binding;
		_namespaces.pop_back();
	}
}

void
NsWriter::reset()
{
	_entCount = 0;
	_stream = 0;

	_prefixCount = 0;
	Bindings::iterator end = _namespaces.end();
	for(Bindings::iterator it = _namespaces.begin();
	    it != end; ++it) {
		delete *it;
	}
	_namespaces.clear();
	_elementInfo.clear();

	// Set the pre-defined namespaces
	Binding *new_binding = new Binding();
	new_binding->prefix = _xmlnsPrefix8;
	new_binding->uri = _xmlnsUri8;
	_namespaces.push_back(new_binding);

	new_binding = new Binding();
	new_binding->prefix = _xmlPrefix8;
	new_binding->uri = _xmlUri8;
	_namespaces.push_back(new_binding);
}

void
NsWriter::xmlDecl(const xmlbyte_t *xmlDecl,
		  const xmlbyte_t *encodingStr,
		  const xmlbyte_t *standaloneStr)
{
	if(_entCount == 0) {
		_stream->write((const xmlbyte_t*)"<?xml version=\"");
		if(xmlDecl) _stream->write(xmlDecl);
		else _stream->write((const xmlbyte_t*)"1.1", 3); // Default to 1.1
		_stream->write((const xmlbyte_t*)"\"", 1);
		if(_writeRealEncoding) {
			// todo: add encoding interface(s), and put transcoding
			// support into the "stream" classes used for output.
			if (encodingStr) {
				_stream->write((const xmlbyte_t*)" encoding=\"");
				_stream->write(encodingStr);
				_stream->write((const xmlbyte_t*)"\"", 1);
			}
		} else {
			// For now, only UTF-8 is supported by NsWriter.
			// UTF-8 is the native encoding of node storage
			_stream->write((const xmlbyte_t*)" encoding=\"UTF-8\"");
		}
		if (standaloneStr) {
			_stream->write((const xmlbyte_t*)" standalone=\"");
			_stream->write(standaloneStr);
			_stream->write((const xmlbyte_t*)"\"", 1);
		}
		_stream->write((const xmlbyte_t*)"?>", 2);
	}
}

void
NsWriter::endDocument()
{
}

void
NsWriter::startElement(const xmlbyte_t *localName,
		       const xmlbyte_t *prefix,
		       const xmlbyte_t *uri,
		       int32_t uriId,
		       NsEventAttrList8 *attrs,
		       const uint32_t attrCount,
		       bool isEmpty,
		       bool isRoot,
		       const nsNode_t *node)
{
	if(_entCount == 0) {
		bool hasNsInfo = false;
		if(node != 0) hasNsInfo = nsHasNsinfo(node);
		storeElementBindings(attrs, attrCount, hasNsInfo);

		// check namespace binding
		bool define = false;
		if(uri) {
			define = checkNamespace(prefix, uri, true);
			_elementInfo.back().prefix = prefix;
		}

		// output element name
		_stream->write((const xmlbyte_t*)"<", 1);
		if(prefix && *prefix) {
			_stream->write(prefix);
			_stream->write((const xmlbyte_t*)":", 1);
		}
		_stream->write(localName);

		// output namespace declaration if needed
		if(define) {
			_stream->write((const xmlbyte_t*)" ", 1);
			_stream->write((const xmlbyte_t*)_xmlnsPrefix8);
			if(*prefix) {
				_stream->write((const xmlbyte_t*)":", 1);
				_stream->write(prefix);
			}
			_stream->write((const xmlbyte_t*)"=\"", 2);
			_stream->write(uri);
			_stream->write((const xmlbyte_t*)"\"", 1);
		}

		// output attributes
		if(attrCount) {
			for (int i = 0; i < attrs->numAttributes(); ++i) {
				if (!attrs->isSpecified(i))
					continue;
				const xmlbyte_t *value = attrs->value(i);
				uint32_t valLen = NsUtil::nsStringLen(value);
				bool freeValue = false;
				// check for escaping
				if (attrs->needsEscape(i)) {
					valLen <<= 3; // *8
					char *buf = (char*)::malloc(valLen);
					valLen = NsUtil::nsEscape(buf, value, valLen, true);
					value = (const xmlbyte_t *)buf;
					freeValue = true;
				}
				// check namesapce binding
				prefix = attrs->prefix(i);
				uri = attrs->uri(i);
				define = false;
				if(uri) {
					define = checkNamespace(prefix, uri, false);
				}

				// output attribute
				_stream->write((const xmlbyte_t*)" ", 1);
				if(prefix && *prefix) {
					_stream->write(prefix);
					_stream->write((const xmlbyte_t*)":", 1);
				}
				_stream->write(attrs->localName(i));
				_stream->write((const xmlbyte_t*)"=\"", 2);
				_stream->write(value, valLen);
				_stream->write((const xmlbyte_t*)"\"", 1);

				// output namespace declaration if needed
				if(define) {
					_stream->write((const xmlbyte_t*)" ", 1);
					_stream->write((const xmlbyte_t*)_xmlnsPrefix8);
					if(*prefix) {
						_stream->write((const xmlbyte_t*)":", 1);
						_stream->write(prefix);
					}
					_stream->write((const xmlbyte_t*)"=\"", 2);
					_stream->write(uri);
					_stream->write((const xmlbyte_t*)"\"", 1);
				}
				if (freeValue)
					::free((void*)value);
			}
		}
		if (isEmpty) {
			_stream->write((const xmlbyte_t*)"/>", 2);
			removeElementBindings(); // endElement will NOT be called
		} else {
			_stream->write((const xmlbyte_t*)">", 1);
		}
	}
}

void
NsWriter::endElement(const xmlbyte_t *localName,
		     const xmlbyte_t *prefix,
		     const xmlbyte_t *uri,
		     bool isRoot,
		     const nsNode_t *node)
{
	if(_entCount == 0) {
		_stream->write((const xmlbyte_t*)"</");

		// lookup the prefix we used for the start tag
		prefix = _elementInfo.back().prefix;
		if (prefix && *prefix) {
			_stream->write(prefix);
			_stream->write((const xmlbyte_t*)":", 1);
		}
		_stream->write(localName);
		_stream->write((const xmlbyte_t*)">", 1);

		removeElementBindings();
	}
}

void
NsWriter::characters(const xmlbyte_t *chars, uint32_t len, bool isCDATA,
		     bool needsEscape)
{
	if(_entCount == 0) {
		if (isCDATA) {
			_stream->write((const xmlbyte_t*)"<![CDATA[", 9);
			_stream->write(chars, len);
			_stream->write((const xmlbyte_t*)"]]>", 3);
		} else {
			char *buf = 0;
			if (needsEscape) {
				len <<= 3; // *8
				buf = (char*)malloc(len);
				len = NsUtil::nsEscape(buf, chars, len);
				chars = (const xmlbyte_t *)buf;
			}
			_stream->write(chars, len);
			if (buf)
				free(buf);
		}
	}
}

void
NsWriter::ignorableWhitespace(const xmlbyte_t *chars,
			      uint32_t len,
			      bool isCDATA)
{
	characters(chars, len, isCDATA, false);
}

void
NsWriter::comment(const xmlbyte_t *comment, uint32_t len)
{
	if(_entCount == 0) {
		_stream->write((const xmlbyte_t*)"<!--", 4);
		_stream->write(comment, len);
		_stream->write((const xmlbyte_t*)"-->", 3);
	}
}

void
NsWriter::processingInstruction(const xmlbyte_t *target,
				const xmlbyte_t *data)
{
	if(_entCount == 0) {
		_stream->write((const xmlbyte_t*)"<?", 2);
		_stream->write(target);
		if(data && *data) {
			_stream->write((const xmlbyte_t*)" ", 1);
			_stream->write(data);
		}
		_stream->write((const xmlbyte_t*)"?>", 2);
	}
}

void
NsWriter::docTypeDecl(const xmlbyte_t *data, uint32_t len)
{
	if(_entCount == 0) {
		_stream->write(data, len);
	}
}

void
NsWriter::startEntity(const xmlbyte_t *name, uint32_t len)
{
	if (!_expandEntities) {
		if(_entCount == 0) {
			_stream->write((const xmlbyte_t*)"&", 1);
			_stream->write(name, len);
			_stream->write((const xmlbyte_t*)";", 1);
		}
		++_entCount;
	}
}

void
NsWriter::endEntity(const xmlbyte_t *name, uint32_t len)
{
	if (!_expandEntities)
		--_entCount;
}

void
NsWriter::setExpandEntities(bool value)
{
	_expandEntities = value;
}

void
NsWriter::storeElementBindings(NsEventAttrList8 *attrs,
			       const uint32_t attrCount,
			       bool hasNsInfo)
{
	//
	// If the element does not have namespace bindings,
	// we store "false" in ElementInfo::hasNamespaces, and don't
	// change _namespaces;
	//
	// If the element does have namespace bindings,
	// we store "true" in ElementInfo::hasNamespaces, and then push
	// a null into _namespaces, to mark the boundary
	// between the previous bindings and our bindings.
	// Then we add the prefix/uri pairs to _namespaces.
	//

	_elementInfo.push_back(ElementInfo());
	if(hasNsInfo && attrCount) {
		for (int i = 0; i < attrs->numAttributes(); ++i) {
			if(NsUtil::nsStringEqual((const xmlbyte_t*)_xmlnsPrefix8, attrs->prefix(i))) {
				// Namespace binding
				if(!_elementInfo.back().hasNamespaces) {
					_namespaces.push_back(0);
					_elementInfo.back().hasNamespaces = true;
				}
				Binding *new_binding = new Binding();
				new_binding->prefix = (const char*)attrs->localName(i);
				new_binding->uri = (const char*)attrs->value(i);
				_namespaces.push_back(new_binding);
			}
			else if(NsUtil::nsStringEqual((const xmlbyte_t*)_xmlnsPrefix8, attrs->localName(i))) {
				// Default namespace declaration
				if(!_elementInfo.back().hasNamespaces) {
					_namespaces.push_back(0);
					_elementInfo.back().hasNamespaces = true;
				}
				Binding *new_binding = new Binding();
				new_binding->uri = (const char*)attrs->value(i);
				_namespaces.push_back(new_binding);
			}
		}
	}
}

void
NsWriter::removeElementBindings()
{
	bool hadNamespaces = _elementInfo.back().hasNamespaces;
	_elementInfo.pop_back();
	if(hadNamespaces) {
		Binding *binding = _namespaces.back();
		while(binding != 0) {
			delete binding;
			_namespaces.pop_back();
			binding = _namespaces.back();
		}
		_namespaces.pop_back(); // Pop the null
	}
}

// Returns the prefix to use for the uri, and true if the namespace needs defining
bool
NsWriter::checkNamespace(const xmlbyte_t *&prefix, const xmlbyte_t *uri, bool useDefault)
{
	if(prefix != 0) {
		bool thisScope;
		const xmlbyte_t *found_uri = lookupUri(prefix, thisScope);
		if(!NsUtil::nsStringEqual(found_uri, uri)) {
			if(!thisScope) {
				if(!_elementInfo.back().hasNamespaces) {
					_namespaces.push_back(0);
					_elementInfo.back().hasNamespaces = true;
				}

				Binding *new_binding = new Binding();
				new_binding->prefix = (const char*)prefix;
				new_binding->uri = (const char*)uri;
				_namespaces.push_back(new_binding);

				return true;
			}
		}
		else {
			return false;
		}
	}

	if(NsUtil::nsStringEqual(uri, (const xmlbyte_t*)_xmlnsUri8)) {
		// the "xmlns" attribute has a namespace, but no prefix
		return false;
	}

	const xmlbyte_t *found_prefix = lookupPrefix(uri);
	if(found_prefix == 0) {
		if(!_elementInfo.back().hasNamespaces) {
			_namespaces.push_back(0);
			_elementInfo.back().hasNamespaces = true;
		}

		Binding *new_binding = new Binding();
		new_binding->uri = (const char*)uri;
		if(!useDefault) {
			// Make up a new prefix
			std::ostringstream oss;
			oss << "ns_" << _prefixCount;
			++_prefixCount;

			new_binding->prefix = oss.str();
		}
		_namespaces.push_back(new_binding);

		prefix = (const xmlbyte_t*)new_binding->prefix.c_str();
		return true;
	}
	else {
		prefix = found_prefix;
		return false;
	}
}

const xmlbyte_t *
NsWriter::lookupUri(const xmlbyte_t *prefix, bool &thisScope)
{
	// Nearer prefix bindings override further ones... - jpcs
	thisScope = _elementInfo.back().hasNamespaces;
	Bindings::reverse_iterator rend = _namespaces.rend();
	for(Bindings::reverse_iterator it = _namespaces.rbegin();
	    it != rend; ++it) {
		if(*it != 0) {
			if(NsUtil::nsStringEqual((const xmlbyte_t*)(*it)->prefix.c_str(), prefix))
				return (const xmlbyte_t*)(*it)->uri.c_str();
		}
		else {
			thisScope = false;
		}
	}
	return 0;
}

const xmlbyte_t *
NsWriter::lookupPrefix(const xmlbyte_t *uri)
{
	// It is assumed the binding is more likely to be nearer
	// the prefix use... - jpcs
	bool thisScope;
	Bindings::reverse_iterator rend = _namespaces.rend();
	for(Bindings::reverse_iterator it = _namespaces.rbegin();
	    it != rend; ++it) {
		if(*it != 0 && NsUtil::nsStringEqual((const xmlbyte_t *)(*it)->uri.c_str(), uri)) {
			// Make sure the prefix has not been overridden
			if(NsUtil::nsStringEqual(lookupUri((const xmlbyte_t*)(*it)->prefix.c_str(), thisScope), uri)) {
				return (const xmlbyte_t*)(*it)->prefix.c_str();
			}
		}
	}
	return 0;
}
