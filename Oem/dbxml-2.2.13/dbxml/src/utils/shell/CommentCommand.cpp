//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: CommentCommand.cpp,v 1.3 2005/04/05 16:44:27 bostic Exp $
//

#include "CommentCommand.hpp"

using namespace DbXml;
using namespace std;

string CommentCommand::getCommandName() const
{
  return "#";
}

string CommentCommand::getBriefHelp() const
{
  return "Comment. Does nothing";
}

string CommentCommand::getMoreHelp() const
{
  return "";
}

void CommentCommand::execute(Args &args, Environment &env)
{
  // Do nothing
}
