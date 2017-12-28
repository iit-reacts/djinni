#include "Error.hpp"

namespace djinni {

void ThrowUnimplemented(const char *, const char * msg) {
    throw gcnew System::NotImplementedException(msclr::interop::marshal_as<System::String^>(msg));
}

void ThrowNativeExceptionFromCurrent(const char *) {
    try {
        throw;
    } catch (const std::exception & e) {
        throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(e.what()));
    }
}

}