#pragma once

#include "WrapperCache.hpp"
#include "../proxy_cache_impl.hpp"

namespace djinni {

struct CsIdentityHash {
  size_t operator() (CsProxyCacheTraits::UnowningImplPointer obj) const;
};
struct CsIdentityEquals {
  bool operator() (CsProxyCacheTraits::UnowningImplPointer obj1, CsProxyCacheTraits::UnowningImplPointer obj2) const;
};

size_t CsIdentityHash::operator() (CsProxyCacheTraits::UnowningImplPointer obj) const {
  //JNIEnv * const env = jniGetThreadEnv();
  //const SystemClassInfo & sys = JniClass<SystemClassInfo>::get();
  //jint res = env->CallStaticIntMethod(sys.clazz.get(), sys.staticmethIdentityHashCode, obj);
  //jniExceptionCheck(env);
  return 0;
}
bool CsIdentityEquals::operator() (CsProxyCacheTraits::UnowningImplPointer obj1, CsProxyCacheTraits::UnowningImplPointer obj2) const {
  //JNIEnv * const env = jniGetThreadEnv();
  //const bool res = env->IsSameObject(obj1, obj2);
  //jniExceptionCheck(env);
  return false;
}

template class ProxyCache<CsProxyCacheTraits>;
template class ProxyCache<CppProxyCacheTraits>;

}