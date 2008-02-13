//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "System/ByteSourceSocketStreamImpl.h"
#include "Command.h"


//////////////////////////////////////////////////////////////////
/// <summary>
/// Execute a command by serializing it to the server
/// </summary>
void MgCommand::ExecuteCommand(MgConnectionProperties* connProp, DataTypes retType, int cmdCode, int numArguments, int serviceId, int operationVer, ...)
{
    // Obtain connection for command
    ServerConnectionType sct = sctRemoteServerToServer;
    if (!connProp->GetUrl().empty())
    {
        sct = sctServerToHttp;
        //TODO:  Parse url and pull machine name into target
    }

    Ptr<MgUserInformation> userInfo = connProp->GetUserInfo();
    Ptr<MgServerConnection> serviceConn = MgServerConnection::Acquire(userInfo, connProp);
    Ptr<MgStream> stream = serviceConn->GetStream();

    MgOperationPacket mop;
    mop.m_PacketHeader = MgPacketParser::mphOperation;
    mop.m_PacketVersion = 1; // TODO: Add the correct version

    mop.m_OperationVersion = operationVer;
    mop.m_OperationID = cmdCode;
    mop.m_ServiceID = serviceId;
    mop.m_NumArguments = numArguments;
    mop.m_UserInfo = userInfo;

    BeginOperation(stream, mop);

    //send each argument
    va_list args;
    va_start(args, operationVer);

    int paramType;
    while(knNone != (paramType = va_arg(args, int)))
    {
        switch(paramType)
        {
            case knInt8:
            {
                int v = va_arg(args, int);
                stream->WriteByte((INT8)v);
                break;
            }
            case knInt16:
            {
                int v = va_arg(args, int);
                stream->WriteInt16((INT16)v);
                break;
            }
            case knInt32:
            {
                INT32 v = va_arg(args, INT32);
                stream->WriteInt32(v);
                break;
            }
            case knInt64:
            {
                INT64 v = va_arg(args, INT64);
                stream->WriteInt64(v);
                break;
            }
            case knSingle:
            {
                double v = va_arg(args, double);
                stream->WriteSingle((float)v);
                break;
            }
            case knDouble:
            {
                double v = va_arg(args, double);
                stream->WriteDouble(v);
                break;
            }
            case knString:
            {
                STRING* v = va_arg(args, STRING*);
                stream->WriteString(*v);
                break;
            }
            case knObject:
            {
                MgObject* obj = va_arg(args, MgObject*);
                stream->WriteObject(obj);
                break;
            }
        }
    }
    va_end(args);

    stream->WriteStreamEnd();

    GetResponse(serviceConn, retType);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get a reference to the return value
/// </summary>
///<returns>Reference to a ReturnValue struct containing the return value</returns>
MgCommand::ReturnValue& MgCommand::GetReturnValue()
{
    return m_retVal;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the response from the server for execute command
/// </summary>
void MgCommand::GetResponse(MgServerConnection* connection, DataTypes retType)
{
    assert(NULL != connection);

    MG_TRY()

    // Get the stream from the connection
    Ptr<MgStream> ptrStream = connection->GetStream();

    MgStreamHeader msh;
    MgOperationResponsePacket morp;

    // Get the stream header i.e. stream_start, stream_data etc.
    ptrStream->GetStreamHeader(msh);

    if (MgStreamParser::mshStreamStart != msh.m_streamStart)
    {
        throw new MgInvalidStreamHeaderException(L"MgCommand.GetResponse",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else if (MgStreamParser::StreamVersion != msh.m_streamVersion)
    {
        throw new MgStreamIoException(L"MgCommand.GetResponse",
            __LINE__, __WFILE__, NULL, L"MgInvalidTCPProtocol", NULL);
    }

    // Get the operation response header
    ptrStream->GetOperationResponseHeader(morp);
    // Process the result based on eCode. In case of exception, it would throw the exception
    ProcessResult((MgPacketParser::MgECode)morp.m_ECode, retType, connection);

    MG_CATCH(L"MgCommand.GetResponse")

    if (NULL != mgException)
    {
        // Close the connection if the stream is corrupted or of newer version.
        if (mgException->IsOfClass(Foundation_Exception_MgInvalidStreamHeaderException)
         || mgException->IsOfClass(Foundation_Exception_MgStreamIoException))
        {
            connection->Close();
        }

        MG_THROW();
    }
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Process the response based on ECode - Status Code
/// </summary>
void MgCommand::ProcessResult(MgPacketParser::MgECode eCode, DataTypes retType, MgServerConnection* serverConn)
{

    Ptr<MgStream> ptrStream = serverConn->GetStream();

    // Decide what to do based on eCode
    switch (eCode)
    {
        // Operation was successful
        case MgPacketParser::mecSuccess:
        {
            // Get the response for the specified return type
            // TODO: It assumes only one return type for now
            GetResponseResult(retType, serverConn);
            break;
        }
        case MgPacketParser::mecSuccessWithWarning:
        {
            // Get the warning
            GetWarning(ptrStream);
            // Get the response for the specified return type
            // TODO: It assumes only one return type for now
            GetResponseResult(retType, serverConn);
            break;
        }
        case MgPacketParser::mecFailure:
        {
            GetResponseResult(MgCommand::knObject, serverConn);
            MgException* mgException = dynamic_cast<MgException*>(m_retVal.val.m_obj);
            if (mgException != NULL)
            {
                // Throw this pointer
                mgException->Raise();
            }
            break;
        }
        default:
            break;
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the response result
/// </summary>
void MgCommand::GetResponseResult(DataTypes retType, MgServerConnection* serverConn)
{
    Ptr<MgStream> ptrStream = serverConn->GetStream();

    bool bShouldEnd = true;

    switch(retType)
    {
        case knInt8:
        {
            //get an integer 8 from the stream
            //m_retVal.val.m_i8 = ???;
            BYTE b;
            ptrStream->GetByte(b);
            m_retVal.val.m_i8 = (INT8)b;
            break;
        }
        case knInt16:
        {
            //get an integer 16 from the stream
            //m_retVal.val.m_i16 = ???;
            ptrStream->GetInt16(m_retVal.val.m_i16);
            break;
        }
        case knInt32:
        {
            //get an integer 32 from the stream
            //m_retVal.val.m_i16 = ???;
            ptrStream->GetInt32(m_retVal.val.m_i32);
            break;
        }
        case knInt64:
        {
            //get an integer 64 from the stream
            //m_retVal.val.m_i64 = ???;
            ptrStream->GetInt64(m_retVal.val.m_i64);
            break;
        }
        case knSingle:
        {
            //get an single floating point from the stream
            //m_retVal.val.m_f = ???;
            ptrStream->GetSingle(m_retVal.val.m_f);
            break;
        }
        case knDouble:
        {
            //get a double floating point from the stream
            //m_retVal.val.m_d = ???;
            ptrStream->GetDouble(m_retVal.val.m_d);
            break;
        }
        case knString:
        {
            //get a string from the stream
            //m_retVal.val.m_str = ???;
            STRING str;
            ptrStream->GetString(str);
            m_retVal.val.m_str = new STRING;
            *(m_retVal.val.m_str) = str;
            break;
        }
        case knObject:
        {
            m_retVal.val.m_obj = ptrStream->GetObject();

            // If we're not returning a socket stream based reader, then no need
            // to read/write end of stream.
            MgByteReader* reader = dynamic_cast<MgByteReader*>(m_retVal.val.m_obj);
            if (reader != NULL && dynamic_cast<ByteSourceSocketStreamImpl*>(reader->GetByteSource()->GetSourceImpl()) != NULL)
            {
                bShouldEnd = false;
            }

            break;
        }
        case knVoid:
        {
            // In case of void no processing needed.
            break;
        }
        default:
        {
            STRING buffer;
            MgUtil::Int32ToString(retType, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgCommand.GetResponseResult",
                __LINE__, __WFILE__, &arguments, L"MgInvalidDataType", NULL);
        }
    }

    if (bShouldEnd)
    {
        EndOperation(ptrStream);
    }
}


void MgCommand::GetWarning(MgStream* stream)
{
    m_warning = (MgWarnings*)stream->GetObject();
}


MgException* MgCommand::GetException(MgStream* stream)
{
    return (MgException*)stream->GetObject();
}


void MgCommand::BeginOperation(MgStream* stream, MgOperationPacket& mop)
{
    MgStreamHeader msh;

    msh.m_streamStart = MgStreamParser::mshStreamStart;
    msh.m_streamVersion = MgStreamParser::StreamVersion;
    msh.m_streamDataHdr = MgStreamParser::mshStreamData;

    stream->WriteStreamHeader(msh);

    //send the operation packet
    stream->WriteOperationHeader(mop);
}

void MgCommand::EndOperation(MgStream* stream)
{
    // End of stream
    stream->GetStreamEnd();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgCommand::~MgCommand()
{
    if (m_warning)
        SAFE_RELEASE(m_warning);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgCommand::MgCommand()
{
    m_retVal.val.m_obj = 0;
    m_warning = NULL;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get a pointer to warning object
/// </summary>
///<returns>Get a pointer to warning object</returns>
MgWarnings* MgCommand::GetWarningObject()
{
    return SAFE_ADDREF(m_warning);
}
