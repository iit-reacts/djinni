#pragma once

#include <msclr\marshal_cppstd.h>

namespace djinni {

// Throws an exception for an unimplemented method call.
void ThrowUnimplemented(const char * ctx, const char * msg);

// Helper function for exception translation. Do not call directly!
void ThrowNativeExceptionFromCurrent(const char * ctx);

}

#define DJINNI_UNIMPLEMENTED(msg) \
    ::djinni::ThrowUnimplemented(__FUNCTION__, msg); \
    return nullptr; // Silence warning C4715: not all control paths return a value

#define DJINNI_TRANSLATE_EXCEPTIONS() \
    catch (const std::exception&) { \
        ::djinni::ThrowNativeExceptionFromCurrent(__FUNCTION__); \
    }