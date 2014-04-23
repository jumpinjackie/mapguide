//
//  Copyright (C) 1996-2011 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE CORRECTNESS
//  OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE IT. AUTODESK
//  PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY DISCLAIMS ANY
//  LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//


#ifndef _DWFTK_DWFXMLSERIALIZER_H
#define _DWFTK_DWFXMLSERIALIZER_H


///
///\file        dwf/package/writer/DWFXMLSerializer.h
///\brief       This file contains the DWFXMLSerializer class declaration.
///

#ifndef DWFTK_READ_ONLY




#include "dwfcore/STL.h"
#include "dwfcore/UUID.h"
#include "dwfcore/BufferOutputStream.h"
#include "dwfcore/XMLEncodingOutputStream.h"
#include "dwfcore/UTF8EncodingOutputStream.h"
using namespace DWFCore;

#include "dwf/Toolkit.h"
#include "dwf/package/XML.h"

    //
    //
    //
#ifndef DWFTK_XMLSERIALIZER_WRITE_BUFFER_BYTES
#define DWFTK_XMLSERIALIZER_WRITE_BUFFER_BYTES      16384
#endif

    //
    //
    //
#ifndef DWFTK_XMLSERIALIZER_ENCODING_BUFFER_BYTES
#define DWFTK_XMLSERIALIZER_ENCODING_BUFFER_BYTES   4096
#endif


namespace DWFToolkit
{

///
///\ingroup     dwfpackage
///
///\class       DWFXMLSerializer   dwf/package/writer/DWFXMLSerializer.h     "dwf/package/writer/DWFXMLSerializer.h"
///\brief       An object that facilitates the publishing of toolkit data objects into XML.
///\since       7.0.1
///
class DWFXMLSerializer  _DWFTK_EXPORT_API_MEMORY_CONTROL_BASE
{

public:

    ///
    ///\brief   Named enumerations for use during the serialization.
    ///
    typedef enum
    {
        eElementOpen = 0x0100

    } teFlags;

public:

    ///
    ///         Constructor
    ///
    ///\param   rUUID       A unique identifier generator that is available
    ///                     to consumers of the serializer.  
    ///\throw   None
    ///
    _DWFTK_API
    DWFXMLSerializer( DWFUUID& rUUID )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFTK_API
    virtual ~DWFXMLSerializer()
        throw();

    ///
    ///         Binds a new output stream to the serializer.
    ///
    ///         All subsequent output will be written to \a rStream.
    ///
    ///\param   rStream     The XML destination stream.
    ///\throw   DWFException
    ///
    _DWFTK_API
    void attach( DWFOutputStream& rStream )
        throw( DWFException );

    ///
    ///         Unbinds an output stream from the serializer.
    ///
    ///\throw   DWFException
    ///
    _DWFTK_API
    void detach()
        throw( DWFException );

    ///
    ///         Helper utility that provides a unique but sequential UUID
    ///         that can be used by client code for object IDs, etc.
    ///
    ///         Sequential IDs are preferred as the compress better resulting
    ///         in smaller DWF package files.
    ///
    ///\param       bSquash     If \e true, the 128-bit uuid will be Base64 encoded
    ///                         into a short form string representation, otherwise the 
    ///                         standard form string representation will be returned.
    ///                         The encoding used for the short form string will be
    ///                         non RFC 1521 compliant as detailed in \a DWFString::EncodeBase64().
    ///\throw   None
    ///
    _DWFTK_API
    const DWFString& nextUUID( bool bSquash )
        throw();

    ///
    ///         Opens a new element tag in the XML data stream.
    ///
    ///\param   zName           The name of the element.
    ///\param   zNamespace      An optional namespace to add to the element.
    ///\throw   DWFException
    ///
    _DWFTK_API
    void startElement( const DWFString& zName, const DWFString& zNamespace = L"" )
        throw( DWFException );

    ///
    ///         Inserts a new attribute into an open element.
    ///
    ///\param   zName           The name of the attribute.
    ///\param   zValue          The value of the attribute.
    ///\param   zNamespace      An optional namespace to add to the attribute.
    ///\throw   DWFException
    ///
    _DWFTK_API
    void addAttribute( const DWFString& zName, const DWFString& zValue, const DWFString& zNamespace = L"" )
        throw( DWFException );

    ///
    ///         Insert a block of CDATA to the XML data stream.
    ///
    ///\param   zCData          The CDATA text to add.
    ///\throw   DWFException
    ///
    _DWFTK_API
    void addCData( const DWFString& zCData )
        throw( DWFException );
        
    ///
    ///         Closes an open element in the XML data stream.
    ///
    ///\throw   DWFException
    ///
    _DWFTK_API
    void endElement()
        throw( DWFException );

private:

    class XMLOutputStream : public DWFBufferOutputStream
    {
    public:

        XMLOutputStream( DWFOutputStream& rOutputStream )
            throw();

        virtual ~XMLOutputStream()
            throw();

        //
        // use this method for publishing the xml data
        // it will be encoded correctly
        //
        size_t writeXML( const void*   pBuffer,
                         size_t        nBytesToWrite )
            throw( DWFException );

        //
        // stream interface - called back into for
        // writing to buffer
        //
        size_t write( const void*   pBuffer,
                      size_t        nBytesToWrite )
            throw( DWFException );

        //
        // toggles XML encoding - certain characters "<", ">", etc
        // shouldn't be XML encoded as they are written to the stream
        //
        void encode( bool bXML = true )
        {
            _bXML = bXML;
        }

    private:
        bool                        _bXML;
        DWFXMLEncodingOutputStream  _oXMLStream;
        DWFUTF8EncodingOutputStream _oUTF8Stream;
    };

private:

    DWFUUID&            _rUUID;
    XMLOutputStream*    _pStream;

    bool                _bLastElementOpen;
    const DWFString*    _pLastElement;
    vector<DWFString>   _oDepthChart;

private:

    //
    // Unimplemented methods
    //

    DWFXMLSerializer( const DWFXMLSerializer& );
    DWFXMLSerializer& operator=( const DWFXMLSerializer& );
};

}


#endif
#endif

