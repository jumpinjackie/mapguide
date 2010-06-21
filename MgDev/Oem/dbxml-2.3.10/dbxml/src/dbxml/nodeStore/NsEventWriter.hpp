//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: NsEventWriter.hpp,v 1.10 2006/11/03 17:26:11 gmf Exp $
//

#ifndef __NSEVENTWRITER_HPP
#define __NSEVENTWRITER_HPP

#include <dbxml/XmlEventWriter.hpp>
#include "NsHandlerBase.hpp"
#include "NsDocument.hpp"
#include <string>

/*
 * NsEventWriter
 *
 * Implementation of XmlEventWriter that will push events
 * to one, and optionally two EventWriter objects for
 * indexing and (optionally) writing.
 *
 * Implements XmlEventWriter and inherits from NsHandlerBase in
 * order to create node storage documents
 */
class DbException;
 
namespace DbXml
{

class NsDocument;
class NsDocumentDatabase;
class NsWriter;
class BufferNsStream;
class EventWriter;
class NsEventNodeAttrList;
class Container;
class UpdateContext;
class Document;
class XmlException;
	
//
// NsEventWriter
//
class NsEventWriter : public XmlEventWriter, public NsHandlerBase
{
public:
	// Construction
	NsEventWriter(Container *container, UpdateContext *uc,
		      Document *document, u_int32_t flags,
		      XER_NS MemoryManager *mmgr);
	// stripped-down version, only used by upgrade (for now)
	NsEventWriter(NsDocumentDatabase *ndd,
		      DictionaryDatabase *dict,
		      const DocID &id,
		      XER_NS MemoryManager *mmgr);
	virtual ~NsEventWriter();

	virtual void close();
	
	void createNsWriter();

	//
	// XmlEventWriter interface
	//

	// attributes (including namespaces)
	virtual void writeAttribute(const unsigned char *localName,
				    const unsigned char *prefix,
				    const unsigned char *uri,
				    const unsigned char *value,
				    bool isSpecified);

	// text, comments, CDATA, ignorable whitespace
	// length does not include trailing null
	virtual void writeText(XmlEventReader::XmlEventType type,
			       const unsigned char *text,
			       int length);

	// processing Instruction
	virtual void writeProcessingInstruction(const unsigned char *target,
						const unsigned char *data);

	// elements
	virtual void writeStartElement(const unsigned char *localName,
				       const unsigned char *prefix,
				       const unsigned char *uri,
				       int numAttributes,
				       bool isEmpty);
	virtual void writeEndElement(const unsigned char *localName,
				     const unsigned char *prefix,
				     const unsigned char *uri);
	
	// xml declaration, DTD
	virtual void writeDTD(const unsigned char *dtd, int length);
	virtual void writeStartDocument(const unsigned char *version,
					const unsigned char *encoding,
					const unsigned char *standalone);
	virtual void writeEndDocument();

	// note the start of entity expansion
	virtual void writeStartEntity(const unsigned char *name,
				      bool expandedInfoFollows);

	// note the end of entity expansion
	virtual void writeEndEntity(const unsigned char *name);
private:
	void doStartElem(NsEventNodeAttrList *attrs = 0);
	void throwBadWrite(const char *msg);
	void exceptionNotify(XmlException &xe);
	void exceptionNotify(DbException &de);
	void completeDoc();
private:
	NsDocument nsdoc_;
	UpdateContext *uc_;
	Container *cont_;
	Document *doc_;
	NsDocumentDatabase *ndd_;
	DocID docId_;
	NsWriter *writer_;
	BufferNsStream *writerStream_;
	bool mustBeEnd_;
	bool success_;
	bool needsStartElement_;
	bool isEmpty_;
	int attrsToGo_;
};
}
	
#endif

