//
//  Copyright (C) 2003-2008 by Autodesk, Inc.
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

#include "./EMapInterface.h"
#include "./EMapConstants.h"
#include "./EMapUtility.h"
using namespace DWFToolkit::DWFEMapExtension;


const wchar_t* const DWFEMapInterface::kzEMap_Name            = L"eMap";
const wchar_t* const DWFEMapInterface::kzEMap_HRef            = L"http://www.autodesk.com/viewers";
const wchar_t* const DWFEMapInterface::kzEMap_ID              = L"69BD6A6D-F788-4A34-BAE9-76AB5F69EF9D";

const wchar_t* const DWFEMapInterface::kzEMapTransaction_Name = L"eMapTransaction";
const wchar_t* const DWFEMapInterface::kzEMapTransaction_HRef = L"http://www.autodesk.com/viewers";
const wchar_t* const DWFEMapInterface::kzEMapTransaction_ID   = L"5FBAADCE-E1D7-4E8E-B463-FBEA7F0CB2E7";


DWFEMapInterface::DWFEMapInterface(  )
throw() : DWFInterface( )
{
}

DWFEMapInterface::DWFEMapInterface( const DWFString& zName,
                                    const DWFString& zHRef,
                                    const DWFString& zObjectID )
                                    throw() : DWFInterface (
                                                    zName,
                                                    zHRef,
                                                    zObjectID )
{
}

DWFEMapInterface::~DWFEMapInterface() throw()
{
}

// End of file.
