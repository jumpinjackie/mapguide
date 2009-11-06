//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
//  Foundation


#include "SqlResult.h"

MG_IMPL_DYNCREATE(MgSqlResult);

MgSqlResult::MgSqlResult()
{
}

MgSqlResult::MgSqlResult(int rowAffected, MgParameterCollection * parameters, MgSqlDataReader * reader)
: m_rowAffected(rowAffected), m_parameters(parameters), m_reader(reader)
{
    if(parameters)
        SAFE_ADDREF(parameters);

    if(reader)
        SAFE_ADDREF(reader);
}

MgSqlResult::~MgSqlResult()
{
}

INT32 MgSqlResult::GetRowAffected()
{
    return m_rowAffected;
}

MgParameterCollection * MgSqlResult::GetParameters()
{
    return SAFE_ADDREF((MgParameterCollection *)m_parameters);
}

MgSqlDataReader * MgSqlResult::GetReader()
{
    return SAFE_ADDREF((MgSqlDataReader *)m_reader);
}

void MgSqlResult::Serialize(MgStream *stream)
{
    stream->WriteInt32(m_rowAffected);
    stream->WriteObject(m_parameters);
    stream->WriteObject(m_reader);
}

void MgSqlResult::Deserialize(MgStream* stream)
{
    stream->GetInt32(m_rowAffected);
    m_parameters = (MgParameterCollection *)stream->GetObject();
    m_reader = (MgSqlDataReader *)stream->GetObject();
}

INT32 MgSqlResult::GetClassId()
{
    return m_cls_id;
}

