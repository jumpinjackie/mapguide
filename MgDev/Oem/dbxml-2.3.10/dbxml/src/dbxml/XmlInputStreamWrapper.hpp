//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: XmlInputStreamWrapper.hpp,v 1.9 2006/10/30 17:45:54 bostic Exp $
//

#ifndef __XMLINPUTSTREAMWRAPPER_HPP
#define	__XMLINPUTSTREAMWRAPPER_HPP

#include <dbxml/XmlInputStream.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

namespace DbXml
{

class XmlBinStream : public XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream
{
public:
        XmlBinStream(XmlInputStream *is) : is_(is) {}
	virtual unsigned int curPos() const {
		return is_->curPos();
	}
	virtual unsigned int readBytes(XMLByte *const toFill,
				       const unsigned int max) {
		return is_->readBytes((char *)toFill, max);
	}
private:
	XmlInputStream *is_;
};
	
/**
 * Wrapper classes for XmlInputStream that implement Xerces
 * InputSource.  Oddly enough, the default implementations of
 * XmlInputStream *use* InputSource, but hide that info here,
 * to allow user-defined instances. Will hold a reference to a
 * Document object, if required to.
 */
class XmlInputStreamWrapper : public XERCES_CPP_NAMESPACE_QUALIFIER InputSource
{
public:
	XmlInputStreamWrapper(XmlInputStream **is)
		: is_(*is), publicId_(0), systemId_(0) {
		*is = 0;
	}
	virtual ~XmlInputStreamWrapper() {
		delete is_;
		if (publicId_)
			delete [] publicId_;
		if (systemId_)
			delete [] systemId_;
	}
	virtual XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream *makeStream() const {
		return new XmlBinStream(is_);
	}
	virtual const XMLCh *getEncoding() const { return 0; }
	virtual const XMLCh *getPublicId() const { return publicId_; }
	virtual const XMLCh *getSystemId() const { return systemId_; }

	void setPublicId(const XMLCh* const id) {
		if (publicId_)
			delete [] publicId_;
		publicId_ = XERCES_CPP_NAMESPACE_QUALIFIER
			XMLString::replicate(id);
	}
	void setSystemId(const XMLCh* const id) {
		if (systemId_)
			delete [] systemId_;
		systemId_ = XERCES_CPP_NAMESPACE_QUALIFIER
			XMLString::replicate(id);
	}
private:
	XmlInputStream *is_;
	XMLCh *publicId_;
	XMLCh *systemId_;
};

}

#endif
