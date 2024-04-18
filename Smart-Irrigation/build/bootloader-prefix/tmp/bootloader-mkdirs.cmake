# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Espressif/frameworks/esp-idf-v5.2.1/components/bootloader/subproject"
  "D:/SUTD/Term_8/IOT/esp-rainmaker/examples/multi_device/build/bootloader"
  "D:/SUTD/Term_8/IOT/esp-rainmaker/examples/multi_device/build/bootloader-prefix"
  "D:/SUTD/Term_8/IOT/esp-rainmaker/examples/multi_device/build/bootloader-prefix/tmp"
  "D:/SUTD/Term_8/IOT/esp-rainmaker/examples/multi_device/build/bootloader-prefix/src/bootloader-stamp"
  "D:/SUTD/Term_8/IOT/esp-rainmaker/examples/multi_device/build/bootloader-prefix/src"
  "D:/SUTD/Term_8/IOT/esp-rainmaker/examples/multi_device/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/SUTD/Term_8/IOT/esp-rainmaker/examples/multi_device/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/SUTD/Term_8/IOT/esp-rainmaker/examples/multi_device/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
