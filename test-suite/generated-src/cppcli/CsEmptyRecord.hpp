// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from test.djinni

#pragma once

#include "empty_record.hpp"

namespace Djinni { namespace TestSuite {

/**
 * Empty record
 * (Second line of multi-line documentation.
 *   Indented third line of multi-line documentation.)
 */
public ref class EmptyRecord {
public:

    System::String^ ToString() override;

internal:
    using CppType = ::testsuite::EmptyRecord;
    using CsType = EmptyRecord;

    static CppType ToCpp(CsType^ cs);
    static CsType^ FromCpp(const CppType& cpp);

private:
};

} }  // namespace Djinni::TestSuite
