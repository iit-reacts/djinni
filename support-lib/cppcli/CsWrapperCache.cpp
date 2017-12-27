#pragma once

#include "CsWrapperCache.hpp"
#include "../proxy_cache_impl.hpp"

namespace djinni {

struct CsIdentityHash {
  size_t operator() (CsUnowningImplPointer obj) const;
};
struct CsIdentityEquals {
  bool operator() (CsUnowningImplPointer obj1, CsUnowningImplPointer obj2) const;
};

size_t CsIdentityHash::operator() (CsUnowningImplPointer obj) const {
  //JNIEnv * const env = jniGetThreadEnv();
  //const SystemClassInfo & sys = JniClass<SystemClassInfo>::get();
  //jint res = env->CallStaticIntMethod(sys.clazz.get(), sys.staticmethIdentityHashCode, obj);
  //jniExceptionCheck(env);
  return 0;
}
bool CsIdentityEquals::operator() (CsUnowningImplPointer obj1, CsUnowningImplPointer obj2) const {
  //JNIEnv * const env = jniGetThreadEnv();
  //const bool res = env->IsSameObject(obj1, obj2);
  //jniExceptionCheck(env);
  return false;
}

template class ProxyCache<CsProxyCacheTraits>;

}