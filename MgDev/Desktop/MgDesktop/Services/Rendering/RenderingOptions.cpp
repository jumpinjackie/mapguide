#include "RenderingOptions.h"

MG_IMPL_DYNCREATE(MgRenderingOptions)


//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgRenderingOptions object
/// </summary>
/// <param name="format">Input
/// Image format. Defines the format of the resulting image
/// </param>
/// <param name="behavior">Input
/// A bitmask with the rendering behavior values
/// </param>
/// <param name="selectionColor">Input
/// Selection color, including filling color and boundary color
/// </param>
MgRenderingOptions::MgRenderingOptions(CREFSTRING format, INT32 behavior, MgColor* selectionColor)
: m_format(MgUtil::ToUpper(format)),
  m_behavior(behavior)
{
    m_selectionColor = SAFE_ADDREF(selectionColor);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the image format
/// </summary>
/// <returns>
/// The string value (is in upper case from CTOR)
/// </returns>
STRING MgRenderingOptions::GetImageFormat()
{
    return m_format;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the rendering behavior
/// </summary>
/// <returns>
/// The integer value
/// </returns>
INT32 MgRenderingOptions::GetBehavior()
{
    return m_behavior;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the selection color
/// </summary>
/// <returns>
/// The MgColor object
/// </returns>
MgColor* MgRenderingOptions::GetSelectionColor()
{
    return SAFE_ADDREF((MgColor*)m_selectionColor);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serialize data to TCP/IP stream
/// </summary>
/// <param name="stream">Input
/// Stream
/// </param>
void MgRenderingOptions::Serialize(MgStream* stream)
{
    stream->WriteString(m_format);
    stream->WriteInt32(m_behavior);
    stream->WriteObject(m_selectionColor);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Deserialize data from TCP/IP stream
/// </summary>
/// <param name="stream">Input
/// Stream
/// </param>
void MgRenderingOptions::Deserialize(MgStream* stream)
{
    stream->GetString(m_format);
    stream->GetInt32(m_behavior);
    m_selectionColor = (MgColor*)stream->GetObject();
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Default constructor
/// </summary>
MgRenderingOptions::MgRenderingOptions()
: m_behavior(0),
  m_selectionColor(NULL)
{
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Virtual destructor
/// </summary>
MgRenderingOptions::~MgRenderingOptions()
{
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Get the class Id
/// </summary>
/// <returns>
/// The integer value
/// </returns>
INT32 MgRenderingOptions::GetClassId()
{
    return m_cls_id;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Dispose this object
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgRenderingOptions::Dispose()
{
    delete this;
}