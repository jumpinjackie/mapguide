#include "../config/pathan_config.h"
#include <xercesc/util/XMLString.hpp>
#include <pathan/internal/utils/UTF8Str.hpp>

UTF8Str::UTF8Str(const XMLCh* const toTranscode)
{
  if(toTranscode == 0) {
    fUTF8Form = new XMLByte[1];
    fUTF8Form[0] = 0;
  }
  else {
    XERCES_CPP_NAMESPACE_QUALIFIER XMLUTF8Transcoder t(0, 512);
    size_t l = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(toTranscode);
    const unsigned int needed = l * 3 + 1; // 3 bytes per XMLCh is the worst case, + '\0'
    fUTF8Form = new XMLByte[needed];
    unsigned int charsEaten= 0;
    t.transcodeTo(toTranscode, l+1, fUTF8Form, needed, charsEaten, XERCES_CPP_NAMESPACE_QUALIFIER XMLTranscoder::UnRep_Throw);
  }
}
