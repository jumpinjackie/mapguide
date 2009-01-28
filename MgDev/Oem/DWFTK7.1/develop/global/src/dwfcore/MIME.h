//
//  Copyright (C) 2003-2009 by Autodesk, Inc.
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

#ifndef _DWFCORE_MIME_H
#define _DWFCORE_MIME_H


///
///\file        dwfcore/MIME.h
///\brief       This header contains a MIME type string to file extension map.
///


#include "dwfcore/Core.h"
#include "dwfcore/STL.h"
#include "dwfcore/Pointer.h"
#include "dwfcore/Iterator.h"
#include "dwfcore/SkipList.h"



namespace DWFCore
{

///
///\class       DWFMIME    dwfcore/MIME.h    "dwfcore/MIME.h"
///\brief       Singleton providing a map of known MIME type strings to their file extensions.
///\since       1.0.1
///
class DWFMIME   : public DWFCoreMemory
{

public:

    ///
    ///\brief   L"model/vnd.dwf"
    ///
    static const wchar_t* const kzMIMEType_DWF1;
    ///
    ///\brief   L"application/x-dwf"
    ///
    static const wchar_t* const kzMIMEType_DWF2;
    ///
    ///\brief   L"drawing/x-dwf"
    ///
    static const wchar_t* const kzMIMEType_DWF3;
    ///
    ///\brief   L"application/x-w2d"
    ///
    static const wchar_t* const kzMIMEType_W2D;
    ///
    ///\brief   L"application/x-w3d"
    ///
    static const wchar_t* const kzMIMEType_W3D;
    ///
    ///\brief   L"application/x-dsd"
    ///
    static const wchar_t* const kzMIMEType_DSD;
    ///
    ///\brief   L"application/x-dwg-state"
    ///
    static const wchar_t* const kzMIMEType_PIA;
    ///
    ///\brief   L"image/bmp"
    ///
    static const wchar_t* const kzMIMEType_BMP;
    ///
    ///\brief   L"image/cals"
    ///
    static const wchar_t* const kzMIMEType_CALS;
    ///
    ///\brief   L"image/flic"
    ///
    static const wchar_t* const kzMIMEType_FLIC;
    ///
    ///\brief   L"image/gif"
    ///
    static const wchar_t* const kzMIMEType_GIF;
    ///
    ///\brief   L"image/ig4"
    ///
    static const wchar_t* const kzMIMEType_IG4;
    ///
    ///\brief   L"image/jpeg"
    ///
    static const wchar_t* const kzMIMEType_JPG;
    ///
    ///\brief   L"image/pcx"
    ///
    static const wchar_t* const kzMIMEType_PCX;
    ///
    ///\brief   L"image/pict"
    ///
    static const wchar_t* const kzMIMEType_PICT;
    ///
    ///\brief   L"image/png"
    ///
    static const wchar_t* const kzMIMEType_PNG;
    ///
    ///\brief   L"image/rlc"
    ///
    static const wchar_t* const kzMIMEType_RLC;
    ///
    ///\brief   L"image/targa"
    ///
    static const wchar_t* const kzMIMEType_TARGA;
    ///
    ///\brief   L"image/tiff"
    ///
    static const wchar_t* const kzMIMEType_TIFF;
    ///
    ///\brief   L"text/plain"
    ///
    static const wchar_t* const kzMIMEType_TXT;
    ///
    ///\brief   L"text/xml"
    ///
    static const wchar_t* const kzMIMEType_XML;
    ///
    ///\brief   L"application-xfont"
    ///
    static const wchar_t* const kzMIMEType_FONT;

    ///
    ///\brief   L"dwf"
    ///
    static const wchar_t* const kzExtension_DWF;
    ///
    ///\brief   L"w2d"
    ///
    static const wchar_t* const kzExtension_W2D;
    ///
    ///\brief   L"w3d"
    ///
    static const wchar_t* const kzExtension_W3D;
    ///
    ///\brief   L"dsd"
    ///
    static const wchar_t* const kzExtension_DSD;
    ///
    ///\brief   L"pia"
    ///
    static const wchar_t* const kzExtension_PIA;
    ///
    ///\brief   L"bmp"
    ///
    static const wchar_t* const kzExtension_BMP;
    ///
    ///\brief   L"cal"
    ///
    static const wchar_t* const kzExtension_CALS;
    ///
    ///\brief   L"flc"
    ///
    static const wchar_t* const kzExtension_FLIC;
    ///
    ///\brief   L"gif"
    ///
    static const wchar_t* const kzExtension_GIF;
    ///
    ///\brief   L"ig4"
    ///
    static const wchar_t* const kzExtension_IG4;
    ///
    ///\brief   L"jpg"
    ///
    static const wchar_t* const kzExtension_JPG;
    ///
    ///\brief   L"pcx"
    ///
    static const wchar_t* const kzExtension_PCX;
    ///
    ///\brief   L"pct"
    ///
    static const wchar_t* const kzExtension_PICT;
    ///
    ///\brief   L"png"
    ///
    static const wchar_t* const kzExtension_PNG;
    ///
    ///\brief   L"rlc"
    ///
    static const wchar_t* const kzExtension_RLC;
    ///
    ///\brief   L"tga"
    ///
    static const wchar_t* const kzExtension_TARGA;
    ///
    ///\brief   L"tif"
    ///
    static const wchar_t* const kzExtension_TIFF;
    ///
    ///\brief   L"txt"
    ///
    static const wchar_t* const kzExtension_TXT;
    ///
    ///\brief   L"xml"
    ///
    static const wchar_t* const kzExtension_XML;
    ///
    ///\brief   L"ef_"
    ///
    static const wchar_t* const kzExtension_FONT;

public:

    ///
    ///         Returns a list of zero or more MIME type strings
    ///         mapped to the file extension specified.
    ///
    ///\param   zExtension  The file extension (without a leading ".")
    ///\return  A pointer to an interator of MIME type string constants.
    ///         This pointer must be deleted by the caller using the \b DWFCORE_FREE_OBJECT macro.
    ///\throw   DWFException
    ///
    _DWFCORE_API 
    static DWFIterator<const wchar_t* const>* GetMIMEType( const wchar_t* const zExtension )
        throw( DWFException );

    ///
    ///         Returns the file extension string mapped to the MIME type specified.
    ///
    ///\param   zMIMEType   The MIME type string.
    ///\return  The file extension (without a leading ".") or NULL if the MIME type is unmapped.
    ///\throw   DWFException
    ///
    _DWFCORE_API
    static const wchar_t* const GetExtension( const wchar_t* const zMIMEType )
        throw( DWFException );

private:

    //
    // Constructor
    //
    DWFMIME()
        throw();

    //
    //
    //
    static void _Build()
        throw( DWFException );

private:

    typedef DWFWCharKeySkipList<const wchar_t*>                             _tList;
    typedef multimap<const wchar_t*, const wchar_t*, tDWFWCharCompareLess>  _tMultiMap;

    static DWFPointer<_tList>       _kapTypeMap;         // MIME -> extension
    static DWFPointer<_tMultiMap>   _kapExtensionMap;    // extension -> MIME

private:

    //
    //
    //
    class _Iterator : public DWFIterator<const wchar_t* const>
    {

    public:

        //
        // Constructor
        //
        _Iterator( _tMultiMap::iterator iBegin,
                   _tMultiMap::iterator iEnd )
            throw()
            : _iBegin( iBegin )
            , _iEnd( iEnd )
            , _iCurrent( iBegin )
        {;}

        //
        // Copy Constructor
        //
        _Iterator( const _Iterator& i )
            throw()
            : _iBegin( i._iBegin )
            , _iEnd( i._iEnd )
            , _iCurrent( i._iCurrent )
        {;}

        //
        // Assignment
        //
        _Iterator& operator=( const _Iterator& i )
            throw()
        {
            _iBegin = i._iBegin;
            _iEnd = i._iEnd;
            _iCurrent = i._iCurrent;

            return *this;
        }

        //
        // Destructor
        //
        virtual ~_Iterator()
            throw()
        {;}

        //
        // Reset the iterator back to its initial position
        //
        void reset()
            throw()
        {
            _iCurrent = _iBegin;
        }

        //
        // Returns true if the iterator points to a valid element
        //
        bool valid()
            throw()
        {
            return (_iCurrent != _iEnd);
        }

        //
        // Move the iterator forward one element
        // Returns true if the iterator points to a valid element
        //      or false if the end of the iterator has been reached
        //
        bool next()
            throw()
        {
            if (valid())
            {
                _iCurrent++;
                return valid();
            }
            else
            {
                return false;
            }
        }

        //
        // Returns the current element or throws an exception
        //  if the end of the iterator has been reached
        //
        const wchar_t* const& get()
            throw( DWFException )
        {
            if (valid())
            {
                return _iCurrent->second;
            }
            else
            {
                _DWFCORE_THROW( DWFDoesNotExistException, L"No more elements" );
            }
        }

    private:

        _tMultiMap::iterator _iBegin;
        _tMultiMap::iterator _iEnd;
        _tMultiMap::iterator _iCurrent;
    };

private:

    //
    // Unimplemented
    //
    DWFMIME( const DWFMIME& );
    DWFMIME& operator=( const DWFMIME& );
};


#ifndef _DWFCORE_DEFINE_FOR_DOXYGEN_ONLY

    //
    // required for win32 dll external linkage
    //
#ifdef  _DWFCORE_WIN32_SYSTEM
#ifndef DWFCORE_STATIC

_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_DWF1 =     L"model/vnd.dwf";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_DWF2 =     L"application/x-dwf";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_DWF3 =     L"drawing/x-dwf";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_W2D =      L"application/x-w2d";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_W3D =      L"application/x-w3d";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_DSD =      L"application/x-dsd";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_PIA =      L"application/x-dwg-state";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_BMP =		 L"image/bmp";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_CALS =     L"image/cals";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_FLIC =     L"image/flic";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_GIF =      L"image/gif";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_IG4 =      L"image/ig4";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_JPG =      L"image/jpeg";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_PCX =      L"image/pcx";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_PICT =     L"image/pict";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_PNG =      L"image/png";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_RLC =      L"image/rlc";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_TARGA =    L"image/targa";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_TIFF =     L"image/tiff";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_TXT =      L"text/plain";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_XML =      L"text/xml";
_declspec(selectany) const wchar_t* const DWFMIME::kzMIMEType_FONT =     L"application/x-font";

_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_DWF =     L"dwf";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_W2D =     L"w2d";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_W3D =     L"w3d";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_DSD =     L"dsd";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_PIA =     L"pia";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_BMP =	 L"bmp";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_CALS =    L"cal";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_FLIC =    L"flc";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_GIF =     L"gif";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_IG4 =     L"ig4";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_JPG =     L"jpg";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_PCX =     L"pcx";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_PICT =    L"pct";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_PNG =     L"png";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_RLC =     L"rlc";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_TARGA=    L"tga";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_TIFF =    L"tif";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_TXT =     L"txt";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_XML =     L"xml";
_declspec(selectany) const wchar_t* const DWFMIME::kzExtension_FONT =    L"ef_";

#endif
#endif
#endif


}

#endif
