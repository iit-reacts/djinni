#pragma once

#include <msclr\marshal_cppstd.h>

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#ifdef _DEBUG
#define ASSERT(condition, ...) ::System::Diagnostics::Debug::Assert(condition, ##__VA_ARGS__)
#else
#define ASSERT(condition, ...) // This macro will completely evaporate in Release.
#endif

namespace djinni {

template<class CppEnum, class CsEnum>
struct Enum {
  using CppType = CppEnum;
  using CsType = CsEnum;

  static CppType ToCpp(CsType e) noexcept {
    return static_cast<CppType>(e);
  }

  static CsType FromCpp(CppType e) noexcept {
    return static_cast<CsType>(e);
  }
};

struct String {
  using CppType = std::string;
  using CsType = System::String^;

  static CppType ToCpp(CsType string) {
    ASSERT(string != nullptr);
    return msclr::interop::marshal_as<CppType>(string);
  }

  static CsType FromCpp(const CppType& string) {
    return msclr::interop::marshal_as<CsType>(string);
  }
};

template<class Key, class Value>
class Map {
  using CppKeyType = typename Key::CppType;
  using CppValueType = typename Value::CppType;
  using CsKeyType = typename Key::CsType;
  using CsValueType = typename Value::CsType;

public:
  using CppType = std::unordered_map<CppKeyType, CppValueType>;
  using CsType = System::Collections::Generic::Dictionary<CsKeyType, CsValueType>;

  static CppType ToCpp(CsType^ map) {
    ASSERT(map != nullptr);
    CppType m;
    m.reserve(map->Count);
    for each (auto& kvp in map) {
      m.emplace(Key::ToCpp(kvp.Key), Value::ToCpp(kvp.Value));
    }
    return m;
  }

  static CsType^ FromCpp(const CppType& m) {
    auto map = gcnew CsType(m.size());
    for (const auto& kvp : m) {
      map->Add(Key::FromCpp(kvp.first), Value::FromCpp(kvp.second));
    }
    return map;
  }
};

} // namespace djinni