//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: AddContentCommand.cpp,v 1.3 2005/04/05 16:44:26 bostic Exp $
//

#include "AddContentCommand.hpp"

using namespace DbXml;
using namespace std;

XmlModify::XmlObject AddContentCommand::getObject(string object)
{
  if(object == "element") {
    return XmlModify::Element;
  }
  else if(object == "attribute") {
    return XmlModify::Attribute;
  }
  else if(object == "text") {
    return XmlModify::Text;
  }
  else if(object == "processinginstruction") {
    return XmlModify::ProcessingInstruction;
  }
  else if(object == "comment") {
    return XmlModify::Comment;
  }
  return (XmlModify::XmlObject)-1;
}
