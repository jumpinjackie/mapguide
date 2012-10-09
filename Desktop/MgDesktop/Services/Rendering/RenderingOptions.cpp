#include "RenderingOptions.h"

MG_IMPL_DYNCREATE(MgdRenderingOptions)


//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgdRenderingOptions object
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
MgdRenderingOptions::MgdRenderingOptions(CREFSTRING format, INT32 behavior, MgColor* selectionColor)
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
STRING MgdRenderingOptions::GetImageFormat()
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
INT32 MgdRenderingOptions::GetBehavior()
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
MgColor* MgdRenderingOptions::GetSelectionColor()
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
void MgdRenderingOptions::Serialize(MgStream* stream)
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
void MgdRenderingOptions::Deserialize(MgStream* stream)
{
    stream->GetString(m_format);
    stream->GetInt32(m_behavior);
    m_selectionColor = (MgColor*)stream->GetObject();
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Default constructor
/// </summary>
MgdRenderingOptions::MgdRenderingOptions()
: m_behavior(0),
  m_selectionColor(NULL)
{
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Virtual destructor
/// </summary>
MgdRenderingOptions::~MgdRenderingOptions()
{
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Get the class Id
/// </summary>
/// <returns>
/// The integer value
/// </returns>
INT32 MgdRenderingOptions::GetClassId()
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
void MgdRenderingOptions::Dispose()
{
    delete this;
}