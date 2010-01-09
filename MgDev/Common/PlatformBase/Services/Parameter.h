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

/// \defgroup MgParameter MgParameter
/// \ingroup Feature_Service_classes
/// \{

#ifndef _MG_PARAMETER_H
#define _MG_PARAMETER_H

/////////////////////////////////////////////////////////////////////
/// \brief
/// The MgParameter class specifies a value for a particular parameter.
/// Instances of this class are used to specify a value to bind to a
/// parameter when a command is executed. Typically, it is used to bind
/// parameters to the SQL command.
///
class MG_PLATFORMBASE_API MgParameter : public MgNamedSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgParameter)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgParameter object. The default parameter direction
    /// is input.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgParameter(MgNullableProperty prop);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgParameter(MgNullableProperty prop);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgParameter(MgNullableProperty prop);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (MgNullableProperty)
    /// An MgNullableProperty instance which contains 
    /// the name and value of the parameter.
    ///
    MgParameter(MgNullableProperty* prop);

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgParameter object with the specified parameter
    /// name, value, and direction.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgParameter(MgNullableProperty prop, int direction);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgParameter(MgNullableProperty prop, int direction);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgParameter(MgNullableProperty prop, int direction);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (MgNullableProperty)
    /// An MgNullableProperty instance which contains 
    /// the name and value of the parameter.
    /// \param value (int)
    /// The function direction of the parameter value.
    ///
    MgParameter(MgNullableProperty* prop, INT32 direction);

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name and value of the parameter.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetProperty(MgNullableProperty prop);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetProperty(MgNullableProperty prop);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetProperty(MgNullableProperty prop);
    /// \htmlinclude SyntaxBottom.html
    /// 
    /// \param value 
    /// Input an MgNullableProperty instance which contains 
    /// the name and value of the parameter.
    /// 
    /// \return
    /// Returns nothing
    /// 
    void SetProperty(MgNullableProperty* prop);

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name and value of the parameter.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgNullableProperty GetProperty();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgNullableProperty GetProperty();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgNullableProperty GetProperty();
    /// \htmlinclude SyntaxBottom.html
    /// 
    /// \return
    /// Returns an MgNullableProperty instance which contains 
    /// the name and value of the parameter.
    /// 
    MgNullableProperty* GetProperty();

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the function direction of the parameter value, which
    /// is defined in MgParameterDirection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetDirection(int value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetDirection(int value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetDirection(int value);
    /// \htmlinclude SyntaxBottom.html
    /// 
    /// \param value 
    /// Input the direction value.
    /// 
    /// \return
    /// Returns nothing.
    /// 
    void SetDirection(INT32 value);

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the function direction value of the the command parameter, which
    /// is defined in MgParameterDirection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetDirection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetDirection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetDirection();
    /// \htmlinclude SyntaxBottom.html
    /// 
    /// \return
    /// Returns the direction value defined in MgParameterDirection.
    /// 
    INT32 GetDirection();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the parameter name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the parameter name.
    ///
    STRING GetName();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the parameter name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetName(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetName(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetName(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The name of parameter.
    ///
    void SetName(CREFSTRING name);

EXTERNAL_API:
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor. Deletes a property and any information contained
    /// in it.
    ///
    virtual ~MgParameter();

INTERNAL_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructor.
    ///
    MgParameter();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Tests whether it allow to set the name.
    ///
    /// \return
    /// true - allows to set the name.
    /// false - can not set the name.
    ///
    virtual bool CanSetName();

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the unique identifier for the class.
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Disposes this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_Parameter;

private:
    MgNullableProperty*         m_parameter;
    INT32                       m_parameterDirection;
};
/// \}

#endif
