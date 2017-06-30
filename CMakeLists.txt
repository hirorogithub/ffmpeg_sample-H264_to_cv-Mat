cmake_minimum_required(VERSION 2.8)
project( H264Decoder)

find_package( OpenCV REQUIRED )

set(CMAKE_CXX_STANDARD 11)
set(files
	    H264Decoder_jni.cpp
        H264Decoder.cpp H264Decoder.h
        )

include_directories(
        ${OpenCV_INCLUDE_DIRS}
	/home/curi/java/jdk1.8.0_131/include
	/home/curi/java/jdk1.8.0_131/include/linux
        /usr/include/libavcodec
        /usr/include/libavformat
        /usr/include/libswscale
)
add_library(H264Decoder SHARED ${files})
target_link_libraries(H264Decoder 
        ${OpenCV_LIBS}
        /usr/lib/x86_64-linux-gnu/libavcodec.so
        /usr/lib/x86_64-linux-gnu/libavformat.so
        /usr/lib/x86_64-linux-gnu/libswscale.so
        )






