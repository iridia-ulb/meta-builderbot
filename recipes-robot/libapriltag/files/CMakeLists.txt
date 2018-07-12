cmake_minimum_required(VERSION 3.1)
# Set project name
project(libapriltag)

# Find dependencies
find_package(Threads REQUIRED)

set(APRILTAG_HEADER_FILES
  src/apriltag.h
  src/apriltag_math.h
  src/doubles_floats_impl.h
  src/doubles.h
  src/floats.h
  src/g2d.h
  src/homography.h
  src/image_f32.h
  src/image_types.h
  src/image_u8.h
  src/image_u8x3.h
  src/image_u8x4.h
  src/matd.h
  src/math_util.h
  src/pam.h
  src/pjpeg.h
  src/pnm.h
  src/postscript_utils.h
  src/string_util.h
  src/svd22.h
  src/tag16h5.h
  src/tag25h7.h
  src/tag25h9.h
  src/tag36artoolkit.h
  src/tag36h10.h
  src/tag36h11.h
  src/thash_impl.h
  src/timeprofile.h
  src/time_util.h
  src/unionfind.h
  src/workerpool.h
  src/zarray.h
  src/zhash.h
  src/zmaxheap.h)

set(APRILTAG_SOURCE_FILES
  ${APRILTAG_HEADER_FILES}
  src/apriltag.c
  src/apriltag_quad_thresh.c
  src/g2d.c
  src/homography.c
  src/image_f32.c
  src/image_u8.c
  src/image_u8x3.c
  src/image_u8x4.c
  src/matd.c
  src/pam.c
  src/pjpeg.c
  src/pjpeg-idct.c
  src/pnm.c
  src/string_util.c
  src/svd22.c
  src/tag16h5.c
  src/tag25h7.c
  src/tag25h9.c
  src/tag36artoolkit.c
  src/tag36h10.c
  src/tag36h11.c
  src/time_util.c
  src/unionfind.c
  src/workerpool.c
  src/zarray.c
  src/zhash.c
  src/zmaxheap.c)

set_source_files_properties(
  ${APRILTAG_SOURCE_FILES}
  PROPERTIES
  COMPILE_FLAGS "-Wno-unused-parameter -Wno-unused-function")

# Create shared library
add_library(apriltag SHARED ${APRILTAG_SOURCE_FILES})
# Compile using C99 standard
set_property(TARGET apriltag PROPERTY C_STANDARD 99)
# Link against math, threads
target_link_libraries(apriltag m Threads::Threads)
set_target_properties(apriltag PROPERTIES SOVERSION 0.0.0
  VERSION 0.0.1)
# install library
install(TARGETS apriltag DESTINATION lib)
# Install header files
install (FILES ${APRILTAG_HEADER_FILES} DESTINATION include/apriltag)


