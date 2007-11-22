//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: InputParser.hpp,v 1.7 2006/10/30 17:46:08 bostic Exp $
//

#ifndef __INPUTPARSER_HPP
#define	__INPUTPARSER_HPP

#include "dbxml/XmlPortability.hpp"
#include <vector>
#include <string>
#include <istream>

class InputParser
{
public:
  typedef std::vector<std::string> Args;

  int parse(std::istream &input, Args &args);

private:
  void reset();
  void whitespace(Args &args);
  void character();
  void escapedCharacter();
  bool inQuotes();
  void quote(Args &args, char c);
  void openCurly();
  void closeCurly();
  void endOfArg(Args &args);
  void addArgument(Args &args, std::string::size_type start, std::string::size_type end);
  void nextLine(std::istream &input, Args &args);

  std::string line_;
  std::string::size_type start_, pos_;
  char quoteChar_;
  int curlyBraceDepth_;
  bool seenNonWS_;
  bool stripQuotes_;
};

#endif
