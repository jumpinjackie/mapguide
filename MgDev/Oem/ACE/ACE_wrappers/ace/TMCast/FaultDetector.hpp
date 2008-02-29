// file      : TMCast/FaultDetector.hpp
// author    : Boris Kolpackov <boris@dre.vanderbilt.edu>
// cvs-id    : FaultDetector.hpp,v 1.1 2003/11/03 23:23:22 boris Exp

#include "Protocol.hpp"

namespace TMCast
{
  class FaultDetector
  {
  public:
    FaultDetector ()
        : silence_period_ (-1)
    {
    }

  public:
    class Failed {};


    void
    insync ()
    {
      silence_period_ = 0;
    }

    void
    outsync ()
    {
      if (++silence_period_ >= Protocol::FATAL_SILENCE_FRAME)
      {
        // cerr << "Silence period has been passed." << endl;
        // cerr << "Decalring the node failed." << endl;
        throw Failed ();
      }
    }

  private:
    short silence_period_;
  };
}
