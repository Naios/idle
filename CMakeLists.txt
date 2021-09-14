
cmake_minimum_required(VERSION 3.11)
project(
  idle
  VERSION 0.1.0
  LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

string(COMPARE EQUAL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}
               IS_TOP_LEVEL_PROJECT)

include(cmake/CMakeLists.txt)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(Idle)

idle_enable_packages()

find_package(Boost REQUIRED COMPONENTS system asio filesystem graph leaf)

add_subdirectory(lib)
add_subdirectory(tools)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()

option(IDLE_WITH_EXAMPLES "Enable building idle examples" ON)
if(IDLE_WITH_EXAMPLES)
  add_subdirectory(examples)
endif()

# Setup CPack for bundling
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "Idle")
set(CPACK_PACKAGE_VENDOR "Denis Blank")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/Readme.md")
set(CPACK_PACKAGE_CONTACT "denis.blank@outlook.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Idle - An asynchronous and hot-reloadable C++ dynamic component framework")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${PROJECT_SOURCE_DIR}/Readme.md")
include(CPack)
