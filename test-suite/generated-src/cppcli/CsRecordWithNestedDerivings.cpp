// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from derivings.djinni

#include "CsRecordWithNestedDerivings.hpp"  // my header
#include "Marshal.hpp"
#include <memory>

namespace Djinni { namespace TestSuite {

RecordWithNestedDerivings::RecordWithNestedDerivings(int key,
                                                     RecordWithDerivings^ rec)
: _key(key)
, _rec(rec)
{}

int RecordWithNestedDerivings::Key::get()
{
    return _key;
}

RecordWithDerivings^ RecordWithNestedDerivings::Rec::get()
{
    return _rec;
}

System::String^ RecordWithNestedDerivings::ToString()
{
    return "RecordWithNestedDerivings[TODO]"; // TODO
}

bool RecordWithNestedDerivings::Equals(RecordWithNestedDerivings^ other) {
    if (ReferenceEquals(nullptr, other)) return false;
    if (ReferenceEquals(this, other)) return true;
    return Key == other->Key && Rec->Equals(other->Rec);
}

bool RecordWithNestedDerivings::Equals(System::Object^ obj) {
    if (ReferenceEquals(nullptr, obj)) return false;
    if (ReferenceEquals(this, obj)) return true;
    return obj->GetType() == GetType() && Equals((RecordWithNestedDerivings^) obj);
}

int RecordWithNestedDerivings::GetHashCode() {
    auto hashCode = Key.GetHashCode();
    hashCode = (hashCode * 397) ^ Rec->GetHashCode();
    return hashCode;
}

int RecordWithNestedDerivings::CompareTo(RecordWithNestedDerivings^ other) {
    if (ReferenceEquals(this, other)) return 0;
    if (ReferenceEquals(nullptr, other)) return 1;
    auto keyComparison = Key.CompareTo(other->Key);
    if (keyComparison != 0) return keyComparison;
    return Rec->CompareTo(other->Rec);
}

RecordWithNestedDerivings::CppType RecordWithNestedDerivings::ToCpp(RecordWithNestedDerivings::CsType^ cs)
{
    ASSERT(cs != nullptr);
    return {::djinni::I32::ToCpp(cs->Key),
            ::Djinni::TestSuite::RecordWithDerivings::ToCpp(cs->Rec)};
}

RecordWithNestedDerivings::CsType^ RecordWithNestedDerivings::FromCpp(const RecordWithNestedDerivings::CppType& cpp)
{
    return gcnew RecordWithNestedDerivings::CsType(::djinni::I32::FromCpp(cpp.key),
                                                   ::Djinni::TestSuite::RecordWithDerivings::FromCpp(cpp.rec));
}

} }  // namespace Djinni::TestSuite
