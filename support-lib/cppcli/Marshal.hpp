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

template<class CppTypeArg, class CsTypeArg>
struct Primitive {
    using CppType = CppTypeArg;
    using CsType = CsTypeArg;

    static CppType ToCpp(CsType x) noexcept { return x; }
    static CsType FromCpp(CppType x) noexcept { return x; }
};

using Bool = Primitive<bool, bool>;
using I8 = Primitive<int8_t, char>;
using I16 = Primitive<int16_t, short>;
using I32 = Primitive<int32_t, int>;
using I64 = Primitive<int64_t, __int64>;
using F32 = Primitive<float, float>;
using F64 = Primitive<double, double>;

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

struct Binary {
    using CppType = std::vector<uint8_t>;
    using CsType = array<System::Byte>;

    using Boxed = Binary;

    static CppType ToCpp(CsType^ data) {
        ASSERT(data != nullptr);
        CppType ret;
        ret.reserve(data->Length);
        System::Runtime::InteropServices::Marshal::Copy(data, 0, System::IntPtr(&ret[0]), data->Length);
        return ret;
    }

    static CsType^ FromCpp(const CppType& bytes) {
        auto len = bytes.size();
        CsType^ ret = gcnew CsType(len);
        System::Runtime::InteropServices::Marshal::Copy(System::IntPtr(const_cast<CppType::value_type*>(&bytes[0])), ret, 0, len);
        return ret;
    }
};

template<template<class> class OptionalType, class T>
struct Optional {
    // SFINAE helper: if C::CppOptType exists, opt_type<T>(nullptr) will return
    // that type. If not, it returns OptionalType<C::CppType>. This is necessary
    // because we special-case optional interfaces to be represented as a nullable
    // std::shared_ptr<T>, not optional<shared_ptr<T>> or optional<nn<shared_ptr<T>>>.
    template <typename C> static OptionalType<typename C::CppType> opt_type(void*);
    template <typename C> static typename C::CppOptType opt_type(typename C::CppOptType*);

    using CppType = decltype(opt_type<T>(nullptr));
    using CsType = typename T::CsType;

    static CppType ToCpp(System::Nullable<CsType> obj) {
        if (obj.HasValue) {
            return T::ToCpp(obj.Value);
        } else {
            return CppType();
        }
    }

    static CppType ToCpp(CsType^ obj) {
        if (obj != nullptr) {
            return T::ToCpp(obj.Value);
        } else {
            return CppType();
        }
    }

    // FromCpp used for normal optionals
    static System::Nullable<CsType> FromCpp(const OptionalType<typename T::CppType>& opt) {
        return opt ? T::FromCpp(*opt) : System::Nullable<CsType>();
    }

    // FromCpp used for nullable objects
    template <typename C = T>
    static System::Nullable<CsType> FromCpp(const typename C::CppOptType& cppOpt) {
        return T::FromCppOpt(cppOpt);
    }
};

template<class Key, class Value>
struct Map {
  using CppType = std::unordered_map<typename Key::CppType, typename Value::CppType>;
  using CsType = System::Collections::Generic::Dictionary<typename Key::CsType, typename Value::CsType>;

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