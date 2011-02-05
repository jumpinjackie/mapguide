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

#include "GeometryCommon.h"
#include "AgfStream.h"

///////////////////////////////////////////////////////////////////////////
// Creates an MgAgfReaderWriter object
//
MgAgfReaderWriter::MgAgfReaderWriter()
{
}

///////////////////////////////////////////////////////////////////////////
// Reads a geometry object from a stream of bytes in AGF format.
//
MgGeometry* MgAgfReaderWriter::Read(MgByteReader* agf)
{
    CHECKARGUMENTNULL(agf, L"MgAgfReaderWriter.Read");

    //wrap a stream around the byte reader to deserialize the geometry
    Ptr<MgByteSink> bs = new MgByteSink(agf);
    Ptr<MgByte> bytes = bs->ToBuffer();

    Ptr<MgMemoryStreamHelper> streamHelper = new MgMemoryStreamHelper((INT8*)bytes->Bytes(), bytes->GetLength());
    MgAgfStream stream(streamHelper);

    //check what is the geometry type without removing advancing the stream
    UINT32 type;
    streamHelper->GetUINT32(type, true, true);

    //create the geometry and deserialize it from the memory stream
    Ptr<MgGeometry> geom = MgGeometryFactory::CreateGeometry(type);
    if (geom == NULL)
        return NULL;    //TODO: should we throw an exception instead?

    geom->Deserialize(&stream);
    return geom.Detach();
}

///////////////////////////////////////////////////////////////////////////
// Reads a geometry object from a stream of bytes in AGF format applying
// the specified transform during the read.
//
MgGeometry* MgAgfReaderWriter::Read(MgByteReader* agf, MgTransform* transform)
{
    //get the untransformed geometry
    Ptr<MgGeometry> geom = Read(agf);
    if (geom == NULL)
        return NULL;    //TODO: should we throw an exception instead?

    //apply supplied transform
    if (transform)
        geom = (MgGeometry*)geom->Transform(transform);

    return geom.Detach();
}

///////////////////////////////////////////////////////////////////////////
// Writes a geometry object to a stream of bytes in AGF format.
//
MgByteReader* MgAgfReaderWriter::Write(MgGeometry* geometry)
{
    CHECKARGUMENTNULL(geometry, L"MgAgfReaderWriter.Write");

    Ptr<MgMemoryStreamHelper> streamHelper = new MgMemoryStreamHelper();
    MgAgfStream stream(streamHelper);

    geometry->Serialize(&stream);

    Ptr<MgByteSource> bsource = new MgByteSource((BYTE_ARRAY_IN)streamHelper->GetBuffer(), streamHelper->GetLength());
    bsource->SetMimeType(MgMimeType::Agf);
    return bsource->GetReader();
}

///////////////////////////////////////////////////////////////////////////
// Writes a geometry object to a stream of bytes in AGF format applying
// the specified transform during the write.
//
MgByteReader* MgAgfReaderWriter::Write(MgGeometry* geometry, MgTransform* transform)
{
    CHECKARGUMENTNULL(geometry, L"MgAgfReaderWriter.Write");

    //apply supplied transform
    Ptr<MgGeometry> geomToWrite;
    if (transform)
        geomToWrite = (MgGeometry*)geometry->Transform(transform);
    else
        geomToWrite = SAFE_ADDREF(geometry);

    //write the transformed geometry
    return Write(geomToWrite);
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgAgfReaderWriter::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgAgfReaderWriter::GetClassId()
{
    return m_cls_id;
}
