cmake_minimum_required(VERSION "3.15.0")

project("mofprint"
  VERSION "1.0.0"
  LANGUAGES CXX
  DESCRIPTION "mofprint.ch"
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

include("CTest")
include("CMakePackageConfigHelpers")
include("Conan")
include("DiscoverTests")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# === Language and Linker Options === #

set(CMAKE_CXX_STANDARD "20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

# === System Dependencies === #

find_package("Threads")

# === Conan Configuration === #

if(EXISTS "/.dockerenv")
  set(CONAN_BUILD_DEPS "all")
else()
  set(CONAN_BUILD_DEPS "missing")
endif()

conan_check(REQUIRED)
conan_add_remote(NAME "fmorgner-public" URL "https://api.bintray.com/conan/fmorgner/conan-public")
conan_cmake_run(CONANFILE "conanfile.py"
  BASIC_SETUP
  CMAKE_TARGETS
  BUILD "${CONAN_BUILD_DEPS}"
  NO_OUTPUT_DIRS
)

# === Object Library === #

add_library("core_objects" OBJECT
  "src/net/connection.cpp"
  "src/net/server.cpp"
)

target_include_directories("core_objects" PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_compile_options("core_objects" PUBLIC
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall>
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wextra>
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Werror>
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-pedantic-errors>
)

target_link_libraries("core_objects" LINK_PUBLIC
  "CONAN_PKG::boost"
  "CONAN_PKG::newtype"
  "CONAN_PKG::spdlog"
)

# === Application === #

add_executable("${PROJECT_NAME}"
  "src/main.cpp"
)

target_link_libraries("${PROJECT_NAME}"
  "core_objects"
  "Threads::Threads"
  "CONAN_PKG::lyra"
)

target_link_libraries("${PROJECT_NAME}"
  "-static"
  "-static-libstdc++"
  "-static-libgcc"
)

install(TARGETS "${PROJECT_NAME}"
  RUNTIME DESTINATION "bin"
)

# === Unit Tests === #

add_executable("unittests"
  "test/src/driver.cpp"

  "test/src/support/logger_mixin_suite.cpp"
)

target_include_directories("unittests" PRIVATE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/test/include>
)

target_link_libraries("unittests"
  "core_objects"
  "CONAN_PKG::CUTE"
  "CONAN_PKG::lyra"
)

discover_tests(TARGET "unittests")

add_custom_command(TARGET "unittests"
  POST_BUILD
  COMMAND "${CMAKE_CTEST_COMMAND}"
  ARGS "--output-on-failure"
  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
  COMMENT "Running unit tests"
)
