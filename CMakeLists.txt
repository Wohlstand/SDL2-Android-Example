cmake_minimum_required(VERSION 3.5)

project(SDL2_AndroidExample LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT ANDROID)
    set(USE_SYSTEM_SDL2 ON)
endif()

if(ANDROID)
    set(ANDROID_PLATFORM "android-16")
    set(ANDROID_CMAKE_FLAGS
        "-DANDROID_ABI=${ANDROID_ABI}"
        "-DANDROID_NDK=${ANDROID_NDK}"
        "-DANDROID_STL=c++_static"
        "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
        "-DANDROID_PLATFORM=${ANDROID_PLATFORM}"
        "-DANDROID_TOOLCHAIN=${ANDROID_TOOLCHAIN}"
        "-DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}"
        "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
        "-DANDROID_ARM_NEON=${ANDROID_ARM_NEON}"
    )
endif()

if(ANDROID)
    include(android-project/ndk-stl-config.cmake)
    set(ANDROID_STL "c++_static")
    set(USE_USE_NDK_MAKE OFF)
    set(USE_LOCAL_SDL2 ON)
    include(android-project/download_sdl2_hg.cmake)
endif()

if(USE_SYSTEM_SDL2)
    find_package(SDL2 REQUIRED)
    if(TARGET SDL2::SDL2)
        set(SDL2_LIBRARIES SDL2::SDL2main SDL2::SDL2)
    endif()
else()
    set(USE_LOCAL_SDL2 ${PGE_USE_LOCAL_SDL2})
    set(SDL2_INCLUDE_DIRS ${DEPENDENCIES_INSTALL_DIR}/include/SDL2)
endif()

if(ANDROID)
    add_library(thextech SHARED main.cpp)
    add_dependencies(thextech SDL2HG)
    target_include_directories(thextech PRIVATE ${CMAKE_BINARY_DIR}/include)
    link_directories(${CMAKE_BINARY_DIR}/lib)
    target_link_libraries(thextech PRIVATE ${CMAKE_BINARY_DIR}/lib/libSDL2.so ${CMAKE_BINARY_DIR}/lib/libSDL2main.a ${CMAKE_BINARY_DIR}/lib/libhidapi.so GLESv1_CM GLESv2 OpenSLES log dl android)

#    # Include own-built libraries into the APK
#    set(APK_PACK_LIBS ${CMAKE_SOURCE_DIR}/android-project/thextech/jniLibs/${CMAKE_BUILD_TYPE_LOWER}/${ANDROID_ABI})
#    add_custom_target(SDL_ApkPackLibs_makeDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${APK_PACK_LIBS}")
#
#    #        add_library(SDL2 SHARED IMPORTED DEPENDS SDL2HG)
#    #        set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION "${DEPENDENCIES_INSTALL_DIR}/lib/libSDL2.so")
#    add_custom_target(SDL_ApkPackLibs_sdl2 ALL
#            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/lib/libSDL2.so" "${APK_PACK_LIBS}"
#            DEPENDS SDL2HG SDL_ApkPackLibs_makeDir ${CMAKE_BINARY_DIR}/lib/libSDL2.so
#            )
#    add_dependencies(thextech SDL_ApkPackLibs_sdl2)
#
#    #    add_library(hidapi SHARED IMPORTED DEPENDS SDL2HG)
#    #    set_target_properties(hidapi PROPERTIES IMPORTED_LOCATION "${DEPENDENCIES_INSTALL_DIR}/lib/libhidapi.so")
#    add_custom_target(SDL_ApkPackLibs_hidapi ALL
#            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/lib/libhidapi.so" "${APK_PACK_LIBS}"
#            DEPENDS SDL2HG SDL_ApkPackLibs_makeDir "${CMAKE_BINARY_DIR}/lib/libhidapi.so"
#            )
#    add_dependencies(thextech SDL_ApkPackLibs_hidapi)
else()
    add_executable(thextech main.cpp)
endif()

target_include_directories(thextech PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(thextech PRIVATE ${SDL2_LIBRARIES})
