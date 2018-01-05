// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from test.djinni

#include "CsConflict.hpp"  // my header
#include "Error.hpp"
#include "Marshal.hpp"
#include "WrapperCache.hpp"

namespace Djinni { namespace TestSuite {

ref class ConflictCppProxy : public Conflict {
    using CppType = std::shared_ptr<::testsuite::Conflict>;
    using HandleType = ::djinni::CppProxyCache::Handle<CppType>;
public:
    ConflictCppProxy(const CppType& cppRef) : _cppRefHandle(new HandleType(cppRef)) {}

    CppType djinni_private_get_proxied_cpp_object() {
        return _cppRefHandle->get();
    }

private:
    AutoPtr<HandleType> _cppRefHandle;
};

Conflict::CppType Conflict::ToCpp(Conflict::CsType cs)
{
    if (!cs) {
        return nullptr;
    }
    return dynamic_cast<ConflictCppProxy^>(cs)->djinni_private_get_proxied_cpp_object();
}

Conflict::CsType Conflict::FromCppOpt(const Conflict::CppOptType& cpp)
{
    if (!cpp) {
        return nullptr;
    }
    return ::djinni::get_cpp_proxy<ConflictCppProxy^>(cpp);
}

} }  // namespace Djinni::TestSuite
