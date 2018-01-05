// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from primitive_list.djinni

#include "CsPrimitiveList.hpp"  // my header
#include "Marshal.hpp"
#include <memory>

namespace Djinni { namespace TestSuite {

PrimitiveList::PrimitiveList(System::Collections::Generic::List<__int64>^ list)
: _list(list)
{}

System::Collections::Generic::List<__int64>^ PrimitiveList::List::get()
{
    return _list;
}

System::String^ PrimitiveList::ToString()
{
    return "PrimitiveList[TODO]"; // TODO
}

PrimitiveList::CppType PrimitiveList::ToCpp(PrimitiveList::CsType^ cs)
{
    ASSERT(cs != nullptr);
    return {::djinni::List<::djinni::I64>::ToCpp(cs->List)};
}

PrimitiveList::CsType^ PrimitiveList::FromCpp(const PrimitiveList::CppType& cpp)
{
    return gcnew PrimitiveList::CsType(::djinni::List<::djinni::I64>::FromCpp(cpp.list));
}

} }  // namespace Djinni::TestSuite
