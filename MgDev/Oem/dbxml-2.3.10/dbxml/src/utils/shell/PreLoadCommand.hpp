//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: PreLoadCommand.hpp,v 1.7 2006/10/30 17:46:08 bostic Exp $
//

#ifndef __PRELOADCOMMAND_HPP
#define	__PRELOADCOMMAND_HPP

#include "dbxml/DbXml.hpp"
#include <vector>
#include <string>

#include "ShellCommand.hpp"

class PreLoadCommand : public ShellCommand
{
public:
	PreLoadCommand() {};
	virtual ~PreLoadCommand() {};
	
	virtual std::string getCommandName() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);

	void info();
private:
	std::vector<DbXml::XmlContainer> containers_;
};

#endif
