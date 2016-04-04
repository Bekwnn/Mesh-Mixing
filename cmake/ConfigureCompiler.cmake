#--- default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
    message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}") 
endif()

#--- Add debug symbols if we are building in debug mode
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
endif()

#--- windows fixes
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_USE_MATH_DEFINES")
endif()

# Tell IDE (esp. MSVC) to use folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_USE_MATH_DEFINES)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

    if(APPLE)
        set(CMAKE_CC_COMPILER "/usr/local/bin/gcc-5")
        set(CMAKE_CXX_COMPILER "/usr/local/bin/g++-5")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    endif()

#    # Clang requires an additional library to be linked to use C++11.
#    if(CLANG)
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#    endif()
endif()
