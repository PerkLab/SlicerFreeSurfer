project(FreeSurfer)

#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.13.4)
#-----------------------------------------------------------------------------

# --------------------------------------------------------------------------
# Options
# --------------------------------------------------------------------------
if(NOT DEFINED BUILD_SHARED_LIBS)
  option(BUILD_SHARED_LIBS "Build with shared libraries." ON)
endif()

# --------------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------------

#
# VTK
#
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

#
# ZLIB
#
find_package(ZLIB REQUIRED)

# --------------------------------------------------------------------------
# Include dirs
# --------------------------------------------------------------------------
set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${ZLIB_INCLUDE_DIRS}
  )
include_directories(${include_dirs})

# --------------------------------------------------------------------------
# Configure headers
# --------------------------------------------------------------------------
set(configure_header_file FreeSurferConfigure.h)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/${configure_header_file}.in
  ${CMAKE_CURRENT_BINARY_DIR}/${configure_header_file}
  )

# --------------------------------------------------------------------------
# Install headers
# --------------------------------------------------------------------------
if(NOT DEFINED ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  set(${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT ON)
endif()
if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  install(
    FILES ${headers} ${CMAKE_CURRENT_BINARY_DIR}/${configure_header_file}
    DESTINATION include/${PROJECT_NAME} COMPONENT Development)
endif()

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------
set(FreeSurfer_SRCS
  vtkFSIO.cxx
  vtkFSSurfaceReader.cxx
  vtkFSSurfaceAnnotationReader.cxx
  vtkFSSurfaceScalarReader.cxx
  vtkFSSurfaceWFileReader.cxx
  vtkFSSurfaceLabelReader.cxx
  vtkFSLookupTable.cxx
  vtkFSSurfaceHelper.cxx
  )

set_source_files_properties(
  vtkFSIO.cxx
  WRAP_EXCLUDE
  )

# --------------------------------------------------------------------------
# Build library
# --------------------------------------------------------------------------
set(lib_name ${PROJECT_NAME})
set(srcs ${FreeSurfer_SRCS})

add_library(${lib_name} ${srcs})

set(libs
  ${VTK_LIBRARIES}
  ${ZLIB_LIBRARIES}
  )
target_link_libraries(${lib_name} ${libs})

# Apply user-defined properties to the library target.
if(Slicer_LIBRARY_PROPERTIES)
  set_target_properties(${lib_name} PROPERTIES ${Slicer_LIBRARY_PROPERTIES})
endif()

# Set loadable modules output
set_target_properties(${lib_name} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_BIN_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_LIB_DIR}"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_LIB_DIR}"
  )

# --------------------------------------------------------------------------
# Folder
# --------------------------------------------------------------------------
if(NOT DEFINED ${PROJECT_NAME}_FOLDER)
  set(${PROJECT_NAME}_FOLDER ${PROJECT_NAME})
endif()
if(NOT "${${PROJECT_NAME}_FOLDER}" STREQUAL "")
  set_target_properties(${lib_name} PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER})
endif()

# --------------------------------------------------------------------------
# Export target
# --------------------------------------------------------------------------
if(NOT DEFINED ${PROJECT_NAME}_EXPORT_FILE)
  set(${PROJECT_NAME}_EXPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)
endif()
export(TARGETS ${lib_name} APPEND FILE ${${PROJECT_NAME}_EXPORT_FILE})

# --------------------------------------------------------------------------
# Install library
# --------------------------------------------------------------------------
if(NOT DEFINED ${PROJECT_NAME}_INSTALL_BIN_DIR)
  set(${PROJECT_NAME}_INSTALL_BIN_DIR ${Slicer_INSTALL_QTLOADABLEMODULES_BIN_DIR})
endif()
if(NOT DEFINED ${PROJECT_NAME}_INSTALL_LIB_DIR)
  set(${PROJECT_NAME}_INSTALL_LIB_DIR ${Slicer_INSTALL_QTLOADABLEMODULES_LIB_DIR})
endif()
if(NOT DEFINED ${PROJECT_NAME}_INSTALL_SHARE_DIR)
  set(${PROJECT_NAME}_INSTALL_SHARE_DIR ${Slicer_INSTALL_QTLOADABLEMODULES_LIB_DIR})
endif()

install(TARGETS ${lib_name}
  RUNTIME DESTINATION ${${PROJECT_NAME}_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
  LIBRARY DESTINATION ${${PROJECT_NAME}_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${${PROJECT_NAME}_INSTALL_LIB_DIR} COMPONENT Development
  )

set_property(GLOBAL APPEND PROPERTY Slicer_TARGETS ${lib_name})

# --------------------------------------------------------------------------
# Python Wrapping
# --------------------------------------------------------------------------
if(VTK_WRAP_PYTHON AND BUILD_SHARED_LIBS)

  set(Slicer_Wrapped_LIBRARIES
    )

  if(${VTK_VERSION} VERSION_LESS "8.90")
    foreach(library ${lib_name})
      if(TARGET ${library}PythonD)
        list(APPEND Slicer_Wrapped_LIBRARIES ${library}PythonD)
      endif()
    endforeach()
  endif()

  SlicerMacroPythonWrapModuleVTKLibrary(
    NAME ${PROJECT_NAME}
    SRCS ${${PROJECT_NAME}_SRCS}
    WRAPPED_TARGET_LIBRARIES ${Slicer_Wrapped_LIBRARIES}
    RELATIVE_PYTHON_DIR "."
    )

  # Set python module logic output
  set_target_properties(${PROJECT_NAME}Python ${PROJECT_NAME}Python PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_LIB_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_LIB_DIR}"
    )
  if(${VTK_VERSION} VERSION_LESS "8.90")
    set_target_properties(${PROJECT_NAME}PythonD PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_BIN_DIR}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_LIB_DIR}"
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${Slicer_QTLOADABLEMODULES_LIB_DIR}"
      )
  endif()

  # Export target
  set_property(GLOBAL APPEND PROPERTY Slicer_TARGETS ${PROJECT_NAME}Python)
  if(${VTK_VERSION} VERSION_LESS "8.90")
    set_property(GLOBAL APPEND PROPERTY Slicer_TARGETS ${PROJECT_NAME}PythonD)
  endif()
endif()

# --------------------------------------------------------------------------
# Testing (requires some of the examples)
# --------------------------------------------------------------------------
# if(BUILD_TESTING)
#  add_subdirectory(Testing)
# endif()

# --------------------------------------------------------------------------
# Install Test Data
# --------------------------------------------------------------------------
if(NOT DEFINED ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  set(${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT ON)
endif()
if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  #file(GLOB_RECURSE PNGFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Testing/*.png")
  #file(GLOB_RECURSE TCLFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Testing/*.tcl")
  #file(GLOB_RECURSE CURVFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Testing/*.curv")
  #file(GLOB_RECURSE ORIGFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Testing/*.orig")

  #foreach(file
  #    ${PNGFILES}
  #    ${TCLFILES}
  #    ${CURVFILES}
  #    ${ORIGFILES}
  #    )
  #  configure_file(
  #    ${CMAKE_CURRENT_SOURCE_DIR}/${file}
  #    ${CMAKE_BINARY_DIR}/share/${PROJECT_NAME}/${file}
  #    COPYONLY
  #    )
  #endforeach()

  install(DIRECTORY
    Testing
    DESTINATION ${${PROJECT_NAME}_INSTALL_SHARE_DIR} COMPONENT Development
    PATTERN "*.cxx" EXCLUDE
    PATTERN ".svn" EXCLUDE
    PATTERN "*.tcl" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
    )
endif()

# --------------------------------------------------------------------------
# Set INCLUDE_DIRS variable
# --------------------------------------------------------------------------
set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
  CACHE INTERNAL "${PROJECT_NAME} include dirs" FORCE)
