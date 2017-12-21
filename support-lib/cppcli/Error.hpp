#pragma once

#include <msclr\marshal_cppstd.h>

namespace djinni {

void ThrowNativeExceptionFromCurrent();

}

#define DJINNI_TRANSLATE_EXCEPTIONS() \
    catch (const std::exception&) { \
        ::djinni::ThrowNativeExceptionFromCurrent(); \
    }