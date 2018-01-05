// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from single_language_interfaces.djinni

#pragma once

#include "CsCsOnlyListener.hpp"
#include "CsJavaOnlyListener.hpp"
#include "CsObjcOnlyListener.hpp"
#include "uses_single_language_listeners.hpp"
#include <memory>

namespace Djinni { namespace TestSuite {

public ref class UsesSingleLanguageListeners abstract {
public:
    virtual void CallForObjC(ObjcOnlyListener^ l) abstract;

    virtual ObjcOnlyListener^ ReturnForObjC() abstract;

    virtual void CallForJava(JavaOnlyListener^ l) abstract;

    virtual JavaOnlyListener^ ReturnForJava() abstract;

    virtual void CallForCs(CsOnlyListener^ l) abstract;

    virtual CsOnlyListener^ ReturnForCs() abstract;

internal:
    using CppType = std::shared_ptr<::testsuite::UsesSingleLanguageListeners>;
    using CppOptType = std::shared_ptr<::testsuite::UsesSingleLanguageListeners>;
    using CsType = UsesSingleLanguageListeners^;

    static CppType ToCpp(CsType cs);
    static CsType FromCppOpt(const CppOptType& cpp);
    static CsType FromCpp(const CppType& cpp) { return FromCppOpt(cpp); }
};

} }  // namespace Djinni::TestSuite
