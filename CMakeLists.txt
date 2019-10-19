cmake_minimum_required(VERSION 3.0.0)
project(rb_tree VERSION 0.1.0)

set(ULTRALIGHT_INCLUDE_DIRS "include")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ULTRALIGHT_LIBRARY_DIRS "lib/win/x64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ULTRALIGHT_LIBRARY_DIRS "lib/win/x86")
endif()

set(
    SOURCES

    src/main.cpp
    src/app.hpp
    src/rb_tree.hpp
)

include_directories("${ULTRALIGHT_INCLUDE_DIRS}")
link_directories("${ULTRALIGHT_LIBRARY_DIRS}")
link_libraries(UltralightCore AppCore Ultralight WebCore)

include(CTest)
enable_testing()

add_executable(rb_tree WIN32 ${SOURCES})

set_target_properties(rb_tree PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
