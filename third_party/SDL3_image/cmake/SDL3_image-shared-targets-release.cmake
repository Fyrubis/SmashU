#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SDL3_image::SDL3_image-shared" for configuration "Release"
set_property(TARGET SDL3_image::SDL3_image-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SDL3_image::SDL3_image-shared PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/SDL3_image.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "SDL3::SDL3-shared"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/SDL3_image.dll"
  )

list(APPEND _cmake_import_check_targets SDL3_image::SDL3_image-shared )
list(APPEND _cmake_import_check_files_for_SDL3_image::SDL3_image-shared "${_IMPORT_PREFIX}/lib/SDL3_image.lib" "${_IMPORT_PREFIX}/bin/SDL3_image.dll" )

# Import target "SDL3_image::external_libwebp" for configuration "Release"
set_property(TARGET SDL3_image::external_libwebp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SDL3_image::external_libwebp PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libwebp.dll"
  )

list(APPEND _cmake_import_check_targets SDL3_image::external_libwebp )
list(APPEND _cmake_import_check_files_for_SDL3_image::external_libwebp "${_IMPORT_PREFIX}/bin/libwebp.dll" )

# Import target "SDL3_image::external_webpdemux" for configuration "Release"
set_property(TARGET SDL3_image::external_webpdemux APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SDL3_image::external_webpdemux PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libwebpdemux.dll"
  )

list(APPEND _cmake_import_check_targets SDL3_image::external_webpdemux )
list(APPEND _cmake_import_check_files_for_SDL3_image::external_webpdemux "${_IMPORT_PREFIX}/bin/libwebpdemux.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
