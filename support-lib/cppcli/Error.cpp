#include "Error.hpp"

namespace djinni {

void ThrowNativeExceptionFromCurrent() {
  try {
    throw;
  } catch (const std::exception & e) {
    throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(e.what()));
  }
}

}