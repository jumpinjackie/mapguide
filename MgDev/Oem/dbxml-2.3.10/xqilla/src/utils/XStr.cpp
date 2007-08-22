#include "../config/xqilla_config.h"
#include <xercesc/util/XMLString.hpp>
#include <xqilla/utils/XStr.hpp>

XStr::XStr(const char* const toTranscode)
{
  fUnicodeForm = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toTranscode);
}

XStr::~XStr()
{
  XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&fUnicodeForm);
}
