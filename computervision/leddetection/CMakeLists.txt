cmake_minimum_required(VERSION 2.8)
project( liveDetection )
find_package( OpenCV REQUIRED )
add_executable( liveDetection liveDetection.cpp )
target_link_libraries( liveDetection ${OpenCV_LIBS} )
