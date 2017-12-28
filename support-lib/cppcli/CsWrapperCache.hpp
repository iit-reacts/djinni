#pragma once

#include "../proxy_cache_interface.hpp"
#include <vcclr.h>

namespace djinni {

template<class CsRefType>
class CsRef;

using CsOwningImplPointer = CsRef<System::Object^>;

struct CsUnowningImplPointer {
public:
    CsUnowningImplPointer(System::Object^ ref) {}
    CsUnowningImplPointer(const CsOwningImplPointer& ptr) {}
};

template<class CsType>
class CsRef {
public:
    CsRef(CsType ref) : _ref(ref) {}

    //CsRef(CsOwningImplPointer ptr) : _ref(dynamic_cast<CsType>(ptr.get())) {
    //    if (!_ref) {
    //        throw std::bad_cast;
    //    }
    //}

    CsRef(CsRef&&) = default;
    CsRef& operator=(CsRef&&) = default;
    
    CsRef(const CsRef&) = delete;
    CsRef& operator=(const CsRef&) = delete;

    operator bool() {
        return static_cast<CsType>(_ref) != nullptr;
    }

    CsType get() const {
        return _ref;
    }

private:
    gcroot<CsType> _ref;
};

struct CsIdentityHash;
struct CsIdentityEquals;
struct CsProxyCacheTraits {
  using UnowningImplPointer = CsUnowningImplPointer;
  using OwningImplPointer = CsOwningImplPointer;
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
    [](const CsOwningImplPointer& cs) -> std::pair<std::shared_ptr<void>, CsUnowningImplPointer> {
        return std::make_pair<std::shared_ptr<void>, CsUnowningImplPointer>(std::make_shared<CppType>(cs), cs);
    }
  ));
}

}