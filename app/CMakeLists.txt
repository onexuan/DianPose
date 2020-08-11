# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)


# header file directory
include_directories(
        src/main/cpp/
        src/main/cpp/include/MNN/
#        src/main/cpp/include/ncnn/ncnn/
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
add_library(libopencv_java3 SHARED IMPORTED)
set_target_properties(libopencv_java3
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libopencv_java3.so)

add_library(libmnn SHARED IMPORTED)
set_target_properties(libmnn
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libMNN.so)


add_library(libmnn_vulkan SHARED IMPORTED)
set_target_properties(libmnn_vulkan
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libMNN_Vulkan.so)


#ncnn
#add_library(libncnn STATIC IMPORTED )
#set_target_properties(libncnn PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/ncnn/libncnn.a)
#
#add_library(libglslang STATIC IMPORTED )
#set_target_properties(libglslang PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/ncnn/libglslang.a)
#
#add_library(libOGLCompiler STATIC IMPORTED )
#set_target_properties(libOGLCompiler PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/ncnn/libOGLCompiler.a)
#
#add_library(libOSDependent STATIC IMPORTED )
#set_target_properties(libOSDependent PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/ncnn/libOSDependent.a)
#
#add_library(libSPIRV STATIC IMPORTED )
#set_target_properties(libSPIRV PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/ncnn/libSPIRV.a)

#openmp
#set(CgiMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fopenmp")
#if(DEFINED ANDROID_NDK_MAJOR AND ${ANDROID_NDK_MAJOR} GREATER 20)
#    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-openmp")
#endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer -fstrict-aliasing -ffast-math")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -fstrict-aliasing -ffast-math")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")




add_library(pose SHARED ${DIANPOSE_COMPILE_CODE})


find_library( # Sets the name of the path variable.
        log-lib log)

target_link_libraries( # Specifies the target library.
        pose
        -Wl,--whole-archive libopencv_java3 libmnn libmnn_vulkan
        -Wl,--no-whole-archive android jnigraphics z  vulkan ${log-lib}  )