# modified by Alexandre Stordeur the 09/02/26

# SPDX-License-Identifier: Apache-2.0



if (CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib")
endif()


# choose the path where the compiled code is
if (SOURCE_PATH)
	set(SOURCE_FINAL_PATH ${SOURCE_PATH})
else()
	set(SOURCE_FINAL_PATH ${PROJECT_BINARY_DIR}/src)
endif()

if(SOURCE_PATH)
   message("Source code is in ${SOURCE_PATH}")
endif()

# include modules
include(GNUInstallDirs)
include(CheckSymbolExists)
include(CMakePushCheckState)


# this section set flags for strict compilation
set(STRICT_OPTIONS_CPP )
set(STRICT_OPTIONS_C )
set(STRICT_OPTIONS_CXX )
if(MSVC)
	if(ENABLE_STRICT)
		set(STRICT_OPTIONS_CPP "${STRICT_OPTIONS_CPP} /WX /Zc:__cplusplus")
	endif()
else()
	# -O2
	set(STRICT_OPTIONS_CXX "${STRICT_OPTIONS_CXX} -std=c++14 -O2")
	set(STRICT_OPTIONS_CPP "${STRICT_OPTIONS_CPP} -Wall -Wuninitialized -Wno-deprecated-declarations -Wno-missing-field-initializers")
	# -O3
	if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(STRICT_OPTIONS_C "${STRICT_OPTIONS_C} -O3")
	endif()
	set(STRICT_OPTIONS_C "${STRICT_OPTIONS_C} -Wstrict-prototypes -Wno-error=strict-prototypes -fvisibility=hidden -funroll-loops -Wno-error=implicit-function-declaration -Wno-error=attributes")
	if(CMAKE_C_COMPILER_ID MATCHES "Clang")
		set(STRICT_OPTIONS_CPP "${STRICT_OPTIONS_CPP} -Wno-error=unknown-warning-option -Qunused-arguments -Wno-tautological-compare")
		set(STRICT_OPTIONS_CPP "${STRICT_OPTIONS_CPP} -Wno-unused-function -Wno-pass-failed")
	endif()
	if(ENABLE_STRICT)
		set(STRICT_OPTIONS_CPP "${STRICT_OPTIONS_CPP} -Werror -Wextra -Wno-unused-parameter -fno-strict-aliasing")
	endif()
endif()

# put all flags together
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${STRICT_OPTIONS_C} ${STRICT_OPTIONS_CPP}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STRICT_OPTIONS_CXX} ${STRICT_OPTIONS_CPP}")