//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef OGCFRAMEWORK_H_
#define OGCFRAMEWORK_H_

#include "stdlib.h"
#include "assert.h"

#ifndef ASSERT
# ifdef _DEBUG
#  define ASSERT(x)  if(!(x)) {assert(#x);}
# else
#  define ASSERT(x)  // compiled out.
# endif
#endif


// String handling abstractions
#define CHARTYPE       wchar_t  // our character type
#define SZBUF          CHARTYPE // buffer/array: string of NUL- (zero-) terminated characters
#define VPSZ           CHARTYPE*// (variable/non-const) pointer to string of NUL- (zero-) terminated characters
#define CPSZ     const CHARTYPE*// constant pointer to string

#define _(x)     L ## x         // Define the appropriate string literal type

#define SZ_EQ(pszLeft,pszRight)         (szcmp(pszLeft,pszRight) == 0)
#define SZ_EQN(pszLeft,pszRight,iCount) (szcmpn(pszLeft,pszRight,iCount) == 0)
#define SZ_NE(pszLeft,pszRight)         (szcmp(pszLeft,pszRight) != 0)
#define SZ_EQI(pszLeft,pszRight)        (szicmp(pszLeft,pszRight) == 0)
#define SZ_NEI(pszLeft,pszRight)        (szicmp(pszLeft,pszRight) != 0)

#define byte_sizeof(x) (sizeof(x))
#define char_sizeof(x) (sizeof(x) / sizeof(CHARTYPE))

// Our own abstractions of strlen, et al, for heightened local control
#ifdef _WIN32

# include <wchar.h>
# define szlen(s)      wcslen(s)
# define szcmp(l,r)    wcscmp(l,r)
# define szcmpn(l,r,c) wcsncmp(l,r,c)
# define szicmp(l,r)   wcsicmp(l,r)
# define szdup(s)      wcsdup(s)
# define sztoi(s)      widetoint(s)
// CHARTYPE-appropriate abstractions for file IO
# define szopen(fn,ac) _wfopen(fn,ac)
# define szgets(buf,ich,h) fgetws(buf,ich,h)
// CHARTYPE-appropriate varargs functions; only function names overridden
# define szprintf  wprintf  // redirect the sprintf to the wide variety
# define szsprintf swprintf // redirect the sprintf to the wide variety

#else// if linux?

# include <wchar.h>
# define szlen(s)      wcslen(s)
# define szcmp(l,r)    wcscmp(l,r)
# define szcmpn(l,r,c) wcsncmp(l,r,c)
# define szicmp(l,r)   wcscasecmp(l,r)
# define szdup(s)      wcsdup(s)
# define sztoi(s)      widetoint(s)
// CHARTYPE-appropriate abstractions for file IO
# define szopen(fn,ac) xwfopen(fn,ac)
# define szgets(buf,ich,h) fgetws(buf,ich,h)
// CHARTYPE-appropriate varargs functions; only function names overridden
# define szprintf  wprintf  // redirect the sprintf to the wide variety
# define szsprintf swprintf // redirect the sprintf to the wide variety

#endif

// Just provided for "sz" consistency.
#define szclose(h)    fclose(h)
#define szeof(h)      feof(h)
#define szseek(h,i,e)     fseek(h,i,e)
#define sztell(h)         ftell(h)
#define szread(buf,c,s,h) fread(buf,c,s,h)

// Further abstraction of other types
// Here is where we suddenly have to drink the koolaid.
// "Resistance Is Futile -- You Will Be Assimilated!"
#include "MapGuideCommon.h"

typedef unsigned long LONGBITS;

/*
TODO: MIGRATE REFERENCES TO MapGuide
*/
///

// WMS spec says these are case-insensitive.  (Research says lowercase is more readable.)
extern CPSZ kpszQueryStringService         ;// = _("service");
extern CPSZ kpszQueryStringRequest         ;// = _("request");
extern CPSZ kpszQueryStringVersion         ;// = _("version");
extern CPSZ kpszQueryStringWmtVersion      ;// = _("wmtver");
extern CPSZ kpszQueryStringFormat          ;// = _("format");
extern CPSZ kpszQueryStringInfoFormat      ;// = _("info_format");
extern CPSZ kpszQueryStringLayers          ;// = _("layers");
extern CPSZ kpszQueryStringQueryLayers     ;// = _("query_layers");
extern CPSZ kpszQueryStringICoord          ;// = _("i");
extern CPSZ kpszQueryStringJCoord          ;// = _("j");
extern CPSZ kpszQueryStringWidth           ;// = _("width");
extern CPSZ kpszQueryStringHeight          ;// = _("height");
extern CPSZ kpszQueryStringCrs             ;// = _("crs");
extern CPSZ kpszQueryStringSrs             ;// = _("srs");
extern CPSZ kpszQueryStringBBox            ;// = _("bbox");

// WMS spec says these values are case-specific.
extern CPSZ kpszQueryValueWms              ;// = _("WMS");
extern CPSZ kpszQueryValueGetCapabilities  ;// = _("GetCapabilities");
extern CPSZ kpszQueryValueCapabilities     ;// = _("Capabilities");
extern CPSZ kpszQueryValueGetMap           ;// = _("GetMap");
extern CPSZ kpszQueryValueMap              ;// = _("Map"); // for Legacy processing
extern CPSZ kpszQueryValueGetFeatureInfo   ;// = _("GetFeatureInfo");
extern CPSZ kpszQueryValueFeatureInfo      ;// = _("FeatureInfo");

// Mime types.  Case Specific
extern CPSZ kpszMimeTypeXml                ;// = _("text/xml");
extern CPSZ kpszMimeTypePlain              ;// = _("text/plain");

// Xml element names.  Case Specific
extern CPSZ kpszElementConfiguration       ;// = _("Configuration");

extern CPSZ kpszElementNameTemplate        ;// = _("Template");
extern CPSZ kpszAttributeService           ;// = _("service");
extern CPSZ kpszAttributeRequest           ;// = _("request");
extern CPSZ kpszAttributeVersion           ;// = _("version");

//extern CPSZ kpszAttrValueWms               ;// = _("WMS");

extern CPSZ kpszElementDefinitions         ;// = _("Definitions");
//extern CPSZ kpszAttributeFor               ;// = _("for");
extern CPSZ kpszElementDefine              ;// = _("Define");
extern CPSZ kpszAttributeItem              ;// = _("item");

extern CPSZ kpszElementResponse            ;// = _("Response");
extern CPSZ kpszAttributeContentType       ;// = _("content-type");

extern CPSZ kpszException                  ;// = _("Exception");

// Xml predefined entities.  Case specific.
extern CPSZ kpszEntityLessThan             ;// = _("lt");
extern CPSZ kpszEntityGreaterThan          ;// = _("gt");
extern CPSZ kpszEntityAmpersand            ;// = _("amp");
extern CPSZ kpszEntityQuote                ;// = _("quot");
extern CPSZ kpszEntityApostrophe           ;// = _("apos");

extern CPSZ kpszDictionaryEnumDictName     ;// = _("EnumDictionary.Name");
extern CPSZ kpszDictionaryEnumDictValue    ;// = _("EnumDictionary.Value");


extern CPSZ kpszEmpty                      ;// = _("");
extern CPSZ kpszDefaultExceptionTemplate   ;// = _("Server Exception:\nType: &Exception.Type;\nMessage: &Exception.Message;\n");
//
//extern CPSZ kpszFilenameGlobalConfig       ;// = _("OgcWmsService.config");

#endif//_OgcFramework_h
