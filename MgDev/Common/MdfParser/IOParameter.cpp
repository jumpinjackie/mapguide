//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#include "stdafx.h"
#include "IOParameter.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOParameter::IOParameter(ParameterCollection* parameterCollection, Version& version) : SAX2ElementHandler(version)
{
    this->m_parameterCollection = parameterCollection;
    this->m_parameter = NULL;
}


IOParameter::~IOParameter()
{
}


void IOParameter::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"Parameter") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_parameter = new Parameter();
    }
    else if (this->m_currElemName == L"DataType") // NOXLATE
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
    {
        this->m_procExtData = true;
    }
    else
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOParameter::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_parameter, Identifier, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_parameter, DefaultValue, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_parameter, DisplayName, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_parameter, Description, ch)
    else if (this->m_currElemName == L"DataType")   // NOXLATE
    {
             if (::wcscmp(ch, L"String") == 0)              this->m_parameter->SetDataType(Parameter::String);              // NOXLATE
        else if (::wcscmp(ch, L"Boolean") == 0)             this->m_parameter->SetDataType(Parameter::Boolean);             // NOXLATE
        else if (::wcscmp(ch, L"Integer") == 0)             this->m_parameter->SetDataType(Parameter::Integer);             // NOXLATE
        else if (::wcscmp(ch, L"Real") == 0)                this->m_parameter->SetDataType(Parameter::Real);                // NOXLATE
        else if (::wcscmp(ch, L"Color") == 0)               this->m_parameter->SetDataType(Parameter::Color);               // NOXLATE
        else if (::wcscmp(ch, L"Angle") == 0)               this->m_parameter->SetDataType(Parameter::Angle);               // NOXLATE
        else if (::wcscmp(ch, L"FillColor") == 0)           this->m_parameter->SetDataType(Parameter::FillColor);           // NOXLATE
        else if (::wcscmp(ch, L"LineColor") == 0)           this->m_parameter->SetDataType(Parameter::LineColor);           // NOXLATE
        else if (::wcscmp(ch, L"LineWeight") == 0)          this->m_parameter->SetDataType(Parameter::LineWeight);          // NOXLATE
        else if (::wcscmp(ch, L"Content") == 0)             this->m_parameter->SetDataType(Parameter::Content);             // NOXLATE
        else if (::wcscmp(ch, L"Markup") == 0)              this->m_parameter->SetDataType(Parameter::Markup);              // NOXLATE
        else if (::wcscmp(ch, L"FontName") == 0)            this->m_parameter->SetDataType(Parameter::FontName);            // NOXLATE
        else if (::wcscmp(ch, L"Bold") == 0)                this->m_parameter->SetDataType(Parameter::Bold);                // NOXLATE
        else if (::wcscmp(ch, L"Italic") == 0)              this->m_parameter->SetDataType(Parameter::Italic);              // NOXLATE
        else if (::wcscmp(ch, L"Underlined") == 0)          this->m_parameter->SetDataType(Parameter::Underlined);          // NOXLATE
        else if (::wcscmp(ch, L"Overlined") == 0)           this->m_parameter->SetDataType(Parameter::Overlined);           // NOXLATE
        else if (::wcscmp(ch, L"ObliqueAngle") == 0)        this->m_parameter->SetDataType(Parameter::ObliqueAngle);        // NOXLATE
        else if (::wcscmp(ch, L"TrackSpacing") == 0)        this->m_parameter->SetDataType(Parameter::TrackSpacing);        // NOXLATE
        else if (::wcscmp(ch, L"FontHeight") == 0)          this->m_parameter->SetDataType(Parameter::FontHeight);          // NOXLATE
        else if (::wcscmp(ch, L"HorizontalAlignment") == 0) this->m_parameter->SetDataType(Parameter::HorizontalAlignment); // NOXLATE
        else if (::wcscmp(ch, L"VerticalAlignment") == 0)   this->m_parameter->SetDataType(Parameter::VerticalAlignment);   // NOXLATE
        else if (::wcscmp(ch, L"Justification") == 0)       this->m_parameter->SetDataType(Parameter::Justification);       // NOXLATE
        else if (::wcscmp(ch, L"LineSpacing") == 0)         this->m_parameter->SetDataType(Parameter::LineSpacing);         // NOXLATE
        else if (::wcscmp(ch, L"TextColor") == 0)           this->m_parameter->SetDataType(Parameter::TextColor);           // NOXLATE
        else if (::wcscmp(ch, L"GhostColor") == 0)          this->m_parameter->SetDataType(Parameter::GhostColor);          // NOXLATE
        else if (::wcscmp(ch, L"FrameLineColor") == 0)      this->m_parameter->SetDataType(Parameter::FrameLineColor);      // NOXLATE
        else if (::wcscmp(ch, L"FrameFillColor") == 0)      this->m_parameter->SetDataType(Parameter::FrameFillColor);      // NOXLATE
        else if (::wcscmp(ch, L"StartOffset") == 0)         this->m_parameter->SetDataType(Parameter::StartOffset);         // NOXLATE
        else if (::wcscmp(ch, L"EndOffset") == 0)           this->m_parameter->SetDataType(Parameter::EndOffset);           // NOXLATE
        else if (::wcscmp(ch, L"RepeatX") == 0)             this->m_parameter->SetDataType(Parameter::RepeatX);             // NOXLATE
        else if (::wcscmp(ch, L"RepeatY") == 0)             this->m_parameter->SetDataType(Parameter::RepeatY);             // NOXLATE
    }
}


void IOParameter::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_parameter->SetUnknownXml(this->m_unknownXml);

        this->m_parameterCollection->Adopt(this->m_parameter);
        this->m_parameterCollection = NULL;
        this->m_parameter = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOParameter::Write(MdfStream& fd, Parameter* parameter, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<Parameter>" << std::endl; // NOXLATE
    tab.inctab();

    MdfStringStream fdExtData;

    EMIT_STRING_PROPERTY(fd, parameter, Identifier, false, NULL, tab)
    EMIT_STRING_PROPERTY(fd, parameter, DefaultValue, false, NULL, tab)
    EMIT_STRING_PROPERTY(fd, parameter, DisplayName, true, L"", tab)         // default is empty string
    EMIT_STRING_PROPERTY(fd, parameter, Description, true, L"", tab)         // default is empty string

    Parameter::DataType dataType = parameter->GetDataType();
    if (dataType != Parameter::String)
    {
        if (!version || (*version >= Version(1, 1, 0)))
        {
            // with symbol definition version 1.1.0 and higher we can directly
            // save the new data type enumerations

            fd << tab.tab() << "<DataType>";                                                      // NOXLATE

                 if (dataType == Parameter::Boolean)             fd << "Boolean";             // NOXLATE
            else if (dataType == Parameter::Integer)             fd << "Integer";             // NOXLATE
            else if (dataType == Parameter::Real)                fd << "Real";                // NOXLATE
            else if (dataType == Parameter::Color)               fd << "Color";               // NOXLATE
            else if (dataType == Parameter::Angle)               fd << "Angle";               // NOXLATE
            else if (dataType == Parameter::FillColor)           fd << "FillColor";           // NOXLATE
            else if (dataType == Parameter::LineColor)           fd << "LineColor";           // NOXLATE
            else if (dataType == Parameter::LineWeight)          fd << "LineWeight";          // NOXLATE
            else if (dataType == Parameter::Content)             fd << "Content";             // NOXLATE
            else if (dataType == Parameter::Markup)              fd << "Markup";              // NOXLATE
            else if (dataType == Parameter::FontName)            fd << "FontName";            // NOXLATE
            else if (dataType == Parameter::Bold)                fd << "Bold";                // NOXLATE
            else if (dataType == Parameter::Italic)              fd << "Italic";              // NOXLATE
            else if (dataType == Parameter::Underlined)          fd << "Underlined";          // NOXLATE
            else if (dataType == Parameter::Overlined)           fd << "Overlined";           // NOXLATE
            else if (dataType == Parameter::ObliqueAngle)        fd << "ObliqueAngle";        // NOXLATE
            else if (dataType == Parameter::TrackSpacing)        fd << "TrackSpacing";        // NOXLATE
            else if (dataType == Parameter::FontHeight)          fd << "FontHeight";          // NOXLATE
            else if (dataType == Parameter::HorizontalAlignment) fd << "HorizontalAlignment"; // NOXLATE
            else if (dataType == Parameter::VerticalAlignment)   fd << "VerticalAlignment";   // NOXLATE
            else if (dataType == Parameter::Justification)       fd << "Justification";       // NOXLATE
            else if (dataType == Parameter::LineSpacing)         fd << "LineSpacing";         // NOXLATE
            else if (dataType == Parameter::TextColor)           fd << "TextColor";           // NOXLATE
            else if (dataType == Parameter::GhostColor)          fd << "GhostColor";          // NOXLATE
            else if (dataType == Parameter::FrameLineColor)      fd << "FrameLineColor";      // NOXLATE
            else if (dataType == Parameter::FrameFillColor)      fd << "FrameFillColor";      // NOXLATE
            else if (dataType == Parameter::StartOffset)         fd << "StartOffset";         // NOXLATE
            else if (dataType == Parameter::EndOffset)           fd << "EndOffset";           // NOXLATE
            else if (dataType == Parameter::RepeatX)             fd << "RepeatX";             // NOXLATE
            else if (dataType == Parameter::RepeatY)             fd << "RepeatY";             // NOXLATE

            fd << "</DataType>" << std::endl;                                                 // NOXLATE
        }
        else if (*version == Version(1, 0, 0))
        {
            // with symbol definition version 1.0.0 we can only directly
            // save the original data type enumerations - the newer ones
            // need to be stored in extended data
            if (dataType == Parameter::Boolean ||
                dataType == Parameter::Integer ||
                dataType == Parameter::Real    ||
                dataType == Parameter::Color)
            {
                // older enumerated value - save directly
                fd << tab.tab() << "<DataType>";                                                      // NOXLATE

                     if (dataType == Parameter::Boolean)             fd << "Boolean";             // NOXLATE
                else if (dataType == Parameter::Integer)             fd << "Integer";             // NOXLATE
                else if (dataType == Parameter::Real)                fd << "Real";                // NOXLATE
                else if (dataType == Parameter::Color)               fd << "Color";               // NOXLATE

                fd << "</DataType>" << std::endl;                                                 // NOXLATE
            }
            else
            {
                // newer enumerated value - save with extended data
                tab.inctab();
                fdExtData << tab.tab() << "<DataType>";                                                      // NOXLATE

                     if (dataType == Parameter::Angle)               fdExtData << "Angle";               // NOXLATE
                else if (dataType == Parameter::FillColor)           fdExtData << "FillColor";           // NOXLATE
                else if (dataType == Parameter::LineColor)           fdExtData << "LineColor";           // NOXLATE
                else if (dataType == Parameter::LineWeight)          fdExtData << "LineWeight";          // NOXLATE
                else if (dataType == Parameter::Content)             fdExtData << "Content";             // NOXLATE
                else if (dataType == Parameter::Markup)              fdExtData << "Markup";              // NOXLATE
                else if (dataType == Parameter::FontName)            fdExtData << "FontName";            // NOXLATE
                else if (dataType == Parameter::Bold)                fdExtData << "Bold";                // NOXLATE
                else if (dataType == Parameter::Italic)              fdExtData << "Italic";              // NOXLATE
                else if (dataType == Parameter::Underlined)          fdExtData << "Underlined";          // NOXLATE
                else if (dataType == Parameter::Overlined)           fdExtData << "Overlined";           // NOXLATE
                else if (dataType == Parameter::ObliqueAngle)        fdExtData << "ObliqueAngle";        // NOXLATE
                else if (dataType == Parameter::TrackSpacing)        fdExtData << "TrackSpacing";        // NOXLATE
                else if (dataType == Parameter::FontHeight)          fdExtData << "FontHeight";          // NOXLATE
                else if (dataType == Parameter::HorizontalAlignment) fdExtData << "HorizontalAlignment"; // NOXLATE
                else if (dataType == Parameter::VerticalAlignment)   fdExtData << "VerticalAlignment";   // NOXLATE
                else if (dataType == Parameter::Justification)       fdExtData << "Justification";       // NOXLATE
                else if (dataType == Parameter::LineSpacing)         fdExtData << "LineSpacing";         // NOXLATE
                else if (dataType == Parameter::TextColor)           fdExtData << "TextColor";           // NOXLATE
                else if (dataType == Parameter::GhostColor)          fdExtData << "GhostColor";          // NOXLATE
                else if (dataType == Parameter::FrameLineColor)      fdExtData << "FrameLineColor";      // NOXLATE
                else if (dataType == Parameter::FrameFillColor)      fdExtData << "FrameFillColor";      // NOXLATE
                else if (dataType == Parameter::StartOffset)         fdExtData << "StartOffset";         // NOXLATE
                else if (dataType == Parameter::EndOffset)           fdExtData << "EndOffset";           // NOXLATE
                else if (dataType == Parameter::RepeatX)             fdExtData << "RepeatX";             // NOXLATE
                else if (dataType == Parameter::RepeatY)             fdExtData << "RepeatY";             // NOXLATE

                fdExtData << "</DataType>" << std::endl;                                                 // NOXLATE
                tab.dectab();
            }
        }
    }

    // Write the unknown XML / extended data
    IOUnknown::Write(fd, parameter->GetUnknownXml(), fdExtData.str(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</Parameter>" << std::endl; // NOXLATE
}
