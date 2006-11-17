#include <xercesc/util/XMLString.hpp>
#include <pathan/internal/DOMutils/XStr.hpp>

XStr::XStr(const char* const toTranscode)
{
  // Call the private transcoding method
  fUnicodeForm = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toTranscode);
}
    
