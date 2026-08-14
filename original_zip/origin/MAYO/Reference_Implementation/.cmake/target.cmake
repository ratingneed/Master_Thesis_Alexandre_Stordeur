# modified by Alexandre Stordeur the 09/02/26

# SPDX-License-Identifier: Apache-2.0

# The whole file is here to tell how the code need to be compiled

# Add -D define flags to the compilation of source files.
# add_definitions(-DFOO -DBAR ...)

# this section set the flag for the current processor
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm64" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    add_definitions(-DTARGET_ARM64)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    add_definitions(-DTARGET_ARM)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    add_definitions(-DTARGET_AMD64)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "i386" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "i686")
    add_definitions(-DTARGET_X86)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "^(s390x.*|S390X.*)")
    add_definitions(-DTARGET_S390X)
    add_definitions(-DTARGET_BIG_ENDIAN)
else()
    add_definitions(-DTARGET_OTHER)
endif()


# this section set the flag for the current OS
if (APPLE)
    add_definitions(-DTARGET_OS_MAC)
elseif (UNIX)
    add_definitions(-DTARGET_OS_UNIX)
else()
    add_definitions(-DTARGET_OS_OTHER)
endif()

set(G_C_OPT_FLAGS "")


# this section set the default build type as ref if not defined
if ((NOT DEFINED MAYO_BUILD_TYPE))
  set(MAYO_BUILD_TYPE ref)
endif()

# this section set the isa for the architecture desired
if (DEFINED MAYO_MARCH)
  list(APPEND G_C_OPT_FLAGS
  ${MAYO_MARCH})
endif()

# CMAKE_CXX_COMPILER_ID is a variable that cmake create after looking which compiler you have
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64"))
  # -march=native tell to use isa of the current architecture for vectorisation
  if (NOT DEFINED MAYO_MARCH)
    list(APPEND G_C_OPT_FLAGS
      -march=native)
  endif()

  # This section set the use of aes isa from the architecture
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    option(ENABLE_AESNI "Use AESni" ON)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  if (NOT DEFINED MAYO_MARCH)
    list(APPEND G_C_OPT_FLAGS
      -mcpu=apple-m1)
  endif()
  option(ENABLE_AESNI "Use AESni" OFF)
  option(ENABLE_AESNEON "Use AES-NEON" ON)
endif()


if (${MAYO_BUILD_TYPE} MATCHES "ref")
  option(ENABLE_AESNI "Use AESni" OFF)
  option(ENABLE_PARAMS_DYNAMIC "Use dynamic parameters" ON)
  add_definitions(-DMAYO_BUILD_TYPE_REF)
elseif(${MAYO_BUILD_TYPE} MATCHES "opt")
  add_definitions(-DMAYO_BUILD_TYPE_OPT)
  option(ENABLE_PARAMS_DYNAMIC "Use dynamic parameters" OFF)
elseif(${MAYO_BUILD_TYPE} MATCHES "avx2")
  add_definitions(-DMAYO_BUILD_TYPE_AVX2)
  option(ENABLE_PARAMS_DYNAMIC "Use dynamic parameters" OFF)
  elseif(${MAYO_BUILD_TYPE} MATCHES "neon")
  add_definitions(-DMAYO_BUILD_TYPE_NEON)
  option(ENABLE_PARAMS_DYNAMIC "Use dynamic parameters" OFF)
endif()

# Parse command-line arguments into a semicolon-separated list.
# separate_arguments(<variable> <mode> [PROGRAM [SEPARATE_ARGS]] <args>)
separate_arguments(C_OPT_FLAGS UNIX_COMMAND "${G_C_OPT_FLAGS}")
