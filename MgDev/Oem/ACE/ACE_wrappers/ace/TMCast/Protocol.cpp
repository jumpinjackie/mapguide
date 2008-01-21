// file      : TMCast/Protocol.cpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : Protocol.cpp,v 1.1 2003/11/03 23:23:22 boris Exp

#include "Protocol.hpp"

namespace TMCast
{
  namespace Protocol
  {
    namespace
    {
      char const* labels[] = {
        "NONE", "BEGIN", "COMMIT", "ABORT", "COMMITED", "ABORTED"};
    }

    /*
      std::string
      tslabel (Protocol::TransactionStatus s)
      {
      return labels[s];
      }

      std::ostream&
      operator << (std::ostream& o, Transaction const& t)
      {
      return o << "{" << t.id << "; " << tslabel (t.status) << "}";
      }
    */
  }
}
