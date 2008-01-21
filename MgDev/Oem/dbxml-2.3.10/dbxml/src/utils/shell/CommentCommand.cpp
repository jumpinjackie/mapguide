//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: CommentCommand.cpp,v 1.5 2006/10/30 17:46:07 bostic Exp $
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
