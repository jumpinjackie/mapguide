//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: AddContentCommand.hpp,v 1.5 2006/10/30 17:46:07 bostic Exp $
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
