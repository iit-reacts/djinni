cmake_minimum_required(VERSION 3.6.0)

project(djinni_support_lib)

include(GNUInstallDirs)
include(CMakeParseArguments)

function(add_djinni_binding2)
  set(options )
  set(oneValueArgs
    OUTPUT
    IDL
    LIST_IN_FILES
    LIST_OUT_FILES
    CPPCLI_OUT
    CS_NAMESPACE
    CPP_NAMESPACE
    CPP_INCLUDE_PREFIX
  )
  set(multiValueArgs )
  cmake_parse_arguments(DJINNI "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(DJINNI_ARGS
    "--idl" "${DJINNI_IDL}"
    "--list-in-files" "${DJINNI_LIST_IN_FILES}"
    "--list-out-files" "${DJINNI_LIST_OUT_FILES}"
    "--cppcli-out" "${DJINNI_CPPCLI_OUT}"
    "--cs-namespace" "${DJINNI_CS_NAMESPACE}"
    "--cpp-namespace" "${DJINNI_CPP_NAMESPACE}"
    "--cpp-include-prefix" "${DJINNI_CPP_INCLUDE_PREFIX}"
    "--ident-cpp-type" "FooBar"
    "--ident-cpp-method" "FooBar"
  )

  # Figure out our input files for dependencies
  execute_process(
    COMMAND ${DJINNI_RUN} ${DJINNI_ARGS} "--skip-generation" "true"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

  file(STRINGS ${DJINNI_LIST_IN_FILES} DJINNI_SRC_FILES)
  file(STRINGS ${DJINNI_LIST_OUT_FILES} DJINNI_OUT_FILES)

  # Generate Djinni output
  add_custom_command(
    OUTPUT ${DJINNI_OUT_FILES}
    COMMAND ${DJINNI_RUN} ${DJINNI_ARGS}
    DEPENDS ${DJINNI_SRC_FILES}
  )

  # Return the list of files that were generated
  set(${DJINNI_OUTPUT} ${DJINNI_OUT_FILES} PARENT_SCOPE)
endfunction()

set(SRC_SHARED
  "support-lib/djinni_common.hpp"
  "support-lib/proxy_cache_interface.hpp"
  "support-lib/proxy_cache_impl.hpp"
)

set(SRC_JNI
  "support-lib/jni/djinni_support.hpp"
  "support-lib/jni/Marshal.hpp"
  "support-lib/jni/djinni_support.cpp"
)

set(SRC_OBJC
  "support-lib/objc/DJICppWrapperCache+Private.h"
  "support-lib/objc/DJIError.h"
  "support-lib/objc/DJIMarshal+Private.h"
  "support-lib/objc/DJIObjcWrapperCache+Private.h"
  "support-lib/objc/DJIError.mm"
  "support-lib/objc/DJIProxyCaches.mm"
)

set(SRC_CPPCLI
  "support-lib/cppcli/Assert.hpp"
  "support-lib/cppcli/AutoPtr.hpp"
  "support-lib/cppcli/Error.cpp"
  "support-lib/cppcli/Error.hpp"
  "support-lib/cppcli/Marshal.hpp"
  "support-lib/cppcli/WrapperCache.cpp"
  "support-lib/cppcli/WrapperCache.hpp"
)

option(DJINNI_STATIC_LIB "Build Djinni support library as a static library instead of dynamic (the default)." off)
if(DJINNI_STATIC_LIB)
  add_library(djinni_support_lib STATIC ${SRC_SHARED})
else()
  add_library(djinni_support_lib SHARED ${SRC_SHARED})
endif()
source_group("" FILES ${SRC_SHARED})

set_target_properties(djinni_support_lib PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED true
  CXX_EXTENSIONS false
  LINKER_LANGUAGE CXX
)

# Objective-C support
option(DJINNI_WITH_OBJC "Include the Objective-C support code in Djinni support library." off)
if(DJINNI_WITH_OBJC)
  target_include_directories(djinni_support_lib PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/support-lib/objc/>")
  target_sources(djinni_support_lib PRIVATE ${SRC_OBJC})
  source_group("objc" FILES ${SRC_OBJC})
  target_compile_options(djinni_support_lib PUBLIC "-fobjc-arc")
endif()

# JNI support
option(DJINNI_WITH_JNI "Include the JNI support code in Djinni support library." off)
if(DJINNI_WITH_JNI)
  if(NOT DJINNI_STATIC_LIB)
    list(APPEND SRC_JNI "support-lib/jni/djinni_main.cpp")
  endif()
  target_include_directories(djinni_support_lib PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/support-lib/jni/>")
  target_sources(djinni_support_lib PRIVATE ${SRC_JNI})
  source_group("jni" FILES ${SRC_JNI})
  # Do not use the host's jni.h on Android as it is provided automatically by the NDK
  if(NOT ANDROID)
    find_package(JNI REQUIRED QUIET)
    target_include_directories(djinni_support_lib PUBLIC "${JNI_INCLUDE_DIRS}")
  endif()
endif()

# C++/CLI support
option(DJINNI_WITH_CPPCLI "Include the C++/CLI support code in Djinni support library." off)
if (DJINNI_WITH_CPPCLI)
  if (NOT MSVC)
    message(FATAL_ERROR "Enabling DJINNI_WITH_CPPCLI without MSVC is not supported")
  endif ()

  if (NOT DJINNI_STATIC_LIB)
    message(FATAL_ERROR "DJINNI_WITH_CPPCLI requires DJINNI_STATIC_LIB to be true")
  endif()

  # D8016: '/clr' and '/RTC1' command-line options are incompatible
  string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")  # TODO Why do we need this?

  add_library(djinni_cppcli_support_lib STATIC ${SRC_SHARED})
  target_include_directories(djinni_cppcli_support_lib PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/support-lib/cppcli/>")
  target_sources(djinni_cppcli_support_lib PRIVATE ${SRC_CPPCLI})
  source_group("cppcli" FILES ${SRC_CPPCLI})
  set_target_properties(djinni_cppcli_support_lib PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Core"
    COMPILE_FLAGS "/clr /EHa"
  )
endif()

if(NOT (DJINNI_WITH_OBJC OR DJINNI_WITH_JNI OR DJINNI_WITH_CPPCLI))
  message(FATAL_ERROR "At least one of DJINNI_WITH_OBJC, DJINNI_WITH_JNI or DJINNI_WITH_CPPCLI must be enabled.")
endif()

# Store path to the "run" executable so it can be passed as argument to add_custom_command() scripts
if (MSVC)
  set(DJINNI_RUN powershell -ExecutionPolicy Bypass ${CMAKE_CURRENT_SOURCE_DIR}/src/run.ps1 CACHE STRING "Path to the Djinni generator executable.")
else()
  set(DJINNI_RUN "${CMAKE_CURRENT_SOURCE_DIR}/src/run" CACHE FILEPATH "Path to the Djinni generator executable.")
endif ()
