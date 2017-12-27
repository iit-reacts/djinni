#pragma once

#include "../proxy_cache_interface.hpp"
#include "../proxy_cache_impl.hpp"
#include <vcclr.h>

namespace djinni {

// TODO Try to replace with CsRef<System::Object^>
struct CsOwningImplPointer {
    CsOwningImplPointer(System::Object^ obj) {}
    System::Object^ get() const {
        return _obj;
    }
private:
    gcroot<System::Object^> _obj;
};

struct CsUnowningImplPointer {
public:
    CsUnowningImplPointer(const CsOwningImplPointer& ptr) {}
};

template<class T>
class CsRef {
public:
    CsRef(T^ ref) : _ref(ref) {}
    CsRef(CsOwningImplPointer ptr) {
        // TODO
    }
    operator bool() { return false; }
    CsOwningImplPointer get() const { return CsOwningImplPointer(_ref); }

private:
    gcroot<T^> _ref;
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
static std::shared_ptr<CppType> get_cs_proxy(CsType^ cs) {
  return std::static_pointer_cast<CppType>(CsProxyCache::get(
    typeid(CppType),
    cs,
    [](const CsOwningImplPointer& cs) -> std::pair<std::shared_ptr<void>, CsUnowningImplPointer> {
        return { std::make_shared<CppType>(cs), cs };
    }
  ));
}

}