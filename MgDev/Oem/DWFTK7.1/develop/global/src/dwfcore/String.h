//
//  Copyright (C) 2003-2010 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED,
//  AS TO THE CORRECTNESS OF THIS CODE OR ANY DERIVATIVE
//  WORKS WHICH INCORPORATE IT.
//
//  AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS
//  AND EXPLICITLY DISCLAIMS ANY LIABILITY, INCLUDING
//  CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//

#ifndef _DWFCORE_STRING_H
#define _DWFCORE_STRING_H

///
///\file        dwfcore/String.h
///\brief       This header contains the declaration of the DWFString class.
///

///
///\example     Strings/Strings.cpp
///             This sample program exercises some of the operations available in DWFCore::DWFString.
///
///             Also shown is some simple file I/O using the following core classes and interfaces:
///                 - DWFCore::DWFFile
///                 - DWFCore::DWFFileOutputStream
///                 - DWFCore::DWFStreamFileDescriptor
///
///             This file may contain Unicode text that may display incorrectly in the documentation
///             and/or not display/save/load correctly on certain filesystems and text editors, etc.
///


#include "dwfcore/Core.h"
#include "dwfcore/Exception.h"
#include "dwfcore/Pointer.h"



#ifndef DWFCORE_STRING_STACK_BUFFER_SIZE_IN_BYTES
///
///\ingroup     StringMacros
///\brief       Sets the stack buffer size in bytes if applicable.
///
#define DWFCORE_STRING_STACK_BUFFER_SIZE_IN_BYTES       32
#define _DWFCORE_STRING_STACK_BUFFER_SIZE_IN_WCHARS     (DWFCORE_STRING_STACK_BUFFER_SIZE_IN_BYTES>>(sizeof(wchar_t)>>1))
#define _DWFCORE_STRING_STACK_BUFFER_SIZE_IN_INTS       (DWFCORE_STRING_STACK_BUFFER_SIZE_IN_BYTES>>(sizeof(int)>>1))
#endif

#ifndef DWFCORE_STRING_PREFER_HEAP_LIMIT_IN_BYTES
///
///\ingroup     StringMacros
///\brief       Sets the heap reuse limit.
///
///             If a DWFString heap buffer \e can be reused because
///             the incoming data is smaller than the currently allocated buffer,
///             this value will determine whether or not the buffer \e will be reused
///             or be discarded because it is too big.
///
#define DWFCORE_STRING_PREFER_HEAP_LIMIT_IN_BYTES       128
#define _DWFCORE_STRING_PREFER_HEAP_LIMIT_IN_WCHARS     (DWFCORE_STRING_PREFER_HEAP_LIMIT_IN_BYTES>>(sizeof(wchar_t)>>1))
#endif



namespace DWFCore
{

///
///\class           DWFString   dwfcore/String.h  "dwfcore/String.h"
///\brief           A platform-independent Unicode string class.
///\since           1.0.1
///
///                 This string class is used as an unambigious boundary object type
///                 especially where both "narrow" and "wide" character strings are
///                 used.  This class always stores Unicode data in the system default
///                 \e wchar_t character type.  This class will always assume a non-Unicode
///                 character string is encoded UTF-8.
///
///\note            This class \b does \b not perform any character set transcoding
///                 except where explicitly designated and only then the conversion
///                 will be wide Unicode to or from UTF-8.
///\note            This class has platform-specific implementations.
///\ingroup         CrossPlatformImpl
///
class DWFString : public DWFCoreMemory
{

public:

    ///
    ///             Performs fast UTF-8 encoding for 16-bit UCS-2 character strings.
    ///
    ///\param       zUCS2String         The source string to encode as UTF-8.  
    ///                                 The character data type must be 16-bit UCS-2
    ///                                 (or equivalently, 16-bit UTF-16 with \b no surrogate pairs or byte-order marker.)
    ///\param       nUCS2StringBytes    The length of \a zUCS2String in bytes.
    ///\param       pUTF8Buffer         The destintation buffer into which the resultant UTF-8 encoded string will be written.
    ///                                 If this buffer is NULL, the function will return the number of bytes
    ///                                 that will be required to hold the resultant UTF-8 encoded string.
    ///\param       nUTF8BufferBytes    The length of \a pUTF8Buffer in bytes.
    ///\return      The actual length (in bytes) of the UTF-8 encoded string written into \a pUTF8Buffer.
    ///             This value does not include any null termination bytes.
    ///\throw       DWFOverflowException
    ///
    _DWFCORE_API
    static size_t EncodeUTF8( const UCS2_char_t*    zUCS2String,
                              size_t                nUCS2StringBytes,
                              ASCII_char_t*         pUTF8Buffer,
                              size_t                nUTF8BufferBytes )
        throw( DWFOverflowException );

    ///
    ///             Performs fast UTF-8 encoding for 32-bit UCS-4 character strings.
    ///
    ///\param       zUCS4String         The source string to encode as UTF-8.  
    ///                                 The character data type must be 32-bit UCS-4
    ///                                 (or equivalently, 32-bit UTF-32.)
    ///\param       nUCS4StringBytes    The length of \a zUCS4String in bytes.
    ///\param       pUTF8Buffer         The destintation buffer into which the resultant UTF-8 encoded string will be written.
    ///                                 If this buffer is NULL, the function will return the number of bytes
    ///                                 that will be required to hold the resultant UTF-8 encoded string.
    ///\param       nUTF8BufferBytes    The length of \a pUTF8Buffer in bytes.
    ///\return      The actual length (in bytes) of the UTF-8 encoded string written into \a pUTF8Buffer.
    ///             This value does not include any null termination bytes.
    ///\throw       DWFOverflowException
    ///
    _DWFCORE_API
    static size_t EncodeUTF8( const UCS4_char_t*    zUCS4String,
                              size_t                nUCS4StringBytes,
                              ASCII_char_t*         pUTF8Buffer,
                              size_t                nUTF8BufferBytes )
        throw( DWFOverflowException );


    ///
    ///             Performs fast UTF-8 decoding into 16-bit UCS-2 character strings.
    ///
    ///\param       zUTF8String         The source string encoded in UTF-8 to decode.
    ///\param       nUTF8StringBytes    The length of \a zUTF8String in bytes.
    ///\param       pUCS2Buffer         The destintation buffer into which the resultant decoded UCS-2 string will be written.
    ///                                 If this buffer is NULL, the function will return the number of bytes
    ///                                 that will be required to hold the resultant decoded UCS-2 string.
    ///\param       nUCS2BufferBytes    The length of \a pUCS2Buffer in bytes.
    ///\return      The actual length (in bytes) of the decoded UCS-2 string written into \a pUCS2Buffer.
    ///             This value does not include any null termination bytes.
    ///\throw       DWFOverflowException
    ///
    _DWFCORE_API
    static size_t DecodeUTF8( const ASCII_char_t*   zUTF8String,
                              size_t                nUTF8StringBytes,
                              UCS2_char_t*          pUCS2Buffer,
                              size_t                nUCS2BufferBytes )
        throw( DWFOverflowException );

    ///
    ///             Performs fast UTF-8 decoding into 32-bit UCS-4 character strings.
    ///
    ///\param       zUTF8String         The source string encoded in UTF-8 to decode.
    ///\param       nUTF8StringBytes    The length of \a zUTF8String in bytes.
    ///\param       pUCS4Buffer         The destintation buffer into which the resultant decoded UCS-4 string will be written.
    ///                                 If this buffer is NULL, the function will return the number of bytes
    ///                                 that will be required to hold the resultant decoded UCS-4 string.
    ///\param       nUCS4BufferBytes    The length of \a pUCS4Buffer in bytes.
    ///\return      The actual length (in bytes) of the decoded UCS-4 string written into \a pUCS4Buffer.
    ///             This value does not include any null termination bytes.
    ///\throw       DWFOverflowException
    ///
    _DWFCORE_API
    static size_t DecodeUTF8( const ASCII_char_t*   zUTF8String,
                              size_t                nUTF8StringBytes,
                              UCS4_char_t*          pUCS4Buffer,
                              size_t                nUCS4BufferBytes )
        throw( DWFOverflowException );


    ///
    ///             Performs fast XML encoding for character strings.
    ///
    ///\param       zASCIIString        The source string to encode for XML.
    ///                                 This string is processed one 8-bit character at a time.
    ///\param       nASCIIStringBytes   The length of \a zASCIIString in bytes.
    ///\param       pXMLBuffer          The destintation buffer into which the resultant XML encoded string will be written.
    ///                                 If this buffer is NULL, the function will return the number of bytes
    ///                                 that will be required to hold the resultant XML encoded string.
    ///\param       nXMLBufferBytes     The length of \a pXMLBuffer in bytes.
    ///\return      The actual length (in bytes) of the XML encoded string written into \a pXMLBuffer.
    ///             This value does not include any null termination bytes.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    static size_t EncodeXML( const ASCII_char_t*    zASCIIString,
                             size_t                 nASCIIStringBytes,
                             ASCII_char_t*          pXMLBuffer,
                             size_t                 nXMLBufferBytes )
        throw( DWFException );

    ///
    ///             Performs a restricted, fast XML encoding for wide character strings.
    ///
    ///             This is special method that only encodes explicit characters in the source string:
    ///
    ///             L'<' (less than)    -> L"&lt;"
    ///             L'>' (greater than) -> L"&gt;"
    ///             L'&' (ampersand)    -> L"&amp;"
    ///             L'"' (quote)        -> L"&quot;"
    ///             L''' (apostrophe)   -> L"&apos;"
    ///
    ///             All other source characters are left unchanged.  Care should be taken when using
    ///             this method since it is definitely a special-case algorithm made available
    ///             for those situations when a "reverse-encoding process" \e Unicode \e -> \e Unicode \e XML \e -> \e UTF-8 is required.
    ///
    ///\param       zWideString         The source string to encode for XML.
    ///                                 The character data type must be match the system \e wchar_t type definition.
    ///\param       nWideStringBytes    The length of \a zWideString in bytes.
    ///\param       pXMLBuffer          The destintation buffer into which the resultant "special" XML string will be written.
    ///                                 Note that this is a \e wchar_t character type string.
    ///                                 If this buffer is NULL, the function will return the number of bytes
    ///                                 that will be required to hold the resultant string.
    ///\param       nXMLBufferBytes     The length of \a pXMLBuffer in bytes.
    ///\return      The actual length (in bytes) of the "special" XML encoded string written into \a pXMLBuffer.
    ///             This value does not include any null termination bytes.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    static size_t EncodeXML( const wchar_t*         zWideString,
                             size_t                 nWideStringBytes,
                             wchar_t*               pXMLBuffer,
                             size_t                 nXMLBufferBytes )
        throw( DWFException );

    ///
    ///             Performs fast XML decoding for character strings.
    ///
    ///\param       zXMLString          The source string with XML character encoding.
    ///                                 This string is processed one 8-bit character at a time.
    ///\param       nXMLStringBytes     The length of \a zXMLString in bytes.
    ///\param       pASCIIBuffer        The destintation buffer into which the resultant decoded string will be written.
    ///                                 If this buffer is NULL, the function will return the number of bytes
    ///                                 that will be required to hold the resultant decoded string.
    ///\param       nASCIIBufferBytes   The length of \a pASCIIBuffer in bytes.
    ///\return      The actual length (in bytes) of the decoded string written into \a pASCIIBuffer.
    ///             This value does not include any null termination bytes.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    static size_t DecodeXML( const ASCII_char_t*   zXMLString,
                             size_t                nXMLStringBytes,
                             ASCII_char_t*         pASCIIBuffer,
                             size_t                nASCIIBufferBytes )
        throw( DWFException );

    ///
    ///             Performs fast Base64 encoding.
    ///
    ///             This method produces two flavors of encoded output.
    ///             The first, as would be expected, complies with RFC 1521 (Section 5.2)
    ///             and will always bad leftover bytes with '='.
    ///             The second is a modification made for DWF where no byte padding
    ///             is used and the 64th character '/' has been replaced by '_'.
    ///             This was done to allow encoded [UUID - see DWFCore::DWFUUID::uuid(bSquash = true)] strings
    ///             to be used in DWF section path naming.
    ///
    ///\param       pRawBuffer          The source byte buffer to encode.
    ///                                 This string is processed one 8-bit character at a time.
    ///\param       nRawBufferBytes     The length of \a pRawBuffer in bytes.
    ///\param       pOutBuffer          The destintation buffer into which the resultant Base64 encoded string will be written.
    ///                                 If this buffer is NULL, the function will return the number of bytes
    ///                                 that will be required to hold the resultant Base64 encoded string.
    ///\param       nOutBufferBytes     The length of \a pOutBuffer in bytes.
    ///\param       bRFC1521Compliant   If \e true, this method will produce a Base64 Content-Transfer-Encoding compliant with the specification
    ///                                 in which it has been detailed, otherwise the encoding will be optimized for DWF usage.
    ///\return      The actual length (in bytes) of the XML encoded string written into \a pOutBuffer.
    ///             This value does not include any null termination bytes.
    ///\throw       DWFOverflowException
    ///
    _DWFCORE_API
    static size_t EncodeBase64( const void*         pRawBuffer,
                                size_t              nRawBufferBytes,
                                void*               pOutBuffer,
                                size_t              nOutBufferBytes,
                                bool                bRFC1521Compliant = true )
        throw( DWFOverflowException );

    ///
    ///             Perform locale-independent conversion from a string representation to double primitive type.
    ///
    ///             This algorithm will convert a string representation of double precision
    ///             floating-point number into a double precision native type regardless of
    ///             the character used (in the string) to represent the decimal separator.
    ///
    ///\param       zString     The string representation of the number.
    ///\return      The number.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    static double StringToDouble( const char* zString )
        throw( DWFException );

    ///
    ///             Perform locale-independent conversion from a wide string representation to double primitive type.
    ///
    ///             This algorithm will convert a wide string representation of double precision
    ///             floating-point number into a double precision native type regardless of
    ///             the character used (in the string) to represent the decimal separator.
    ///
    ///\param       zString     The wide string representation of the number.
    ///\return      The number.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    static double StringToDouble( const wchar_t* zString )
        throw( DWFException );

    ///
    ///             This function will convert any non-whitespace, non-numeric character
    ///             that occurs between two numeric characters into a decimal point (period) '.'
    ///
    ///\warning     This function will modify the original string.
    ///\note        This function expects \a zString to represent one number.
    ///
    ///\param       zString     The wide string representation of the number.
    ///\return      The repaired string (this will match \a zString.)
    ///\throw       DWFException
    ///
    _DWFCORE_API
    static wchar_t* RepairDecimalSeparators( wchar_t* zString )
        throw( DWFException );

    ///
    ///             Searches for a character in a string from a given position in the string.
    ///
    ///\param       zString     The string to search.
    ///\param       cFind       The character to search for.
    ///\param       iOffset     The index (inclusive) from which to start the search.
    ///                         For reverse finds this is relative to the end of \a zString.
    ///                         This value must always be greater than or equal to zero.
    ///\param       bReverse    If \e true, the search will start from the end of \a zString.
    ///\return      The index of the first occurence of \a cFind in \a zString after \a iOffset
    ///             or \b -1 if not found.
    ///\throw       DWFInvalidArgumentException
    ///
    _DWFCORE_API
    static off_t Find( const wchar_t*   zString,
                       const wchar_t    cFind,
                       off_t            iOffset = 0,
                       bool             bReverse = false )
        throw( DWFInvalidArgumentException );

    ///
    ///             Searches for a substring in a string from a given position in the string.
    ///
    ///\param       zString     The string to search.
    ///\param       zFind       The substring to search for.
    ///\param       iOffset     The index (inclusive) from which to start the search.
    ///                         For reverse finds this is relative to the end of \a zString.
    ///                         This value must always be greater than or equal to zero.
    ///\param       bReverse    If \e true, the search will start from the end of \a zString.
    ///\return      The index of the first occurence of \a zFind in \a zString after \a iOffset
    ///             or \b -1 if not found.
    ///\throw       DWFInvalidArgumentException
    ///
    _DWFCORE_API
    static off_t Find( const wchar_t*   zString,
                       const wchar_t*   zFind,
                       off_t            iOffset = 0,
                       bool             bReverse = false )
        throw( DWFInvalidArgumentException );

    ///
    ///             Searches for the first occurence of a character in a string.
    ///
    ///\param       zString     The string to search.
    ///\param       cFind       The character to search for.
    ///\return      The index of the first occurence of \a cFind in \a zString or \b -1 if not found.
    ///\throw       None
    ///
    _DWFCORE_API
    static off_t FindFirst( const wchar_t*  zString,
                            const wchar_t   cFind )
        throw();

    ///
    ///             Searches for the last occurence of a character in a string.
    ///
    ///\param       zString     The string to search.
    ///\param       cFind       The character to search for.
    ///\return      The index of the last occurence of \a cFind in \a zString or \b -1 if not found.
    ///\throw       None
    ///
    _DWFCORE_API
    static off_t FindLast( const wchar_t*   zString,
                           const wchar_t    cFind )
        throw();

public:

    ///
    ///             Destructor
    ///
    ///\throw       None
    ///
    _DWFCORE_API 
    ~DWFString()
        throw();

    ///
    ///             Constructor
    ///
    ///\throw       None
    ///
    _DWFCORE_API 
    DWFString()
        throw();

    ///
    ///             Constructor
    ///
    ///             Build content from an ASCII character buffer.
    ///             This buffer is assumed to be a valid UTF-8 encoded string
    ///             and will be decoded as such and stored internally as Unicode.
    ///
    ///\param       pBuffer         The buffer from which to build.
    ///\param       nBufferBytes    The number of bytes in \a pBuffer.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    DWFString( const char*      pBuffer,
               size_t           nBufferBytes )
        throw( DWFException );

    ///
    ///             Constructor
    ///
    ///             Build content from an ASCII character string.
    ///             This buffer is assumed to be a valid UTF-8 encoded string
    ///             and will be decoded as such and stored internally as Unicode.
    ///
    ///\param       zASCIIString    The null-terminated string from which to build.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    DWFString( const char*      zASCIIString )
        throw( DWFException );

    ///
    ///             Constructor
    ///
    ///             Build content from a wide character buffer.
    ///
    ///\param       pBuffer         The buffer from which to build.
    ///\param       nBufferBytes    The number of bytes in \a pBuffer.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    DWFString( const wchar_t*   pBuffer,
               size_t           nBufferBytes )
        throw( DWFException );

    ///
    ///             Constructor
    ///
    ///             Build content from a wide character string.
    ///
    ///\param       zWideString    The null-terminated string from which to build.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    DWFString( const wchar_t*   zWideString )
        throw( DWFException );

    ///
    ///             Copy Constructor
    ///
    ///\param       rDWFString      The string from which to build.
    ///\throw       DWFException
    ///
    _DWFCORE_API 
    DWFString( const DWFString& rDWFString )
        throw( DWFException );

    ///
    ///             Assignment Operator
    ///
    ///\param       rDWFString      The string from which to build.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    DWFString& operator=( const DWFString& rDWFString )
        throw( DWFException );

    ///
    ///             Set the string content from another DWFString.
    ///
    ///\param       rDWFString      The string from which to copy content.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    void assign( const DWFString& rDWFString )
        throw( DWFException );

    ///
    ///             Set the string content from an ASCII character buffer.
    ///             This buffer is assumed to be a valid UTF-8 encoded string
    ///             and will be decoded as such and stored internally as Unicode.
    ///
    ///\param       pBuffer         The buffer from which to build.
    ///\param       nBufferBytes    The number of bytes in \a pBuffer.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    void assign( const char*    pBuffer,
                 size_t         nBufferBytes )
        throw( DWFException );

    ///
    ///             Set the string content from an ASCII character string.
    ///             This buffer is assumed to be a valid UTF-8 encoded string
    ///             and will be decoded as such and stored internally as Unicode.
    ///
    ///\param       zASCIIString    The null-terminated string from which to build.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    void assign( const char*    zASCIIString )
        throw( DWFException );

    ///
    ///             Build content from a wide character buffer.
    ///
    ///\param       pBuffer         The buffer from which to build.
    ///\param       nBufferBytes    The number of bytes in \a pBuffer.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    void assign( const wchar_t* pBuffer,
                 size_t         nBufferBytes )
        throw( DWFException );

    ///
    ///             Build content from a wide character string.
    ///
    ///\param       zWideString    The null-terminated string from which to build.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    void assign( const wchar_t* zWideString )
        throw( DWFException );

    ///
    ///             Equality Operator
    ///
    ///\param       rDWFString      The string to compare.
    ///\return      \e true if the content of the two strings are equal, \e false otherwise.
    ///\throw       None
    ///
    _DWFCORE_API
    bool operator==( const DWFString& rDWFString ) const
        throw();

    ///
    ///             Equality Operator
    ///
    ///\param       zWideString     A null-terminated wide character string to compare.
    ///\return      \e true if the content of the two strings are equal, \e false otherwise.
    ///\throw       None
    ///
    _DWFCORE_API
    bool operator==( const wchar_t* zWideString ) const
        throw();

    ///
    ///             Inequality Operator
    ///
    ///\param       rDWFString      The string to compare.
    ///\return      \e true if the content of the two strings differs, \e false otherwise.
    ///\throw       None
    ///
    _DWFCORE_API
    bool operator!=( const DWFString& rDWFString ) const
        throw();

    ///
    ///             Inequality Operator
    ///
    ///\param       zWideString     A null-terminated wide character string to compare.
    ///\return      \e true if the content of the two strings differs, \e false otherwise.
    ///\throw       None
    ///
    _DWFCORE_API
    bool operator!=( const wchar_t* zWideString ) const
        throw();

    ///
    ///             Less Than Operator
    ///
    ///\param       rDWFString      The string to compare.
    ///\return      \e true if the content this object is ordered 'less than' the content of \a rDWFString, \e false otherwise.
    ///\throw       None
    ///
    _DWFCORE_API
    bool operator<( const DWFString& rDWFString ) const
        throw();

    ///
    ///             Less Than Operator
    ///
    ///\param       zWideString     A null-terminated wide character string to compare.
    ///\return      \e true if the content this object is ordered 'less than' \a zWideString, \e false otherwise.
    ///\throw       None
    ///
    _DWFCORE_API
    bool operator<( const wchar_t* zWideString ) const
        throw();

    ///
    ///             Greater Than Operator
    ///
    ///\param       rDWFString      The string to compare.
    ///\return      \e true if the content this object is ordered 'greater than' the content of \a rDWFString, \e false otherwise.
    ///\throw       None
    ///
    _DWFCORE_API
    bool operator>( const DWFString& rDWFString ) const
        throw();

    ///
    ///             Greater Than Operator
    ///
    ///\param       zWideString     A null-terminated wide character string to compare.
    ///\return      \e true if the content this object is ordered 'greater than' \a zWideString, \e false otherwise.
    ///\throw       None
    ///
    _DWFCORE_API
    bool operator>( const wchar_t* zWideString ) const
        throw();

    ///
    ///             Frees all current resources and resets the object.
    ///
    ///\throw       None
    ///
    _DWFCORE_API
    void destroy()
        throw( DWFException );

    ///
    ///             Add content from another DWFString.
    ///
    ///\param       rDWFString      The string from which to copy content.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    void append( const DWFString& rDWFString )
        throw( DWFException );

    ///
    ///             Add content from an ASCII character buffer.
    ///             This buffer is assumed to be a valid UTF-8 encoded string
    ///             and will be decoded as such and stored internally as Unicode.
    ///
    ///\param       pBuffer         The buffer from which to build.
    ///\param       nBufferBytes    The number of bytes in \a pBuffer.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    void append( const char*    pBuffer,
                 size_t         nBufferBytes )
        throw( DWFException );

    ///
    ///             Add content from an ASCII character string.
    ///             This buffer is assumed to be a valid UTF-8 encoded string
    ///             and will be decoded as such and stored internally as Unicode.
    ///
    ///\param       zASCIIString    The null-terminated string from which to build.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    void append( const char*    zASCIIString )
        throw( DWFException );

    ///
    ///             Add content from a wide character buffer.
    ///
    ///\param       pBuffer         The buffer from which to build.
    ///\param       nBufferBytes    The number of bytes in \a pBuffer.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    void append( const wchar_t* pBuffer,
                 size_t         nBufferBytes )
        throw( DWFException );

    ///
    ///             Add content from a wide character string.
    ///
    ///\param       zWideString    The null-terminated string from which to build.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    void append( const wchar_t* zWideString )
        throw( DWFException );

    ///
    ///             Returns the content length as byte count.
    ///
    ///\return      The number of bytes in the string.
    ///
    _DWFCORE_API
    size_t bytes() const
        throw();

    ///
    ///             Returns the content length as wide character count.
    ///
    ///\return      The number of wide characters in the string.
    ///
    _DWFCORE_API
    size_t chars() const
        throw();

    ///
    ///             Returns a UTF-8 encoded representation of the content into the buffer provided.
    ///
    ///\param       pBuffer         The buffer to hold the UTF-8 encoded string.  
    ///                             If NULL, the method will return the number of bytes required
    ///                             to hold the encoded string.
    ///\param       nBufferBytes    The number of bytes in \a pBuffer.
    ///\return      The number of bytes used in \a pBuffer by the UTF-8 encoded string.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    size_t getUTF8( char*   pBuffer,
                    size_t  nBufferBytes ) const
        throw( DWFException );

    ///
    ///             Returns a UTF-8 encoded representation of the content into
    ///             a newly allocated memory buffer.  The caller must free this
    ///             buffer with the \b DWFCORE_FREE_MEMORY macro.
    ///
    ///\param       ppBuffer        A pointer to the newly allocated buffer containing
    ///                             the UTF-8 encoded string representation.
    ///\return      The number of bytes used in \a ppBuffer by the UTF-8 encoded string.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    size_t getUTF8( char** ppBuffer ) const
        throw( DWFException );

    ///
    ///             Cast Operator
    ///
    ///\return      A pointer to the internal content.  This pointer will be NULL if the string
    ///             has no content.
    ///\throw       None
    ///
    _DWFCORE_API
    operator const wchar_t*() const
        throw();

    ///
    ///             Searches for a character in a string from a given position in the string.
    ///
    ///\param       cFind       The character to search for.
    ///\param       iOffset     The index (inclusive) from which to start the search.
    ///                         For reverse finds this is relative to the end of \a zString.
    ///                         This value must always be greater than or equal to zero.
    ///\param       bReverse    If \e true, the search will start from the end of the content.
    ///\return      The index of the first occurence of \a cFind after \a iOffset
    ///             or \b -1 if not found.
    ///\throw       DWFInvalidArgumentException
    ///
    _DWFCORE_API
    off_t find( const wchar_t cFind,
                off_t         iOffset = 0,
                bool          bReverse = false ) const
        throw( DWFInvalidArgumentException );

    ///
    ///             Searches for a substring in a string from a given position in the string.
    ///
    ///\param       zFind       The substring to search for.
    ///\param       iOffset     The index (inclusive) from which to start the search.
    ///                         For reverse finds this is relative to the end of \a zString.
    ///                         This value must always be greater than or equal to zero.
    ///\param       bReverse    If \e true, the search will start from the end of the content.
    ///\return      The index of the first occurence of \a zFind after \a iOffset
    ///             or \b -1 if not found.
    ///\throw       DWFInvalidArgumentException
    ///
    _DWFCORE_API
    off_t find( const wchar_t* zFind,
                off_t          iOffset = 0,
                bool           bReverse = false ) const
        throw( DWFInvalidArgumentException );

    ///
    ///             Searches for the first occurence of a character in this string.
    ///
    ///\param       cFind       The character to search for.
    ///\return      The index of the first occurence of \a cFind in or \b -1 if not found.
    ///\throw       None
    ///
    _DWFCORE_API
    off_t findFirst( const wchar_t cFind ) const
        throw();

    ///
    ///             Searches for the last occurence of a character in this string.
    ///
    ///\param       cFind       The character to search for.
    ///\return      The index of the last occurence of \a cFind in or \b -1 if not found.
    ///\throw       None
    ///
    _DWFCORE_API
    off_t findLast( const wchar_t cFind ) const
        throw();

    ///
    ///             Creates a substring.
    ///
    ///\param       iPos        The index (inclusive) of first character in the new string.
    ///\param       iLen        The number of characters to copy into the new string, or \b -1
    ///                         to copy to end of string.
    ///\return      The new substring.
    ///\throw       DWFException
    ///
    _DWFCORE_API
    DWFString substring( size_t iPos,
                         size_t iLen = -1 )
         throw( DWFException );

private:

    void _store( const void*    pBuffer,
                 size_t         nBufferBytes,
                 bool           bDecodeUTF8 = false )
        throw( DWFException );

    void _append( const void*   pBuffer,
                  size_t        nBufferBytes,
                  bool          bDecodeUTF8 = false )
        throw( DWFException );

private:

#ifndef DWFCORE_STRING_DISABLE_STACK_BUFFER

    wchar_t     _zStackBuffer[_DWFCORE_STRING_STACK_BUFFER_SIZE_IN_WCHARS];

#endif

    wchar_t*    _pHeapBuffer;
    size_t      _nBufferChars;
    size_t      _nDataChars;
    bool        _bDataInHeap;
};

//
//
//

_DWFCORE_API
inline
size_t
DWFString::bytes()
const
throw()
{
    return (_nDataChars * sizeof(wchar_t));
}

_DWFCORE_API
inline
size_t
DWFString::chars()
const
throw()
{
    return _nDataChars;
}

_DWFCORE_API
inline
DWFString::operator const wchar_t*()
const
throw()
{
#ifdef  DWFCORE_STRING_DISABLE_STACK_BUFFER

    return _pHeapBuffer;

#endif

        //
        // return heap if it exists
        //
    if (_bDataInHeap)
    {
        return _pHeapBuffer;
    }

        //
        // with empty heap, return stack if any data exists
        //
    if (_nDataChars > 0)
    {
        return _zStackBuffer;
    }

        //
        // no data
        //
    return NULL;
}


}

#endif
