# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# header file directory
include_directories(
        src/main/cpp/
        src/main/cpp/include/MNN/
        src/main/cpp/include/opencv_include/
        src/main/cpp/base/
        src/main/cpp/detection/
        src/main/cpp/pose/
)

# source code files
file(GLOB DIANPOSE_SRC
        src/main/cpp/*.cpp
        src/main/cpp/base/*.cpp
        src/main/cpp/detection/*.cpp
        src/main/cpp/pose/*.cpp
        )

set(DIANPOSE_COMPILE_CODE ${DIANPOSE_SRC})
set(LOCAL_EXPORT_CXX_INCLUDES ${CMAKE_SOURCE_DIR}/src/main/cpp)

# add third library
#add_library(libmnn STATIC IMPORTED)
#set_target_properties(libmnn
#        PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libMNN.a)

add_library(libopencv_java3 SHARED IMPORTED)
set_target_properties(libopencv_java3
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libopencv_java3.so)

add_library(libmnn SHARED IMPORTED)
set_target_properties(libmnn
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libMNN.so)
add_library(libmnn_cl SHARED IMPORTED)
set_target_properties(libmnn_cl
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libMNN_CL.so)
add_library(libmnn_gl SHARED IMPORTED)
set_target_properties(libmnn_gl
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libMNN_GL.so)
add_library(libmnn_vulkan SHARED IMPORTED)
set_target_properties(libmnn_vulkan
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libMNN_Vulkan.so)
add_library(libmnn_express SHARED IMPORTED)
set_target_properties(libmnn_express
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libMNN_Express.so)



add_library(pose SHARED ${DIANPOSE_COMPILE_CODE})


find_library( # Sets the name of the path variable.
        log-lib log)

target_link_libraries( # Specifies the target library.
        pose
        -Wl,--whole-archive libopencv_java3 libmnn libmnn_cl libmnn_gl libmnn_vulkan libmnn_express
        -Wl,--no-whole-archive android jnigraphics z ${log-lib})