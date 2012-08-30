#include "ResourceDefs.h"
#include "OperationParameter.h"

// Resource Service operation parameters
// TODO: Share the following strings with the Web tier.

const STRING MgdOperationParameter::Type=L"TYPE";
const STRING MgdOperationParameter::Depth=L"DEPTH";
const STRING MgdOperationParameter::ResourceId=L"RESOURCEID";
const STRING MgdOperationParameter::ResourceContent=L"CONTENT";
const STRING MgdOperationParameter::ResourceHeader=L"HEADER";
const STRING MgdOperationParameter::SourceResourceId=L"SOURCE";
const STRING MgdOperationParameter::DestinationResourceId=L"DESTINATION";
const STRING MgdOperationParameter::DataName=L"DATANAME";
const STRING MgdOperationParameter::DataType=L"DATATYPE";
const STRING MgdOperationParameter::DataLength=L"DATALENGTH";
const STRING MgdOperationParameter::Data=L"DATA";
const STRING MgdOperationParameter::OldDataName=L"OLDDATANAME";
const STRING MgdOperationParameter::NewDataName=L"NEWDATANAME";
const STRING MgdOperationParameter::Overwrite=L"OVERWRITE";
const STRING MgdOperationParameter::Package=L"PACKAGE";
const STRING MgdOperationParameter::Owner=L"OWNER";
const STRING MgdOperationParameter::IncludeDescendants=L"INCLUDEDESCENDANTS";


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgdOperationParameter::MgdOperationParameter()
{
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Copy constructor.
///
MgdOperationParameter::MgdOperationParameter(const MgdOperationParameter& opParam)
{
    *this = opParam;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgdOperationParameter::~MgdOperationParameter()
{
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Assignment operator.
///
MgdOperationParameter& MgdOperationParameter::operator=(
    const MgdOperationParameter& opParam)
{
    if (&opParam != this)
    {
        m_value = opParam.m_value;
        m_contentType = opParam.m_contentType;
    }

    return *this;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the value of the parameter.
///
void MgdOperationParameter::SetValue(CREFSTRING value)
{
    m_value = value;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the content type of the parameter.
///
void MgdOperationParameter::SetContentType(CREFSTRING contentType)
{
    m_contentType = contentType;
}
