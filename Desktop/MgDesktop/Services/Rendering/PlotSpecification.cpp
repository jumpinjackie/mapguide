#include "PlotSpecification.h"

MG_IMPL_DYNCREATE(MgPlotSpecification)

// Construct a MgPlotSpecification object
//
MgPlotSpecification::MgPlotSpecification(float paperWidth, float paperHeight, CREFSTRING pageUnits,
                                         float left, float top, float right, float bottom) :
    m_marginLeft(left), m_marginTop(top), m_marginRight(right), m_marginBottom(bottom)
{
    m_paperWidth = paperWidth;
    m_paperHeight = paperHeight;
    m_pageUnits = pageUnits;
}

MgPlotSpecification::MgPlotSpecification() : m_marginLeft(0), m_marginTop(0), m_marginRight(0), m_marginBottom(0)
{
    m_paperWidth = 0.;
    m_paperHeight = 0.;
}

// Returns the paper width in page units
//
float MgPlotSpecification::GetPaperWidth()
{
    return m_paperWidth;
}

// Sets the paper width in page units
//
void MgPlotSpecification::SetPaperWidth(float width)
{
    m_paperWidth = width;
}

// Returns the paper height in page units
//
float MgPlotSpecification::GetPaperHeight()
{
    return m_paperHeight;
}

// Sets the paper height in page units
//
void MgPlotSpecification::SetPaperHeight(float height)
{
    m_paperHeight = height;
}

// Returns the units of measure for the page.  See MgPageUnitsType for valid units.
//
STRING MgPlotSpecification::GetPageSizeUnits()
{
    return m_pageUnits;
}

// Set the units of measure for the page.  See MgPageUnitsType for valid units.
//
void MgPlotSpecification::SetPageSizeUnits(CREFSTRING pageUnits)
{
    m_pageUnits = pageUnits;
}

float MgPlotSpecification::GetMarginLeft()
{
    return m_marginLeft;
}

float MgPlotSpecification::GetMarginTop()
{
    return m_marginTop;
}

float MgPlotSpecification::GetMarginRight()
{
    return m_marginRight;
}

float MgPlotSpecification::GetMarginBottom()
{
    return m_marginBottom;
}

void MgPlotSpecification::SetMargins(float left, float top, float right, float bottom)
{
    m_marginLeft   = left;
    m_marginTop    = top;
    m_marginRight  = right;
    m_marginBottom = bottom;
}

// Serialize data to a stream
//
void MgPlotSpecification::Serialize(MgStream* stream)
{
    stream->WriteSingle(m_paperWidth);
    stream->WriteSingle(m_paperHeight);
    stream->WriteString(m_pageUnits);

    stream->WriteSingle(m_marginLeft);
    stream->WriteSingle(m_marginTop);
    stream->WriteSingle(m_marginRight);
    stream->WriteSingle(m_marginBottom);
}

// Deserialize data from a stream
//
void MgPlotSpecification::Deserialize(MgStream* stream)
{
    MgStreamReader* streamReader = (MgStreamReader*)stream;

    streamReader->GetSingle(m_paperWidth);
    streamReader->GetSingle(m_paperHeight);
    streamReader->GetString(m_pageUnits);

    streamReader->GetSingle(m_marginLeft);
    streamReader->GetSingle(m_marginTop);
    streamReader->GetSingle(m_marginRight);
    streamReader->GetSingle(m_marginBottom);
}
