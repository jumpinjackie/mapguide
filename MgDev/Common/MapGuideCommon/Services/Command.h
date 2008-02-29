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

#ifndef MG_COMMAND_H
#define MG_COMMAND_H

/// \cond INTERNAL
//////////////////////////////////////////////////////////////////
/// \brief
/// The class Command provides a generic mechanism to send commands
/// to the server.
///
class MG_MAPGUIDE_API MgCommand
{
    DECLARE_CLASSNAME(MgCommand)

public:
    //parameter and return types
    typedef enum DataTypes
    {
        knNone = 0x1111F601,
        knVoid = 0x1111F602,
        knInt8 = 0x1111F603,
        knInt16 = 0x1111F604,
        knInt32 = 0x1111F605,
        knInt64 = 0x1111F606,
        knSingle = 0x1111F607,
        knDouble = 0x1111F608,
        knString = 0x1111F609,
        knObject = 0x1111F60A
    } DataTypes;

    //utility class to hold the return value
    class ReturnValue
    {
    public:

        ReturnValue()
        {
            ::memset(&val, 0, sizeof(val));
        }

        union
        {
            INT8        m_i8;
            INT16       m_i16;
            INT32       m_i32;
            INT64       m_i64;
            float       m_f;
            double      m_d;
            STRING*     m_str;
            MgObject* m_obj;
        } val;
    };

    MgWarnings* m_warning;

public:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructor
    ///
    MgCommand();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Execute a command by serializing it to the server
    ///
    /// \param connProp
    /// Valid connection to the site server.
    /// \param retType
    /// Return type.
    /// \param cmdCode
    /// Command identifier.
    ///
    /// \note
    ///  Arguments of the command are passed to this function as pair:
    ///     \li arg        (DataTypes)
    ///     \li arg value  (basic value or pointer to an object)
    ///
    void ExecuteCommand(MgConnectionProperties* connProp, DataTypes retType, int cmdCode, int numArguments, int serviceId, int operationVer, ...);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get a reference to the return value
    ///
    /// \return
    /// Reference to a ReturnValue struct containing the return value
    ///
    ReturnValue&    GetReturnValue();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get a pointer to warning object
    ///
    /// \return
    /// Get a pointer to warning object
    ///
    MgWarnings* GetWarningObject();

EXTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor
    ///
    virtual ~MgCommand();

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the response from the server for execute command
    ///
    void GetResponse(MgServerConnection* connection, DataTypes retType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Process the response based on ECode - Status Code
    ///
    void ProcessResult(MgPacketParser::MgECode eCode, DataTypes retType, MgServerConnection* ptrStream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the response result
    ///
    void GetResponseResult(DataTypes retType, MgServerConnection* ptrStream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the warning object
    ///
    void GetWarning(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the exception object
    ///
    MgException* GetException(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Begins the operation by sending packets
    ///
    void BeginOperation(MgStream* stream, MgOperationPacket& mop);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Ends the operation by sending end of stream marker
    ///
    void EndOperation(MgStream* stream);

    ReturnValue     m_retVal;
};
/// \endcond

#endif
