# Install script for directory: /run/media/starlord/Datos/fork-baloo/src/lib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Baloo" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libKF6Baloo.so.6.30.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libKF6Baloo.so.6"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/run/media/starlord/Datos/fork-baloo/src/build/bin/libKF6Baloo.so.6.30.0"
    "/run/media/starlord/Datos/fork-baloo/src/build/bin/libKF6Baloo.so.6"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libKF6Baloo.so.6.30.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libKF6Baloo.so.6"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/run/media/starlord/Datos/fork-baloo/src/build/bin:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Baloo" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/run/media/starlord/Datos/fork-baloo/src/build/bin/libKF6Baloo.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/KF6/Baloo/baloo" TYPE FILE FILES
    "/run/media/starlord/Datos/fork-baloo/src/build/src/lib/core_export.h"
    "/run/media/starlord/Datos/fork-baloo/src/build/src/lib/baloosettings.h"
    "/run/media/starlord/Datos/fork-baloo/src/lib/query.h"
    "/run/media/starlord/Datos/fork-baloo/src/lib/queryrunnable.h"
    "/run/media/starlord/Datos/fork-baloo/src/lib/resultiterator.h"
    "/run/media/starlord/Datos/fork-baloo/src/lib/file.h"
    "/run/media/starlord/Datos/fork-baloo/src/lib/filemonitor.h"
    "/run/media/starlord/Datos/fork-baloo/src/lib/taglistjob.h"
    "/run/media/starlord/Datos/fork-baloo/src/lib/indexerconfig.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/KF6/Baloo/Baloo" TYPE FILE FILES
    "/run/media/starlord/Datos/fork-baloo/src/build/src/lib/Baloo/Query"
    "/run/media/starlord/Datos/fork-baloo/src/build/src/lib/Baloo/QueryRunnable"
    "/run/media/starlord/Datos/fork-baloo/src/build/src/lib/Baloo/ResultIterator"
    "/run/media/starlord/Datos/fork-baloo/src/build/src/lib/Baloo/File"
    "/run/media/starlord/Datos/fork-baloo/src/build/src/lib/Baloo/FileMonitor"
    "/run/media/starlord/Datos/fork-baloo/src/build/src/lib/Baloo/TagListJob"
    "/run/media/starlord/Datos/fork-baloo/src/build/src/lib/Baloo/IndexerConfig"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Baloo" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/config.kcfg" TYPE FILE FILES "/run/media/starlord/Datos/fork-baloo/src/lib/baloosettings.kcfg")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "_install_html_docs_KF6Baloo")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/share/doc/qt6/baloo/")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/share/doc/qt6/baloo" TYPE DIRECTORY FILES "/run/media/starlord/Datos/fork-baloo/src/build/.doc/baloo/")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "_install_qch_docs_KF6Baloo")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/share/doc/qt6/baloo.qch")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/share/doc/qt6" TYPE FILE FILES "/run/media/starlord/Datos/fork-baloo/src/build/.doc/baloo.qch")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Baloo" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/run/media/starlord/Datos/fork-baloo/src/build/src/lib/KF6Baloo.pc")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/run/media/starlord/Datos/fork-baloo/src/build/src/lib/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
