//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#include "stdafx.h"
#include "SE_SymbolManager.h"
#include "Base64.h"

// Called when an inlined base64-encoded image is encountered in the symbol
// definition.  The purpose of this method is to convert the base64 data to
// a specific image format.  Currently only inlined PNG images are supported,
// and the default implementation of this method converts the base64 data to
// PNG format.  Override this method if you want to convert the base64 data
// to a different image format.
//
// The converted image data is stored in the supplied ImageData parameter,
// and should not be cached by the symbol manager.
bool SE_SymbolManager::GetImageData(const wchar_t* base64Data, const int size, ImageData& imageData)
{
    // strip out any non-base64 characters - you'll get these if the image
    // data is inlined in a symbol definition and there are line breaks
    char* src_ascii = new char[size];

    char* ptr = src_ascii;
    for (int i=0; i<size; ++i)
    {
        char c = (char)base64Data[i];
        if (Base64::IsBase64(c))
            *ptr++ = c;
    }

    size_t actlen = ptr - src_ascii;

    unsigned long dstlen = Base64::GetDecodedLength((unsigned long)actlen);
    imageData.data = new unsigned char[dstlen];

    // the actual decoded size may be slightly smaller than the estimate
    imageData.size = Base64::Decode(imageData.data, src_ascii, (unsigned long)actlen);

    // for now we only support inlined PNG images
    imageData.format = RS_ImageFormat_PNG;
    imageData.width = -1;
    imageData.height = -1;

    delete [] src_ascii;

    return true;
}
