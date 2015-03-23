// $Id: coverity_model.cpp 97544 2014-01-23 12:53:08Z johnnyw $

namespace CORBA
{
  typedef bool Boolean;

  static CORBA::Boolean is_nil (void* x)
  {
    return x == 0;
  }
}
