//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: AddContentCommand.hpp,v 1.3 2005/04/05 16:44:26 bostic Exp $
//

#ifndef __ADDCONTENTCOMMAND_HPP
#define	__ADDCONTENTCOMMAND_HPP

#include "ShellCommand.hpp"

class AddContentCommand : public ShellCommand
{
public:
	virtual ~AddContentCommand() {};

protected:
  AddContentCommand() {};
  DbXml::XmlModify::XmlObject getObject(std::string object);
};

#endif
