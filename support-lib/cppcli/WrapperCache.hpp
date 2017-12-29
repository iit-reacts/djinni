#pragma once

#include "AutoPtr.hpp"
#include "../proxy_cache_interface.hpp"
#include <vcclr.h>

namespace djinni {

template<class CsRefType>
class CsRef;

template<class CsType>
class CsRef {
public:
    CsRef() : _ref(nullptr) {}
    CsRef(CsType ref) : _ref(ref) {}

    CsRef(CsRef&&) = default;
    CsRef& operator=(CsRef&&) = default;

    CsRef(const CsRef&) = default;
    CsRef& operator=(const CsRef&) = default;

    operator bool() {
        return static_cast<CsType>(_ref) != nullptr;
    }
    CsType operator->() const {
        return get();
    }
    CsType get() const {
        return _ref;
    }

private:
    gcroot<CsType> _ref;
};

class WeakCsRef {
public:
    WeakCsRef(System::Object^ ref) : _weak_ref(gcnew System::WeakReference(ref)) {}
    WeakCsRef(const CsRef<System::Object^>& ref) : _weak_ref(gcnew System::WeakReference(ref.get())) {}

    CsRef<System::Object^> lock() const {
        return dynamic_cast<System::Object^>(_weak_ref->Target);
    }
    bool expired() const {
        return !_weak_ref->IsAlive;
    }

private:
    CsRef<System::WeakReference^> _weak_ref;
};

struct CsIdentityHash;
struct CsIdentityEquals;
struct CsProxyCacheTraits {
  using UnowningImplPointer = WeakCsRef;
  using OwningImplPointer = CsRef<System::Object^>;
  using OwningProxyPointer = std::shared_ptr<void>;
  using WeakProxyPointer = std::weak_ptr<void>;
  using UnowningImplPointerHash = CsIdentityHash;
  using UnowningImplPointerEqual = CsIdentityEquals;
};

// This declares that GenericProxyCache will be instantiated separately. The actual
// explicit instantiations are in DJIProxyCaches.mm.
extern template class ProxyCache<CsProxyCacheTraits>;
using CsProxyCache = ProxyCache<CsProxyCacheTraits>;

template <typename CppType, typename CsType>
static std::shared_ptr<CppType> get_cs_proxy(CsType cs) {
  return std::static_pointer_cast<CppType>(CsProxyCache::get(
    typeid(CppType),
    cs,
    [](const CsProxyCacheTraits::OwningImplPointer& cs) -> std::pair<std::shared_ptr<void>, CsProxyCacheTraits::UnowningImplPointer> {
        return std::make_pair<std::shared_ptr<void>, CsProxyCacheTraits::UnowningImplPointer>(std::make_shared<CppType>(cs), cs);
    }
  ));
}

struct CppProxyCacheTraits {
    using UnowningImplPointer = void *;
    using OwningImplPointer = std::shared_ptr<void>;
    using OwningProxyPointer = CsRef<System::Object^>;
    using WeakProxyPointer = WeakCsRef;
    using UnowningImplPointerHash = std::hash<void *>;
    using UnowningImplPointerEqual = std::equal_to<void *>;
};

// This declares that GenericProxyCache will be instantiated separately. The actual
// explicit instantiations are in CppWrapperCache.cpp.
extern template class ProxyCache<CppProxyCacheTraits>;
using CppProxyCache = ProxyCache<CppProxyCacheTraits>;

//// Helper for get_cpp_proxy_impl that takes a std::shared_ptr.
//template <typename CsType, typename CppType>
//CsType get_cpp_proxy_impl(const std::shared_ptr<CppType> & cppRef) {
//    return CppProxyCache::get(
//        typeid(cppRef),
//        cppRef,
//        [](const std::shared_ptr<void> & cppRef) -> std::pair<CppProxyCacheTraits::OwningProxyPointer, void *> {
//        return {
//            [[CsType alloc] initWithCpp:std::static_pointer_cast<CppType>(cppRef)],
//            cppRef.get()
//        };
//    }
//    );
//}

// get_cpp_proxy takes any smart pointer type, as long as it can be implicitly cast
// to std::shared_ptr. This means get_cpp_proxy can also be passed non-nullable pointers.
template <typename CsType, typename CppPtrType>
CsType get_cpp_proxy(const CppPtrType& cppRef) {
    return nullptr; // get_cpp_proxy_impl<CsType, typename std::remove_reference<decltype(*cppRef)>::type>(cppRef);
}

}