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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef _MG_SERVER_SQL_QUERY_RESULT_H_
#define _MG_SERVER_SQL_QUERY_RESULT_H_

class MgStream;
class MgByteReader;

/////////////////////////////////////////////////////////////////
/// <summary>
/// The SqlReader interface provides a forward-only, read-only
/// iterator for reading relational table data.
/// The initial position of the SqlReader interface is prior to the first item.
/// Thus, you must call ReadNext to begin accessing any data..
/// </summary>

class MG_MAPGUIDE_API  MgSqlResult : public MgSerializable
{
MG_DECL_DYNCREATE();
DECLARE_CLASSNAME(MgSqlResult)

public:
    MgSqlResult();
    MgSqlResult(int rowAffected, MgParameterCollection * parameters, MgSqlDataReader * reader);
    ~MgSqlResult();

    virtual int GetRowAffected();

    virtual MgParameterCollection * GetParameters();

    virtual MgSqlDataReader * GetReader();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Serializes all features into an XML.
    /// XML is serialized from the current position of feature reader in the order
    /// data are retrieved.
    /// <returns>MgByteReader holding XML.</returns>
    MgByteReader* ToXml();

    virtual INT32 GetClassId();

protected:

    virtual void Dispose()
    {
        delete this;
    }

    
    void Deserialize(MgStream* stream);
    void Serialize(MgStream* stream);

private:

    int                         m_rowAffected;
    Ptr<MgParameterCollection>  m_parameters;
    Ptr<MgSqlDataReader>        m_reader;

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Service_SqlResult;

};

#endif
