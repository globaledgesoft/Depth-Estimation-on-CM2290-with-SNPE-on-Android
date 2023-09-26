# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Declares and names the project.

project("depthestimation")

set(OpenCV_STATIC on)
set(OpenCV_DIR "<OPENCV_PATH>/opencv-4.8.0-android-sdk/OpenCV-android-sdk/sdk/native/jni")
find_package(OpenCV REQUIRED)

set(CMAKE_VERBOSE_MAKEFILE on)

# build native_app_glue as a static lib
include_directories(${ANDROID_NDK}/sources/android/native_app_glue ${COMMON_SOURCE_DIR})

include_directories(<SNPE_PATH>/snpe-1.68.0.3932/include/zdl)
add_library(app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        depthestimation

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp
        Image_Reader.cpp
        Depth.cpp
        Native_Camera.cpp
        qcsnpe.cpp

        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.


add_library(SNPE SHARED IMPORTED )
set_target_properties(SNPE PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libSNPE.so)

add_library(CPP SHARED IMPORTED )
set_target_properties(CPP PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libc++_shared.so)

add_library(CALC SHARED IMPORTED )
set_target_properties(CALC PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libcalculator.so)
#
#add_library(CALC_HTP SHARED IMPORTED )
#set_target_properties(CALC_HTP PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libcalculator_htp.so)
#

add_library(PlatformValidatorShared SHARED IMPORTED )
set_target_properties(PlatformValidatorShared PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libPlatformValidatorShared.so)

add_library(DSP_V2 SHARED IMPORTED )
set_target_properties(DSP_V2 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libsnpe_dsp_domains_v2.so)
#
#add_library(DSP_V3 SHARED IMPORTED )
#set_target_properties(DSP_V3 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a/libsnpe_dsp_domains_v2_system.so)
#
#



target_link_libraries( # Specifies the target library.
        depthestimation
        SNPE
        CPP
        CALC
        PlatformValidatorShared
        DSP_V2
        camera2ndk
        mediandk
        android
        log
        m

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${OpenCV_LIBS}
        )
