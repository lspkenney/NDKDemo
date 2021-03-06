# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

#配置工程路径
set(path_project F:/workspace/NDKDemo)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.c )

add_library( # Sets the name of the library.
          effect-lib

          # Sets the library as a shared library.
          SHARED

          # Provides a relative path to your source file(s).
          src/main/cpp/voiceChange.cpp )

#添加两个预编译的so库
add_library( fmod
             SHARED
             IMPORTED
            )

add_library( libfmodL
             SHARED
             IMPORTED
            )

#设置两个预编译的库的路径，注意这里最好要使用绝对路径，不然会链接错误
set_target_properties(
                       fmod
                       PROPERTIES IMPORTED_LOCATION
                       ${path_project}/app/libs/${ANDROID_ABI}/libfmod.so
                    )

set_target_properties(
                       libfmodL
                       PROPERTIES IMPORTED_LOCATION
                       ${path_project}/app/libs/${ANDROID_ABI}/libfmodL.so
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
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

#把需要的库链接到自己的库中
target_link_libraries(
            effect-lib
            ${log-lib}
            fmod
            libfmodL
            )