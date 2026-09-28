#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KF6::Baloo" for configuration "Release"
set_property(TARGET KF6::Baloo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KF6::Baloo PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "KF6::ConfigCore;Qt6::DBus;KF6::Solid;KF6::BalooEngine"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libKF6Baloo.so.6.30.0"
  IMPORTED_SONAME_RELEASE "libKF6Baloo.so.6"
  )

list(APPEND _cmake_import_check_targets KF6::Baloo )
list(APPEND _cmake_import_check_files_for_KF6::Baloo "${_IMPORT_PREFIX}/lib/libKF6Baloo.so.6.30.0" )

# Import target "KF6::BalooEngine" for configuration "Release"
set_property(TARGET KF6::BalooEngine APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KF6::BalooEngine PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "KF6::I18n"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libKF6BalooEngine.so.6.30.0"
  IMPORTED_SONAME_RELEASE "libKF6BalooEngine.so.6"
  )

list(APPEND _cmake_import_check_targets KF6::BalooEngine )
list(APPEND _cmake_import_check_files_for_KF6::BalooEngine "${_IMPORT_PREFIX}/lib/libKF6BalooEngine.so.6.30.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
