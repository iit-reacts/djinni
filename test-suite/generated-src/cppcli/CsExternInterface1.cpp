// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from yaml-test.djinni

#include "CsExternInterface1.hpp"  // my header
#include "Error.hpp"
#include "Marshal.hpp"
#include "WrapperCache.hpp"

namespace Djinni { namespace TestSuite {

ref class ExternInterface1CppProxy : public ExternInterface1 {
    using CppType = std::shared_ptr<::ExternInterface1>;
    using HandleType = ::djinni::CppProxyCache::Handle<CppType>;
public:
    ExternInterface1CppProxy(const CppType& cppRef) : _cppRefHandle(new HandleType(cppRef)) {}

    ClientReturnedRecord^ Foo(ClientInterface^ i) override {
        try {
            auto cs_result = _cppRefHandle->get()->foo(::Djinni::TestSuite::ClientInterface::ToCpp(i));
            return ::Djinni::TestSuite::ClientReturnedRecord::FromCpp(cs_result);
        } DJINNI_TRANSLATE_EXCEPTIONS()
        return nullptr; // Unreachable! (Silencing compiler warnings.)
    }

    CppType djinni_private_get_proxied_cpp_object() {
        return _cppRefHandle->get();
    }

private:
    AutoPtr<HandleType> _cppRefHandle;
};

ExternInterface1::CppType ExternInterface1::ToCpp(ExternInterface1::CsType cs)
{
    if (!cs) {
        return nullptr;
    }
    return dynamic_cast<ExternInterface1CppProxy^>(cs)->djinni_private_get_proxied_cpp_object();
}

ExternInterface1::CsType ExternInterface1::FromCppOpt(const ExternInterface1::CppOptType& cpp)
{
    if (!cpp) {
        return nullptr;
    }
    return ::djinni::get_cpp_proxy<ExternInterface1CppProxy^>(cpp);
}

} }  // namespace Djinni::TestSuite
