//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: QuitCommand.cpp,v 1.6 2006/10/30 17:46:08 bostic Exp $
//

#include "QuitCommand.hpp"

using namespace DbXml;
using namespace std;

string QuitCommand::getCommandName() const
{
  return "quit";
}

// allow exit to work -- not really compatibility, just helpful
string QuitCommand::getCommandNameCompat() const
{
  return "exit";
}

string QuitCommand::getBriefHelp() const
{
  return "Exit the program";
}

string QuitCommand::getMoreHelp() const
{
  return "Usage: quit";
}

void QuitCommand::execute(Args &args, Environment &env)
{
  if(args.size() != 1) {
    throw CommandException("Wrong number of arguments");
  }

  env.quit() = true;
}
