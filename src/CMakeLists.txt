cmake_minimum_required (VERSION 2.8)

set (opencv_include_dir C:/project/lib/opencv3.3/include	C:/project/lib/opencv3.3/include/opencv2)
set (opencv_lib_dir C:/project/lib/opencv3.3/x64/vc15/lib)
set (opencv_lib debug opencv_world330d.lib optimized opencv_world330.lib)

set (
        src circle_detection.cpp circle_taehwa.h
    )

project (circle_detection)

include_directories (${opencv_include_dir})
link_directories (${opencv_lib_dir})
add_executable (circle_detection ${src})
set(EXECUTABLE_OUTPUT_PATH "../bin")

target_link_libraries (circle_detection ${opencv_lib})