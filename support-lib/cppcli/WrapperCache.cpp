#pragma once

#include "WrapperCache.hpp"
#include "../proxy_cache_impl.hpp"

namespace djinni {

using UnowningImplPointer = CsProxyCacheTraits::UnowningImplPointer;

struct CsHashCode { size_t operator() (UnowningImplPointer obj) const; };
struct CsReferenceEquals { bool operator() (UnowningImplPointer obj1, UnowningImplPointer obj2) const; };

size_t CsHashCode::operator() (UnowningImplPointer obj) const {
    return obj.hash_code();
}
bool CsReferenceEquals::operator() (UnowningImplPointer obj1, UnowningImplPointer obj2) const {
    auto ptr1 = obj1.lock();
    auto ptr2 = obj2.lock();
    return System::Object::ReferenceEquals(ptr1.get(), ptr2.get());
}

template class ProxyCache<CsProxyCacheTraits>;
template class ProxyCache<CppProxyCacheTraits>;

}