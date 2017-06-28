cmake_minimum_required(VERSION 3.7)
project(opencvideo_master)

find_package( OpenCV REQUIRED )
include_directories(
        ${OpenCV_INCLUDE_DIRS}
        /usr/include/libavcodec
        /usr/include/libavutil
        /usr/include/libavformat
        /usr/include/libswscale
)

set(SOURCE_FILES
#        getYUVFrame.cpp )
        opencvideo.cpp)

add_executable(opencvideo_master ${SOURCE_FILES} )
target_link_libraries( opencvideo_master
        ${OpenCV_LIBS}
        /usr/lib/x86_64-linux-gnu/libavcodec.so
        /usr/lib/x86_64-linux-gnu/libavformat.so
        /usr/lib/x86_64-linux-gnu/libavutil.so
        /usr/lib/x86_64-linux-gnu/libswscale.so
        )
set_target_properties(opencvideo_master PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        )
