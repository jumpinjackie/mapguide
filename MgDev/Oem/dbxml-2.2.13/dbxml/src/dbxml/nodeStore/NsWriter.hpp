//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsWriter.hpp,v 1.13 2005/11/22 18:10:33 gmf Exp $
//

#ifndef __DBXMLNSWRITER_HPP
#define __DBXMLNSWRITER_HPP

#include <iostream>
#include <vector>

#include "NsEvent.hpp"
#include "../Buffer.hpp"
#include "NsUtil.hpp"

namespace DbXml {

class NsStream8 {
public:
	virtual ~NsStream8() {}
	virtual void write(const xmlbyte_t *data) = 0;
	virtual void write(const xmlbyte_t *data, uint32_t len) = 0;
};

class OStreamNsStream8 : public NsStream8 {
public:
	OStreamNsStream8(std::ostream &os) : _os(os) {}
	virtual void write(const xmlbyte_t *data)
		{ _os << (char*)data; }
	virtual void write(const xmlbyte_t *data, uint32_t len)
		{ _os << (char*)data; }
private:
	std::ostream &_os;
};

class BufferNsStream8 : public NsStream8 {
public:
	virtual ~BufferNsStream8() {}
	virtual void write(const xmlbyte_t *data)
		{ write(data, NsUtil::nsStringLen(data)); }
	virtual void write(const xmlbyte_t *data, uint32_t len)
		{ buffer.write(data, len); }
	Buffer buffer;
};

/**
 * NsWriter
 *
 * An instance of NsEventHandler that is used to serialized node
 * storage documents. It uses the NsEventHandler8 interface.
 */
class NsWriter : public NsEventHandler8 {
public:
	NsWriter(bool writeRealEncoding = false);
	virtual ~NsWriter();
	//
	// the NsEventHandler8 interface
	//
	virtual void xmlDecl(const xmlbyte_t *xmlDecl,
			     const xmlbyte_t *encodingStr,
			     const xmlbyte_t *standaloneStr);
	virtual void endDocument();
	virtual void startElement(const xmlbyte_t *localName,
				  const xmlbyte_t *prefix,
				  const xmlbyte_t *uri,
				  int32_t uriId,
				  NsEventAttrList8 *attrs,
				  const uint32_t attrCount,
				  bool isEmpty,
				  bool isRoot,
				  const nsNode_t *node);
	virtual void endElement(const xmlbyte_t *localName,
				const xmlbyte_t *prefix,
				const xmlbyte_t *uri,
				bool isRoot,
				const nsNode_t *node);
	virtual void characters(const xmlbyte_t *characters, uint32_t len,
				bool isCDATA, bool needsEscape);
	virtual void ignorableWhitespace(const xmlbyte_t *characters,
					 uint32_t len,
					 bool isCDATA);
	virtual void comment(const xmlbyte_t *comment, uint32_t len);
	virtual void processingInstruction(const xmlbyte_t *target,
					   const xmlbyte_t *data);
	virtual void setTypeInfo(const xmlbyte_t *typeName,
				 const xmlbyte_t *typeUri) {}
	virtual void docTypeDecl(const xmlbyte_t *data, uint32_t len);
	virtual void startEntity(const xmlbyte_t *name, uint32_t len);
	virtual void endEntity(const xmlbyte_t *name, uint32_t len);
	
	// class-specific implementation
	void setStream(NsStream8 *stream) { _stream = stream; }
	NsStream8 *getStream() const { return _stream; }

	void setExpandEntities(bool value); // defaults to false
	void reset(void);
private:
	struct ElementInfo {
		ElementInfo() : hasNamespaces(false), prefix(0) {}
		bool hasNamespaces;
		const xmlbyte_t *prefix;
	};
	typedef std::vector<ElementInfo> ElementInfoVector;

	struct Binding {
		std::string prefix;
		std::string uri;
	};
	typedef std::vector<Binding*> Bindings;

	void storeElementBindings(NsEventAttrList8 *attrs,
				  const uint32_t attrCount,
				  bool hasNsInfo);
	void removeElementBindings();
	// Returns the prefix to use for the uri, and true if the namespace needs defining
	bool checkNamespace(const xmlbyte_t *&prefix, const xmlbyte_t *uri, bool useDefault);
	const xmlbyte_t *lookupUri(const xmlbyte_t *prefix, bool &thisScope);
	const xmlbyte_t *lookupPrefix(const xmlbyte_t *uri);
private:
	bool _writeRealEncoding;
	bool _expandEntities;
	uint32_t _entCount;
	NsStream8 *_stream;

	unsigned int _prefixCount;
	ElementInfoVector _elementInfo;
	Bindings _namespaces;
};

}

#endif
