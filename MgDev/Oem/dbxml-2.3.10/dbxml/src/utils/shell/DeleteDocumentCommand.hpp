//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: DeleteDocumentCommand.hpp,v 1.6 2006/10/30 17:46:07 bostic Exp $
//

#ifndef __DELETEDOCUMENTCOMMAND_HPP
#define	__DELETEDOCUMENTCOMMAND_HPP

#include "ShellCommand.hpp"

class DeleteDocumentCommand : public ShellCommand
{
public:
	DeleteDocumentCommand() {};
	virtual ~DeleteDocumentCommand() {};

	virtual std::string getCommandName() const;
	virtual std::string getCommandNameCompat() const;
	virtual std::string getBriefHelp() const;
	virtual std::string getMoreHelp() const;
	
	virtual void execute(Args &args, Environment &env);
};

#endif
