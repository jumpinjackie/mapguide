//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#include "Watermark.h"
#include "MdfModel.h"
#include "SAX2Parser.h"

using namespace std;

//static method to create the watermark definition
MdfModel::WatermarkDefinition* MgWatermark::GetWatermarkDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId)
{
    // get and parse the watermark definition
    Ptr<MgByteReader> reader = svcResource->GetResourceContent(resId, L"");
    Ptr<MgByteSink> sink = new MgByteSink(reader);
    Ptr<MgByte> bytes = sink->ToBuffer();

    assert(bytes->GetLength() > 0);

    MdfParser::SAX2Parser parser;
    parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());

    if (!parser.GetSucceeded())
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);
        throw new MgInvalidWatermarkDefinitionException(L"MgWatermark::GetWatermarkDefinition", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // detach the watermark definition from the parser - it's
    // now the caller's responsibility to delete it
    MdfModel::WatermarkDefinition* wdef = parser.DetachWatermarkDefinition();
    assert(wdef != NULL);

    return wdef;
}

//static method to create the watermark definition
MdfModel::WatermarkDefinition* MgWatermark::GetWatermarkDefinition(CREFSTRING resourceContent)
{
    // get and parse the watermark definition
    MdfParser::SAX2Parser parser;
    parser.ParseString(resourceContent.c_str(), resourceContent.length());

    if (!parser.GetSucceeded())
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);
        throw new MgInvalidWatermarkDefinitionException(L"MgWatermark::GetWatermarkDefinition", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // detach the watermark definition from the parser - it's
    // now the caller's responsibility to delete it
    MdfModel::WatermarkDefinition* wdef = parser.DetachWatermarkDefinition();
    assert(wdef != NULL);

    return wdef;
}
