/*
 * Copyright (c) 2001, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __LEXERDOMSTRINGSTREAM_HPP
#define __LEXERDOMSTRINGSTREAM_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/util/XercesDefs.hpp>

class PATHAN_EXPORT LexerDOMStringStream {

public:
  LexerDOMStringStream(const XMLCh* const inputBuffer);
  ~LexerDOMStringStream();
 
  ///Return the number of characters read in from the last read operation
  int gcount() { return last_retrieved; }
  ///Return true if last read, read past the end of 'file'
  bool eof() const; 
  ///Mimic istream fail() (always return false)
  int fail() const { return false; }
  ///Mimic istream bad() (always return false)
  int bad() const { return false; }
  ///Read in (hopefully) numBytes/sizeof(YY_CHAR) YY_CHAR's and write them to ptr, returning the number actually read
  int read(unsigned char *ptr, unsigned int numBytes);

private:
  //FIXME: buffer will have to change once YY_CHAR isn't an unsigned short anymore
  ///The entire stream
  unsigned short *buffer;
  ///The length of the stream;
  unsigned int length;
  ///The current position to next be read in the stream
  unsigned int position;
  ///The number of characters last read in (used by gcount)
  unsigned int last_retrieved;  
};//LexerDOMStringStream

#endif
