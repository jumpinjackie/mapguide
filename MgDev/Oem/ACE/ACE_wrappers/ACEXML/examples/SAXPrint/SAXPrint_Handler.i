// -*- C++ -*-  SAXPrint_Handler.i,v 1.2 2002/01/24 21:00:01 nanbor Exp

ACEXML_INLINE void
ACEXML_SAXPrint_Handler::inc_indent (void)
{
  this->indent_ += 1;
}

ACEXML_INLINE void
ACEXML_SAXPrint_Handler::dec_indent (void)
{
  this->indent_ -= 1;
}
