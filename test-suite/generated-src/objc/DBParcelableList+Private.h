// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from parcelable.djinni

#import "DBParcelableList.h"
#include "parcelable_list.hpp"

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@class DBParcelableList;

namespace djinni_generated {

struct ParcelableList
{
    using CppType = ::testsuite::ParcelableList;
    using ObjcType = DBParcelableList*;

    using Boxed = ParcelableList;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCpp(const CppType& cpp);
};

}  // namespace djinni_generated
