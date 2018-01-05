// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from map.djinni

#pragma once

#include "map_record.hpp"

namespace Djinni { namespace TestSuite {

public ref class MapRecord {
public:

    property System::Collections::Generic::Dictionary<System::String^, __int64>^ Map
    {
        System::Collections::Generic::Dictionary<System::String^, __int64>^ get();
    }

    property System::Collections::Generic::Dictionary<int, int>^ Imap
    {
        System::Collections::Generic::Dictionary<int, int>^ get();
    }

    MapRecord(System::Collections::Generic::Dictionary<System::String^, __int64>^ map,
              System::Collections::Generic::Dictionary<int, int>^ imap);

    System::String^ ToString() override;

internal:
    using CppType = ::testsuite::MapRecord;
    using CsType = MapRecord;

    static CppType ToCpp(CsType^ cs);
    static CsType^ FromCpp(const CppType& cpp);

private:
    System::Collections::Generic::Dictionary<System::String^, __int64>^ _map;
    System::Collections::Generic::Dictionary<int, int>^ _imap;
};

} }  // namespace Djinni::TestSuite
